#define DEBUG true

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

void MessageDumpScreen::UpdatePrint() {
  if (!_hasUpdate) return; // Don't do anything if there's nothing new;
  
  LiquidCrystal_I2C lcd = *this->_lcdPtr;
  lcd.clear();
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
    }
    else {
      nextChar = SPACE_CHAR;
    }

    iCol++;
    if (iCol >= LCD_WIDTH) {
      iCol = 0;
      iRow++;
    }

    lcd.print(nextChar);
  }

  _hasUpdate = false; // Clear the hasUpdate flag
}

void MessageDumpScreen::Reset() {
  memset(_msg, 0, sizeof(_msg)); // Init the message buffer
  
}

void MessageDumpScreen::InitialPrint() {
  LiquidCrystal_I2C lcd = *this->_lcdPtr;
  lcd.clear();
  lcd.setCursor(0,0);
}

void MessageDumpScreen::ProcessMessage(NmeaParser* parser) {
  #if DEBUG
  Serial.println(F("MessageDumpScreen ProcessMessage"));
  #endif
  
  memset(_msg, 0, sizeof(_msg)); // Init the message buffer
  parser->getSentence().toCharArray(this->_msg, NMEA_MSG_MAX_LENGTH);
  _hasUpdate = true; // Flag that there's something new
}
