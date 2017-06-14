#ifndef PING_HELPER_H
#define PING_HELPER_H

#include <cstring>

#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

//#define PUBNUB_DEBUG
#define PUBNUB_DEFINE_STRSPN_AND_STRNCASECMP
#define PubNub_BASE_CLIENT WiFiClient
#include <PubNub.h>

#include "debug.h"

#define CHANNEL "lights"
#define BUFFER_SIZE 64
#define PING_CHECK_INTERVAL 20000


/*
 * Helper functions for PING function
 */
class PingService {
public:

  void setupMessaging(const char* pubkey, const char* subkey);

  void setEnabled(bool isEnabled);

  // Checks for incoming PING requests from other devices
  // params:
  //    pixels - NeoPixel strip on which the ping will be executed
  void checkForPings(Adafruit_NeoPixel& pixels);

  // Make PING request to other devices
  void pingOthers();

private:
  bool isEnabled = true;
  unsigned long lastPingCheckMillis = 0;

  // Make lighting effect for PING request
  // params:
  //    pixels - NeoPixel strip in which the effect will be executed
  void executePing(Adafruit_NeoPixel& pixels);
  
};

#endif
