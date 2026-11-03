/**
 * @file buzzer_melody.cpp
 * @brief Standalone melody playback demonstration using tone() on a passive buzzer.
 */

#include <Arduino.h>
#include <string>
#include <cmath>

using namespace std;

const int speakerPin = 16; // Pico buzzer pin (GP16)

/**
 * Base note frequencies:
 *     A          B         C         D         E        F         G
 * {27.50000, 30.86771, 16.35160, 18.35405, 20.60172, 21.82676, 24.49971}
 */
const float frequencies[] = {27.50000, 30.86771, 16.35160, 18.35405, 20.60172, 21.82676, 24.49971};

const string score = "G A B C5 B C5 G A A G F E E E F D G A B C5 B C5 G A A A B C5 C5 C5 C5 D5 D5 D5 - "
                     "G A B C5 B C5 E5 A5 A5 G5 F5 E5 E5 E5 E5 D5 D5 A B C5 C5 C5 C5 D5 C5 C5 B C5 D5 D5 D5 D5 C5 B C5 C5 C5 C5";

void playMelody(const string &noteNames, int tempo = 120)
{
    int noteDuration = 60000 / tempo;
    string notes = noteNames + "\0";

    for (size_t i = 0; i < notes.length(); i += 2)
    {
        digitalWrite(speakerPin, LOW);
        if (notes[i] == '-')
        {
            digitalWrite(speakerPin, LOW);
            delay(noteDuration);
            continue;
        }
        else if (notes[i + 1] != '\0' && notes[i + 1] != ' ')
        {
            tone(speakerPin, frequencies[notes[i] - 'A'] * pow(2, notes[i + 1] - '0'), noteDuration);
            i++;
        }
        else if (notes[i] >= 'A' && notes[i] <= 'G')
        {
            tone(speakerPin, frequencies[notes[i] - 'A'] * 16, noteDuration);
        }
        delay(noteDuration);
    }
    digitalWrite(speakerPin, LOW);
}

void setup()
{
    pinMode(speakerPin, OUTPUT);
    playMelody(score);
}

void loop()
{
    // Idle after playing
    delay(1000);
}
