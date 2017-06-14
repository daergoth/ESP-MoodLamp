#include "modes.h"

// Sets the given NeoPixel to be used
// params:
//  p - NeoPixel object
void ModeHandler::setupModes(Adafruit_NeoPixel& p) {
  pixels = p;

  EEPROM.get(0, firstPowerOn);

  // 0xAA, because it's 1010 1010 in binary
  // Special pattern enough to not be accidental
  // and memory garbage
  if (firstPowerOn == 0xAA) {
    // Not first power on
    EEPROM.get(1, currentModeNum);
    EEPROM.get(2, currentBrightness);
    EEPROM.get(3, fixColor_CurrentColor);
    EEPROM.get(4, colorCycle_ChangeSpeed);
    EEPROM.get(6, colorCycle_ModePos);
    EEPROM.get(8, rainbow_ChangeSpeed);
    EEPROM.get(10, rainbow_ModePos);
    EEPROM.get(12, breathe_CurrentColor);
  } else {
    // First power on

    //firstPowerOn
    EEPROM.put(0, 0xAA);
    //currentModeNum
    EEPROM.put(1, (byte)0);
    //currentBrightness
    EEPROM.put(2, (byte)50);
    //fixColor_CurrentColor
    EEPROM.put(3, (byte)0);
    //colorCycle_ChangeSpeed
    EEPROM.put(4, (unsigned short)50);
    //colorCycle_ModePos
    EEPROM.put(6, (short)0);
    //rainbow_ChangeSpeed
    EEPROM.put(8, (unsigned short)40);
    //rainbow_ModePos
    EEPROM.put(10, (short)0);
    //breathe_CurrentColor
    EEPROM.put(12, (byte)0);

#ifdef ESP8266
    EEPROM.commit();
#endif
  }
  
}

// Resets all settings to the default value.
void ModeHandler::factoryReset() {
  EEPROM.write(0, 0);

  setupModes(pixels);

  isModeChanged = true;
}

// Switches to the next lighting mode
void ModeHandler::nextMode() {
  currentModeNum = (currentModeNum + 1) % TOTAL_MODE_NUM;

  isModeChanged = true;
  lastModeChanged = millis();

  debugLog("Next Mode - #");
  debugLogln(currentModeNum);
}

// Updates lighting on current mode
void ModeHandler::updateMode() {
  if (pixelPowerState) {
    switch(currentModeNum) {
      // Full white
      case 0:
        if (isModeChanged) {
          effectFullWhiteUpdate();
        }
        break;

      // Fix color
      case 1:
        if (isModeChanged) {
          effectFixColorUpdate();
        }
        break;

      // Color cycle
      case 2:
        effectColorCycleUpdate();
        break;

      // Rainbow
      case 3:
        effectRainbowUpdate();
        break;

      // Breathe
      case 4:
        effectBreatheUpdate();
        break;

      // Fire
      case 5:
        effectFireUpdate();
        break;
    }
  } else {
    // Turned-off lights
    pixels.clear();
  }

  pixels.show();

  if (isModeChanged == true && millis() - lastModeChanged >= 3000) {
    uint8_t t = 0;

    EEPROM.get(1, t);
    if (t != currentModeNum) {
      EEPROM.put(1, currentModeNum);
    }
    
    EEPROM.get(2, t);
    if (t != currentBrightness) {
      EEPROM.put(2, currentBrightness);
    }
    
    EEPROM.get(3, t);
    if (t != fixColor_CurrentColor) {
      EEPROM.put(3, fixColor_CurrentColor);
    }
    
    EEPROM.get(4, t);
    if (t != colorCycle_ChangeSpeed) {
      EEPROM.put(4, colorCycle_ChangeSpeed);
    }

    EEPROM.get(6, t);
    if (t != colorCycle_ModePos) {
      EEPROM.put(6, colorCycle_ModePos);
    }

    EEPROM.get(8, t);
    if (t != rainbow_ChangeSpeed) {
      EEPROM.put(8, rainbow_ChangeSpeed);
    }

    EEPROM.get(10, t);
    if (t != rainbow_ModePos) {
      EEPROM.put(10, rainbow_ModePos);
    }
    
    EEPROM.get(12, t);
    if (t != breathe_CurrentColor) {
      EEPROM.put(12, breathe_CurrentColor);
    }

#ifdef ESP8266
    EEPROM.commit();
#endif

    debugLogln("Saved!");
  
    isModeChanged = false;
  }

}

// Update encoder position for current mode
// params: 
//  encoderPos - new rotary encoder position to update to
void ModeHandler::updateModePos(long encoderPos) {
  modePosChange = encoderPos - currentEncoderPos; 

  switch(currentModeNum) {
    // Full white
    case 0:
      effectFullWhitePosUpdate();
      break;

    // Fix color
    case 1:
      effectFixColorPosUpdate(modePosChange);
      break;

    // Color cycle
    case 2:
      effectColorCyclePosUpdate(modePosChange);
      break;

    // Rainbow
    case 3:
      effectRainbowPosUpdate(modePosChange);
      break;

    // Breathe
    case 4:
      effectBreathePosUpdate(modePosChange);
      break;

    // Fire
    case 5:
      effectFirePosUpdate();
      break;
  }
  
  currentEncoderPos = encoderPos;

  isModeChanged = true;
  lastModeChanged = millis();
}

// Toggle power state of the NeoPixel strip
void ModeHandler::togglePowerState() {   
  pixelPowerState = !pixelPowerState;

  isModeChanged = true;
  lastModeChanged = millis();

  debugLog("Toggle Power - ");
  debugLogln(pixelPowerState?"ON":"OFF");
}

// Update brightness according to the given encoder position
// params:
//  encoderPos - new encoder position to update to
void ModeHandler::updateBrightness(long encoderPos) {
  
  if (encoderPos - currentEncoderPos <= 0 && currentBrightness <= (currentEncoderPos - encoderPos) * 3) {
    currentBrightness = 0;
  } else {
    currentBrightness += (encoderPos - currentEncoderPos) * 3;

    if (currentBrightness > 100) {
      currentBrightness = 100;
    }
  }

  debugLog("New Brightness: ");
  debugLogln(currentBrightness);

  currentEncoderPos = encoderPos;

  isModeChanged = true;
  lastModeChanged = millis();
}



// Sets a pixels color with the given brightness
// params:
//  n - pixel number starting from 0
//  red - red intensity ranging 0-255
//  green - green intensity ranging 0-255
//  blue - blue intensity ranging 0-255
//  brightness - brightness of the color in percents ranging 0-100
void ModeHandler::setPixelWithBrightness(uint16_t n, uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness) {
  if (brightness > 100) {
    brightness = 100;
  }
  float b = brightness / 100.0;
  
  pixels.setPixelColor(n, (uint8_t)(red * b), (uint8_t)(green * b), (uint8_t)(blue * b));
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t ModeHandler::colorWheelWithBrightness(byte WheelPos, uint8_t brightness) {
  if (brightness > 100) {
    brightness = 100;
  }
  float b = brightness / 100.0;   
  
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color((255 - WheelPos * 3) * b, 0, (WheelPos * 3) * b);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, (WheelPos * 3) * b, (255 - WheelPos * 3) * b);
  }
  WheelPos -= 170;
  return pixels.Color((WheelPos * 3) * b, (255 - WheelPos * 3) * b, 0);
}

/*
 * MODES
 */

// Full white effect
void ModeHandler::effectFullWhiteUpdate() {
  for (uint16_t i = 0; i < pixels.numPixels(); ++i) {
    setPixelWithBrightness(i, 255, 255, 255, currentBrightness);
  }
}

void ModeHandler::effectFullWhitePosUpdate(){}

// Fix color
void ModeHandler::effectFixColorUpdate() {
  uint32_t colorToSet = colorWheelWithBrightness(fixColor_CurrentColor,currentBrightness);
  
  for (uint16_t i = 0; i < pixels.numPixels(); ++i) {
    pixels.setPixelColor(i, colorToSet);
  } 
}

void ModeHandler::effectFixColorPosUpdate(long modePosChange) {
  fixColor_CurrentColor = (fixColor_CurrentColor + modePosChange) % 256;
}

// Color cycle
void ModeHandler::effectColorCycleUpdate() {
  if (millis() - colorCycle_LastChangeMillis >= colorCycle_ChangeSpeed) {
    uint32_t colorToSet = colorWheelWithBrightness(colorCycle_CurrentColor,currentBrightness);
    
    for (uint16_t i = 0; i < pixels.numPixels(); ++i) {
      pixels.setPixelColor(i, colorToSet);
    }

    colorCycle_LastChangeMillis = millis();

    colorCycle_CurrentColor++;
  }
}

void ModeHandler::effectColorCyclePosUpdate(long modePosChange) {
  colorCycle_ModePos += modePosChange;
  
  colorCycle_ChangeSpeed = min(exp((colorCycle_ModePos) / 5.0), 25000.0);
}

// Rainbow
void ModeHandler::effectRainbowUpdate() {
  if (millis() - rainbow_LastChangeMillis >= rainbow_ChangeSpeed) {
    for (uint16_t i = 0; i < pixels.numPixels(); ++i) {
      pixels.setPixelColor(i, colorWheelWithBrightness(rainbow_CurrentColor + (256 / pixels.numPixels()) * i,currentBrightness));
    }

    rainbow_LastChangeMillis = millis();

    rainbow_CurrentColor++;
  }
}

void ModeHandler::effectRainbowPosUpdate(long modePosChange) {
  rainbow_ModePos += modePosChange;
  
  rainbow_ChangeSpeed = min(exp((rainbow_ModePos) / 5.0), 25000.0);
}

// Breathe
void ModeHandler::effectBreatheUpdate() {
  if (millis() - breathe_LastChangeMillis > 80) {
    uint32_t colorToSet = colorWheelWithBrightness(breathe_CurrentColor,currentBrightness * ((sin(breathe_CurrentIntensity) + 1) / 2.0));
    
    for (uint16_t i = 0; i < pixels.numPixels(); ++i) {
      pixels.setPixelColor(i, colorToSet);
    }

    breathe_CurrentIntensity += breathe_CurrentWay;

    if (breathe_CurrentIntensity >= TWO_PI) {
      breathe_CurrentIntensity = 0;
    }

    breathe_LastChangeMillis = millis();
  }
}

void ModeHandler::effectBreathePosUpdate(long modePosChange) {
  breathe_CurrentColor = (breathe_CurrentColor + modePosChange) % 256;
}

// Fire
void ModeHandler::effectFireUpdate() {
  if (millis() - fire_LastChangeMillis > fire_RandomDelay ) {
    for (uint16_t i = 0; i < pixels.numPixels(); ++i) {
      uint8_t flicker = random(10, 75);
      uint8_t randomBrightness = random(55, 100);
      
      setPixelWithBrightness(i, fire_RedColor - flicker, fire_GreenColor - flicker, 0, currentBrightness * (randomBrightness / 100.0));
    }

    fire_RandomDelay = random(50, 112);
    fire_LastChangeMillis =  millis();
  }
}

void ModeHandler::effectFirePosUpdate() {}


