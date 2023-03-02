#include "BMSInterface.h"

void BMS::CreateVerboseMessages()
{
    for (uint16_t i = 0; i < kNumVoltageMessages; i++)
    {
        voltage_signals_[i * kSignalsPerMessage + 0] = new MakeUnsignedCANSignal(float, 0, 8, 0.012, 2);
        *(voltage_signals_[i * kSignalsPerMessage + 0]) = 0;
        voltage_signals_[i * kSignalsPerMessage + 1] = new MakeUnsignedCANSignal(float, 8, 8, 0.012, 2);
        *(voltage_signals_[i * kSignalsPerMessage + 1]) = 0;
        voltage_signals_[i * kSignalsPerMessage + 2] = new MakeUnsignedCANSignal(float, 16, 8, 0.012, 2);
        *(voltage_signals_[i * kSignalsPerMessage + 2]) = 0;
        voltage_signals_[i * kSignalsPerMessage + 3] = new MakeUnsignedCANSignal(float, 24, 8, 0.012, 2);
        *(voltage_signals_[i * kSignalsPerMessage + 3]) = 0;
        voltage_signals_[i * kSignalsPerMessage + 4] = new MakeUnsignedCANSignal(float, 32, 8, 0.012, 2);
        *(voltage_signals_[i * kSignalsPerMessage + 4]) = 0;
        voltage_signals_[i * kSignalsPerMessage + 5] = new MakeUnsignedCANSignal(float, 40, 8, 0.012, 2);
        *(voltage_signals_[i * kSignalsPerMessage + 5]) = 0;
        voltage_signals_[i * kSignalsPerMessage + 6] = new MakeUnsignedCANSignal(float, 48, 8, 0.012, 2);
        *(voltage_signals_[i * kSignalsPerMessage + 6]) = 0;

        voltage_messages_[i] = new CANRXMessage<7>{can_interface_, 512 + i,
                                                   *(voltage_signals_[i * kSignalsPerMessage + 0]),
                                                   *(voltage_signals_[i * kSignalsPerMessage + 1]),
                                                   *(voltage_signals_[i * kSignalsPerMessage + 2]),
                                                   *(voltage_signals_[i * kSignalsPerMessage + 3]),
                                                   *(voltage_signals_[i * kSignalsPerMessage + 4]),
                                                   *(voltage_signals_[i * kSignalsPerMessage + 5]),
                                                   *(voltage_signals_[i * kSignalsPerMessage + 6])};
        can_interface_.RegisterRXMessage(*(voltage_messages_[i]));
    }

    for (uint16_t i = 0; i < kNumTemperatureMessages; i++)
    {
        temperature_signals_[i * kSignalsPerMessage + 0] = new MakeUnsignedCANSignal(float, 0, 8, 1, -40);
        *(temperature_signals_[i * kSignalsPerMessage + 0]) = -40;
        temperature_signals_[i * kSignalsPerMessage + 1] = new MakeUnsignedCANSignal(float, 8, 8, 1, -40);
        *(temperature_signals_[i * kSignalsPerMessage + 1]) = -40;
        temperature_signals_[i * kSignalsPerMessage + 2] = new MakeUnsignedCANSignal(float, 16, 8, 1, -40);
        *(temperature_signals_[i * kSignalsPerMessage + 2]) = -40;
        temperature_signals_[i * kSignalsPerMessage + 3] = new MakeUnsignedCANSignal(float, 24, 8, 1, -40);
        *(temperature_signals_[i * kSignalsPerMessage + 3]) = -40;
        temperature_signals_[i * kSignalsPerMessage + 4] = new MakeUnsignedCANSignal(float, 32, 8, 1, -40);
        *(temperature_signals_[i * kSignalsPerMessage + 4]) = -40;
        temperature_signals_[i * kSignalsPerMessage + 5] = new MakeUnsignedCANSignal(float, 40, 8, 1, -40);
        *(temperature_signals_[i * kSignalsPerMessage + 5]) = -40;
        temperature_signals_[i * kSignalsPerMessage + 6] = new MakeUnsignedCANSignal(float, 48, 8, 1, -40);
        *(temperature_signals_[i * kSignalsPerMessage + 6]) = -40;

        temperature_messages_[i] = new CANRXMessage<7>{can_interface_, 544 + i,
                                                       *(temperature_signals_[i * kSignalsPerMessage + 0]),
                                                       *(temperature_signals_[i * kSignalsPerMessage + 1]),
                                                       *(temperature_signals_[i * kSignalsPerMessage + 2]),
                                                       *(temperature_signals_[i * kSignalsPerMessage + 3]),
                                                       *(temperature_signals_[i * kSignalsPerMessage + 4]),
                                                       *(temperature_signals_[i * kSignalsPerMessage + 5]),
                                                       *(temperature_signals_[i * kSignalsPerMessage + 6])};
        can_interface_.RegisterRXMessage(*(temperature_messages_[i]));
    }
}