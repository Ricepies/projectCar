#include "Gamepad.h"

short Gamepad::ReadData(short command)
{
    short i, j = 1, res = 0;
    for (i = 0; i <= 7; i++)
    {
        if (command & 0x01)
            digitalWrite(MISO, HIGH);
        else
            digitalWrite(MISO, LOW);
        command = command >> 1;
        delayMicroseconds(10);
        digitalWrite(SCK, LOW);
        delayMicroseconds(10);
        if (digitalRead(MOSI) == HIGH)
            res = res + j;
        j = j << 1;
        digitalWrite(SCK, HIGH);
        delayMicroseconds(10);
    }
    digitalWrite(MISO, HIGH);
    delayMicroseconds(50);
    return res;
}

void Gamepad::ClearData()
{
    memset(dynamicButtonStatus, false, sizeof(dynamicButtonStatus) / sizeof(dynamicButtonStatus[0]));
    memset(Data, 0, sizeof(Data) / sizeof(Data[0]) * sizeof(short));
    return;
}

Gamepad::Gamepad(short MOSIpin, short MISOpin, short CSpin, short SCKpin)
{
    MOSI = MOSIpin;
    MISO = MISOpin;
    CS = CSpin;
    SCK = SCKpin;
    pinMode(MISO, OUTPUT);
    pinMode(SCK, OUTPUT);
    pinMode(MOSI, INPUT);
    pinMode(CS, OUTPUT);

    digitalWrite(SCK, HIGH);
    digitalWrite(CS, HIGH);
    digitalWrite(MISO, HIGH);
}

void Gamepad::ScanForInput()
{
    short index = 0, i = 0;
    ClearData();

    digitalWrite(CS, LOW);
    for (i = 0; i < 9; i++)
        Data[i] = ReadData(scan[i]);
    digitalWrite(CS, HIGH);

    Handkey = (Data[4] << 8) | Data[3];
    for (index = 0; index < 16; index++)
    {
        if ((Handkey & (1 << (MASK[index] - 1))) == 0)
            dynamicButtonStatus[index + 1] = true;
    }
    return;
}

short Gamepad::joystickData(short axis)
{
    short data = Data[axis];
    if (axis == LX || axis == RX)
        return (data - 128) * 100 / 127;
    return (255 - data - 128) * 100 / 127;
}

bool Gamepad::buttonPressed(short button)
{
    if (dynamicButtonStatus[button] && millis() - lastPressed[button] > 200)
    {
        lastPressed[button] = millis();
        return true;
    }
    return false;
}
