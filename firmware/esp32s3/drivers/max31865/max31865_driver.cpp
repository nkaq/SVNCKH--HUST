
#include "max31865_driver.h"

#include <math.h>

namespace
{
constexpr uint8_t kConfig = 0x00;
constexpr uint8_t kRtd = 0x01;
constexpr uint8_t kFault = 0x07;

constexpr uint8_t kBias = 0x80;
constexpr uint8_t kAuto = 0x40;
constexpr uint8_t kOneShot = 0x20;
constexpr uint8_t kThreeWire = 0x10;
constexpr uint8_t kClearFault = 0x02;
constexpr uint8_t k50Hz = 0x01;

// Persistent configuration bits.
constexpr uint8_t kPersistentMask = 0xD1;

// Conservative conversion wait durations.
constexpr uint32_t kConversion60HzMs = 55;
constexpr uint32_t kConversion50HzMs = 66;
}

bool Max31865Driver::begin(
    IMax31865Bus& bus,
    float rref_ohms
)
{
    if (
        initialized_ ||
        !isfinite(rref_ohms) ||
        rref_ohms <= 0.0f
    )
    {
        return false;
    }

    bus_ = &bus;
    rref_ohms_ = rref_ohms;

    initialized_ = true;
    configured_ = false;
    expected_config_ = 0;

    config_status_ =
        Max31865ConfigStatus::NotConfigured;

    cancelOneShot();

    return true;
}

void Max31865Driver::cancelOneShot()
{
    shot_state_ = ShotState::Idle;

    phase_started_ms_ = 0;
    bias_settle_ms_ = 0;
    conversion_ms_ = 0;
}

bool Max31865Driver::shutdownHardware()
{
    if (!initialized_ || bus_ == nullptr)
    {
        return false;
    }

    // Cancel pending software conversion immediately.
    cancelOneShot();

    // Do not allow previous configuration to be treated
    // as usable after a shutdown attempt.
    configured_ = false;
    expected_config_ = 0;
    config_status_ =
        Max31865ConfigStatus::NotConfigured;

    // Write all-zero configuration:
    // VBIAS = 0
    // Automatic conversion = 0
    // One-shot = 0
    // Fault clear = 0
    if (!bus_->writeRegister(0x00, 0x00))
    {
        return false;
    }

    uint8_t observed = 0;

    if (!bus_->readRegisters(
            0x00,
            &observed,
            1
        ))
    {
        return false;
    }

    // Verify VBIAS (D7) and automatic conversion (D6)
    // are disabled. Self-clearing one-shot (D5) cannot
    // prove that an earlier conversion has completed.
    constexpr uint8_t kPowerAndAutoMask = 0xC0;

    return (observed & kPowerAndAutoMask) == 0;
}

void Max31865Driver::end()
{
    // Software state reset only.
    // Does not switch off the physical sensor.
    cancelOneShot();

    initialized_ = false;
    configured_ = false;

    bus_ = nullptr;
    rref_ohms_ = NAN;
    expected_config_ = 0;

    config_status_ =
        Max31865ConfigStatus::NotInitialized;
}

uint8_t Max31865Driver::encodeConfig(
    const Max31865Config& config
)
{
    uint8_t result = 0;

    if (config.bias_enabled)
    {
        result |= kBias;
    }

    if (
        config.conversion ==
        Max31865ConversionMode::Automatic
    )
    {
        result |= kAuto;
    }

    if (
        config.wires ==
        Max31865WireMode::ThreeWire
    )
    {
        result |= kThreeWire;
    }

    if (
        config.filter ==
        Max31865Filter::Hz50
    )
    {
        result |= k50Hz;
    }

    return result;
}

bool Max31865Driver::configure(
    const Max31865Config& config
)
{
    if (!initialized_ || bus_ == nullptr)
    {
        config_status_ =
            Max31865ConfigStatus::NotInitialized;

        return false;
    }

    // Any configuration attempt invalidates
    // a pending software conversion.
    cancelOneShot();

    configured_ = false;
    expected_config_ = 0;

    // Automatic conversion requires VBIAS.
    if (
        !config.bias_enabled &&
        config.conversion ==
            Max31865ConversionMode::Automatic
    )
    {
        config_status_ =
            Max31865ConfigStatus::NotConfigured;

        return false;
    }

    const uint8_t encoded =
        encodeConfig(config);

    if (!bus_->writeRegister(
            kConfig,
            encoded
        ))
    {
        config_status_ =
            Max31865ConfigStatus::WriteFailed;

        return false;
    }

    uint8_t observed = 0;

    if (!bus_->readRegisters(
            kConfig,
            &observed,
            1
        ))
    {
        config_status_ =
            Max31865ConfigStatus::ReadbackFailed;

        return false;
    }

    if (
        (observed & kPersistentMask) !=
        (encoded & kPersistentMask)
    )
    {
        config_status_ =
            Max31865ConfigStatus::Mismatch;

        return false;
    }

    expected_config_ = encoded;
    configured_ = true;

    config_status_ =
        Max31865ConfigStatus::Configured;

    return true;
}

bool Max31865Driver::isConfigured() const
{
    return configured_;
}

Max31865ConfigStatus
Max31865Driver::configStatus() const
{
    return config_status_;
}

uint8_t Max31865Driver::expectedConfigRegister() const
{
    return expected_config_;
}

bool Max31865Driver::isInitialized() const
{
    return initialized_;
}

bool Max31865Driver::oneShotActive() const
{
    return shot_state_ != ShotState::Idle;
}

// =====================================================
// Legacy diagnostic read
// =====================================================

Max31865Sample Max31865Driver::readSample()
{
    Max31865Sample sample = {};

    if (!initialized_ || bus_ == nullptr)
    {
        return sample;
    }

    uint8_t raw[2] = {};

    if (!bus_->readRegisters(
            kRtd,
            raw,
            2
        ))
    {
        sample.status =
            Max31865ReadStatus::TransportError;

        return sample;
    }

    const Max31865RtdReading decoded =
        Max31865Codec::decode(
            raw[0],
            raw[1],
            rref_ohms_
        );

    sample.adc_code = decoded.adc_code;
    sample.fault_flag = decoded.fault_flag;
    sample.resistance_ohms =
        decoded.resistance_ohms;

    if (decoded.fault_flag)
    {
        sample.status =
            Max31865ReadStatus::SensorFault;

        if (!bus_->readRegisters(
                kFault,
                &sample.fault_status,
                1
            ))
        {
            sample.status =
                Max31865ReadStatus::TransportError;
        }

        return sample;
    }

    if (!decoded.valid)
    {
        sample.status =
            Max31865ReadStatus::SensorFault;

        return sample;
    }

    sample.status = Max31865ReadStatus::Valid;
    sample.valid = true;

    return sample;
}

// =====================================================
// One-shot start
// =====================================================

bool Max31865Driver::startOneShot(
    uint32_t now_ms,
    uint32_t bias_settle_ms
)
{
    if (
        !initialized_ ||
        !configured_ ||
        bus_ == nullptr ||
        shot_state_ != ShotState::Idle ||
        bias_settle_ms == 0 ||
        (expected_config_ & (kBias | kAuto)) !=
            kBias
    )
    {
        return false;
    }

    // Clear faults remaining from previous readings.
    // The clear-fault bit is self-clearing.
    const uint8_t clear_command =
        static_cast<uint8_t>(
            expected_config_ | kClearFault
        );

    if (!bus_->writeRegister(
            kConfig,
            clear_command
        ))
    {
        return false;
    }

    // Verify persistent bits after clear-fault.
    uint8_t observed = 0;

    if (
        !bus_->readRegisters(
            kConfig,
            &observed,
            1
        )
    )
    {
        return false;
    }

    if (
        (observed & kPersistentMask) !=
        (expected_config_ & kPersistentMask)
    )
    {
        return false;
    }

    bias_settle_ms_ = bias_settle_ms;

    conversion_ms_ =
        (expected_config_ & k50Hz)
            ? kConversion50HzMs
            : kConversion60HzMs;

    phase_started_ms_ = now_ms;
    shot_state_ = ShotState::BiasSettling;

    return true;
}

// =====================================================
// Nonblocking one-shot polling
// =====================================================

Max31865Sample Max31865Driver::pollOneShot(
    uint32_t now_ms
)
{
    Max31865Sample result = {};

    if (!initialized_ || bus_ == nullptr)
    {
        return result;
    }

    result.status =
        Max31865ReadStatus::InvalidState;

    if (shot_state_ == ShotState::Idle)
    {
        return result;
    }

    result.status =
        Max31865ReadStatus::NotReady;

    // Phase 1: VBIAS settling.
    if (shot_state_ == ShotState::BiasSettling)
    {
        const uint32_t elapsed =
            static_cast<uint32_t>(
                now_ms - phase_started_ms_
            );

        if (elapsed < bias_settle_ms_)
        {
            return result;
        }

        const uint8_t one_shot_command =
            static_cast<uint8_t>(
                expected_config_ | kOneShot
            );

        if (!bus_->writeRegister(
                kConfig,
                one_shot_command
            ))
        {
            cancelOneShot();

            result.status =
                Max31865ReadStatus::TransportError;

            return result;
        }

        // Start conversion timer at the
        // supplied polling timestamp.
        phase_started_ms_ = now_ms;
        shot_state_ = ShotState::Converting;

        return result;
    }

    // Phase 2: conversion in progress.
    const uint32_t elapsed =
        static_cast<uint32_t>(
            now_ms - phase_started_ms_
        );

    if (elapsed < conversion_ms_)
    {
        return result;
    }

    // Consume the request once, including
    // failed or faulty measurements.
    cancelOneShot();

    return readCheckedSample();
}

// =====================================================
// Checked RTD read
// =====================================================

Max31865Sample
Max31865Driver::readCheckedSample()
{
    Max31865Sample result = {};

    result.status =
        Max31865ReadStatus::TransportError;

    uint8_t raw[2] = {};

    if (!bus_->readRegisters(
            kRtd,
            raw,
            2
        ))
    {
        return result;
    }

    const Max31865RtdReading decoded =
        Max31865Codec::decode(
            raw[0],
            raw[1],
            rref_ohms_
        );

    result.adc_code = decoded.adc_code;
    result.fault_flag = decoded.fault_flag;

    // Always inspect Fault Status.
    // RTD fault bit alone is insufficient.
    if (!bus_->readRegisters(
            kFault,
            &result.fault_status,
            1
        ))
    {
        return result;
    }

    if (
        decoded.fault_flag ||
        result.fault_status != 0 ||
        !decoded.valid
    )
    {
        result.status =
            Max31865ReadStatus::SensorFault;

        return result;
    }

    result.resistance_ohms =
        decoded.resistance_ohms;

    result.valid = true;
    result.status = Max31865ReadStatus::Valid;

    return result;
}
