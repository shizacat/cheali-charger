/*
    cheali-charger - open source firmware for a variety of LiPo chargers
    Copyright (C) 2013  Pawel Stawicki. All right reserved.

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

#include "cpu/memory.h"
#include "Utils.h"

#include "Screen.h"
#include "ScreenStartInfo.h"
#include "ScreenCycle.h"
#include "ScreenBalancer.h"
#include "ScreenMethods.h"
#include "ScreenEditable.h"
#include "Balancer.h"

namespace Screen { namespace Pages {

/*condition bits:
 * 0..8:    PAGE_PROGRAM(..)
 * 9..16:   PAGE_BATTERY(CLASS)
 * 29:      PAGE_START_INFO
 * 30:      PAGE_BALANCE_PORT
*/
    struct PageInfo {
        VoidMethod displayMethod;
        uint32_t conditionEnable;
        uint32_t conditionDisable;
    };

    const PageInfo pageInfo[] PROGMEM = {
            {Screen::StartInfo::displayStartInfo,   PAGE_START_INFO,                      PAGE_NONE},
            {Screen::Editable::displayLEDScreen,     PAGE_BATTERY(ProgramData::ClassLED), PAGE_START_INFO},
            {Screen::Methods::displayFirstScreen,   PAGE_ALWAYS, PAGE_START_INFO + PAGE_PROGRAM(Program::Balance)},
            {Screen::Cycle::displayCycles,          PAGE_PROGRAM(Program::CapacityCheck)+PAGE_PROGRAM(Program::DischargeChargeCycle), PAGE_START_INFO},

            {Screen::Methods::displayDeltaFirst,    PAGE_BATTERY(ProgramData::ClassNiXX), PAGE_START_INFO + PAGE_PROGRAM(Program::Discharge)},
            {Screen::Methods::displayDeltaVout,     PAGE_BATTERY(ProgramData::ClassNiXX), PAGE_START_INFO + PAGE_PROGRAM(Program::Discharge)},
            {Screen::Methods::displayDeltaTextern,  PAGE_BATTERY(ProgramData::ClassNiXX), PAGE_START_INFO},
            {Screen::Methods::displayEnergy,        PAGE_ALWAYS, PAGE_START_INFO + PAGE_PROGRAM(Program::Balance)},

            {Screen::Balancer::displayVoltage1_3,   PAGE_START_INFO + PAGE_BALANCE_PORT , PAGE_NONE},
            {Screen::Balancer::displayVoltage4_6,   PAGE_START_INFO + PAGE_BALANCE_PORT , PAGE_NONE},
BALANCER_PORTS_GT_6(
            {Screen::Balancer::displayVoltage7_9,   PAGE_START_INFO + PAGE_BALANCE_PORT , PAGE_NONE},)

            {Screen::Balancer::displayResistance1_3,PAGE_BALANCE_PORT, PAGE_START_INFO + PAGE_PROGRAM(Program::Balance)},
            {Screen::Balancer::displayResistance4_6,PAGE_BALANCE_PORT, PAGE_START_INFO + PAGE_PROGRAM(Program::Balance)},
BALANCER_PORTS_GT_6(
            {Screen::Balancer::displayResistance7_9,PAGE_BALANCE_PORT, PAGE_START_INFO + PAGE_PROGRAM(Program::Balance)},)

            {Screen::Methods::displayR,             PAGE_ALWAYS, PAGE_START_INFO + PAGE_PROGRAM(Program::Balance)},

            {Screen::Methods::displayTime,          PAGE_ALWAYS, PAGE_START_INFO},
            {Screen::Methods::displayTemperature,   PAGE_ALWAYS, PAGE_NONE},
            {Screen::Methods::displayCIVlimits,     PAGE_ALWAYS, PAGE_PROGRAM(Program::Balance)},
            {Screen::Methods::displayVinput,        PAGE_ALWAYS, PAGE_NONE},

            {NULL, PAGE_ALWAYS, PAGE_NONE}
    };

}};
