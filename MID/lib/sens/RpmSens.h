//
// Created by Admin on 7/23/2016.
//

#ifndef ARDUINOMID_RpmSens_H
#define ARDUINOMID_RpmSens_H

//
// Sensor configs
const bool RpmSensDebug = 0;
const int RpmCorrection = 50;

//
// Rpm Container
int CUR_RPM = 0;

//
// Working vars
int rpmHitsCount = 0;
int rpmCycles = 0;
int long rpmTimerStart = 0, rpmTimerEnds = 0;

/**
 * Callback attachInterrupt
 */
void catchRpmHits () {
//  rpmHitsCount++;
}

/** deprecated
 * Setup Rpm
 */
void setupRpmSens (int pinTarget) {
  //
  // Pin mode ...
//  pinMode (pinTarget, INPUT_PULLUP);
  //
  // Convert to interrupt pin
//  attachInterrupt (digitalPinToInterrupt (pinTarget), catchRpmHits, FALLING);
}

/**
 * Gets current RPM
 */
int getRpmSens () {
  return CUR_RPM;
}


/**
 * Detect RPMs
 */
void sensRpm () {
  rpmTimerEnds = millis ();
  if (rpmTimerEnds >= (rpmTimerStart + 600))
	{
	  //
	  // Handle cycles
	  rpmTimerStart = rpmTimerEnds;
	  //
	  // Pass rpm to global
	  CUR_RPM = rpmHitsCount * RpmCorrection;

	  //
	  // debug info
	  if (RpmSensDebug)
		{
		  Serial.print ("\n");
		  Serial.print (" RPM count:  \t");
		  Serial.print (rpmHitsCount);
		  Serial.print (" RPM is:  \t");
		  Serial.print (rpmHitsCount * 450);
		  Serial.print ("\n");
		}
	  //
	  // Clear value
	  rpmHitsCount = 0;
	}
}


#endif //ARDUINOMID_RpmSens_H
