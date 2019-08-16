#pragma once

#include "NmeaParser.h"

class BaseScreen {
  public:
    virtual void InitialPrint() = 0;
    virtual void UpdatePrint() = 0;
    virtual void ProcessMessage(NmeaParser* parser) = 0;
    virtual void ProcessChar(char newChar) = 0;
    virtual void Reset() = 0;
    virtual void TitlePrint() = 0;
};
