#include "Motors.h"

Motor::Motor(char pin1, char pin2)
{
    pinA = pin1;
    pinB = pin2;
    pinMode(pinA, OUTPUT);
    pinMode(pinB, OUTPUT);
    return;
}

void Motor::run(short speed)
{
    analogWrite(pinB, 0);
    analogWrite(pinA, 0);

    if (speed == 0)
        return;
    else if (speed > 100)
        speed = 100;
    else if (speed < -100)
        speed = -100;

    analogWrite(speed > 0 ? pinA : pinB, abs(speed) * 2 + 40);
    return;
}

void Motor::stop()
{
    analogWrite(pinA, 0);
    analogWrite(pinB, 0);
    return;
}