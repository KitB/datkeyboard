#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "KitBounce.h"

#define DEBOUNCED_STATE 0
#define UNSTABLE_STATE  1
#define STATE_CHANGED   3


Bounce::Bounce()
    : previous_millis(0)
    , up_millis(0)
    , down_millis(0)
    , interval_millis(10)
    , state(0)
    , pin(0)
{}

void Bounce::attach(int pin) {
    this->pin = pin;
    state = 0;
    if (digitalRead(pin)) {
        state = _BV(DEBOUNCED_STATE) | _BV(UNSTABLE_STATE);
    }
    previous_millis = millis();
}

void Bounce::attach(int pin, int mode){
  pinMode(pin, mode);
  this->attach(pin);
}

unsigned long Bounce::getUpAt(){
    return this->up_millis;
}

unsigned long Bounce::getDownAt(){
    return this->down_millis;
}

void Bounce::interval(uint16_t interval_millis)
{
    this->interval_millis = interval_millis;
}

void Bounce::setTime(bool currentState){
        previous_millis = millis();
        if(currentState){
            up_millis = previous_millis;
        }else{
            down_millis = previous_millis;
        }
}

bool Bounce::update()
{
    // Read the state of the switch in a temporary variable.
    bool currentState = digitalRead(pin);
    state &= ~_BV(STATE_CHANGED);

    // If the reading is different from last reading, reset the debounce counter
    if ( currentState != (bool)(state & _BV(UNSTABLE_STATE)) ) {
        setTime(currentState);
        state ^= _BV(UNSTABLE_STATE);
    } else
        if ( millis() - previous_millis >= interval_millis ) {
            // We have passed the threshold time, so the input is now stable
            // If it is different from last state, set the STATE_CHANGED flag
            if ((bool)(state & _BV(DEBOUNCED_STATE)) != currentState) {
                setTime(currentState);
                state ^= _BV(DEBOUNCED_STATE);
                state |= _BV(STATE_CHANGED);
            }
        }

    return state & _BV(STATE_CHANGED);
}

bool Bounce::read()
{
    return state & _BV(DEBOUNCED_STATE);
}

bool Bounce::rose()
{
    return ( state & _BV(DEBOUNCED_STATE) ) && ( state & _BV(STATE_CHANGED));
}

bool Bounce::fell()
{
    return !( state & _BV(DEBOUNCED_STATE) ) && ( state & _BV(STATE_CHANGED));
}
