#include <Adafruit_NeoPixel.h>
#include <WiFiManager.h>     

#include "ping.h"
#include "modes.h"
#include "debug.h"
#include "input.h"

#define NEOPIXEL_PIN 13
#define NEOPIXEL_NUM 12

/*                                                                                                                  
 * Main services
 */
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NEOPIXEL_NUM, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

InputHandler inputHandler;

PingService pingService;

ModeHandler modeHandler;

// PubNub API keys
static const char publishKey[] = "<PUBNUB_PUBLISH_KEY>";
static const char subscribeKey[] = "<PUBNUB_SUBSCRIBE_KEY>";

void setup() {

#ifdef DEBUG
  Serial.begin(115200);
#endif

#ifdef ESP8266
  EEPROM.begin(32);
#endif

  // Must set this before calling setupInputs() to work correctly!
  inputHandler.setOnStartupMainDown(onStartupMainDown);

  inputHandler.setOnRotation(onRotation);
  inputHandler.setOnPushedRotation(onPushedRotation);
  inputHandler.setOnMainDown(onMainDown);
  inputHandler.setOnMainLongDown(onMainLongDown);
  inputHandler.setOnPingDown(onPingDown);

  inputHandler.setupInputs();
  
  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.setConfigPortalTimeout(120);
  wifiManager.autoConnect("MoodLight");

  modeHandler.setupModes(pixels);

  pingService.setupMessaging(publishKey, subscribeKey);

  pixels.begin();
  pixels.show();
}

void loop() {
  inputHandler.checkInputs();

  pingService.checkForPings(pixels);

  modeHandler.updateMode();
}

void configModeCallback(WiFiManager *myWiFiManager) {
  pingService.setEnabled(false);
}

void saveConfigCallback() {
  pingService.setEnabled(true);
}

void onStartupMainDown() {
  modeHandler.factoryReset();
}

void onRotation(long rotation) {
  modeHandler.updateModePos(rotation/2);
}

void onPushedRotation(long rotation) {
  modeHandler.updateBrightness(rotation/2);
}

void onMainDown() {
  modeHandler.nextMode();
}

void onMainLongDown() {
  modeHandler.togglePowerState();
}

void onPingDown() {
  pingService.pingOthers();
}




