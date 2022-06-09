#include <Arduino.h>

using namespace std;

const int speakerPin = 14;

/**
 *     A          B         C         D         E        F         G
 * {27.50000, 30.86771, 16.35160, 18.35405, 20.60172, 21.82676, 24.49971}
 */
const float frequencies[] = {27.50000, 30.86771, 16.35160, 18.35405, 20.60172, 21.82676, 24.49971};

string score = "G A B C5 B C5 G A A G F E E E F D G A B C5 B C5 G A A A B C5 C5 C5 C5 D5 D5 D5 - G A B C5 B C5 E5 A5 A5 G5 F5 E5 E5 E5 E5 D5 D5 A B C5 C5 C5 C5 D5 C5 C5 B C5 D5 D5 D5 D5 C5 B C5 C5 C5 C5";

void playMelody(string noteNames, int tempo = 120)
{
    tempo = 60000 / tempo;
    noteNames += "\0";
    for (int i = 0; i < noteNames.length(); i += 2)
    {
        digitalWrite(speakerPin, LOW);
        if (noteNames[i] == "-"[0])
        {
            digitalWrite(speakerPin, LOW);
            delay(tempo);
            continue;
        }
        else if (noteNames[i + 1] != '\0' && noteNames[i + 1] != ' ')
        {
            tone(speakerPin, frequencies[noteNames[i] - 'A'] * pow(2, noteNames[i + 1] - '0'), tempo);
            i++;
        }
        else
            tone(speakerPin, frequencies[noteNames[i] - 'A'] * 16, tempo);
        delay(tempo);
    }
    digitalWrite(speakerPin, LOW);
    return;
}

void setup()
{
    pinMode(speakerPin, OUTPUT);
    playMelody(score);
}
void loop()
{
}
