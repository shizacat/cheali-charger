/*
    cheali-charger - open source firmware for a variety of LiPo chargers
    Copyright (C) 2016  Paweł Stawicki. All right reserved.

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
#include "ProgramMenus.h"
#include "OptionsMenu.h"
#include "MainMenu.h"
#include "Menu.h"
#include "ProgramData.h"
#include "LcdPrint.h"
#include "cpu/memory.h"

using namespace options;

namespace MainMenu {

    void printItem(uint8_t i) {
        if(i < 1) {
            lcdPrint_P(string_options);
        } else {
            ProgramData::printProgramData(i - 1);
        }
    }

    void run()
    {
        int8_t index = 0;
        while(true) {
            Menu::initialize(MAX_PROGRAMS + 1);
            Menu::printMethod_ = printItem;
            Menu::setIndex(index);
            index = Menu::run();

            if(index >= 0)  {
                if(index == 0) {
                    OptionsMenu::run();
                } else {
                    ProgramMenus::selectProgram(index - 1);
                }
            } else {
                index = 0;
            }
        }
    }

}



