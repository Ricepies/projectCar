#include "Gamepad.h"

Gamepad::Gamepad(uint8_t MOSIpin, uint8_t MISOpin, uint8_t CSpin, uint8_t SCKpin)
    : MOSI(MOSIpin), MISO(MISOpin), CS(CSpin), SCK(SCKpin)
{
    pinMode(MISO, OUTPUT);
    pinMode(SCK, OUTPUT);
    pinMode(MOSI, INPUT);
    pinMode(CS, OUTPUT);

    digitalWrite(SCK, HIGH);
    digitalWrite(CS, HIGH);
    digitalWrite(MISO, HIGH);
}

uint8_t Gamepad::ReadData(uint8_t command)
{
    uint8_t res = 0;
    uint8_t j = 1;

    for (int i = 0; i < 8; i++)
    {
        if (command & 0x01)
            digitalWrite(MISO, HIGH);
        else
            digitalWrite(MISO, LOW);

        command >>= 1;
        delayMicroseconds(10);
        digitalWrite(SCK, LOW);
        delayMicroseconds(10);

        if (digitalRead(MOSI) == HIGH)
            res += j;

        j <<= 1;
        digitalWrite(SCK, HIGH);
        delayMicroseconds(10);
    }

    digitalWrite(MISO, HIGH);
    delayMicroseconds(50);
    return res;
}

void Gamepad::ClearData()
{
    memset(dynamicButtonStatus, false, sizeof(dynamicButtonStatus));
    memset(Data, 0, sizeof(Data));
}

void Gamepad::ScanForInput()
{
    ClearData();

    digitalWrite(CS, LOW);
    for (int i = 0; i < 9; i++)
    {
        Data[i] = ReadData(scan[i]);
    }
    digitalWrite(CS, HIGH);

    Handkey = (static_cast<uint16_t>(Data[4]) << 8) | Data[3];
    for (int index = 0; index < 16; index++)
    {
        if ((Handkey & (1 << (MASK[index] - 1))) == 0)
        {
            dynamicButtonStatus[index + 1] = true;
        }
    }
}

int16_t Gamepad::joystickData(uint8_t axis)
{
    uint8_t data = Data[axis];
    if (axis == LX || axis == RX)
    {
        return (static_cast<int>(data) - 128) * 100 / 127;
    }
    return (255 - static_cast<int>(data) - 128) * 100 / 127;
}

bool Gamepad::buttonPressed(uint8_t button)
{
    if (button < 19 && dynamicButtonStatus[button] && (millis() - lastPressed[button] > 200))
    {
        lastPressed[button] = millis();
        return true;
    }
    return false;
}
