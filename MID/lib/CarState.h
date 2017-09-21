//
// Created by Angel Zaprianov on 29.11.2016 г..
//

#ifndef ARDUINO_MID_CAR_STATE_H
#define ARDUINO_MID_CAR_STATE_H

#include "AmpTime.h"

#ifndef CAR_STT_AC_ALERT
#define CAR_STT_AC_ALERT 3
#endif

#ifndef CAR_STT_TM_BELT
#define CAR_STT_TM_BELT 85000
#endif

#ifndef CAR_STT_AR_FILTER
#define CAR_STT_AR_FILTER 40000
#endif


struct Diagnostic {
    boolean all;    // All combined
    boolean oil;    // oil level
    boolean cnt;    // Coolant level
    boolean win;    // Window washer level
    boolean brk;    // Brake ware
    boolean vol;    // Car Voltage
    boolean blt;    // Belt ware change
    boolean la1;    // 1 Incandescent lamps
    boolean la2;    // 2 incandescent lamps
    boolean wnt;    // Winter warning
    boolean ovh;    // Overheating warning
};

/**
 *
 */
class CarState {

private:
    Diagnostic result;

    AmpTime *amp;
    CarSens *car;
    int lastVoltageValue = 0;
    float workDistance;

    uint8_t alertState = 0;


    boolean isBadVoltage();

    uint8_t pinOil, pinCnt, pinWin, pinBrk, pinVol, code = 0b1000000;
    uint8_t cursorMenu = 1;
    uint8_t lastUserWarn = 0;

    boolean userWarnStack[11];


/**
 *
 */
    boolean in_state(const uint8_t index) {
        return userWarnStack[index];
    }

    boolean add_state(const uint8_t index, const boolean value) {
        lastUserWarn = index;
        userWarnStack[index] = value;
    }

public:
    static constexpr uint8_t MENU_SERVICE = 101;

    /**
 * Construction Car State class
 * @param amp
 */
    CarState(AmpTime &_amp, CarSens &_car) : amp(&_amp), car(&_car) { }

    void setWorkState(float distance);

    boolean isAlert();

    /**
     * Display warning screen
     */
    void menu(LcdUiInterface *lcd) {


        if (result.oil) {
            lcd->warnMotorOil();
            add_state(1, true);
        }
        else if (result.cnt) {
            lcd->warnCoolant();
            add_state(2, true);
        }
        else if (result.win) {
            lcd->warnWasher();
            add_state(3, true);
        }
        else if (result.brk) {
            lcd->warnBreakWare();
            add_state(4, true);
        }
        else if (result.vol) {
            lcd->warnBattery(this->getVoltage());
            add_state(5, true);
        }
        else if (result.la1) {
            lcd->warnLightsFront();
            add_state(6, true);
        }
        else if (result.la2) {
            lcd->warnLightsBack();
            add_state(7, true);
        }
        else if (result.blt) {
            lcd->warnTmBelt();
            add_state(8, true);
        }
        else if (result.wnt) {
            lcd->warnWinter();
            add_state(9, true);
        }
        else if (result.ovh) {
            lcd->warnOverheat();
            add_state(10, true);
        }
        else;


        if (amp->is4Seconds()) {
            MidCursorMenu = cursorMenu; // return user to last usable screen
        }
    }


    /**
     *
     * @return int Voltage
     */
    float getVoltage(void);

    boolean getLiveOil();

    boolean getLiveCnt();

    boolean getLiveWin();

    boolean getLiveBrk();

    boolean getLiveVol();

/**
 *
 * @param pinO
 * @param pinC
 * @param pinW
 * @param pinB
 * @param pinV
 */
    void setup(uint8_t pinO, uint8_t pinC, uint8_t pinW, uint8_t pinB, uint8_t pinV);

    void listener();

/**
 *
 */
    void cursor() {
        if (amp->is4Seconds() && this->isAlert() && !lastUserWarn) {
            cursorMenu = MidCursorMenu;
            MidCursorMenu = MENU_SERVICE;
        }
    }

    boolean isWinter() {
        if (car->getTmpOut() < -3) {
            return true;
        }
        return false;
    }

    boolean isOverhead() {
        if (car->getEngTmp() > 98) {
            return true;
        }
        return false;
    }

    Diagnostic getResult();
};


/**
 *
 * @param pinO
 * @param pinC
 * @param pinW
 * @param pinB
 * @param pinV
 */
void CarState::setup(uint8_t pinO, uint8_t pinC, uint8_t pinW, uint8_t pinB, uint8_t pinV) {
    //
    // Sets mode
    pinMode(pinO, INPUT_PULLUP);
    pinMode(pinC, INPUT_PULLUP);
    pinMode(pinW, INPUT_PULLUP);
    pinMode(pinB, INPUT_PULLUP);
    pinMode(pinV, INPUT);

    pinOil = pinO;
    pinCnt = pinC;
    pinWin = pinW;
    pinBrk = pinB;
    pinVol = pinV;


}

/**
 * Readings states
 */
void CarState::listener() {
    if (amp->is10Seconds()) {
        result.oil = (boolean) digitalRead(pinOil);
        result.brk = (boolean) digitalRead(pinBrk);
        result.cnt = (boolean) digitalRead(pinCnt);
        result.win = (boolean) digitalRead(pinWin);
        result.vol = isBadVoltage();

        result.wnt = isWinter();
        result.ovh = isOverhead();
        //
        // Car servicing
        if (workDistance > CAR_STT_TM_BELT) { // Timing belt change
            result.blt = true;
        }

        //
        // Car state
        if (result.oil || result.brk || result.cnt || result.win || result.vol || result.wnt || result.ovh) {
            alertState++;
        }
    }
    //
    // Handle menu cursor
    cursor();
}

/**
 *
 * @return
 */
boolean CarState::isBadVoltage(void) {

    int readingVoltage = analogRead(pinVol);
    //
    // Voltage too high
    if (lastVoltageValue > 0 && lastVoltageValue == readingVoltage && readingVoltage > 990) {
        return true;
    }
    //
    // Voltage too low
    if (lastVoltageValue > 0 && lastVoltageValue == readingVoltage && readingVoltage < 850) {
        return true;
    }
    //
    // Save last reading
    if (readingVoltage > 0 && lastVoltageValue != readingVoltage) {
        lastVoltageValue = readingVoltage;
    }
    //
    // Voltage is good
    return false;
}

/**
 * Sets work distance from EepRom for calculation
 * @param distance
 */
void CarState::setWorkState(float distance) {
    workDistance = distance;
}

/**
 * Gets live Oil level
 * @return boolean
 */
boolean CarState::getLiveOil() {
    return (boolean) digitalRead(pinOil);
}

/**
 * Gets live Window Wash level
 * @return boolean
 */
boolean CarState::getLiveCnt() {
    return (boolean) digitalRead(pinCnt);
}

/**
 * Gets live Window Wash level
 * @return boolean
 */
boolean CarState::getLiveWin() {
    return (boolean) digitalRead(pinWin);
}

/**
 * Gets live Brake status
 * @return boolean
 */
boolean CarState::getLiveBrk() {
    return (boolean) digitalRead(pinBrk);
}

/**
 * Gets live voltage
 * @return boolean
 */
boolean CarState::getLiveVol() {
    return isBadVoltage();
}

/**
 *  Gets car voltage
 * @return integer
 */
float CarState::getVoltage(void) {
    return analogRead(pinVol) / 67;
}

/**
 * Gets diagnostic result
 * @return Diagnostic
 */
Diagnostic CarState::getResult() {
    return result;
}

/**
 * Is car have some issue with simple verification
 * @return boolean
 */
boolean CarState::isAlert() {

    if (alertState >= CAR_STT_AC_ALERT) {
        //
        // Check for older state
        if (in_state(lastUserWarn)) {
            alertState = 0;
            return false;
        }
        return true;
    }


    return false;


}

#endif //ARDUINOMID_CARSTAT_H
