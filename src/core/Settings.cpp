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
#include "cpu/memory.h"
#include "SettingsMenu.h"
#include "Buzzer.h"
#include "Settings.h"
#include "Monitor.h"
#include "eeprom.h"

#ifndef SETTINGS_EXTERNAL_T_DEFAULT
#define SETTINGS_EXTERNAL_T_DEFAULT 0
#endif

#ifndef SETTINGS_ADC_NOISE_DEFAULT
#define SETTINGS_ADC_NOISE_DEFAULT  0
#endif

#ifndef SETTINGS_MAX_CHARGE_I
#define SETTINGS_MAX_CHARGE_I       MAX_CHARGE_I
#endif

#ifndef SETTINGS_MAX_DISCHARGE_I
#define SETTINGS_MAX_DISCHARGE_I    MAX_DISCHARGE_I
#endif

#ifndef SETTINGS_MAX_CHARGE_P
#define SETTINGS_MAX_CHARGE_P    MAX_CHARGE_P
#endif

#ifndef SETTINGS_MAX_DISCHARGE_P
#define SETTINGS_MAX_DISCHARGE_P    MAX_DISCHARGE_P
#endif



Settings settings;

const Settings defaultSettings PROGMEM = {
        70,                 //backlight
        Settings::FanTemperature, //fanOn
        ANALOG_CELCIUS(50), //fanTempOn
        ANALOG_CELCIUS(60), //dischargeTempOff

        1,                  //AudioBeep: yes/no
        ANALOG_AMP(0.050),  //minIc
        SETTINGS_MAX_CHARGE_I,       //maxIc
        ANALOG_AMP(0.050),  //minId
        SETTINGS_MAX_DISCHARGE_I,    //maxId

        SETTINGS_MAX_CHARGE_P, //maxPc
        SETTINGS_MAX_DISCHARGE_P, //maxPd

        ANALOG_VOLT(10.000),//inputVoltageLow

        SETTINGS_ADC_NOISE_DEFAULT, //adcNoise
        Settings::Disabled, //UART - disabled
        3,                   //57600
        Settings::TempOutput, //UARToutput
        Settings::MenuSimple, //menuType
        Settings::MenuButtonsReversed, //menuButtons
};


const uint32_t Settings::UARTSpeedValue[Settings::UARTSpeeds] PROGMEM = {
    9600,
    19200,
    38400,
    57600,
    115200,
};


uint32_t Settings::getUARTspeed() const {
    return pgm::read(&UARTSpeedValue[UARTspeed]);
}

void Settings::load() {
    eeprom::read(settings, &eeprom::data.settings);
    settings.apply();
}

void Settings::save() {
    eeprom::write(&eeprom::data.settings, settings);
    eeprom::restoreSettingsCRC();

    settings.apply();
}

void Settings::setDefault()
{
    pgm::read(*this, &defaultSettings);
}
void Settings::restoreDefault() {
    settings.setDefault();
    Settings::save();
}

void Settings::check() {
    if(settings.maxIc < settings.minIc) {
        settings.maxIc = settings.minIc;
    }
    if(settings.maxIc > MAX_CHARGE_I) {
        settings.maxIc = MAX_CHARGE_I;
    }

    if(settings.maxId < settings.minId) {
        settings.maxId = settings.minId;
    }
    if(settings.maxId > MAX_DISCHARGE_I) {
        settings.maxId = MAX_DISCHARGE_I;
    }
}


void Settings::apply() {
#ifdef ENABLE_LCD_BACKLIGHT
    hardware::setLCDBacklight(backlight);
#endif
//    hardware::setExternalTemperatueOutput(externT);
}

