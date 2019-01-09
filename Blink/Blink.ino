#include <FastLED.h>

#define NUM_LEDS 144
#define DATA_PIN 6

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;

CRGB leds[NUM_LEDS];


// the setup routine runs once when you press reset:
void setup() {
    // initialize the digital pin as an output.
    pinMode(led, OUTPUT);
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}


// the loop routine runs over and over again forever:
int i = 0;
void loop() {
    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(100);               // wait for a second
    digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
    delay(100);               // wait for a second

    // WS2812 Lights
    leds[i] = CRGB::White;
    FastLED.show();
    i++;
    if (i >= NUM_LEDS) {
        i = 0;
    }
}
