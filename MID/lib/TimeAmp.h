//
// Created by Angel Zaprianov on 29.9.2016 г..
//
#include <Arduino.h>

#ifndef ARDUINOMID_TIMEAMP_H
#define ARDUINOMID_TIMEAMP_H


/** @description
 *
 */
class TimeAmp {

private:
    unsigned long curLow = 0, curSec = 0, curMid = 0, curMin = 0, curBig = 0, curMax = 0;
    long int ampLow = 0, ampSec = 0, ampMid = 0, ampMin = 0, ampBig = 0, ampMax = 0;
    long int _isLow = 0, _isSec = 0, _isMid = 0, _isMin = 0, _isBig = 0, _isMax = 0;
    /**
     * MAX 1,193,046 Hour	(h)
     */
    unsigned long loopCounter = 0;
public:
    TimeAmp(int intervalMin, int intervalLow, int intervalMid, int intervalSec, int intervalBig, int intervalMax);

    void listener();

    bool isLow() { return (boolean) _isLow; }

    bool isMin() { return (boolean) _isMin; }

    bool isSec() { return (boolean) _isSec; }

    bool isMid() { return (boolean) _isMid; }

    bool isBig() { return (boolean) _isBig; }

    bool isMax() { return (boolean) _isMax; }

    unsigned long getLoopIndex() { return loopCounter; }
};


/***********************************************************************************************
 *                                                                                             *
 *                                   CPP part of file                                          *
 *                                                                                             *
 ***********************************************************************************************/


TimeAmp::TimeAmp(int intervalMin, int intervalLow, int intervalMid, int intervalSec, int intervalBig, int intervalMax) {

    ampMin = intervalMin;
    ampLow = intervalLow;
    ampSec = intervalSec;
    ampMid = intervalMid;
    ampBig = intervalBig;
    ampMax = intervalMax;

}

void TimeAmp::listener() {

    unsigned long curTime = loopCounter;

    if (curTime >= curMin + ampMin) {
        curMin = curTime;
        _isMin = 1;
    } else {
        _isMin = 0;
    }

    if (curTime >= curLow + ampLow) {
        curLow = curTime;
        _isLow = 1;
    } else {
        _isLow = 0;
    }


    if (curTime >= curSec + ampSec) {
        curSec = curTime;
        _isSec = 1;
    } else {
        _isSec = 0;
    }

    if (curTime >= curMid + ampMid) {
        curMid = curTime;
        _isMid = 1;
    } else {
        _isMid = 0;
    }

    if (curTime >= curBig + ampBig) {
        curBig = curTime;
        _isBig = 1;
    } else {
        _isBig = 0;
    }

    if (curTime >= curMax + ampMax) {
        curMax = curTime;
        _isMax = 1;
    } else {
        _isMax = 0;
    }

    loopCounter++;
};
#endif //ARDUINOMID_TIMEAMP_H
