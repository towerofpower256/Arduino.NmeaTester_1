#ifndef COMPASSSCREEN_INCLUDED
#define COMPASSSCREEN_INCLUDED

#include "BaseScreen.h"
#include "NmeaParser.h"
#include <LiquidCrystal_I2C.h>

const char COMPASS_DIFF_POSITIVE_CHAR = 'W';
const char COMPASS_DIFF_NEGATIVE_CHAR = 'E';
const char COMPASS_BEARING_MAGNETIC_LABEL = 'M';
const char COMPASS_BEARING_TRUE_LABEL = 'T';
const char COMPASS_BEARING_DIFF_LABEL = 'D';
const char COMPASS_EMPTY_NUMBER_CHAR = '-';

class CompassScreen: public BaseScreen {
  public:
    CompassScreen(LiquidCrystal_I2C* lcdPtr);
    void InitialPrint();
    void UpdatePrint();
    void ProcessMessage(NmeaParser* parser);
    void Reset();
    void TitlePrint();
  private:
    LiquidCrystal_I2C * _lcdPtr;
    bool _hasUpdate;
    float _bearingM;
    bool _hasBearingM;
    float _bearingT;
    bool _hasBearingT;
};

#endif
