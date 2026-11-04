#include "Gamepad.h"

Gamepad::Gamepad(u8 MOSIpin, u8 MISOpin, u8 CSpin, u8 SCKpin) : MOSI(MOSIpin), MISO(MISOpin), CS(CSpin), SCK(SCKpin) {
    pinMode(MISO, OUTPUT);
    pinMode(SCK, OUTPUT);
    pinMode(MOSI, INPUT);
    pinMode(CS, OUTPUT);

    digitalWrite(SCK, HIGH);
    digitalWrite(CS, HIGH);
    digitalWrite(MISO, HIGH);
}

u8 Gamepad::ReadData(u8 command) {
    u8 res = 0;
    u8 j = 1;

    for (u8 i = 0; i < 8; i++) {
        if (command & 0x01) {
            digitalWrite(MISO, HIGH);
        } else {
            digitalWrite(MISO, LOW);
        }

        command >>= 1;
        delayMicroseconds(10);
        digitalWrite(SCK, LOW);
        delayMicroseconds(10);

        if (digitalRead(MOSI) == HIGH) {
            res += j;
        }

        j <<= 1;
        digitalWrite(SCK, HIGH);
        delayMicroseconds(10);
    }

    digitalWrite(MISO, HIGH);
    delayMicroseconds(50);
    return res;
}

void Gamepad::ClearData() {
    memset(dynamicButtonStatus, false, sizeof(dynamicButtonStatus));
    memset(Data, 0, sizeof(Data));
}

void Gamepad::ScanForInput() {
    ClearData();

    digitalWrite(CS, LOW);
    for (u8 i = 0; i < 9; i++) {
        Data[i] = ReadData(scan[i]);
    }
    digitalWrite(CS, HIGH);

    Handkey = (static_cast<u16>(Data[4]) << 8) | Data[3];
    for (u8 index = 0; index < 16; index++) {
        if ((Handkey & (1U << (MASK[index] - 1))) == 0) {
            dynamicButtonStatus[index + 1] = true;
        }
    }
}

i16 Gamepad::joystickData(GamepadAxis axis) {
    return joystickData(static_cast<u8>(axis));
}

i16 Gamepad::joystickData(u8 axis) {
    u8 data = Data[axis];
    if (axis == LX || axis == RX) {
        return (static_cast<i16>(data) - 128) * 100 / 127;
    }
    return (255 - static_cast<i16>(data) - 128) * 100 / 127;
}

bool Gamepad::buttonPressed(GamepadButton button) {
    return buttonPressed(static_cast<u8>(button));
}

bool Gamepad::buttonPressed(u8 button) {
    if (button < 19 && dynamicButtonStatus[button] && (millis() - lastPressed[button] > 200)) {
        lastPressed[button] = millis();
        return true;
    }
    return false;
}
