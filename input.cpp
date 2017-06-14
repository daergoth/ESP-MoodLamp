#include "input.h"

void InputHandler::setupInputs() {
  pinMode(MAIN_BUTTON_PIN, INPUT_PULLUP);
  pinMode(PING_BUTTON_PIN, INPUT_PULLUP);

  if (digitalRead(MAIN_BUTTON_PIN) == LOW) {
    onStartupMainDown();
  }
  
}

void InputHandler::checkInputs() {
  // Check for Main long push
  if (wasLongMainPress != true && oldMainButtonState == LOW && millis() - lastMainButtonUpdate > LONG_PRESS_MILLIS) {
    wasLongMainPress = true;

    onMainLongDown();
  }
   
   // Main button check
   int mainButtonState;
   if ((mainButtonState = digitalRead(MAIN_BUTTON_PIN)) != oldMainButtonState) {
     if (millis() - lastMainButtonUpdate > DEBOUNCE_MILLIS) {        
      if (mainButtonState == HIGH ) {
        if (!wasLongMainPress) {
          onMainDown(); 
        } else {
          wasLongMainPress = false;
        }
      }
      
      oldMainButtonState = mainButtonState;
      lastMainButtonUpdate = millis();
     }
   }
  
   // Ping button check
   int pingButtonState;
   if ((pingButtonState = digitalRead(PING_BUTTON_PIN)) != oldPingButtonState) {
     if (millis() - lastPingButtonUpdate > DEBOUNCE_MILLIS) {
      if (pingButtonState == LOW) {
        onPingDown();
      }
  
      oldPingButtonState = pingButtonState;
      lastPingButtonUpdate = millis();
     }
   }
  
   // Rotary encoder check
   long encoderPos = encoder.read();
   if (encoderPos != oldEncoderPos) {
    if (oldMainButtonState == LOW) {
      wasLongMainPress = true;
      onPushedRotation(encoderPos);    
    } else {
      onRotation(encoderPos);
    }
    
    oldEncoderPos = encoderPos;
   }
}

// Must set this before setupInputs() to work!
void InputHandler::setOnStartupMainDown(void(*functionPointer)()) {
  onStartupMainDown = functionPointer;
}

void InputHandler::setOnRotation(void(*functionPointer)(long)) {
  onRotation = functionPointer;
}

void InputHandler::setOnPushedRotation(void(*functionPointer)(long)) {
  onPushedRotation = functionPointer;
}

void InputHandler::setOnMainDown(void(*functionPointer)()) {
  onMainDown = functionPointer;
}

void InputHandler::setOnMainLongDown(void(*functionPointer)()) {
  onMainLongDown = functionPointer;
}

void InputHandler::setOnPingDown(void(*functionPointer)()) {
  onPingDown = functionPointer;
}

