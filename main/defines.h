// Includes a bunch of libraries
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <WebSocketsServer.h>
#include <Hash.h>

///////////////////////////////////////////////////////////////////////////////
// ----------------------- My code ----------------------- 
// This is and indiate where my own code is, my code is below this line


Servo servo; // Creates a servo object

#define numberOfPresses 5 // Global variable
unsigned long timerPressed = 0; // Becomes a timer later
unsigned long timerLastPressed = 0; // Becomes a timer later
unsigned long startTimer = 0; // Becomes a timer later
unsigned long phoneLastFoundTimer = 0; // Becomes a timer later

boolean buttonStateOld = false; // Determines the initial state of the buttons old state, this is used later to remember and validate the state change of the button

int timesPressed[numberOfPresses]; // Creates an array with numberOfPresses slots (5)
int timesPressedPosition = 0; // Sets the initial position of the times we have pressed the button to 0

bool hasBeenUnlocked = false; // States that the lock initially haven't been unlocked

// Sets the reset counter to 0. 
// The reset counter is used to keep track of how many times we have reached the reset timer limit
int resetCount = 0 ; 
// Desired reset count is the number of resets we want to happen before the system reverts back to a locked state
int desiredResetCount = 2;
// Reset timer is milliseconds before the system should count 1 reset (5000 millis = 5 seconds. 5 seconds is a reset cycle)
int resetTimer = 5000;

// This is the unlock pattern which we want to unlock the door. It is also written in milliseconds
int unlockPatteren[] = {200, 200, 200, 200, 200}; 
// The amount of milliseconds we wish to buffer with. eg. 50 millis means between 150 and 200 millis will count as valid
int unlockPatternBufferZone = 50; 

// Unlock and Lock degrees. 0 is for locked position (should always be 0). 75 degreesis enough to unlock my door
int unlockDegree = 75;
int lockDegree = 0;

// This is and indiate where my own code is, my code is above this line
// ----------------------- My code ----------------------- 
///////////////////////////////////////////////////////////////////////////////


extern "C" {
#include <user_interface.h>
}

#define DATA_LENGTH           112

#define TYPE_MANAGEMENT       0x00
#define TYPE_CONTROL          0x01
#define TYPE_DATA             0x02
#define SUBTYPE_PROBE_REQUEST 0x04

#define DISABLE 0
#define ENABLE  1

static void ICACHE_FLASH_ATTR sniffer_callback(uint8_t *buffer, uint16_t length);
#define CHANNEL_HOP_INTERVAL_MS   1000
static os_timer_t channelHop_timer;

struct RxControl {
  signed rssi: 8; // signal intensity of packet
  unsigned rate: 4;
  unsigned is_group: 1;
  unsigned: 1;
  unsigned sig_mode: 2; // 0:is 11n packet; 1:is not 11n packet;
  unsigned legacy_length: 12; // if not 11n packet, shows length of packet.
  unsigned damatch0: 1;
  unsigned damatch1: 1;
  unsigned bssidmatch0: 1;
  unsigned bssidmatch1: 1;
  unsigned MCS: 7; // if is 11n packet, shows the modulation and code used (range from 0 to 76)
  unsigned CWB: 1; // if is 11n packet, shows if is HT40 packet or not
  unsigned HT_length: 16; // if is 11n packet, shows length of packet.
  unsigned Smoothing: 1;
  unsigned Not_Sounding: 1;
  unsigned: 1;
  unsigned Aggregation: 1;
  unsigned STBC: 2;
  unsigned FEC_CODING: 1; // if is 11n packet, shows if is LDPC packet or not.
  unsigned SGI: 1;
  unsigned rxend_state: 8;
  unsigned ampdu_cnt: 8;
  unsigned channel: 4; //which channel this packet in.
  unsigned: 12;
};

struct SnifferPacket {
  struct RxControl rx_ctrl;
  uint8_t data[DATA_LENGTH];
  uint16_t cnt;
  uint16_t len;
};
