
#include <Arduino.h>
#include <unity.h>

#include "../../drivers/max31865/max31865_spi_protocol.h"

// =====================================================
// Recorded SPI operations
// =====================================================

enum class SpiEvent : uint8_t
{
    Begin,
    Select,
    Delay,
    Transfer,
    Deselect,
    End
};

struct SpiRecord
{
    SpiEvent event;
    uint8_t value;
};

class FakeSpiIo final : public IMax31865SpiIo
{
public:
    static constexpr size_t kCapacity = 32;

    SpiRecord records[kCapacity] = {};
    size_t count = 0;

    uint8_t response[kCapacity] = {};
    size_t transfer_count = 0;

    bool overflow = false;

    void record(
        SpiEvent event,
        uint8_t value = 0
    )
    {
        if (count >= kCapacity)
        {
            overflow = true;
            return;
        }

        records[count].event = event;
        records[count].value = value;
        ++count;
    }

    void beginTransaction() override
    {
        record(SpiEvent::Begin);
    }

    void setChipSelect(bool selected) override
    {
        record(
            selected
                ? SpiEvent::Select
                : SpiEvent::Deselect
        );
    }

    void csDelay() override
    {
        record(SpiEvent::Delay);
    }

    uint8_t transfer(uint8_t value) override
    {
        record(SpiEvent::Transfer, value);

        if (transfer_count >= kCapacity)
        {
            overflow = true;
            return 0;
        }

        const uint8_t result =
            response[transfer_count];

        ++transfer_count;
        return result;
    }

    void endTransaction() override
    {
        record(SpiEvent::End);
    }
};

static FakeSpiIo io;

void setUp()
{
    io = FakeSpiIo();
}

void tearDown()
{
}

// =====================================================
// Assertion helper
// =====================================================

static void expectEvent(
    size_t index,
    SpiEvent event,
    uint8_t value = 0
)
{
    TEST_ASSERT_TRUE(index < io.count);

    TEST_ASSERT_EQUAL_UINT8(
        static_cast<uint8_t>(event),
        static_cast<uint8_t>(
            io.records[index].event
        )
    );

    TEST_ASSERT_EQUAL_HEX8(
        value,
        io.records[index].value
    );
}

// =====================================================
// TEST 01
// RTD register read uses one transaction.
// =====================================================

void test_read_two_bytes_transaction_sequence()
{
    // First response corresponds to the address
    // byte and is intentionally discarded.
    io.response[0] = 0xAA;
    io.response[1] = 0x80;
    io.response[2] = 0x00;

    uint8_t result[2] = {};

    TEST_ASSERT_TRUE(
        Max31865SpiProtocol::read(
            io,
            0x01,
            result,
            2
        )
    );

    TEST_ASSERT_EQUAL_HEX8(
        0x80,
        result[0]
    );

    TEST_ASSERT_EQUAL_HEX8(
        0x00,
        result[1]
    );

    TEST_ASSERT_FALSE(io.overflow);
    TEST_ASSERT_EQUAL_UINT32(10, io.count);
    TEST_ASSERT_EQUAL_UINT32(3, io.transfer_count);

    expectEvent(0, SpiEvent::Begin);
    expectEvent(1, SpiEvent::Select);
    expectEvent(2, SpiEvent::Delay);

    // Read address D7=0.
    expectEvent(3, SpiEvent::Transfer, 0x01);

    // Consecutive reads under the same CS.
    expectEvent(4, SpiEvent::Transfer, 0x00);
    expectEvent(5, SpiEvent::Transfer, 0x00);

    expectEvent(6, SpiEvent::Delay);
    expectEvent(7, SpiEvent::Deselect);
    expectEvent(8, SpiEvent::Delay);
    expectEvent(9, SpiEvent::End);
}

// =====================================================
// TEST 02
// Write sets D7 and sends exactly one data byte.
// =====================================================

void test_write_configuration_transaction_sequence()
{
    TEST_ASSERT_TRUE(
        Max31865SpiProtocol::write(
            io,
            0x00,
            0x91
        )
    );

    TEST_ASSERT_FALSE(io.overflow);
    TEST_ASSERT_EQUAL_UINT32(9, io.count);
    TEST_ASSERT_EQUAL_UINT32(2, io.transfer_count);

    expectEvent(0, SpiEvent::Begin);
    expectEvent(1, SpiEvent::Select);
    expectEvent(2, SpiEvent::Delay);

    // Register 0x00 with write flag -> 0x80.
    expectEvent(3, SpiEvent::Transfer, 0x80);
    expectEvent(4, SpiEvent::Transfer, 0x91);

    expectEvent(5, SpiEvent::Delay);
    expectEvent(6, SpiEvent::Deselect);
    expectEvent(7, SpiEvent::Delay);
    expectEvent(8, SpiEvent::End);
}

// =====================================================
// TEST 03
// Fault Status uses logical read address 0x07.
// =====================================================

void test_read_fault_status_register()
{
    io.response[1] = 0x80;

    uint8_t fault = 0;

    TEST_ASSERT_TRUE(
        Max31865SpiProtocol::read(
            io,
            0x07,
            &fault,
            1
        )
    );

    TEST_ASSERT_EQUAL_HEX8(
        0x80,
        fault
    );

    TEST_ASSERT_EQUAL_UINT32(2, io.transfer_count);

    expectEvent(3, SpiEvent::Transfer, 0x07);
    expectEvent(4, SpiEvent::Transfer, 0x00);
}

// =====================================================
// TEST 04
// Null buffer must not start an SPI transaction.
// =====================================================

void test_rejects_null_destination_without_io()
{
    TEST_ASSERT_FALSE(
        Max31865SpiProtocol::read(
            io,
            0x01,
            nullptr,
            2
        )
    );

    TEST_ASSERT_EQUAL_UINT32(0, io.count);
}

// =====================================================
// TEST 05
// Zero-length read must not touch SPI.
// =====================================================

void test_rejects_zero_length_without_io()
{
    uint8_t data[2] = {};

    TEST_ASSERT_FALSE(
        Max31865SpiProtocol::read(
            io,
            0x01,
            data,
            0
        )
    );

    TEST_ASSERT_EQUAL_UINT32(0, io.count);
}

// =====================================================
// TEST 06
// Reading past register 0x07 is prohibited.
// =====================================================

void test_rejects_register_boundary_overflow()
{
    uint8_t data[2] = {};

    TEST_ASSERT_FALSE(
        Max31865SpiProtocol::read(
            io,
            0x07,
            data,
            2
        )
    );

    TEST_ASSERT_EQUAL_UINT32(0, io.count);
}

// =====================================================
// TEST 07
// Illegal addresses cannot create transactions.
// =====================================================

void test_rejects_invalid_register_addresses()
{
    uint8_t data = 0;

    TEST_ASSERT_FALSE(
        Max31865SpiProtocol::read(
            io,
            0x08,
            &data,
            1
        )
    );

    TEST_ASSERT_FALSE(
        Max31865SpiProtocol::read(
            io,
            0x80,
            &data,
            1
        )
    );

    TEST_ASSERT_FALSE(
        Max31865SpiProtocol::write(
            io,
            0x08,
            0x91
        )
    );

    TEST_ASSERT_FALSE(
        Max31865SpiProtocol::write(
            io,
            0x80,
            0x91
        )
    );

    TEST_ASSERT_EQUAL_UINT32(0, io.count);
}

// =====================================================
// Unity runner — 7 tests
// =====================================================

void setup()
{
    delay(3000);

    UNITY_BEGIN();

    RUN_TEST(
        test_read_two_bytes_transaction_sequence
    );

    RUN_TEST(
        test_write_configuration_transaction_sequence
    );

    RUN_TEST(
        test_read_fault_status_register
    );

    RUN_TEST(
        test_rejects_null_destination_without_io
    );

    RUN_TEST(
        test_rejects_zero_length_without_io
    );

    RUN_TEST(
        test_rejects_register_boundary_overflow
    );

    RUN_TEST(
        test_rejects_invalid_register_addresses
    );

    UNITY_END();
}

void loop()
{
}
