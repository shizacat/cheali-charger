/*
    cheali-charger - open source firmware for a variety of LiPo chargers
    Copyright (C) 2013  Paweł Stawicki. All right reserved.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "Hardware.h"
#include "cpu/Timer1.h"
#include "cpu/Timer0.h"
#include "SMPS_PID.h"
#include "AnalogInputsADC.h"
#include "SerialLog.h"
#include "cpu/IO.h"
#include "Keyboard.h"
#include "LiquidCrystal.h"


#ifndef PINS_H_
#error pins not defined (include *pins.h header in your HardwareConfig.h)
#endif

uint8_t hardware::getKeyPressed()
{
    return   (IO::digitalRead(BUTTON_STOP_PIN) ? 0 : BUTTON_STOP)
            | (IO::digitalRead(BUTTON_DEC_PIN)  ? 0 : BUTTON_DEC)
            | (IO::digitalRead(BUTTON_INC_PIN)  ? 0 : BUTTON_INC)
            | (IO::digitalRead(BUTTON_START_PIN)? 0 : BUTTON_START);
}


void hardware::setBalancerOutput(bool enable)
{
    uint8_t mode = INPUT;
    if(enable)
        mode = OUTPUT;
    IO::pinMode(BALANCER1_LOAD_PIN, mode);
    IO::pinMode(BALANCER2_LOAD_PIN, mode);
    IO::pinMode(BALANCER3_LOAD_PIN, mode);
    IO::pinMode(BALANCER4_LOAD_PIN, mode);
    IO::pinMode(BALANCER5_LOAD_PIN, mode);
    IO::pinMode(BALANCER6_LOAD_PIN, mode);

}


void hardware::initializePins()
{
    setBalancer(0);
    setBatteryOutput(false);
    setBuzzer(0);

    IO::analogReference(EXTERNAL);
    IO::pinMode(OUTPUT_DISABLE_PIN, OUTPUT);

    IO::pinMode(DISCHARGE_VALUE_PIN, OUTPUT);
    IO::pinMode(DISCHARGE_DISABLE_PIN, OUTPUT);

    IO::pinMode(BUZZER_PIN, OUTPUT);

    IO::pinMode(SMPS_VALUE_BUCK_PIN, OUTPUT);
    IO::pinMode(SMPS_VALUE_BOOST_PIN, OUTPUT);
    IO::pinMode(SMPS_DISABLE_PIN, OUTPUT);
}

void hardware::initialize()
{
    LiquidCrystal::init();
    LiquidCrystal::begin(LCD_COLUMNS, LCD_LINES);

    Timer0::initialize();
    Timer1::initialize();
    AnalogInputsADC::initialize();
    setVoutCutoff(MAX_CHARGE_V);
}


void hardware::setBatteryOutput(bool enable)
{
    IO::digitalWrite(OUTPUT_DISABLE_PIN, !enable);
    if(!enable) {
        setChargerOutput(false);
        setDischargerOutput(false);
    }
}

void hardware::setBalancer(uint8_t v)
{
    IO::digitalWrite(BALANCER1_LOAD_PIN, v&1);
    IO::digitalWrite(BALANCER2_LOAD_PIN, v&2);
    IO::digitalWrite(BALANCER3_LOAD_PIN, v&4);
    IO::digitalWrite(BALANCER4_LOAD_PIN, v&8);
    IO::digitalWrite(BALANCER5_LOAD_PIN, v&16);
    IO::digitalWrite(BALANCER6_LOAD_PIN, v&32);
}

