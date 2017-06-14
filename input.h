#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <Encoder.h>

#define DEBOUNCE_MILLIS 150

#define LONG_PRESS_MILLIS 1500

#define MAIN_BUTTON_PIN 5
#define ENCODER_CLOCK_PIN 12
#define ENCODER_DATA_PIN 14

#define PING_BUTTON_PIN 4

class InputHandler {
public:
  void setupInputs();
  
  void checkInputs();

  // Must set this before setupInputs() to work!
  void setOnStartupMainDown(void(*functionPointer)());

  void setOnRotation(void(*functionPointer)(long));

  void setOnPushedRotation(void(*functionPointer)(long));

  void setOnMainDown(void(*functionPointer)());

  void setOnMainLongDown(void(*functionPointer)());

  void setOnPingDown(void(*functionPointer)());
  

private:
  Encoder encoder{ENCODER_DATA_PIN, ENCODER_CLOCK_PIN};

  long oldEncoderPos = 0;

  int oldMainButtonState = HIGH;
  unsigned long lastMainButtonUpdate = 0;
  bool wasLongMainPress = false;
  
  int oldPingButtonState = HIGH;
  unsigned long lastPingButtonUpdate = 0;

  void (*onStartupMainDown)();
  void (*onRotation)(long);
  void (*onPushedRotation)(long);
  void (*onMainDown)();
  void (*onMainLongDown)();
  void (*onPingDown)();
  
};

#endif
