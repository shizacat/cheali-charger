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
#include "ChealiCharger2.h"
#include "MainMenu.h"
#include "ProgramData.h"
#include "AnalogInputs.h"
#include "Utils.h"
#include "Buzzer.h"
#include "Version.h"
#include "Settings.h"
#include "StackInfo.h"
#include "Hardware.h"
#include "SerialLog.h"
#include "eeprom.h"
#include "cpu/cpu.h"
#include "cpu/Serial.h"
#include "Screen.h"
#include "helper.h"
#include "cpu/memory.h"


void setup()
{
    hardware::initializePins();
    cpu::init();

    hardware::initialize();
    Time::initialize();
    SMPS::initialize();
    Discharger::initialize();
    AnalogInputs::initialize();
    Serial::initialize();

#ifdef ENABLE_STACK_INFO
    StackInfo::initialize();
#endif

    Settings::load();
    Screen::initialize();

    Screen::runWelcomeScreen();
}


int main()
{
    setup();
#ifdef ENABLE_HELPER
    helperMain();
#else
    eeprom::check();
    MainMenu::run();
#endif
}
