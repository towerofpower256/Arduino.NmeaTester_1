
#include "BaseScreen.h"
#include "NmeaParser.h"
#include <LiquidCrystal_I2C.h>


class MessageDumpScreen: public BaseScreen {
  public:
    MessageDumpScreen(LiquidCrystal_I2C* lcdPtr);
    virtual void InitialPrint();
    virtual void UpdatePrint(bool forcePrint);
    virtual void ProcessMessage(NmeaParser* parser);
    void ProcessChar(char newChar);
    virtual void Reset();
    virtual void TitlePrint();
  private:
    LiquidCrystal_I2C * _lcdPtr;
    char _msg[NMEA_MSG_MAX_LENGTH];
    bool _hasUpdate;
};
