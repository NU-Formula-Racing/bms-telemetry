#pragma once
#include <Arduino.h>
#include <array>
#include <can_interface.h>

class BMS
{
public:
    BMS(ICAN &can_interface) : can_interface_{can_interface} { CreateVerboseMessages(); }

    enum class BMSState
    {
        kShutdown = 0,
        kPrecharge = 1,
        kActive = 2,
        kCharging = 3,
        kFault = 4
    };

    enum class BMSFault : bool
    {
        kNotFaulted = 0,
        kFaulted = 1,
    };

    void Shutdown()
    {
        current_state_ = BMSState::kShutdown;
    }

    void PrechargeAndCloseContactors()
    {
        current_state_ = BMSState::kActive;
    }

    float GetCurrent()
    {
        return 5.63;
    }

    float GetVoltage(uint8_t index)
    {
        return *(voltage_signals_[index]);
    }

    float GetTemperature(uint8_t index)
    {
        return *(temperature_signals_[index]);
    }

    String GetState()
    {
        switch (current_state_)
        {
        case BMSState::kActive:
            return "Active";
            break;
        case BMSState::kShutdown:
            return "Shutdown";
            break;

        default:
            return "Error";
            break;
        }
    }

    BMSFault GetUndervoltage() { return BMSFault::kNotFaulted; }
    BMSFault GetOvervoltage() { return BMSFault::kNotFaulted; }
    BMSFault GetUndertemperature() { return BMSFault::kFaulted; }
    BMSFault GetOvertemperature() { return BMSFault::kNotFaulted; }
    BMSFault GetOvercurrent() { return BMSFault::kNotFaulted; }
    BMSFault GetExternal_kill() { return BMSFault::kNotFaulted; }

private:
    ICAN &can_interface_;
    static const uint16_t kSignalsPerMessage = 7;
    static const uint16_t kNumVoltageMessages = 20;
    static const uint16_t kNumTemperatureMessages = 16;

    std::array<ITypedCANSignal<float> *, kNumVoltageMessages * kSignalsPerMessage> voltage_signals_;
    std::array<ITypedCANSignal<float> *, kNumTemperatureMessages * kSignalsPerMessage> temperature_signals_;

    std::array<CANRXMessage<kSignalsPerMessage> *, kNumVoltageMessages> voltage_messages_;
    std::array<CANRXMessage<kSignalsPerMessage> *, kNumTemperatureMessages> temperature_messages_;
    BMSState current_state_{BMSState::kShutdown};
    // TODO: missing state and current signals/messages, also add missing command message

    void CreateVerboseMessages();
};