#include <Motors.cpp>
#include <Gamepad.cpp>

#define buzzerPin 16

Motor motorFL(0, 1);
Motor motorFR(2, 3);
Motor motorRL(6, 7);
Motor motorRR(4, 5);

Gamepad gamepad(12, 13, 14, 15);

bool motorStarted = false;

void setup()
{
}

void loop()
{
    gamepad.ScanForInput();
    int lx = gamepad.joystickData(LX);
    int ly = gamepad.joystickData(LY);
    int rx = gamepad.joystickData(RX);
    int ry = gamepad.joystickData(RY);

    if (gamepad.buttonPressed(buttonStart))
    {
        motorStarted = !motorStarted;
        digitalWrite(buzzerPin, HIGH);
        delay(100);
        digitalWrite(buzzerPin, LOW);
    }

    if (motorStarted)
    {
        if (lx || ly)
        {
            motorFL.run(lx + ly);
            motorFR.run(ly - lx);
            motorRL.run(lx + ly);
            motorRR.run(ly - lx);
        }
        else
        {
            motorFL.run(ry + rx);
            motorFR.run(ry - rx);
            motorRL.run(ry - rx);
            motorRR.run(ry + rx);
        }
    }
    else
    {
        motorFL.run(0);
        motorFR.run(0);
        motorRL.run(0);
        motorRR.run(0);
    }

    delay(50);
}