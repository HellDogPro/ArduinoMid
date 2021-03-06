//
// Created by Angel Zaprianov on 14.2.2017 г..
//

#ifndef ARDUINO_MID_MENU_BTN_H
#define ARDUINO_MID_MENU_BTN_H

#include "AmpTime.h"
#include "WhlSens.h"
#include "CarSens.h"
#include "EepRom.h"
#include "CarState.h"
#include "InitObj.h"


//#define BUTTONS_DEBUG

#ifndef BTN_DEBOUNCE_FAST
#define BTN_DEBOUNCE_FAST 100
#endif

#ifndef BTN_DEBOUNCE_RATE
#define BTN_DEBOUNCE_RATE 350
#endif

#ifndef AWAITING_HOLD_BTN
#define AWAITING_HOLD_BTN 2000
#endif


class MenuBtn {

    AmpTime *amp;
    CarSens *car;
    EepRom *eep;
    WhlSens *whl;
    CarState *stt;

private:

    boolean
            isNavigationActive = true,
            playSecondTone = false,
            activateSteering = false,
            isHoldState = false,
            entryDownState = false,
            editorActivate = false,
            togetherPress = false;


    uint8_t btnUp, btnDw, btnBk, pinTn;
    uint8_t lastButtonPushed = 0;
    uint8_t debounceRate = BTN_DEBOUNCE_RATE / 10;
    const uint8_t btnMn = 200;

    unsigned long holdTimeHandler = 0;
    unsigned long lastUsed = 0;

    float controlledValue;


    void shortcut(void);


    void valueControl() {
        if (this->isOk()) {
            controlledValue = controlledValue + 1;
        }
        if (this->isNo()) {
            controlledValue = controlledValue - 1;
        }
    }

    void startup() {

        //
        // Startup reset trip
        if (millis() < 3000) {
            if (isBk()) {

                if (isDw()) {
                    eep->saveResetData();
                }


            }
        }
    }


public:
    MenuBtn(AmpTime &_amp, CarSens &_car, EepRom &_eep, WhlSens &_whl, CarState &_stt) :
            amp(&_amp), car(&_car), eep(&_eep), whl(&_whl), stt(&_stt) {
    }

    void listener(void);

    void begin(uint8_t buttonPinUp, uint8_t buttonPinDw, uint8_t brakePedal, uint8_t pinTones);

    inline void useDebounceFast() {
        debounceRate = BTN_DEBOUNCE_FAST / 10;
    }

    inline void useDebounceNormal() {
        debounceRate = BTN_DEBOUNCE_RATE / 10;
    }

    inline uint16_t getDebounceRate() {
        return debounceRate * 10;
    }

    inline void clearLastButton() {
        lastButtonPushed = 0;
    }

    inline void setValueControlled(float value) {
        controlledValue = value;
        useDebounceFast();
    }


    inline float getValueControlled() {
        return controlledValue;
    }

    inline void resetStates() {
        this->setNavigationState(true);
        this->setEditorState(false);
    }

    /**
     * Activate or disable navigation
     */
    inline void setNavigationState(boolean state) {
        isNavigationActive = state;
    }

    inline void setEditorState(boolean state) {
        editorActivate = state;
    }

    inline boolean getNavigationState() {
        return isNavigationActive;
    }

     uint8_t getPinUp(void) {
        return btnUp;
    }

     uint8_t getPinDw(void) {
        return btnDw;
    }

     uint8_t getPinTn(void) {
        return pinTn;
    }

     uint8_t getLastBtn() {
        return lastButtonPushed;
    }

    inline boolean isMn() {
        return lastButtonPushed == btnMn;
    }

    inline boolean isUp() {
        return (!isNavigationActive) ? false : lastButtonPushed == btnUp;
    }

    inline boolean isDw() {
        return (!isNavigationActive) ? false : lastButtonPushed == btnDw;
    }

    inline boolean isHl() {
        return isHoldState;
    }

    inline boolean isOk() {
        return (isNavigationActive) ? false : lastButtonPushed == btnUp;
    }

    inline boolean isNo() {
        return (isNavigationActive) ? false : lastButtonPushed == btnDw;
    }

    inline boolean isEd() {
        return editorActivate;
    }

    inline boolean isTg() {
        return togetherPress;
    }

    inline boolean isBk() {
        return digitalRead(btnBk) == HIGH && car->getVss() == 0;
    }

    inline AmpTime *passAmp(void) {
        return amp;
    }

    inline CarSens *passCar(void) {
        return car;
    }

    inline EepRom *passEep(void) {
        return eep;
    }

    inline WhlSens *passWhl(void) {
        return whl;
    }

    inline CarState *passStt(void) {
        return stt;
    }

    //
    // TODO change BTN_DEBOUNCE_RATE to faster
    boolean lastUseDebounce() {
        if (millis() - lastUsed > getDebounceRate()) {
#if defined(BUTTONS_DEBUG) || defined(GLOBAL_SENS_DEBUG)
            if (amp->isSecond()) {
                Serial.println("Debounce is true");
            }

#endif
            return true;
        } else {
            return false;
#if defined(BUTTONS_DEBUG) || defined(GLOBAL_SENS_DEBUG)
            if (amp->isSecond()) {
                Serial.println("Debounce is false");
            }


#endif
        }
    }


    void captureUp(void) {
        if (!digitalRead(btnUp) == HIGH) {
            if (amp->isMid() && !digitalRead(btnUp) == HIGH) { // From isLow to isMid
                lastUsed = millis();
                lastButtonPushed = btnUp;
            }
        }
    }


    void captureDw(void) {

        //
        // Single press button
        if (!digitalRead(btnDw) == HIGH && !entryDownState || !digitalRead(btnDw) == HIGH && !isNavigationActive) {
            //
            // Clear noise
            if (amp->isMid() && !digitalRead(btnDw) == HIGH) { // From isLow to isMid
                lastUsed = millis();
                tone(TONE_ADT_PIN, 700, 20);
                entryDownState = true;
                playSecondTone = true;
                holdTimeHandler = lastUsed;
                lastButtonPushed = btnDw;
                whl->disable();
            }

        } else if (!digitalRead(btnDw) == LOW && entryDownState) {
            entryDownState = false;
            whl->enable();
        }

        if (amp->isBig() && playSecondTone) {
            tone(pinTn, 1100, 50);
            playSecondTone = false;
        }

    }

    void captureMn() {
        if (!digitalRead(btnDw) == HIGH && !digitalRead(btnUp) == HIGH) {
            if (!digitalRead(btnDw) == HIGH && !digitalRead(btnUp) == HIGH && amp->isMin()) {
                lastUsed = millis();
                lastButtonPushed = btnMn;
            }
        }
    }

    void captureHl(void) {
        //
        // Hold button detection
        if ((AWAITING_HOLD_BTN + holdTimeHandler) < millis() && (!digitalRead(btnDw)) == HIGH && entryDownState) {
            if (amp->isLow() && !digitalRead(btnDw) == HIGH) {
                //
                // Cut the method if shortcut is executed
                shortcut();
                holdTimeHandler = 0;
                isHoldState = true;
                entryDownState = false;
                activateSteering = true;
                whl->disable();
            }
        }

        //
        // Reactivate steering wheel buttons
        if (amp->isSecond() && activateSteering) {
            activateSteering = false;
            whl->enable();
            //
            // Preventing navigation when is deactivated
            if (isNavigationActive) {
                lastButtonPushed = btnUp;
            }
            tone(pinTn, 1300, 100);
        }

    }

    void captureTg() {
        togetherPress = false;
        if (!digitalRead(btnDw) == HIGH && !digitalRead(btnUp) == HIGH) {
            if (amp->isLow()) {
                togetherPress = true;
                lastUsed = millis();
            }
        }
    };

    void captureEd() {

        //
        // LISTEN for Editor activation
        if (!digitalRead(btnDw) == HIGH && !digitalRead(btnUp) == HIGH && !entryDownState) {
            this->setNavigationState(false);
        }
        //
        // LISTEN for Editor deactivation
        if (editorActivate && this->isBk() && !this->getNavigationState()) {
            this->setNavigationState(true);
        }

        //
        // Manages given values
        if (editorActivate && !this->getNavigationState()) valueControl();
    }


};


void MenuBtn::begin(uint8_t buttonPinUp, uint8_t buttonPinDw, uint8_t brakePedal, uint8_t pinTones) {
    btnUp = buttonPinUp;
    btnDw = buttonPinDw;
    btnBk = brakePedal;
    pinTn = pinTones;
    //
    // Pin button mode
    pinMode(pinTn, INPUT);
    pinMode(btnUp, INPUT_PULLUP);
    pinMode(btnDw, INPUT_PULLUP);
    pinMode(btnBk, INPUT);

    //
    // Turn on  Internal pull up resistor
//    digitalWrite(btnUp, HIGH);
//    digitalWrite(btnDw, HIGH);
//    analogWrite(btnUp, 240);
//    analogWrite(btnDw, 240);

    digitalWrite(btnBk, LOW);

}


void MenuBtn::listener() {
    //
    // Delete hold state
    isHoldState = false;

//    Serial.print("Btn dw ");
//    Serial.println(digitalRead(btnDw));
//    Serial.print("Btn up ");
//    Serial.println(digitalRead(btnUp));
//    Serial.print("Break is ");
//    Serial.println(isBk());
    //
    // Debounce the buttons
    if (lastUseDebounce()) {

        //
        // Detect up state button
        captureUp();

        //
        // Detect down state button
        captureDw();

        //
        // Capture R+S
        captureMn();

        //
        // Provides editor futures
        captureEd();

        // TODO  make this hold as additional option
        // and other other waiting press after hold to activate shortcuts
    }
    //
    // Detect Hold button state
    captureHl();
    //
    // Detect together pressed
    captureTg();
    //
    //  Startup short cuts
    startup();

#if defined(BUTTONS_DEBUG) || defined(GLOBAL_SENS_DEBUG)
    if (amp->isMid()) {
        Serial.print("Break is: ");
        Serial.println(this->isBk());
        Serial.print("Up is: ");
        Serial.println(this->isUp());

        Serial.print("Dw is: ");
        Serial.println(this->isDw());
    }
    if (amp->isSecond()) {

        Serial.print(F("Last button is :"));
        Serial.println(lastButtonPushed);
        Serial.print(F("Navigation state :"));
        Serial.println(isNavigationActive);
        Serial.print(F("Editor active  :"));
        Serial.println(editorActivate);
        Serial.print(F("Controlled value  :"));
        Serial.println(controlledValue);

    }
#endif

}


void MenuBtn::shortcut(void) {

    /*********** [SHORTCUTS] *********** *********** *********** *********** START ***********/
    // Steering zero
    // Steering button is pressed
    if (whl->getCurrentState() == WhlSens::STR_BTN_ATT || isBk()) {
        tone(TONE_ADT_PIN, 1000, 10);
        delay(10);
        tone(TONE_ADT_PIN, 1000, 10);
        delay(10);

        tone(TONE_ADT_PIN, 2500, 10);
        delay(20);
        tone(TONE_ADT_PIN, 2500, 10);
        eep->saveResetData();
        delay(20);
        lastButtonPushed = 0;
    }
    // Steering SEEK UP
    // Change Speed alarm Up
    if (whl->getCurrentState() == WhlSens::STR_BTN_SKU) {
        car->speedingAlarmsUp();
        tone(TONE_ADT_PIN, 800, 50);
        delay(50);
        tone(TONE_ADT_PIN, 2000, 80);
        delay(80);
        lastButtonPushed = 0;
    }
    // Steering SEEK DOWN
    // Change Speed alarm Down
    if (whl->getCurrentState() == WhlSens::STR_BTN_SKD) {
        car->speedingAlarmsDw();
        tone(TONE_ADT_PIN, 2000, 50);
        delay(50);
        tone(TONE_ADT_PIN, 800, 80);
        delay(80);
        lastButtonPushed = 0;
    }

    if (millis() < (AWAITING_HOLD_BTN * 2)) {

    }

    /*********** [SHORTCUTS] *********** *********** *********** *********** END   ***********/



}

#endif //ARDUINOMID_BUTTONS_H
