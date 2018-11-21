
static void showMetadata(SnifferPacket *snifferPacket) {

  unsigned int frameControl = ((unsigned int)snifferPacket->data[1] << 8) + snifferPacket->data[0];

  uint8_t version      = (frameControl & 0b0000000000000011) >> 0;
  uint8_t frameType    = (frameControl & 0b0000000000001100) >> 2;
  uint8_t frameSubType = (frameControl & 0b0000000011110000) >> 4;
  uint8_t toDS         = (frameControl & 0b0000000100000000) >> 8;
  uint8_t fromDS       = (frameControl & 0b0000001000000000) >> 9;

  // Only look for probe request packets
  if (frameType != TYPE_MANAGEMENT ||
      frameSubType != SUBTYPE_PROBE_REQUEST)
    return;

  ///////////////////////////////////////////////////////////////////////////////
  // ----------------------- My code -----------------------


  char addr[] = "00:00:00:00:00:00";
  getMAC(addr, snifferPacket->data, 10);

  //Serial.println(addr);

  String sniffedMac = addr;
  String myMac = findAddr;

  if (sniffedMac != myMac && phoneLastFoundTimer > 0)
  {
    Serial.println("Phone not fount anymore");
  }

    // if the phone Mac is not found after reset timer has expired, then you have left the house and the door should be locked
    if (sniffedMac != myMac && phoneLastFoundTimer > 0 && (millis() - phoneLastFoundTimer) >= (resetTimer * desiredResetCount) )
    {
      if (servo.read() < lockDegree)
      {
        Serial.println("Phone not found");
        Lock(true);
        phoneLastFoundTimer = 0;
      }
    }
    // if the phone mac is still found after the resetTimer and the timer has a value above 0, then reset the timer to now
    // maintain status that the phone has not left the house
    if (sniffedMac == myMac && phoneLastFoundTimer > 0 && (millis() - phoneLastFoundTimer) >= resetTimer)
    {
      Serial.println("Phone still found");
      phoneLastFoundTimer = millis();
    }

    if (sniffedMac == myMac && phoneLastFoundTimer == 0)
    {
      phoneLastFoundTimer = millis();
      Unlock(true);

      Serial.print(phoneLastFoundTimer);
      Serial.print(" - found Phone - ");
      Serial.println( sniffedMac );
    }
  }

  // ----------------------- My code -----------------------
  ///////////////////////////////////////////////////////////////////////////////

  /**
     Callback for promiscuous mode
  */
  static void ICACHE_FLASH_ATTR sniffer_callback(uint8_t *buffer, uint16_t length) {
    struct SnifferPacket *snifferPacket = (struct SnifferPacket*) buffer;
    showMetadata(snifferPacket);
  }

  static void printDataSpan(uint16_t start, uint16_t size, uint8_t* data) {
    for (uint16_t i = start; i < DATA_LENGTH && i < start + size; i++) {
      Serial.write(data[i]);
    }
  }

  static void getMAC(char *addr, uint8_t* data, uint16_t offset) {
    sprintf(addr, "%02x:%02x:%02x:%02x:%02x:%02x", data[offset + 0], data[offset + 1], data[offset + 2], data[offset + 3], data[offset + 4], data[offset + 5]);
  }



  /**
     Callback for channel hoping
  */
  void channelHop()
  {
    // hoping channels 1-14
    uint8 new_channel = wifi_get_channel() + 1;
    if (new_channel > 14)
      new_channel = 1;
    wifi_set_channel(new_channel);
  }

