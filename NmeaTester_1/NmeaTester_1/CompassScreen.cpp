#define DEBUG true

#include "Constants.h"
#include "CompassScreen.h"
#include "NmeaParser.h"
#include "Utils.h"
#include <Arduino.h>

CompassScreen::CompassScreen(LiquidCrystal_I2C* lcdPtr) {
  _lcdPtr = lcdPtr;
  this->Reset();
  
}

void CompassScreen::ProcessChar(char newChar) {
  
}

void CompassScreen::TitlePrint() {
  LiquidCrystal_I2C lcd = *_lcdPtr;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(F("Compass"));
}

void CompassScreen::UpdatePrint() {
  if (!_hasUpdate) return;
  
  LiquidCrystal_I2C lcd = *_lcdPtr;
  char buffer[5]; memcpy(buffer, 0, sizeof buffer); // Init the buffer
  
  // Print magnetic bearing
  lcd.setCursor(2,0);
  if (!_hasBearingM) {
    lcd.print(COMPASS_EMPTY_NUMBER_CHAR);
  }
  else {
    // TODO print the magnetic bearing
    dtostrf(_bearingM, 5, 1, buffer);
    lcd.print(buffer);
  }

  // Print true bearing
  lcd.setCursor(2,1);
  if (!_hasBearingT) {
    lcd.print(COMPASS_EMPTY_NUMBER_CHAR);
  }
  else {
    dtostrf(_bearingT, 5, 1, buffer);
    lcd.print(buffer);
  }

  // Print the difference / deviation
  lcd.setCursor(10,0);
  if (_hasBearingM && _hasBearingT && (_bearingM != _bearingT)) {
    // TODO calculate and print the difference
    //dtostrf(abs(diff), 5, 1, buffer);
    //memcpy(buffer, 0, sizeof buffer); // Clear the buffer. Do I need to do this?
    dtostrf(abs(_diff), 5, 1, buffer); // Value, string width, decimal places, output buffer
    lcd.print(buffer);

    // Print the trailing East / West char
    if (_diff > 0) lcd.print(COMPASS_DIFF_POSITIVE_CHAR);
    else if (_diff < 0) lcd.print(COMPASS_DIFF_NEGATIVE_CHAR);
    else lcd.print(COMPASS_SPACE_CHAR);
  }
  else {
    lcd.print(COMPASS_EMPTY_NUMBER_CHAR);
  }

  _hasUpdate = false;
}

void CompassScreen::Reset() {
  _hasUpdate = true;
  _hasBearingM = false;
  _hasBearingT = false;
  _hasDiff = false;
  _bearingM = 0;
  _bearingT = 0;
  _diff = 0;
}

void CompassScreen::InitialPrint() {
  LiquidCrystal_I2C lcd = *_lcdPtr;
  lcd.clear();
  
  lcd.setCursor(0,0);
  lcd.print(COMPASS_BEARING_MAGNETIC_LABEL);
  lcd.print(COLON_CHAR);

  lcd.setCursor(0,1);
  lcd.print(COMPASS_BEARING_TRUE_LABEL);
  lcd.print(COLON_CHAR);

  lcd.setCursor(8,0);
  lcd.print(COMPASS_BEARING_DIFF_LABEL);
  lcd.print(COLON_CHAR);
}

void CompassScreen::ProcessMessage(NmeaParser* parser) {
  #if DEBUG
  Serial.println(F("CompassScreen ProcessMessage"));
  Serial.print(F("Val: ")); Serial.println(parser->getTerm(1).toFloat());
  #endif
  String messageDesc = parser->getMessageDescription();
  
  // Process a HDT message
  // $GPHDT,123.456,T*00
  // https://www.trimble.com/OEM_ReceiverHelp/V4.44/en/NMEA-0183messages_HDT.html
  if (messageDesc == "HDT") {
    #if DEBUG
    Serial.println(F("Processing HDT"));
    #endif
    
    float newValue = parser->getTerm(1).toFloat();
    if (_bearingT != newValue) {
      _bearingT = newValue;
      _hasBearingT = true;
      _hasUpdate = true;
      _CalculateDiff();
    }
    
    return;
  }

  // Process a HDM message
  // $GPHDM,123.456,M*00
  // https://gpsd.gitlab.io/gpsd/NMEA.html#_hdm_heading_magnetic
  if (messageDesc == "HDM") {
    #if DEBUG
    Serial.println(F("Processing HDM"));
    #endif
    float newValue = parser->getTerm(1).toFloat();
    if (_bearingM != newValue) {
      _bearingM = newValue;
      _hasBearingM = true;
      _hasUpdate = true;
      _CalculateDiff();
    }
    
    return;
  }
}

void CompassScreen::_CalculateDiff() {
  if (!_hasBearingM || !_hasBearingT) {
    _hasDiff = false;
    _diff = 0;
  }
  else
  {
    // Calculate the difference between Magnetic and True
    
    // TODO math is fucky.
    // If you set it both to 0.1, it says deviation is -90.
    // If you set it to 0.1 and 0.2, it shows a negative deviation (ABS should never let that happen).
    
    _diff = Utils::ConstrainAngle(_bearingM - _bearingT);
    #if DEBUG
    Serial.print(F("DiffCalc: ")); Serial.println(_diff);
    #endif
  }
}
