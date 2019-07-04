#include <Arduino.h>
#include <FastLED.h>

#define TRUE 1
#define FALSE 0

#define NUM_LEDS 150
#define DATA_PIN 7

// Pin 13 has an LED connected on most Arduino boards.
// (comment out to disable blinking)
//#define BLINK_LED 13
#define BUTTON_PIN 6
#define ANALOG_PIN A2

#define DEFAULT_BRIGHTNESS 0x40

typedef struct {
    CRGB *leds;
    int length;
} CRGBString;

typedef struct {
    uint16_t count;
    uint16_t threshold;
    int state;
} ButtonState;

// Globals
CRGB g_leds[NUM_LEDS];
CRGBString myLeds = {g_leds, NUM_LEDS};

#define MODE_SPARKLE 0
#define MODE_SPARKLE_MONO 1
#define MODE_RAINBOW 2
#define MODE__COUNT 3

uint8_t g_mode = MODE_SPARKLE;

void setup() {
    // Initialize the digital pins
    #ifdef BLINK_LED
    pinMode(BLINK_LED, OUTPUT);
    #endif
    #ifdef BUTTON_PIN
    pinMode(BUTTON_PIN, INPUT);
    #endif

    // Initialise LED buffer
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(g_leds, NUM_LEDS);
}

// ============ MODES ============
void fadeAll(CRGBString *leds, uint8_t amount) {
    for (int i=0; i<leds->length; i++) {
        leds->leds[i].fadeToBlackBy(amount);
    }
}

void setRunner(CRGBString *leds, int dir, int count, CRGB color) {
    static int i = 0;
    for (int j=0; j<count; j++) {
        int idx = (i + ((leds->length / count) * j)) % leds->length;
        leds->leds[idx] = color;
    }
    i = (i + dir) % leds->length;
}

void setSparkleMonochrome(CRGBString *leds, uint8_t level) {
    int idx = random(leds->length);
    // Random Shade of white
    uint8_t l_c = random(level);
    leds->leds[idx].r = l_c;
    leds->leds[idx].g = l_c;
    leds->leds[idx].b = l_c;
}

void setSparkle(CRGBString *leds, uint8_t level) {
    int idx = random(leds->length);
    // Random Colour
    leds->leds[idx].r = random(level);
    leds->leds[idx].g = random(level);
    leds->leds[idx].b = random(level);
}

void setRainbow(CRGBString *leds, uint8_t offset) {
    //int i = 0;
    uint8_t l_hue;
    for (int i=0; i<leds->length; i++) {
        l_hue = (uint8_t)((int)(i * leds->length) / leds->length);
        leds->leds[(i + offset) % leds->length].setHSV(l_hue, 0xff, 0xff);
    }
}

// ============ MAIN LOOP ============
void loop() {
    // Local Variables
    boolean l_buttonState = FALSE;
    static boolean l_lastButtonState = FALSE;
    static uint8_t l_brightness = DEFAULT_BRIGHTNESS;

    // Blink SMD LED
    #ifdef BLINK_LED
    digitalWrite(BLINK_LED, !digitalRead(BLINK_LED));  // toggle
    #endif

    // Read Dial
    #ifdef ANALOG_PIN
    l_brightness = (uint8_t)((analogRead(ANALOG_PIN) >> 2) & 0xff);
    #endif

    // Check switch
    #ifdef BUTTON_PIN
    l_buttonState = digitalRead(BUTTON_PIN);
    if (l_buttonState && !l_lastButtonState) { // rising edge
        g_mode = (g_mode + 1) % MODE__COUNT;  // increment state (cycle)
    }
    l_lastButtonState = l_buttonState;
    #endif

    // State Machine
    switch (g_mode) {
        case MODE_SPARKLE_MONO:  // monochrome sparkle
            fadeAll(&myLeds, l_brightness >> 2);
            setSparkleMonochrome(&myLeds, l_brightness);
            break;
        case MODE_RAINBOW:
            setRainbow(&myLeds, (uint8_t)(((millis() << 8) / 10000) & 0xff));
            fadeAll(&myLeds, 0xff - l_brightness);
            break;
        case MODE_SPARKLE:  // coloured sparkle
        default:
            fadeAll(&myLeds, l_brightness >> 2);
            setSparkle(&myLeds, l_brightness);
            break;
    }

    // Flush pixel buffer
    FastLED.show();
    delay(50);
}
