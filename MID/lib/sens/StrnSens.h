//
// Created by Angel Zaprianov on 5/10/2016.
//


#ifndef ARDUINO_MID_STR_SENS_H
#define ARDUINO_MID_STR_SENS_H

#include <SPI.h>

class StrButtonsSony {

private:

    uint8_t pinSteering, pinDigitalOut;

public:
    StrButtonsSony(uint8_t pinTargetSteering, uint8_t pinDigitalPod) {
        pinSteering = pinTargetSteering;
        pinDigitalOut = pinDigitalPod;
    }
    void setup();
    void listenButtons();

    /* @deprecated */
    float getVoltage(float volt) {
        return 255 * (volt / 3.80);
    }

};

void StrButtonsSony::setup() {
    pinMode(pinSteering, INPUT);
    pinMode(pinDigitalOut, OUTPUT);
//  pinMode (SPICCLOCK, OUTPUT);//Needed to be defined?
//  pinMode (SLAVESELECT,OUTPUT); //same as above?
// initialize SPI:
    SPI.begin();
    SPI.setDataMode(SPI_MODE0);//SPI_MODE0, SPI_MODE1, SPI_MODE2, or SPI_MODE3
    SPI.setBitOrder(MSBFIRST); // LSBFIRST or MSBFIRST

}

/*
 *          SONY buttons value
 *
                resistance	voltage
        src      	    2200	4.1
        att	            4400	3.47
        off	            35	    4.98
        vol down	    23850	1.47
        vol up	        17000	1.85
        seek down	    12000	2.25
        seek up	        8800	2.65
        back	        6660	3
        front	        33940	1.13
        bottom	        48800	0.84
*/

void StrButtonsSony::listenButtons() {

    int readingSteeringButton = analogRead(pinSteering);

    if (readingSteeringButton > 250) {
        SPI.transfer(0);
    }


    if (Serial.available()) {
//        mySerial.write(Serial.read());
//        sendCode(Serial.read());
//        analogWrite(ALP_PIN_OUT, getVoltage(Serial.read()));
    }

    //
    // Zero button
    if (readingSteeringButton > 25 && readingSteeringButton < 30) {
        Serial.print("Zero button");
        SPI.transfer(11);

    }

    //
    // Volume up
    if (readingSteeringButton > 14 && readingSteeringButton < 20) {
        SPI.transfer(21);

        if (ampInt.isMid()) {
            Serial.print("Volume Up");
            Serial.print("\t value:");
            Serial.print(getVoltage(1.85));
            Serial.print("\n");
        }
    }

    //
    // Volume down
    if (readingSteeringButton > 5 && readingSteeringButton < 14) {
        SPI.transfer(141);
        if (ampInt.isMid()) {
            Serial.print("Volume Down");
            Serial.print("\t value:");
            Serial.print(getVoltage(1.37));
            Serial.print("\n");
        }
    }


    //
    // Right arrow / seek up
    if (readingSteeringButton > 40 && readingSteeringButton < 50) {

        SPI.transfer(131);
        if (ampInt.isMid()) {
            Serial.print("Seek Up");
            Serial.print("\t value:");
            Serial.print(getVoltage(2.55));
            Serial.print("\n");
        }
    }

    //
    // Left arrow / seek down
    if (readingSteeringButton > 70 && readingSteeringButton < 80) {

        SPI.transfer(151);
        if (ampInt.isMid()) {
            Serial.print("Seek Down");
            Serial.print("\t value:");
            Serial.print(getVoltage(2.15));
            Serial.print("\n");
        }
    }


    //
    // Back button
    if (readingSteeringButton > 130 && readingSteeringButton < 140) {

        SPI.transfer(200);
        if (ampInt.isMid()) {
            Serial.print("Back Button");
            Serial.print("\t value:");
            Serial.print(getVoltage(2.95));
            Serial.print("\n");
        }
    }


}


#endif