#include "ping.h"

void PingService::setupMessaging(const char* pubkey, const char* subkey) {
  PubNub.begin(pubkey, subkey);
}

void PingService::setEnabled(bool isEnabled) {
  this->isEnabled = isEnabled;
}

// Checks for incoming PING requests from other devices
// params:
//    pixels - NeoPixel strip on which the ping will be executed
void PingService::checkForPings(Adafruit_NeoPixel& pixels) {
  if(!isEnabled)
    return;
  
  if (millis() - lastPingCheckMillis > PING_CHECK_INTERVAL) {
    debugLogln("Checking for pings...");
    
    PubSubClient* sclient = PubNub.subscribe(CHANNEL, 1);

    yield();

    debugLog("sclient: ");
    debugLogln((unsigned long)sclient);
  
    if (sclient != 0) {
      debugLogln("sclient not 0");
      
      char buffer[BUFFER_SIZE];
      size_t bufferSize = 0;
      
      while (sclient->wait_for_data()) {
          buffer[bufferSize++] = sclient->read();

          if (bufferSize >= BUFFER_SIZE - 1) {
            break;
          }
      }

      yield();

      debugLog("bufferSize after read: ");
      debugLogln(bufferSize);

      //Cut down first char: "["
      //Last 3 chars ( "]]\0" ) aren't needed for us
      memmove(buffer, buffer + 1, bufferSize - 3);

      //Cut down last 4 chars: "}]]\0"
      buffer[bufferSize - 3] = '\0';
      
      debugLog("buffer: ");
      debugLogln(buffer);

      yield();

      //Only care about JSON with at least 1 key-value pair
      // strlen("{\"\":}") == 5
      if (bufferSize > 5) {
        StaticJsonBuffer<200> jsonBuffer;
        
        JsonObject& root = jsonBuffer.parseObject(buffer);

        yield();

        debugLog("json: ");
        root.printTo(Serial);
        debugLogln();

        yield();

        if (root["id"] != ESP.getChipId()) {
          debugLogln("Ping received!");
          executePing(pixels);
        }
      }

      sclient->stop();
    }

    lastPingCheckMillis = millis();
    debugLogln("End checking for pings!");
  }
  
}

// Make PING request to other devices
void PingService::pingOthers() {
  if (!isEnabled) 
    return;
  
  StaticJsonBuffer<200> jsonBuffer;
  
  JsonObject& root = jsonBuffer.createObject();

  yield();

  root["id"] = ESP.getChipId();

  String out;
  root.printTo(out);
  debugLog("json: ");
  debugLogln(out);

  yield();

  PubNub_BASE_CLIENT *client = PubNub.publish(CHANNEL, out.c_str(), 3);

  yield();
  
  if (client != 0) {
    client->stop();
  }

  debugLogln("Ping others!");
}

// Make lighting effect for PING request
// params:
//    pixels - NeoPixel strip in which the effect will be executed
void PingService::executePing(Adafruit_NeoPixel& pixels) {

  // Only needed for first LED, because only static modes
  // won't update the NeoPixel strip instantly
  uint32_t pastColor = pixels.getPixelColor(0);
  
  for (uint8_t block = 0; block < 3; ++block) {
    for (uint8_t light = 0; light < 2; ++light) {
      for (uint8_t i = 0; i < pixels.numPixels(); ++i) {
        pixels.setPixelColor(i, 255, 0, 0);
      }

      pixels.show();
      
      delay(135);
      
      pixels.clear();
      pixels.show();
  
      delay(135);
    }

    delay(500);
  }

  // Set pixels to saved color from before the ping
  for (uint8_t i = 0; i < pixels.numPixels(); ++i) {
    pixels.setPixelColor(i, pastColor);
  }
  pixels.show();
}

