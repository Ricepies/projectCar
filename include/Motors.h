#pragma once

#include <Arduino.h>
#include "Types.h"

/**
 * @class Motor
 * @brief Simple dual-pin H-Bridge DC motor controller with PWM speed control.
 */
class Motor {
private:
    u8 pinA;
    u8 pinB;

public:
    /**
     * @brief Construct a new Motor driver instance.
     * @param pin1 Forward PWM pin
     * @param pin2 Reverse PWM pin
     */
    Motor(u8 pin1, u8 pin2);

    /**
     * @brief Set motor speed and direction.
     * @param speed Speed from -100 (full reverse) to 100 (full forward). 0 stops motor.
     */
    void run(i16 speed);

    /**
     * @brief Immediately stop the motor by driving both pins LOW.
     */
    void stop();
};
