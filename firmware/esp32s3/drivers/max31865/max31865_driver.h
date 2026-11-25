
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <math.h>

#include "max31865_codec.h"

// Logical MAX31865 register addresses.
// The physical SPI adapter must set bit 7 on writes.
class IMax31865Bus
{
public:
    virtual ~IMax31865Bus() = default;

    virtual bool readRegisters(
        uint8_t start_address,
        uint8_t* destination,
        size_t length
    ) = 0;

    virtual bool writeRegister(
        uint8_t address,
        uint8_t value
    ) = 0;
};

enum class Max31865ReadStatus : uint8_t
{
    NotInitialized,
    TransportError,
    SensorFault,
    Valid,
    NotReady,
    InvalidState
};

enum class Max31865WireMode : uint8_t
{
    TwoOrFourWire,
    ThreeWire
};

enum class Max31865Filter : uint8_t
{
    Hz60,
    Hz50
};

enum class Max31865ConversionMode : uint8_t
{
    NormallyOff,
    Automatic
};

struct Max31865Config
{
    Max31865WireMode wires =
        Max31865WireMode::TwoOrFourWire;

    Max31865Filter filter =
        Max31865Filter::Hz60;

    Max31865ConversionMode conversion =
        Max31865ConversionMode::NormallyOff;

    bool bias_enabled = true;
};

enum class Max31865ConfigStatus : uint8_t
{
    NotInitialized,
    NotConfigured,
    WriteFailed,
    ReadbackFailed,
    Mismatch,
    Configured
};

struct Max31865Sample
{
    Max31865ReadStatus status =
        Max31865ReadStatus::NotInitialized;

    uint16_t adc_code = 0;
    float resistance_ohms = NAN;

    bool fault_flag = false;
    uint8_t fault_status = 0;
    bool valid = false;
};

class Max31865Driver
{
public:
    bool begin(
        IMax31865Bus& bus,
        float rref_ohms
    );

    void end();
    bool shutdownHardware();
    bool configure(
        const Max31865Config& config
    );

    bool isConfigured() const;

    Max31865ConfigStatus configStatus() const;

    uint8_t expectedConfigRegister() const;

    bool isInitialized() const;

    // Legacy diagnostic API.
    // Does NOT guarantee fresh conversion data.
    // Do not use directly in acquisition pipeline.
    Max31865Sample readSample();

    // Start a nonblocking one-shot measurement.
    //
    // now_ms must come from a monotonic clock,
    // such as Arduino millis().
    //
    // bias_settle_ms must be calculated for
    // the real circuit:
    // at least 10.5 * R * C + 1 ms,
    // with R*C expressed in milliseconds.
    //
    // Requirements:
    // - driver initialized and configured
    // - VBIAS enabled
    // - automatic conversion disabled
    // - bias_settle_ms >= 1
    // - no other one-shot currently active
    bool startOneShot(
        uint32_t now_ms,
        uint32_t bias_settle_ms
    );

    // Returns NotReady while settling/converting.
    // The completed result can be consumed once.
    // Further polls return InvalidState until
    // another startOneShot().
    Max31865Sample pollOneShot(
        uint32_t now_ms
    );

    bool oneShotActive() const;

private:
    enum class ShotState : uint8_t
    {
        Idle,
        BiasSettling,
        Converting
    };

    static uint8_t encodeConfig(
        const Max31865Config& config
    );

    Max31865Sample readCheckedSample();

    void cancelOneShot();

    IMax31865Bus* bus_ = nullptr;
    float rref_ohms_ = NAN;

    bool initialized_ = false;
    bool configured_ = false;

    uint8_t expected_config_ = 0;

    Max31865ConfigStatus config_status_ =
        Max31865ConfigStatus::NotInitialized;

    ShotState shot_state_ = ShotState::Idle;

    uint32_t phase_started_ms_ = 0;
    uint32_t bias_settle_ms_ = 0;
    uint32_t conversion_ms_ = 0;
};
