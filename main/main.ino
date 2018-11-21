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
  startTimer = millis(); // Creates timer || Gives times since the Arduino board was activated

  if (buttonState == true && buttonStateOld == false) // Button is pressed
  {
    timerPressed = millis(); // Gets the time since the board started of the time when the button is pressed
    timerLastPressed = millis(); // Gets the number of millisec since the board started, at the instant button is clicked // eg value 2866, its been 2866 seconds since the board was started. timerLastPressed's value is 2866 further on.
    resetCount = 0;
  }

  if (buttonState == false && buttonStateOld == true) // Button is released
  {
    timesPressed[(timesPressedPosition++) % numberOfPresses] = millis() - timerPressed;
    timerLastPressed = millis();
  }
  buttonStateOld = buttonState;


  if ( (startTimer - timerLastPressed) > resetTimer && hasBeenUnlocked == true) // Reset button push recording session
  {
    Serial.println("Reset made");
    timesPressedPosition = 0;
    timerLastPressed = millis();
    resetCount++;
    Serial.println(resetCount);
  }

  if (timesPressedPosition == numberOfPresses) // Verify pattern
  {
    int clearedPattern = 0;
    for (int i = 0; i < numberOfPresses; i ++)
    {
      int pressedPatterenValue = timesPressed[i];
      int ppv = pressedPatterenValue;

      int unlockPatterenValueForI = unlockPatteren[i];
      int upv = unlockPatterenValueForI;
      int lowerupv = (upv - unlockPatternBufferZone);
      int higherupv = (upv + unlockPatternBufferZone);

      if ( lowerupv <= ppv && ppv <= higherupv )
      {
        Serial.print("Success    ");
        Serial.print(lowerupv);
        Serial.print(" |||| " );
        Serial.print(ppv);
        Serial.print(" |||| ");
        Serial.println(higherupv);
        clearedPattern++;
      }
      else
      {
        Serial.println(timesPressed[i]);
      }
    }
    //////////////////////////////////////////////////////////////////
    if(clearedPattern == numberOfPresses) { Unlock(true); }
    //////////////////////////////////////////////////////////////////
    timesPressedPosition = 0;
    Serial.println();
  }

    //////////////////////////////////////////////////////////////////
    if( resetCount == desiredResetCount && hasBeenUnlocked == true) { resetCount = 0; Lock(true); }
    //////////////////////////////////////////////////////////////////

}
