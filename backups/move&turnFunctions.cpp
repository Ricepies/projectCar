#include <Motors.cpp>

Motor motorFL(0, 1);
Motor motorFR(2, 3);
Motor motorRL(6, 7);
Motor motorRR(4, 5);

#define LEFT 1
#define RIGHT 2
#define FORWARD 3
#define BACKWARD 4

void move(int direction, int speed)
{
    switch (direction)
    {
    case FORWARD:
        motorFL.run(speed);
        motorFR.run(speed);
        motorRL.run(speed);
        motorRR.run(speed);
        break;
    case BACKWARD:
        move(FORWARD, -speed);
        break;
    case LEFT:
        motorFL.run(-speed);
        motorFR.run(speed);
        motorRL.run(speed);
        motorRR.run(-speed);
        break;
    case RIGHT:
        move(LEFT, -speed);
        break;
    }
    return;
}

void turn(int direction, int speed)
{
    switch (direction)
    {
    case LEFT:
        motorFL.run(-speed);
        motorFR.run(speed);
        motorRL.run(-speed);
        motorRR.run(speed);
        break;
    case RIGHT:
        turn(LEFT, -speed);
        break;
    default:
        break;
    }
    return;
}

void setup()
{
}
void loop()
{
    move(LEFT, 30);
    delay(2000);
    move(RIGHT, 30);
    delay(2000);
}