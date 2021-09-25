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
#include "ProgramMenus.h"
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
#include "Balancer.h"
#include "SettingsMenu.h"
#include STRINGS_HEADER
#include "cpu/atomic.h"
#include "cpu/memory.h"
#include "LcdPrint.h"
#include "Menu.h"

namespace BalancePortAnalyzer {

STRING_CPP(v_menu_cell0,    "Vb0_pin: ");
STRING_CPP(v_menu_cell1,    "Vb1_pin: ");
STRING_CPP(v_menu_cell2,    "Vb2_pin: ");
STRING_CPP(v_menu_cell3,    "Vb3_pin: ");
STRING_CPP(v_menu_cell4,    "Vb4_pin: ");
STRING_CPP(v_menu_cell5,    "Vb5_pin: ");
STRING_CPP(v_menu_cell6,    "Vb6_pin: ");
#if MAX_BALANCE_CELLS > 6
STRING_CPP(v_menu_cell7,    "Vb7_pin: ");
STRING_CPP(v_menu_cell8,    "Vb8_pin: ");
#endif
STRING_CPP(v_menu_output_plus, "V+_pin:  ");
STRING_CPP(v_menu_output_minus,"V-_pin:  ");
STRING_CPP(v_menu_settings, "settings");

const char * const voltageMenu[] PROGMEM = {string_v_menu_cell0, string_v_menu_cell1, string_v_menu_cell2,
        string_v_menu_cell3, string_v_menu_cell4, string_v_menu_cell5, string_v_menu_cell6,
        BALANCER_PORTS_GT_6( string_v_menu_cell7, string_v_menu_cell8,)
        string_v_menu_output_plus, string_v_menu_output_minus, string_v_menu_settings, NULL};

const AnalogInputs::Name voltageName[] PROGMEM = {
       AnalogInputs::Vb0_pin,
       AnalogInputs::Vb1_pin,
       AnalogInputs::Vb2_pin,
       AnalogInputs::Vb3_pin,
       AnalogInputs::Vb4_pin,
       AnalogInputs::Vb5_pin,
       AnalogInputs::Vb6_pin,
       BALANCER_PORTS_GT_6(AnalogInputs::Vb7_pin, AnalogInputs::Vb8_pin,)
       AnalogInputs::Vout_plus_pin,
       AnalogInputs::Vout_minus_pin,
};

static const uint8_t dig_ = 5;

static void printItem(uint8_t index) {
    lcdPrint_P(voltageMenu, index);

    AnalogInputs::Name name = pgm::read(&voltageName[index]);
    if(index < MAX_BALANCE_CELLS +3) {
            lcdPrintUnsigned(AnalogInputs::getADCValue(name), dig_);

        if(index > 0) {
            if((Balancer::balance >> (index-1)) & 1)
                lcdPrintChar('*');
        }
    }
}


void run() {
    SerialLog::powerOn();
    AnalogInputs::powerOn();
    Balancer::powerOn();

    Menu::initialize(sizeOfArray(voltageMenu));
    Menu::printMethod_ = printItem;
    uint8_t index = 0;
    do {
        Menu::setIndex(index);
        index = Menu::run(true);
        if(index < 1) continue;
        if(index < MAX_BALANCE_CELLS+1) {
            Balancer::setBalance(Balancer::balance ^ (1<<(index-1)));
        }
        if(index == MAX_BALANCE_CELLS + 3) {
            Balancer::powerOff();
            AnalogInputs::powerOff();
            SerialLog::powerOff();
            SettingsMenu::run();
            SerialLog::powerOn();
            AnalogInputs::powerOn();
            Balancer::powerOn();
        }
    } while(true);
}

} //namespace BalancePortAnalyzer

