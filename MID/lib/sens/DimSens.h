//
// Created by Angel Zaprianov on 5.8.2016 г..
//


#ifndef ARDUINOMID_DimSens_H
#define ARDUINOMID_DimSens_H


//
// Set up pins
//
// vars for resolve dim value
int backLightDefault = 16;       // value to dim display when car lights are off
int isGaugesActive = LOW;         // is car lights on
int unsigned backLightLevel = 0;  // resolved display dim

const int dimCorrection = 15;

static void handleBackLight(void);

/**
 * Setup pins of display dim
 */
void setupBackLight(int pinInputInstrumentValue, int pinOutputDisplayContrast) {
    pinMode(pinInputInstrumentValue, INPUT);
    pinMode(pinOutputDisplayContrast, OUTPUT);

    analogWrite(pinOutputDisplayContrast, backLightDefault);
    handleBackLight();
}

/**
 * Handle display dim
 */
const int numReadingsDim = 100;
int indexReadValDim = 0;
int lastReadingsDim[numReadingsDim];
int totalReadingDim = 0;
int lastReadValueDim = 0;

static void handleBackLight(void) {


    int dimReadVal = analogRead(DIM_PIN_VAL);

    backLightLevel = map(dimReadVal, 0, 1023, 0, 255);
    totalReadingDim = totalReadingDim - lastReadingsDim[indexReadValDim];
    lastReadingsDim[indexReadValDim] = dimReadVal;
    totalReadingDim = totalReadingDim + lastReadingsDim[indexReadValDim];



    indexReadValDim = indexReadValDim + 1;
    // if we're at the end of the array...
    if (indexReadValDim >= numReadingsDim) {
        // ...wrap around to the beginning:
        indexReadValDim = 0;
    }

    if (backLightLevel < 15) {
        backLightLevel = backLightDefault;
    }

    if (lastReadValueDim != backLightLevel && backLightLevel > 0) {
        lastReadValueDim = backLightLevel;
        analogWrite(DIM_PIN_OUT, backLightLevel - dimCorrection);
    }
}


#endif //ARDUINOMID_DimSens_H
