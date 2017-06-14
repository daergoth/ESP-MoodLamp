#ifndef MODE_HANDLER_H
#define MODE_HANDLER_H

#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#include "debug.h"

#define TOTAL_MODE_NUM 6

/*
 * EEPROM layout
 * 
 * Variable name          | Variable type   | Byte size | EEPROM starting byte
 * -----------------------------------------------------------------------
 * firstPowerOn           | byte            | 1         | 0
 * currentModeNum         | byte            | 1         | 1
 * currentBrightness      | byte            | 1         | 2
 * fixColor_CurrentColor  | byte            | 1         | 3
 * colorCycle_ChangeSpeed | unsigned short  | 2         | 4
 * colorCycle_ModePos     | short           | 2         | 6
 * rainbow_ChangeSpeed    | unsigned short  | 2         | 8 
 * rainbow_ModePos        | short           | 2         | 10
 * breathe_CurrentColor   | byte            | 1         | 12
 */

class ModeHandler {
public:

  // Sets the given NeoPixel to be used
  // params:
  //  p - NeoPixel object
  void setupModes(Adafruit_NeoPixel& p);

  // Resets all settings to the default value.
  void factoryReset();

  // Switches to the next lighting mode
  void nextMode();

  // Updates lighting on current mode
  void updateMode();

  // Update encoder position for current mode
  // params: 
  //  encoderPos - new rotary encoder position to update to
  void updateModePos(long encoderPos);

  // Toggle power state of the NeoPixel strip
  void togglePowerState();

  // Update brightness according to the given encoder position
  // params:
  //  encoderPos - new encoder position to update to
  void updateBrightness(long encoderPos);
  
private:
  Adafruit_NeoPixel pixels;

  byte firstPowerOn = 0;
  byte currentModeNum = 0;  
  int currentEncoderPos = 0;
  
  bool pixelPowerState = true;
  byte currentBrightness = 50;
  
  short modePosChange = 0;
  bool isModeChanged = true;
  unsigned long lastModeChanged = 0;

  // Sets a pixels color with the given brightness
  // params:
  //  n - pixel number starting from 0
  //  red - red intensity ranging 0-255
  //  green - green intensity ranging 0-255
  //  blue - blue intensity ranging 0-255
  //  brightness - brightness of the color in percents ranging 0-100
  void setPixelWithBrightness(uint16_t n, uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness);

  // Input a value 0 to 255 to get a color value.
  // The colours are a transition r - g - b - back to r.
  uint32_t colorWheelWithBrightness(byte WheelPos, uint8_t brightness);

  /*
   * MODES
   */

  // Full white effect
  void effectFullWhiteUpdate();

  void effectFullWhitePosUpdate();

  // Fix color effect
  byte fixColor_CurrentColor = 0;
  
  void effectFixColorUpdate();

  void effectFixColorPosUpdate(long modePosChange);

  
  // Color cycle effect
  short colorCycle_ModePos = 0;
  byte colorCycle_CurrentColor = 0;
  unsigned short colorCycle_ChangeSpeed = 50;
  unsigned long colorCycle_LastChangeMillis = 0;

  void effectColorCycleUpdate();

  void effectColorCyclePosUpdate(long modePosChange);

  // Rainbow effect
  short rainbow_ModePos = 0;
  byte rainbow_CurrentColor = 0;
  unsigned short rainbow_ChangeSpeed = 20;
  unsigned long rainbow_LastChangeMillis = 0;
  
  void effectRainbowUpdate();

  void effectRainbowPosUpdate(long modePosChange);

  // Breathe effect
  byte breathe_CurrentColor = 0;
  float breathe_CurrentIntensity = 0;
  float breathe_CurrentWay = 0.1;
  unsigned long breathe_LastChangeMillis = 0;
  
  void effectBreatheUpdate();

  void effectBreathePosUpdate(long modePosChange);

  // Fire effect
  uint8_t fire_RedColor = 246;
  uint8_t fire_GreenColor = 91;
  uint8_t fire_RandomDelay = 55;
  unsigned long fire_LastChangeMillis = 0;
  
  void effectFireUpdate();

  void effectFirePosUpdate();
  
};

#endif
