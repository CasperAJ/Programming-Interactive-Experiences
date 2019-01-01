// Includes the defines.h so we can use the varibles and clas in it
#include "defines.h"

// Creates the intial setup of the system with all variables and methos initiated
void setup()
{
// set the WiFi chip to "promiscuous" mode aka monitor mode
  Serial.begin(115200);
  delay(10);
  wifi_set_opmode(STATION_MODE);
  wifi_set_channel(1);
  wifi_promiscuous_enable(DISABLE);
  delay(10);
  wifi_set_promiscuous_rx_cb(sniffer_callback);
  delay(10);
  wifi_promiscuous_enable(ENABLE);

  // setup the channel hoping callback timer
  os_timer_disarm(&channelHop_timer);
  os_timer_setfn(&channelHop_timer, (os_timer_func_t *) channelHop, NULL);
  os_timer_arm(&channelHop_timer, CHANNEL_HOP_INTERVAL_MS, 1);

///////////////////////////////////////////////////////////////////////////////
// ----------------------- My code ----------------------- 
  pinMode(D6, INPUT_PULLUP); // INPUT_PULLUP means that the pin is taking inputs
  
  // A standard for loop that increment i evertime it's runned, until i is equal to numberOfPresses (5)
  for (int i = 0; i < numberOfPresses; i ++)
  {
    timesPressed[i] = 0; // Filles the array with 0's
  }
  Serial.begin(115200); // Tell's the system to use the 115200 serial channel
  servo.attach(2);  // Connects the servo and assign it to pin D4
  servo.write(lockDegree); // Reset servo to locked position
}

void loop()
{
  delay(1);
  boolean buttonState = !digitalRead(D6); // Gives true when button is pressed/held down
  //Serial.println(buttonState);
  startTimer = millis(); // Creates timer || Gives times since the Arduino board was activated || This behavior is repeated for similar variables

  if (buttonState == true && buttonStateOld == false) // Button is pressed
  {
    timerPressed = millis(); // Gets the time since the board started of the time when the button is pressed
    timerLastPressed = millis(); // Gets the number of millisec since the board started, at the instant button is clicked // eg value 2866, its been 2866 seconds since the board was started. timerLastPressed's value is 2866 further on.
    resetCount = 0; // Reset the reset counter
  }

  if (buttonState == false && buttonStateOld == true) // Button is released
  {
    timesPressed[(timesPressedPosition++) % numberOfPresses] = millis() - timerPressed; // 
    timerLastPressed = millis(); // Gets the time since the button was last pressed
  }
  buttonStateOld = buttonState; // Changes the buttons state, so that i holds the late state in StateOld


  if ( (startTimer - timerLastPressed) > resetTimer && hasBeenUnlocked == true) // Reset button push recording session
  {
    Serial.println("Reset made"); // Prints out reset made
    timesPressedPosition = 0; // Reset PressedPosition
    timerLastPressed = millis(); // Gets a new LastPressed timer
    resetCount++; // Increases the reset counter with 1
    Serial.println(resetCount); // Prints out the reset count
  }

  if (timesPressedPosition == numberOfPresses) // Verify pattern
  {
    int clearedPattern = 0; // Set and create a ClearedPattern counter variable
    for (int i = 0; i < numberOfPresses; i ++) // Continues as long as I has a value lower than numberOfPresses
    {
      int pressedPatterenValue = timesPressed[i]; // Set pressedPatterenValue to be equal to the value of timesPressed of I || eg timesPressed[2], value on index nr 2
      int ppv = pressedPatterenValue; // No actual function besides decreasing the number of characters I need to write to get the value (Im lazy but still want proper naming as much as possible)

      int unlockPatterenValueForI = unlockPatteren[i]; // Set unlockPatterenValueForI to be equal to the correct value for the index of I
      int upv = unlockPatterenValueForI; // No actual function, Im still just lazy
      int lowerupv = (upv - unlockPatternBufferZone); // Creates the lower acceptable unlock value by saing the actual unlock value minus the buffer
      int higherupv = (upv + unlockPatternBufferZone); // Creates upper accetable value, same way as the line above

      if ( lowerupv <= ppv && ppv <= higherupv ) // Check if the ppv is withing the lower and upper acceptable values
      {
        Serial.print("Success    ");
        Serial.print(lowerupv);
        Serial.print(" |||| " );
        Serial.print(ppv);
        Serial.print(" |||| ");
        Serial.println(higherupv);
        clearedPattern++;
        // The abovr, prints out data if the ppv-value is acceptable and increase clearedPattern by 1
      }
      else // if ppv is notacceptable is prints our the correct value for that index spot
      {
        Serial.println(timesPressed[i]);
      }
    }
    //////////////////////////////////////////////////////////////////
    if(clearedPattern == numberOfPresses) { Unlock(true); } // if clearedPattern count is equal to numberOfPResses use the Unlock method
    //////////////////////////////////////////////////////////////////
    timesPressedPosition = 0; // Reset timesPressedPosition
    Serial.println(); // Print empty line
  }

    //////////////////////////////////////////////////////////////////
    if( resetCount == desiredResetCount && hasBeenUnlocked == true) { resetCount = 0; Lock(true); }
    // if resetCount is equal to the desriedResetCount and the lock has been unlocked, then reset resetCount and lock the lock
    //////////////////////////////////////////////////////////////////

}
