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

    virtual void setup() = 0;
    virtual void update() = 0;
};
#endif
