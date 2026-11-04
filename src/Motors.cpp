#include "Motors.h"

Motor::Motor(u8 pin1, u8 pin2) : pinA(pin1), pinB(pin2) {
    pinMode(pinA, OUTPUT);
    pinMode(pinB, OUTPUT);
    stop();
}

void Motor::run(i16 speed) {
    if (speed == 0) {
        stop();
        return;
    }

    if (speed > 100) {
        speed = 100;
    } else if (speed < -100) {
        speed = -100;
    }

    // Map speed (1..100) to PWM duty cycle (approx 42..240)
    int pwmVal = abs(speed) * 2 + 40;

    if (speed > 0) {
        analogWrite(pinB, 0);
        analogWrite(pinA, pwmVal);
    } else {
        analogWrite(pinA, 0);
        analogWrite(pinB, pwmVal);
    }
}

void Motor::stop() {
    analogWrite(pinA, 0);
    analogWrite(pinB, 0);
}
