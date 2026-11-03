/**
 * @file autonomous_steering.cpp
 * @brief Autonomous directional movement and turning routines for 4WD robot car.
 */

#include <Arduino.h>
#include "Motors.h"

// 4WD Motors (Front-Left, Front-Right, Rear-Left, Rear-Right)
Motor motorFL(0, 1);
Motor motorFR(2, 3);
Motor motorRL(6, 7);
Motor motorRR(4, 5);

enum Direction {
    DIR_LEFT = 1,
    DIR_RIGHT = 2,
    DIR_FORWARD = 3,
    DIR_BACKWARD = 4
};

void move(Direction direction, int speed)
{
    switch (direction)
    {
    case DIR_FORWARD:
        motorFL.run(speed);
        motorFR.run(speed);
        motorRL.run(speed);
        motorRR.run(speed);
        break;
    case DIR_BACKWARD:
        move(DIR_FORWARD, -speed);
        break;
    case DIR_LEFT:
        motorFL.run(-speed);
        motorFR.run(speed);
        motorRL.run(speed);
        motorRR.run(-speed);
        break;
    case DIR_RIGHT:
        move(DIR_LEFT, -speed);
        break;
    }
}

void turn(Direction direction, int speed)
{
    switch (direction)
    {
    case DIR_LEFT:
        motorFL.run(-speed);
        motorFR.run(speed);
        motorRL.run(-speed);
        motorRR.run(speed);
        break;
    case DIR_RIGHT:
        turn(DIR_LEFT, -speed);
        break;
    default:
        break;
    }
}

void stopAll()
{
    motorFL.stop();
    motorFR.stop();
    motorRL.stop();
    motorRR.stop();
}

void setup()
{
    // Initialize serial monitor for debug
    Serial.begin(115200);
}

void loop()
{
    // Simple autonomous test sequence
    move(DIR_FORWARD, 40);
    delay(2000);

    stopAll();
    delay(500);

    turn(DIR_LEFT, 40);
    delay(1000);

    stopAll();
    delay(500);

    turn(DIR_RIGHT, 40);
    delay(1000);

    stopAll();
    delay(2000);
}
