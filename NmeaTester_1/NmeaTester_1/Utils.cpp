#include "Utils.h"

// Constrain / normalize an angle value to accomodate for angle-wrap
// 0.8 - 353.60 should equal -7.2, not -352.8
// 45 - 315 should equal -90, not -270
// Credit: https://stackoverflow.com/questions/11498169/dealing-with-angle-wrap-in-c-code
float Utils::ConstrainAngle(float x) {
  x = fmod(x + 180, 360);
  if (x < 0)
    x += 360;
  
  x = x - 180;
  return x;
}
