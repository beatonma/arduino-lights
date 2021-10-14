#include "input.h"

/**
 * Passive infra-red sensor
 */
class AbstractPirHandler: AbstractInputHandler {
    public:
    AbstractPirHandler(uint8_t pin): AbstractInputHandler(pin) {}

    // Motion has been detected.
    virtual void onMotionEventStart(void) {}

    // Motion continues to be detected.
    virtual void onMotionEventContinued(void) {}

    // Motion is no longer detected.
    virtual void onMotionEventEnd(void) {}

    // Motion is still not detected.
    virtual void onIdle(void) {}

    // Input state changed either on or off.
    virtual void onChange(void) {}

    void setup(void) {
        setPinMode(INPUT);
    }

    void update(void) {
        current_value_ = readDigital();
        current_timestamp_ = getTimestamp();

        if (current_value_ != previous_value_) {
            updateWithNewState();
        }
        else {//if (action_started_timestamp_ > 0) {
            updateWithContinuingState();
        }

        previous_value_ = current_value_;
        current_timestamp_ = 0;
    }

    protected:
    unsigned long previous_value_ = false;
    unsigned long current_value_ = false;

    private:
    unsigned long current_timestamp_ = 0;
    unsigned long action_started_timestamp_ = 0;
    unsigned long action_duration_ = 0;
    bool action_consumed_ = false;

    void updateWithNewState(void) {
        onChange();

        if (current_value_ == HIGH) {
            action_started_timestamp_ = current_timestamp_;
            onMotionEventStart();
        } else {
            if (!action_consumed_) {
                onMotionEventEnd();
            }

            action_started_timestamp_ = 0;
            action_duration_ = 0;
            action_consumed_ = false;
        }
    }

    void updateWithContinuingState(void) {
        if (action_consumed_) {
            return;
        }

        if (current_value_ == HIGH) {
            onMotionEventContinued();
        }
        else {
            onIdle();
        }
    }
};
