#include "Hardware.h"
#include "SMPS_PID.h"
#include "cpu/IO.h"
#include "AnalogInputs.h"
#include "cpu/atomic.h"
#include "Monitor.h"
#include "Utils.h"

//#define ENABLE_DEBUG
#include "debug.h"


STATIC_ASSERT(MAX_PID_MV_FACTOR < 1.61);

namespace {
    volatile uint16_t i_PID_setpoint;
    //we have to use i_PID_CutOffVoltage, on some chargers (M0516) ADC can read up to 60V
    volatile uint16_t i_PID_CutOffVoltage;
    volatile long i_PID_MV;
    volatile bool i_PID_enable;
}

#define A 4

uint16_t hardware::getPIDValue()
{
//    return PID_setpoint;
    uint16_t v;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        v = i_PID_MV>>PID_MV_PRECISION;
    }
    return v;
}

extern bool adcDebugStop;

void SMPS_PID::update()
{
    if(!i_PID_enable) return;
    //if Vout is too high disable PID
    if(AnalogInputs::getADCValue(AnalogInputs::Vout_plus_pin) >= i_PID_CutOffVoltage) {
        hardware::setChargerOutput(false);
        i_PID_enable = false;
        runDebug(adcDebugStop = true);
        LogDebug(AnalogInputs::getADCValue(AnalogInputs::Vout_plus_pin), ">=", i_PID_CutOffVoltage);
        Monitor::i_externalError = MONITOR_EXTERNAL_ERROR_BATTERY_DISCONNECTED;
        return;
    }

    //TODO: rewrite PID
    //this is the PID - actually it is an I (Integral part) - should be rewritten
    uint16_t PV = AnalogInputs::getADCValue(AnalogInputs::Ismps);
    long error = i_PID_setpoint;
    error -= PV;
    i_PID_MV += error*A;

    if(i_PID_MV<0) i_PID_MV = 0;
    if(i_PID_MV > MAX_PID_MV_PRECISION) {
        i_PID_MV = MAX_PID_MV_PRECISION;
    }
    SMPS_PID::setPID_MV(i_PID_MV>>PID_MV_PRECISION);
}

void SMPS_PID::init(uint16_t Vin, uint16_t Vout)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        i_PID_setpoint = 0;
        if(Vout>Vin) {
            i_PID_MV = TIMER1_PRECISION_PERIOD;
        } else {
            i_PID_MV = 0;
        }
        i_PID_MV <<= PID_MV_PRECISION;
        i_PID_enable = true;
    }
}

namespace {
    void enableChargerBuck() {
        Timer1::disablePWM(SMPS_VALUE_BUCK_PIN);
        IO::digitalWrite(SMPS_VALUE_BUCK_PIN, 1);
    }
    void disableChargerBuck() {
        Timer1::disablePWM(SMPS_VALUE_BUCK_PIN);
        IO::digitalWrite(SMPS_VALUE_BUCK_PIN, 0);
    }
    void disableChargerBoost() {
        Timer1::disablePWM(SMPS_VALUE_BOOST_PIN);
        IO::digitalWrite(SMPS_VALUE_BOOST_PIN, 0);
    }
}

void SMPS_PID::setPID_MV(uint16_t value) {
    if(value > MAX_PID_MV)
        value = MAX_PID_MV;

    if(value <= TIMER1_PRECISION_PERIOD) {
        disableChargerBoost();
        Timer1::setPWM(SMPS_VALUE_BUCK_PIN, value);
    } else {
        enableChargerBuck();
        uint16_t v2 = value - TIMER1_PRECISION_PERIOD;
        Timer1::setPWM(SMPS_VALUE_BOOST_PIN, v2);
    }
}

void hardware::setVoutCutoff(AnalogInputs::ValueType v) {
    if(v > MAX_CHARGE_V) {
        v = MAX_CHARGE_V;
    }
    AnalogInputs::ValueType cutOff = AnalogInputs::reverseCalibrateValue(AnalogInputs::Vout_plus_pin, v);
    if(cutOff > ANALOG_INPUTS_MAX_ADC_Vout_plus_pin) {
        //extra limit if calibration is wrong
        cutOff = ANALOG_INPUTS_MAX_ADC_Vout_plus_pin;
    }

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        i_PID_CutOffVoltage = cutOff;
    }
}

void hardware::setChargerValue(uint16_t value)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        i_PID_setpoint = value;
    }
}

void hardware::setChargerOutput(bool enable)
{
    if(enable) setDischargerOutput(false);
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        disableChargerBuck();
        disableChargerBoost();
        i_PID_enable = false;
    }
    IO::digitalWrite(SMPS_DISABLE_PIN, !enable);
    if(enable) {
        SMPS_PID::init(AnalogInputs::getRealValue(AnalogInputs::Vin), AnalogInputs::getRealValue(AnalogInputs::Vout_plus_pin));
    }
}


void hardware::setDischargerOutput(bool enable)
{
    if(enable) setChargerOutput(false);
    IO::digitalWrite(DISCHARGE_DISABLE_PIN, !enable);
}

void hardware::setDischargerValue(uint16_t value)
{
    Timer1::setPWM(DISCHARGE_VALUE_PIN, value);
}

