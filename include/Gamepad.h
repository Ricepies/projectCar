#include <Arduino.h>

#ifndef _GAMEPAD_MACROS_
#define _GAMEPAD_MACROS_

#define buttonSelect 1
#define buttonL3 2
#define buttonR3 3
#define buttonStart 4
#define buttonUp 5
#define buttonRight 6
#define buttonDown 7
#define buttonLeft 8
#define buttonL2 9
#define buttonR2 10
#define buttonL1 11
#define buttonR1 12
#define buttonTriangle 13
#define buttonCircle 14
#define buttonX 15
#define buttonSquare 16

#define RX 5
#define RY 6
#define LX 7
#define LY 8

#endif

#ifndef _GAMEPAD_LIB_H_
#define _GAMEPAD_LIB_H_

class Gamepad
{
private:
    short MOSI, MISO, CS, SCK;

    unsigned short MASK[16] = {
        buttonSelect,
        buttonL3,
        buttonR3,
        buttonStart,
        buttonUp,
        buttonRight,
        buttonDown,
        buttonLeft,
        buttonL2,
        buttonR2,
        buttonL1,
        buttonR1,
        buttonTriangle,
        buttonCircle,
        buttonX,
        buttonSquare};

    unsigned short Handkey;
    short scan[9] = {0x01, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    short Data[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    unsigned long lastPressed[19] = {};
    bool dynamicButtonStatus[19] = {};

    short ReadData(short command);
    void ClearData();

public:
    Gamepad(short MOSIpin, short MISOpin, short CSpin, short SCKpin);
    void ScanForInput();
    short joystickData(short axis);
    bool buttonPressed(short button);
};

#endif