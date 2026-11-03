#pragma once

#include <Arduino.h>

/**
 * @class Motor
 * @brief Simple dual-pin H-Bridge DC motor controller with PWM speed control.
 */
class Motor {
private:
    uint8_t pinA;
    uint8_t pinB;

public:
    /**
     * @brief Construct a new Motor driver instance.
     * @param pin1 Forward PWM pin
     * @param pin2 Reverse PWM pin
     */
    Motor(uint8_t pin1, uint8_t pin2);

    /**
     * @brief Set motor speed and direction.
     * @param speed Speed from -100 (full reverse) to 100 (full forward). 0 stops motor.
     */
    void run(int16_t speed);

    /**
     * @brief Immediately stop the motor by driving both pins LOW.
     */
    void stop();
};
