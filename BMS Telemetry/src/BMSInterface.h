#pragma once
#include <Arduino.h>
#include <array>
#include <can_interface.h>
#include "virtualTimer.h"

class BMS
{
public:
    BMS(ICAN &can_interface, VirtualTimerGroup &timer_group) : can_interface_{can_interface}, timer_group_{timer_group}
    {
        CreateVerboseMessages();
        can_interface_.RegisterRXMessage(status_message_);
        can_interface_.RegisterRXMessage(soe_message_);
        can_interface_.RegisterRXMessage(fault_message_);
    }

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

    enum class Command : uint8_t
    {
        kNoAction = 0,
        kPrechargeAndCloseContactors = 1,
        kShutdown = 2,
        kClearFaults = 3
    };

    void Shutdown()
    {
        command_signal_ = Command::kShutdown;
    }

    void PrechargeAndCloseContactors()
    {
        command_signal_ = Command::kPrechargeAndCloseContactors;
    }

    float GetCurrent()
    {
        return battery_current_signal_;
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
        switch (state_signal_)
        {
        case BMSState::kShutdown:
            return "Shutdown";
            break;
        case BMSState::kActive:
            return "Active";
            break;
        case BMSState::kPrecharge:
            return "Precharge";
            break;
        case BMSState::kFault:
            return "Fault";
            break;
        case BMSState::kCharging:
            return "Charging";
            break;
        default:
            return "error";
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
    VirtualTimerGroup &timer_group_;
    static const uint16_t kSignalsPerMessage = 7;
    static const uint16_t kNumVoltageMessages = 20;
    static const uint16_t kNumTemperatureMessages = 16;

    std::array<ITypedCANSignal<float> *, kNumVoltageMessages * kSignalsPerMessage> voltage_signals_;
    std::array<ITypedCANSignal<float> *, kNumTemperatureMessages * kSignalsPerMessage> temperature_signals_;

    std::array<CANRXMessage<kSignalsPerMessage> *, kNumVoltageMessages> voltage_messages_;
    std::array<CANRXMessage<kSignalsPerMessage> *, kNumTemperatureMessages> temperature_messages_;
    MakeUnsignedCANSignal(BMSState, 0, 8, 1, 0) state_signal_;
    MakeUnsignedCANSignal(float, 8, 8, 1, -40) max_cell_temperature_signal_;
    MakeUnsignedCANSignal(float, 16, 8, 1, -40) min_cell_temperature_signal_;
    MakeUnsignedCANSignal(float, 24, 8, 0.012, 2) max_cell_voltage_signal_;
    MakeUnsignedCANSignal(float, 32, 8, 0.012, 2) min_cell_voltage_signal_;
    MakeUnsignedCANSignal(float, 40, 8, 0.5, 0) soc_signal_;
    CANRXMessage<6> status_message_{can_interface_, 577,
                                    state_signal_,
                                    max_cell_temperature_signal_,
                                    min_cell_temperature_signal_,
                                    max_cell_voltage_signal_,
                                    min_cell_voltage_signal_,
                                    soc_signal_};

    MakeUnsignedCANSignal(bool, 0, 1, 1, 0) summary_signal_;
    MakeUnsignedCANSignal(bool, 2, 1, 1, 0) undervoltage_signal_;
    MakeUnsignedCANSignal(bool, 3, 1, 1, 0) overvoltage_signal_;
    MakeUnsignedCANSignal(bool, 3, 1, 1, 0) undertemperature_signal_;
    MakeUnsignedCANSignal(bool, 4, 1, 1, 0) overtemperature_signal_;
    MakeUnsignedCANSignal(bool, 5, 1, 1, 0) overcurrent_signal_;
    MakeUnsignedCANSignal(bool, 6, 1, 1, 0) external_kill_signal_;

    CANRXMessage<7> fault_message_{can_interface_, 592,
                                   summary_signal_,
                                   undervoltage_signal_,
                                   overvoltage_signal_,
                                   undertemperature_signal_,
                                   overtemperature_signal_,
                                   overcurrent_signal_,
                                   external_kill_signal_};

    MakeUnsignedCANSignal(float, 0, 12, 0.1, 0) max_discharge_signal_;
    MakeUnsignedCANSignal(float, 12, 12, 0.1, 0) max_regen_signal_;
    MakeUnsignedCANSignal(float, 24, 16, 0.01, 0) battery_voltage_signal_;
    MakeUnsignedCANSignal(float, 40, 8, 1, -40) battery_temperature_signal_;
    MakeSignedCANSignal(float, 48, 16, 0.01, 0) battery_current_signal_;

    CANRXMessage<5> soe_message_{can_interface_, 576,
                                 max_discharge_signal_,
                                 max_regen_signal_,
                                 battery_voltage_signal_,
                                 battery_temperature_signal_,
                                 battery_current_signal_};

    MakeUnsignedCANSignal(Command, 0, 8, 1, 0) command_signal_;
    CANTXMessage<1> command_message_{can_interface_, 0x242, 1, 100, timer_group_, command_signal_};

    void CreateVerboseMessages();
};