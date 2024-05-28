#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <SPI.h>
#include "common.h"

#define START_ACTION 0
#define SELECT_ACTION 1
#define LEADERBOARD_ACTION 2

void initMenu() {
  crtLine = 1;
  crtLine2 = 0;
  cursorOnTopPos = true;
}

void startMenu() {
  DOWNbuttonState = digitalRead(downButton);
  UPbuttonState = digitalRead(upButton);
  OKbuttonState = digitalRead(okButton);

  // Handling button presses
  static String actions[3] = {"START", "SELECT USER", "LEADERBOARD"};
  if (DOWNbuttonState == HIGH && DOWNbuttonState != DOWNlastButtonState) {
    if (crtLine2 < 2) {
      crtLine2++;
      if (!cursorOnTopPos) { 
        lcd.clear();
      } else {
        // clear cursor
        lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.print(" ");
        cursorOnTopPos = !cursorOnTopPos;
      }
    }
  } else if (UPbuttonState == HIGH && UPbuttonState != UPlastButtonState) {
    if (crtLine2 > 0) {
      crtLine2--;
      if (cursorOnTopPos) { 
        lcd.clear();
      } else {
        // clear cursor
        lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.print(" ");
        cursorOnTopPos = !cursorOnTopPos;
      }
    }
  }

  // on top row
  lcd.setCursor(1, 0);
  if (cursorOnTopPos) {
    lcd.print(actions[crtLine2]);
  } else {
    lcd.print(actions[crtLine2 - 1]);
  }
  // on bottom row
  lcd.setCursor(1, 1);
  if (!cursorOnTopPos) {
    lcd.print(actions[crtLine2]);
  } else {
    lcd.print(actions[crtLine2 + 1]);
  }

  if (cursorOnTopPos) {
    lcd.setCursor(0, 0);
  } else {
    lcd.setCursor(0, 1);
  }
  lcd.print(">");

  if (OKbuttonState == HIGH && OKbuttonState != OKlastButtonState) {
    switch(crtLine2) {
      case START_ACTION:
        lcd.clear();
        state = GAME;
        initGame();
        break;
      case SELECT_ACTION:
        lcd.clear();
        state = USER_SELECT;
        initMenu();
        break;
      case LEADERBOARD_ACTION:
        lcd.clear();
        state = LEADERBOARD;
        initMenu();
        lcd.clear();
        break;
    }
  }

  DOWNlastButtonState = DOWNbuttonState;
  UPlastButtonState = UPbuttonState;
  OKlastButtonState = OKbuttonState;

  delay(120);
}

void userSelect() {
  DOWNbuttonState = digitalRead(downButton);
  UPbuttonState = digitalRead(upButton);
  OKbuttonState = digitalRead(okButton);

  // Handling button presses
  if (DOWNbuttonState == HIGH && DOWNbuttonState != DOWNlastButtonState) {
    crtLine2++;
    lcd.clear();
    if (crtLine2 > lineCount - 1) {
      crtLine2 = 0;
    }
  } else if (UPbuttonState == HIGH && UPbuttonState != UPlastButtonState) {
    if (crtLine2 > 0) {
      crtLine2--;
      lcd.clear();
      // if (crtLine2 < 0) {
      //   crtLine2 = lineCount - 1;
      // }
    }
  }

  // Displaying leaderboard entries

  // Open the leaderboard file
  File file = SD.open("users.txt");
  if (!file) {
    return;
  }

  // Move to the correct line in the file
  String previousLine ="";

  for (int i = 0; i < crtLine2; i++) {
    if (!file.available()) {
      // Reached the end of the file
      file.close();
      return;
    }
    // The previous line will be displayed on the top row
    previousLine = readLine(file);
  }
  previousLine.remove(previousLine.length() - 1);
  if (crtLine2 > 0) {
    // Display top row:
    lcd.setCursor(0, 0);
    lcd.print(previousLine);
  } else {
    lcd.setCursor(0, 0);
    lcd.print("SELECT:");
  }

  // Display the current line on the bottom row
  String line = readLine(file);
  line.remove(line.length() - 1);
  lcd.setCursor(0, 1);
  lcd.print(">");
  lcd.print(line);

  file.close();

  if (OKbuttonState == HIGH && OKbuttonState != OKlastButtonState) {
    lcd.clear();
    state = START_MENU;
    initMenu();
    switchUser(line);
  }

  DOWNlastButtonState = DOWNbuttonState;
  UPlastButtonState = UPbuttonState;
  OKlastButtonState = OKbuttonState;

  delay(120);
}

void gameOver() {
  DOWNbuttonState = digitalRead(downButton);
  UPbuttonState = digitalRead(upButton);
  OKbuttonState = digitalRead(okButton);

  lcd.setCursor(0, 0);
  lcd.print("GAME OVER!");
  lcd.setCursor(0, 1);
  lcd.print("SCORE: ");
  lcd.print(score - 1);

  if (OKbuttonState == HIGH && OKbuttonState != OKlastButtonState) {
      lcd.clear();
      state = LEADERBOARD;
      initMenu();
    }

  DOWNlastButtonState = DOWNbuttonState;
  UPlastButtonState = UPbuttonState;
  OKlastButtonState = OKbuttonState;
}

void leaderboard() {
  DOWNbuttonState = digitalRead(downButton);
  UPbuttonState = digitalRead(upButton);
  OKbuttonState = digitalRead(okButton);

  // Handling button presses
  if (DOWNbuttonState == HIGH && DOWNbuttonState != DOWNlastButtonState) {
    crtLine++;
    lcd.clear();
    if (crtLine > lineCount - 1) {
      crtLine = 1;
    }
  } else if (UPbuttonState == HIGH && UPbuttonState != UPlastButtonState) {
    crtLine--;
    lcd.clear();
    if (crtLine < 1) {
      crtLine = lineCount - 1;
    }
  }

  // Displaying leaderboard entries

  // Open the leaderboard file
  File file = SD.open("lead.csv");
  if (!file) {
    return;
  }

  // Move to the correct line in the file
  String previousLine ="";

  for (byte i = 0; i < crtLine; i++) {
    if (!file.available()) {
      // Reached the end of the file
      file.close();
      return;
    }
    // The previous line will be displayed on the top row
    previousLine = readLine(file);
  }
  byte commaIndex = previousLine.indexOf(',');
  String playerName = previousLine.substring(0, commaIndex);
  int playerScore = previousLine.substring(commaIndex + 1).toInt();

  // Display top row:
  lcd.setCursor(0, 0);
  lcd.print(crtLine);
  lcd.print(" ");
  lcd.print(playerName);
  lcd.setCursor(16 - getDigitCount(playerScore), 0);
  lcd.print(playerScore);

  // Display the current line on the bottom row
  String line = readLine(file);
  commaIndex = line.indexOf(',');
  playerName = line.substring(0, commaIndex);
  playerScore = line.substring(commaIndex + 1).toInt();
  lcd.setCursor(0, 1);
  lcd.print(crtLine + 1);
  lcd.print(" ");
  lcd.print(playerName);
  lcd.setCursor(16 - getDigitCount(playerScore), 1);
  lcd.print(playerScore);
  file.close();

  if (OKbuttonState == HIGH && OKbuttonState != OKlastButtonState) {
    lcd.clear();
    state = START_MENU;
    initMenu();
  }

  DOWNlastButtonState = DOWNbuttonState;
  UPlastButtonState = UPbuttonState;
  OKlastButtonState = OKbuttonState;

  delay(120);
}