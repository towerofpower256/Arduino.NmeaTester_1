/*
 * NMEA Tester Tool - Version 1
 * By David McDonald - 13/08/2019
 * 
 * The aim here is to create a portable hand-held tool
 * that is able to be connected to an NMEA 0182 / 0183 device
 * and see testing information.
 * The drive behind this is that you can do this with a laptop,
 * but a large laptop with a serial converter isn't all that 
 * portable in a pinch.
 */

#define DEBUG true
#define READ_INDICATOR_PIN 13
#define READ_INDICATOR_TIMEOUT 100
#define SPLASH_MSG_TIMEOUT 3000 // How long should the startup splash message be on the screen for
#define SCREEN_CHANGE_SPLASH_TIMEOUT 1000 // How long to show the splash message for a screen

#include <math.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "Constants.h"
#include "NmeaParser.h"
#include "MessageDumpScreen.h"
#include "CompassScreen.h"

LiquidCrystal_I2C lcd(0x27,LCD_WIDTH,LCD_HEIGHT);  // set the LCD address to 0x27 for a 16 chars and 2 line display

NmeaParser nmeaParser = NmeaParser();

// Messages
const char STARTUP_MSG_1[] = "=NMEA Tester v1=";
const char STARTUP_MSG_2[] = "-David McDonald-";

// Working variables

bool serialReadLedOn = false;
long serialReadLedTimeout = 0;

long startupSplashTimeout = 0;
bool startupSplashDone = false;
long screenChangeSplashTimeout = 0;
bool screenChangeSplashDone = false;

BaseScreen* screens[8];
MessageDumpScreen messageDumpScreen = MessageDumpScreen(&lcd);
CompassScreen compassScreen = CompassScreen(&lcd);
byte currentScreen = 0;
byte screenCount = 0;

void setup() {
  // put your setup code here, to run once:

  // setup the Serial Read indicator
  pinMode(READ_INDICATOR_PIN, OUTPUT);
  digitalWrite(READ_INDICATOR_PIN, LOW);

  // Start serial at NMEA baud rate
  Serial.begin(4800);
  Serial.flush();
  #if DEBUG
  Serial.println(STARTUP_MSG_1);
  Serial.println(STARTUP_MSG_2);
  Serial.println(F("Debug enabled"));
  #endif
  
  // Start the LCD
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print(STARTUP_MSG_1);
  lcd.setCursor(0,1);
  lcd.print(STARTUP_MSG_2);

  startupSplashTimeout = millis() + SPLASH_MSG_TIMEOUT;

  // Populate screens array
  AddScreen(&messageDumpScreen);
  AddScreen(&compassScreen);

  Serial.println(F("Startup complete"));
}

void loop() {
  CheckSerialReadLed();
  if (startupSplashDone) CheckScreenChangeSplashTimeout();

  // Update the screen
  if (!startupSplashDone && millis() > startupSplashTimeout) {
    #if DEBUG
    Serial.println(F("Ending startup splash"));
    #endif
    startupSplashDone = true;
    ChangeScreen(1); // Change to the first screen
  }

  if (startupSplashDone && screenChangeSplashDone) {
    screens[currentScreen]->UpdatePrint();
  }
}

void serialEvent() {
  while (Serial.available()) {
    TurnSerialReadLedOn();
    char inChar = (char)Serial.read();

    if (nmeaParser.doNext(inChar)) {
      #if DEBUG
      Serial.println(F("New NMEA msg: ")); Serial.println(nmeaParser.getSentence());
      #endif

      for (int i=0; i < screenCount; i++) {
        #if DEBUG
        Serial.print(F("Screen process message: ")); Serial.println(i);
        #endif
        screens[i]->ProcessMessage(&nmeaParser);
      }
    }
  }
}

// Screen related functionality
void AddScreen(BaseScreen* newScreenPtr) {
  screens[screenCount] = newScreenPtr;
  screenCount++;
}

void ChangeScreen(byte screenID) {
  #if DEBUG
  Serial.print(F("ScreenChange: ")); Serial.println(screenID);
  #endif
  
  currentScreen = screenID;
  lcd.clear();
  screens[screenID]->TitlePrint();
  screenChangeSplashTimeout = millis() + SCREEN_CHANGE_SPLASH_TIMEOUT;
  screenChangeSplashDone = false;
}

void CheckScreenChangeSplashTimeout() {
  if (!screenChangeSplashDone && millis() >= screenChangeSplashTimeout) {
    #if DEBUG
    Serial.println(F("Ending screen change splash"));
    #endif

    screenChangeSplashDone = true;
    screens[currentScreen]->InitialPrint(); // Do the intial print for the current screen
  }
}

// Serial read LED indicator functionality
void CheckSerialReadLed() {
  if (serialReadLedOn && (millis() >= serialReadLedTimeout)) {
    #if DEBUG
    Serial.println(F("SerialLedOff"));
    #endif
    // Timeout has passed, turn off the indicator
    serialReadLedOn = false;
    digitalWrite(READ_INDICATOR_PIN, LOW);
  }
}

void TurnSerialReadLedOn() {
  if (serialReadLedOn) return; // Don't do anything if the LED is already on
  
  #if DEBUG
  Serial.println(F("SerialLedOn"));
  #endif
  
  digitalWrite(READ_INDICATOR_PIN, HIGH); // Turn the indicator on
  serialReadLedOn = true;
  serialReadLedTimeout = millis() + READ_INDICATOR_TIMEOUT;
}
