//#define DEBUG

#include <Arduino.h>
#include "Constants.h"
#include "MessageDumpScreen.h"
#include "NmeaParser.h"



MessageDumpScreen::MessageDumpScreen(LiquidCrystal_I2C* lcdPtr) {
  _lcdPtr = lcdPtr;
  _hasUpdate = false;
}

void MessageDumpScreen::ProcessChar(char newChar) {
  
}

void MessageDumpScreen::TitlePrint() {
  LiquidCrystal_I2C lcd = *this->_lcdPtr;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(F("Message dump"));
}

void MessageDumpScreen::UpdatePrint(bool forcePrint) {
  if (_mustDoFirstPrint) InitialPrint();
  if (!forcePrint && !_hasUpdate) return; // Don't do anything if there's nothing new;
  
  LiquidCrystal_I2C lcd = *this->_lcdPtr;
  //lcd.clear();
  lcd.home();
  
  int msgSize = sizeof(_msg);
  int displaySize = LCD_WIDTH * LCD_HEIGHT;
  int iCol = 0; 
  int iRow = 0;
  for (int iCell=0; iCell < displaySize; iCell++) {
    if (iRow >= LCD_HEIGHT) {
      return; // Screen has maxed out, nothing to do here
    }
    
    lcd.setCursor(iCol, iRow);
    char nextChar;
    if (iCell < msgSize) {
      nextChar = this->_msg[iCell];
      if (nextChar == '\0') nextChar = DUMP_EMPTY_CHAR; // If it's a string terminator char, use the dump char.
    }
    else
    {
      nextChar = DUMP_EMPTY_CHAR;
    }
    
    // Print the char
    lcd.print(nextChar);

    // Ready the next char
    iCol++;
    if (iCol >= LCD_WIDTH) {
      iCol = 0;
      iRow++;
    }

    
  }

  _hasUpdate = false; // Clear the hasUpdate flag
}

void MessageDumpScreen::Reset() {
  memset(_msg, 0, sizeof(_msg)); // Init the message buffer
  _mustDoFirstPrint = true;
}

void MessageDumpScreen::InitialPrint() {
  LiquidCrystal_I2C lcd = *this->_lcdPtr;
  lcd.clear();
  lcd.setCursor(0,0);

  _mustDoFirstPrint = false;
}

void MessageDumpScreen::ProcessMessage(NmeaParser* parser) {
  #ifdef DEBUG
  Serial.println(F("MessageDumpScreen ProcessMessage"));
  Serial.print(F("New sentence: ")); Serial.println(parser->getSentence());
  #endif

  memset(_msg, 0, sizeof(_msg)); // Init the message buffer
  parser->getSentence().toCharArray(this->_msg, NMEA_MSG_MAX_LENGTH);
  _hasUpdate = true; // Flag that there's something new
}
