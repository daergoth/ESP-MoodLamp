# ESP-MoodLamp
Mood lighting lamp with an ESP8266 and NeoPixels

A simple mood lamp with a few modes and a dedicated
button to ping other lamps using PubNub's
free publish-subscribe service. This was a small
side project for me to get familiar with the Arduino firmware
on the ESP8266.  
[https://github.com/esp8266/Arduino](https://github.com/esp8266/Arduino)

## Controlls
The main input for the lamp is a rotary encoder with a built-in
push button. The secondary button is only used for pinging.
- Simple rotation - changes the variable property of the mode
- Simple push - next mode
- Long push - Turn on/off the lights
- Pushed-down rotation - changes the brightness
- Ping button push - ping other lamps

## Pinging other lamps
Upon receiving a ping, the lamp will blink
a few times with red color. Basically, it's
a 'I-think-of-you' feature. This feature is using PubNub's
free publish-subscribe service, so you will need
API keys for it. You can get them pretty easily,
if you register on the site:  
[https://www.pubnub.com/](https://www.pubnub.com/)

## Lighting modes:
- Full white - simple white color
- Fix color - choose a color
- Color cycle - colors fading into eachother
- Rainbow effect - full rainbow spinning in a circle
- Breathing effect - pulsing a chosen color
- Fire effect - randomized warm colors looking like a candle

## Future plans
I'm planning to add a webpage hosted by the ESP8266
to control the effects and modes of the lamp, but
there is no ETA for it. If you would like to participate or
simply correct my mistakes, please feel free to do it.
