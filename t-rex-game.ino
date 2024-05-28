#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <SPI.h>
#include "common.h"

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// states
#define START_MENU 0
#define GAME 1
#define GAME_OVER 2
#define LEADERBOARD 3
#define USER_SELECT 4

// pins
#define downButton 2
#define upButton 7
#define okButton 4
#define BUZZER_PIN 8

// sound
#define JUMP_SOUND_FREQUENCY 800 // A5
#define JUMP_SOUND_DURATION 75 // jump sound duration in milliseconds

#define VOLUME 128

// graphics
void graphics();

// logic
void switchUser(String userName);
int getUserHighScore(String userName);
byte countLines(File &file);
void updateLeaderboard();
void InitializeCard();
void readMyFile();
String readLine(File &file);
byte getDigitCount(int n);

// gameplay
void initGame();
void detectCollision();
void game();

// menus
void initMenu();
void startMenu();
void userSelect();
void gameOver();
void leaderboard();

// sound
void playGameOverMelodyWithVolume(int volume);
void playNoteWithVolume(int note, int duration, int volume);

void setup() {
  // Debugging 
  // Serial.begin(9060);
  // while(!Serial) {};

  // SD Card
  InitializeCard();

  // LCD Screen
  lcd.init();         // initialize the lcd
  lcd.backlight();    // Turn on the LCD screen backlight

  // Buttons
  pinMode(downButton, INPUT);
  pinMode(okButton, INPUT);
  pinMode(upButton, INPUT);

  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT);

  // Graphics
  graphics();

  crtUser = "Dino1";
  crtUsersHighScore = getUserHighScore(crtUser);
}

void loop() {
  switch (state) {
    case START_MENU:
      startMenu();
      break;
    case GAME:
      game();
      break;
    case GAME_OVER:
      gameOver();
      break;
    case LEADERBOARD:
      leaderboard();
      break;
    case USER_SELECT:
      userSelect();
      break;
  }
}