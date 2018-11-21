
// Unlock method. shouldIRotate is simply to used for the purpose of double ensuring that the lock should rotate.
void Unlock(bool shouldIRotate)
{
  if (shouldIRotate == true)
  {
	// unlockDegree can be found in the defines.h. It is the degree at which the lock os fully unlocked
	// This may differ from lock to lock, and does require some testing to find
    servo.write(unlockDegree);
	// The follo 3 lines is for debugging purpose if you wish to follow along in the Arduino serial window
    Serial.print("Rotated to ");
    Serial.print(unlockDegree);
    Serial.println( " degrees -> Unlocked");
	
	// hasBeenUnlocked changes to true, in order to validate whether or not it should be unlocked later on
    hasBeenUnlocked = true;
  }
}

// Lock method, shouldIRotate serves same purpose as in Unlock
void Lock(bool shouldIRotate)
{
  if (shouldIRotate == true)
  {
	// lockDegree can be found in the defines.h.
	// lockDegree is at what degree that the lock is in a locked position, 0 degrees in this setup
    servo.write(lockDegree);
	// The follo 3 lines is for debugging purpose if you wish to follow along in the Arduino serial window
    Serial.print("Rotated to ");
    Serial.print(lockDegree);
    Serial.println(" degrees -> Locked");
	// hasBeenUnlocked changes to false, in order to validate whether or not it should be unlocked later on
    hasBeenUnlocked = false;
  }
}


