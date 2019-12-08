#include <Arduino.h>
#ifndef INPUT_H
#define INPUT_H
class AbstractInputHandler
{
    public:
    byte pin_;

    AbstractInputHandler(byte pin) {
        pin_ = pin;
    }

    virtual void setup(void) = 0;
    virtual void update(void) = 0;
};
#endif
