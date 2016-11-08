//
// Created by Angel Zaprianov on 9/17/2016.
//
#include <OneWire.h>
#include <DallasTemperature.h>


#ifndef ARDUINOMID_TMPSENS_H
#define ARDUINOMID_TMPSENS_H

//#define  DEBUG_TEMPERATURE_OU
//#define  DEBUG_TEMPERATURE_IN

// Data wire is plugged into pin A7 on the Arduino
#define ONE_WIRE_BUS 7

OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature temperatureSensors(&oneWire);


bool isInitTemperature = 1;

float CUR_OUT_TMP = 0;
float CUR_INS_TMP = 0;

float getTmpOut() {
    return CUR_OUT_TMP;
}

float getTmpIns() {
    return CUR_INS_TMP;
}

void setupTemperature() {
    temperatureSensors.begin();
}
//
// balance/pad resistor value, set this to
// the measured resistance of your pad resistor
float pad = 5000;
//
//
float thermistor(int RawADC) {
    long Resistance;
    float Temp;  // Dual-Purpose variable to save space.

    Resistance = long(pad * ((1024.0 / RawADC) - 1));
    Temp = log(Resistance); // Saving the Log(resistance) so not to calculate  it 4 times later
    Temp = 1 / (0.001129148 + (0.000234125 * Temp) + (0.0000000876741 * Temp * Temp * Temp));
    Temp = Temp - 273.15;  // Convert Kelvin to Celsius
    return Temp;
}


/**
 * Temperature sensor
 *      new DATA:
 *      Temperature range to [°C]: 250
 *      Resistance [Ohm]: 5000
 */
void sensTmp() {

#if defined(DEBUG_TEMPERATURE_IN)
    if (ampInt.isBig()) {
        temperatureSensors.requestTemperatures();
        Serial.print("Dallas temperature: \t");
        Serial.println(temperatureSensors.getTempCByIndex(0)); // Why "byIndex"?
    }
#endif

    if (ampInt.isBig()) {
        temperatureSensors.requestTemperatures();
        CUR_INS_TMP = temperatureSensors.getTempCByIndex(0);
    }
//    temperatureSensors.requestTemperatures();
//    temperatureSensors.getTempCByIndex(0);

    float temperatureC;
    //
    // 141 = 22* ??
    // 120 = 28*
    // 119 = 29*
    // 116 = 33* ???
    // 180 = 16*
    // 197 = 14*
    // 202 = 13*
    //~~~~~~~~~~~~~~~~
    // 131 = 27.00
    //
    //
    // New measurement
    // --------------------------------------------
    // 147 = 21*
    // 140/139 = 22 / 21
    // 139 =  25/6
    // 118 = 27/8
    if (isInitTemperature || ampInt.isBig()) {
        //
        // Read new data
        int reading = analogRead(TMP_PIN_OUT);

        //      255 max reading
        //      4.34 is voltage passes temperature sensor
        float cofVolt = /*3.8*/ 5 ;

        // not correct
        /// new type  id: (147 / 2.666666 - 76) *1
        // min -40	max 215	°C	 {formula A-40}
        // separate reading
        temperatureC = ((reading / cofVolt) - (250 / cofVolt)) * -1;
        //
        //
//        temperatureC = temperatureC;

#if defined(DEBUG_TEMPERATURE_OU)
        if (ampInt.isMid()) {
            Serial.print("Read Temp: ");
            Serial.println(reading);
        }
#endif

        //
        // Close first loop
        isInitTemperature = 0;

        //
        // Pass value to global
        CUR_OUT_TMP = temperatureC;
    }

}

#endif //ARDUINOMID_TMPSENS_H