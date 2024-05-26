#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <SPI.h>
#include "common.h"

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

#define START_MENU 0
#define GAME 1
#define GAME_OVER 2
#define LEADERBOARD 3
#define USER_SELECT 4

#define jump_duration 3

#define downButton 2
#define upButton 7
#define okButton 4

#define START_ACTION 0
#define SELECT_ACTION 1
#define LEADERBOARD_ACTION 2

void initMenu() {
  crtLine = 1;
  crtLine2 = 0;
  cursorOnTopPos = true;
}

void setup() {
  // SD Card
  InitializeCard();

  // LCD Screen
  lcd.init();         // initialize the lcd
  lcd.backlight();    // Turn on the LCD screen backlight

  // Buttons
  pinMode(downButton, INPUT); // initialize the pushbutton pin as an input
  pinMode(okButton, INPUT);
  pinMode(upButton, INPUT); // initialize the pushbutton pin as an input

  // Initialize graphics
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