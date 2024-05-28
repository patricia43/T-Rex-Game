#include "pitches.h"
// Define the pin for the buzzer

// Define the frequency and duration for the jump beep sound (e.g., 880 Hz for an A5 note)

int gameOverMelody[] = {
  NOTE_G3, 0, NOTE_FS3, 0, NOTE_F3, 0, NOTE_E3, 0, NOTE_B2
};

int gameOverNoteDurations[] = {
  8, 16, 8, 16, 8, 16, 8, 16, 4
};

void playGameOverMelodyWithVolume(int volume) {
  for (int thisNote = 0; thisNote < sizeof(gameOverMelody); thisNote++) {
    // Calculate the note duration, take one second divided by the note type.
    int noteDuration = 1000 / gameOverNoteDurations[thisNote];

    // Play the note with the desired volume
    playNoteWithVolume(gameOverMelody[thisNote], noteDuration, volume);

    // Stop the tone playing
    noTone(BUZZER_PIN);
  }
}

// Function to play a note with a specific volume using PWM
void playNoteWithVolume(int note, int duration, int volume) {
  if (note == 0) { // If the note is a rest
    delay(duration);
    return;
  }
  // Set up the PWM frequency for the buzzer pin
  int period = 1000000L / note; // Calculate the period in microseconds

  for (long i = 0; i < duration * 1000L; i += period) {
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(volume); // Adjust the on-time for volume control
    digitalWrite(BUZZER_PIN, LOW);
    delayMicroseconds(period - volume); // Adjust the off-time
  }
}
