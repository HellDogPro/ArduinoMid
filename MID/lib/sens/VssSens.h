//
// Created by Angel Zaprianov on 27.6.2016 г..
//

#ifndef ARDUINOMID_VssSens_H
#define ARDUINOMID_VssSens_H
//
// Sensor configs
const bool VssSensDebug = 1;
const int VssCorrection = 1.6;
//
// Rpm Container
int CUR_VSS = 0;
//
// Working vars
int vssHitsCount = 0;
int long vssTimerStart = 0, vssTimerEnds = 0;
int vssTimeHits = 0;

/**
 * Callback attachInterrupt
 */
void catchVssHits () {
  vssHitsCount++;
}
/**
  * Setup Vss
 */
void setupVssSens (int pinTarget) {
  pinMode (pinTarget, INPUT_PULLUP);
  attachInterrupt (digitalPinToInterrupt (pinTarget), catchVssHits, FALLING);
}
/**
 * Gets current Vss
 */
 int getVssSens () {
   return  CUR_VSS;
}
/**
 * Detect Vss
 */
void sensVss(){

  vssTimerEnds = millis ();
  if (vssTimerEnds >= (vssTimerStart + 150))
	{
	  //
	  // Handle cycles
	  vssTimerStart = vssTimerEnds;
	  //
	  // Pass vss to global
	  CUR_VSS = vssHitsCount * VssCorrection;
	  //
	  // debug info
	  if (VssSensDebug)
		{
		  Serial.print ("\n");
		  Serial.print (" vss count:  \t");
		  Serial.print (vssHitsCount);
		  Serial.print (" vss is:  \t");
		  Serial.print (vssHitsCount * 1.6);
		  Serial.print ("\n");
		}

	  vssHitsCount = 0;
	}

}

#endif //ARDUINOMID_VssSens_H
