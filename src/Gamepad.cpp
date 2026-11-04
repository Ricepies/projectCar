#include "Gamepad.h"

namespace {

constexpr u8 kStartCmd = 0x01;
constexpr u8 kPollCmd = 0x42;
constexpr u32 kDebounceMs = 200;

// Compile-time lookup table mapping raw 0..255 axis bytes to centered -100..+100.
// Avoids runtime multiplication and division during SPI polling.
struct AxisLut {
    i8 table[256];

    constexpr AxisLut() : table{} {
        for (i32 i = 0; i < 256; ++i) {
            table[i] = static_cast<i8>((i - 128) * 100 / 127);
        }
    }
};

constexpr AxisLut kAxisLut{};

} // namespace

// =============================================================================
// Constructor & Initialization
// =============================================================================

Gamepad::Gamepad(u8 MOSIpin, u8 MISOpin, u8 CSpin, u8 SCKpin) : MOSI(MOSIpin), MISO(MISOpin), CS(CSpin), SCK(SCKpin) {
    pinMode(MISO, OUTPUT);
    pinMode(SCK, OUTPUT);
    pinMode(MOSI, INPUT);
    pinMode(CS, OUTPUT);

    digitalWrite(SCK, HIGH);
    digitalWrite(CS, HIGH);
    digitalWrite(MISO, HIGH);
}

// =============================================================================
// SPI Communication & Polling
// =============================================================================

u8 Gamepad::transferByte(u8 cmd) const {
    u8 in = 0;

    for (u8 bit = 0; bit < 8; ++bit) {
        digitalWrite(MISO, (cmd & 0x01U) ? HIGH : LOW);
        cmd >>= 1;

        delayMicroseconds(10);
        digitalWrite(SCK, LOW);
        delayMicroseconds(10);

        if (digitalRead(MOSI) == HIGH) {
            in |= static_cast<u8>(1U << bit);
        }

        digitalWrite(SCK, HIGH);
        delayMicroseconds(10);
    }

    digitalWrite(MISO, HIGH);
    delayMicroseconds(50);
    return in;
}

void Gamepad::ScanForInput() {
    digitalWrite(CS, LOW);

    // Bytes 0-2: Header handshake & mode detection
    (void)transferByte(kStartCmd);
    controllerMode = transferByte(kPollCmd);
    (void)transferByte(0x00);

    // Bytes 3-4: 16 digital buttons (active-LOW on wire -> invert to active-HIGH bitfield)
    const u8 btnLow = transferByte(0x00);
    const u8 btnHigh = transferByte(0x00);
    buttonsDown = static_cast<u16>(~(static_cast<u16>(btnLow) | (static_cast<u16>(btnHigh) << 8)));

    // Bytes 5-8: Analog sticks (RX, RY, LX, LY). Y axes are inverted so forward is +100.
    axes[0] = kAxisLut.table[transferByte(0x00)];
    axes[1] = kAxisLut.table[static_cast<u8>(255U - transferByte(0x00))];
    axes[2] = kAxisLut.table[transferByte(0x00)];
    axes[3] = kAxisLut.table[static_cast<u8>(255U - transferByte(0x00))];

    digitalWrite(CS, HIGH);
}

void Gamepad::scanForInput() {
    ScanForInput();
}

// =============================================================================
// Joystick Data Accessors
// =============================================================================

i16 Gamepad::joystickData(GamepadAxis axis) const {
    return joystickData(static_cast<u8>(axis));
}

i16 Gamepad::joystickData(u8 axis) const {
    const u8 idx = static_cast<u8>(axis - RX);
    return (idx < 4) ? static_cast<i16>(axes[idx]) : 0;
}

i16 Gamepad::getLX() const {
    return axes[2];
}

i16 Gamepad::getLY() const {
    return axes[3];
}

i16 Gamepad::getRX() const {
    return axes[0];
}

i16 Gamepad::getRY() const {
    return axes[1];
}

// =============================================================================
// Button State Accessors
// =============================================================================

bool Gamepad::isButtonDown(GamepadButton button) const {
    return isButtonDown(static_cast<u8>(button));
}

bool Gamepad::isButtonDown(u8 button) const {
    const u8 idx = static_cast<u8>(button - 1);
    return (idx < 16) && (((buttonsDown >> idx) & 1U) != 0);
}

bool Gamepad::buttonPressed(GamepadButton button) {
    return buttonPressed(static_cast<u8>(button));
}

bool Gamepad::buttonPressed(u8 button) {
    const u8 idx = static_cast<u8>(button - 1);
    if (idx >= 16 || ((buttonsDown >> idx) & 1U) == 0) {
        return false;
    }

    const u32 now = millis();
    if (now - lastPressed[idx] > kDebounceMs) {
        lastPressed[idx] = now;
        return true;
    }
    return false;
}

// =============================================================================
// Controller Mode Accessor
// =============================================================================

u8 Gamepad::getControllerMode() const {
    return controllerMode;
}
