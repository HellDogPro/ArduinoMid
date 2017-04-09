//
// Created by Angel Zaprianov on 8.2.2017 г..
//

#ifndef ARDUINO_MID_LCD240X64_H
#define ARDUINO_MID_LCD240X64_H
//#include "CarGames.h"
#include <Arduino.h>
#include "../conf.h"
#include "MainFunc.h"
#include "Menu16x2.h"
#include "ShutDw.h"
#include "CarState.h"
#include <U8g2lib.h>
#include "Menu240x64.h"
#include "graphics/240x64-logo.h"

#ifndef _U8G2LIB_HH
// Some IDE syntax mishmash fixer
#include "../../libraries/U8g2/src/U8g2lib.h"

#endif

//
// Drowing counter
uint8_t drawEntry = 0;

class Lcd240x62 : virtual public LcdMenuInterface {


    U8G2 *lcd;
    IntAmp *amp;
    CarSens *car;
    EepRom *eep;
    CarState *stt;
    WhlSens *whl;
    MenuBase *mbs;
    ShutDw *sdw;


protected:

    //
    // Defining content generate container variables
    char displayChar_2[2];
    char displayChar_3[3];
    char displayChar_4[4];

    void menus(uint8_t);

    void aniHrzChar(u8g2_uint_t x, u8g2_uint_t y, const char *str) {
        lcd->drawUTF8(aniIndex * 3, 36, str);
    }

    void prepareScreen() {
        // default u8g2_font_6x10_tf
        lcd->setFont(u8g2_font_6x10_tf);
        lcd->setFontRefHeightExtendedText();
        lcd->setDrawColor(1);
        lcd->setFontPosTop();
        lcd->setFontDirection(0);
    }

/**
 *
 * @param index of loop
 */
    void displayEntry(uint8_t index) {
        if (drawEntry > 10) {
            drawEntry = 0;
        }

        lcd->drawStr(0, 1, "MENU CHANGE ");
//        lcd->drawBox(0, 0, 240, 14);

        switch (index) {

            case 0:
                lcd->clear();
                mbs->startEntry();
                drawEntry = 0;
                btnMenu.setNavigationState(false);
                tone(TONE_ADT_PIN, 2800, 16);
                break;
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:

                lcd->drawFrame(0, 27, 78, 15);

                lcd->drawStr(10, 15 - (index * 2), usedMenu.back);
                lcd->drawStr(10, 30 - (index * 2), usedMenu.used);
                lcd->drawStr(10, 45 - (index * 2), usedMenu.next);
                lcd->drawStr(10, 45 - (index * 2), usedMenu.last);

                lcd->drawStr(78, 30, usedMenu.down);
//                lcd->drawStr(0, 15 + (index * 2), ">");
                lcd->drawStr(4, 30, ">");


                break;
            case 9:
                lcd->clearBuffer();
                lcd->clear();
                break;
            case 10:

                btnMenu.setNavigationState(true);
                usedMenu = {};
//                free(usedMenu);
                mbs->finishEntry();
                lcd->clear();
                break;
        }


    }

public:
/**
 *
 * @param _lcd
 * @param _btn
 * @param _mbs
 * @param _sdw
 */
    Lcd240x62(U8G2 *_lcd, MenuBtn *_btn, MenuBase *_mbs, ShutDw *_sdw) {
        lcd = _lcd;
        amp = _btn->passAmp();
        car = _btn->passCar();
        eep = _btn->passEep();
        whl = _btn->passWhl();
        stt = _btn->passStt();
        sdw = _sdw;

    }

    void intro(void) {
        //
        // Test tone
        tone(TONE_ADT_PIN, 400, 20);
        delay(10);
        lcd->firstPage();
        do {
//            lcd->drawXBM(49, 16, 142, 33, OpelLogoBits);
            lcd->drawXBMP(0, 0, 240, 64, OpelLogoBits);
        } while (lcd->nextPage());

        delay(1500);
        lcd->clear();
/*//        tone(TONE_ADT_PIN, 800, 10);
//        delay(10);
//        lcd->firstPage();
//        do {
//            lcd->drawXBMP(51, 20, 137, 25, ArduinoLogoBits);
//        } while (lcd->nextPage());
//        lcd->sendBuffer();
//        tone(TONE_ADT_PIN, 800, 10);
//        delay(1500);
//        lcd->clear();
//        tone(TONE_ADT_PIN, 800, 10);
//        delay(10);
//        lcd->firstPage();
//        do {
//            lcd->drawXBMP(68, 2, 104, 60, Fire1LogoBits);
//        } while (lcd->nextPage());
//        lcd->sendBuffer();
//        tone(TONE_ADT_PIN, 800, 10);
//        delay(1500);
//        delay(10);*/
        tone(TONE_ADT_PIN, 800, 10);
    }

    void begin(void) {
        lcd->begin();
        prepareScreen();
        lcd->enableUTF8Print();

    }

/**
 * Draw graphic
 */
    void draw() {
        if (amp->isMid()) {
            lcd->firstPage();
            do {
                menus(drawEntry);
            } while (lcd->nextPage());
            drawEntry++;
        }
    }

private:

    float getConsumedFuel() {
        //
        // Load saved data
        SavedData data = eep->getData();
        //
        // Switching between LPG / BNZ
        if (car->getFuelState() == 0) { // BNZ [default]
            return data.fuel_def + car->getDefFuelCns();
        }
        if (car->getFuelState() == 1) { // LPG [additional]
            return data.fuel_adt + car->getAdtFuelCns();
        }
    }

    unsigned int aniIndex;

    void setTextMode(void) {
        //
        // u8g2_font_4x6_tf, u8g2_font_5x7_tf
        lcd->setFont(u8g2_font_6x10_tf);
        lcd->setFontRefHeightExtendedText();
        lcd->setDrawColor(1);
        lcd->setFontPosTop();
        lcd->setFontDirection(0);
    }

    void setCharMode(void) {
        lcd->drawStr(0, 0, "Unicode");
        // u8g_font_04b_03b
        lcd->setFont(u8g2_font_unifont_t_symbols);
        lcd->setFontPosTop();
    }

/**
 * Displays consumed fuel
 */
    void displayTotalConsumption() {
        lcd->drawStr(2, 15, "Consumption: ");
        displayFloat(getConsumedFuel(), displayChar_3);
        lcd->drawStr(150, 20, displayChar_3);
    }

/**
 * Displays average consumption
 */
    void displayAverageConsumption() {
        lcd->drawStr(2, 30, "Average: ");
        displayFloat(((eep->getData().dist_trv + car->getDst()) / getConsumedFuel()), displayChar_3);
        lcd->drawStr(11, 30, displayChar_3);
        lcd->drawStr(140, 30, "L/100km");
    }

    void displayInsideTemperature() {
        lcd->drawStr(2, 45, "Temperatures: ");
        displayFloat(car->getTmpIns(), displayChar_3);
        lcd->drawStr(16, 30, displayChar_3);
        lcd->drawUTF8(190, 30, "℃");
    }

};

/**
 *
 * @param index
 */
void Lcd240x62::menus(uint8_t index) {
    switch (MidCursorMenu) {
        default:
        case MENU_ENTRY:
            displayEntry(index);
            break;
            //
            // Main / first menu
        case 1:
            lcd->drawStr(0, 0, "HOME MENU");
            displayInsideTemperature();
            displayTotalConsumption();
            displayAverageConsumption();
            break;
            //
            // Dashboard
        case 11:
            lcd->drawStr(0, 0, "Dashboard");
            break;

        case 12:
            lcd->drawStr(0, 0, "Menu");
            break;

            //
            // Travel menu
        case 2:
            lcd->drawStr(0, 0, "Travel menu");
            break;
        case 21:
            lcd->drawStr(0, 0, "Sub Travel");
            break;
        case 22:
            lcd->drawStr(0, 0, "Sub Travel");
            break;

            //
            // Fuel menu
        case 3:
            lcd->drawStr(0, 0, " Fuel menu");
            break;
        case 32:
            lcd->drawStr(0, 0, " Sub Fuel");
            break;
        case 4:
        case 41:
        case 42:
        case 43:
        case 44:
        case 45:
            lcd->drawStr(0, 0, "Servicing ");
            break;
/*            //
            // Games menu
        case 5:

            break;
        case 51:
            gms->listenAStopwatch();

            break;
        case 52:

            break;
        case 53:
            gms->listen0to100();

            break;*/

        case ShutDw::MENU_SHUTDOWN:
            lcd->drawStr(0, 0, "Shut down");
            break;
    }

}


#endif //ARDUINOMID_LCD240X64_H
