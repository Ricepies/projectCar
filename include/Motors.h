#include <Arduino.h>

#ifndef _MOTORS_H_
#define _MOTORS_H_

class Motor
{
private:
    char pinA, pinB;

public:
    Motor(char pin1, char pin2);
    void run(short speed);
    void stop();
};

#endif