#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "common.h"

void initGame() {
  lcd.clear();
  for (int i = 0; i < 16; i++) {
    ground[i] = false;
  }

  score = 0;
  isJumping = false;
  jumpCounter = 0;
  cactusSpawnCounter = 0;
  
  UPbuttonState = 0;
  DOWNbuttonState = 0;
  DOWNlastButtonState = 0;
  UPlastButtonState = 0;

  scoreDigitsCount = 1;
  leftFoot = true;

  difficulty = 0;
  difficultyIncreaseCounter = 0;

  cactusSpacing = 6;
}

void detectCollision() {
  // check for collision with cactus
  if (!isJumping && ground[2]) {

    // collision detected!

    // update leaderboard if the player got a new personal high score:
    int time_spent = millis();

    if (score > crtUsersHighScore) {
      crtUsersHighScore = score;
      updateLeaderboard();
      readMyFile();
    }
    time_spent = millis() - time_spent;
    //Serial.println(time_spent);
    // freeze the display to indicate game over:
    if (time_spent < 400 && time_spent >= 0) {
      delay(400 - time_spent);
    }
    // clear the screen
    lcd.clear();
    // switch state => go to game over screen
    state = GAME_OVER;
  }
}

void game() {

  // detect jump
  UPbuttonState = digitalRead(upButton);
  if (UPbuttonState == HIGH && UPbuttonState != UPlastButtonState && !isJumping) {
    isJumping = true;
    jumpCounter = jump_duration; // Set jump duration
  }

  // display ground
  lcd.setCursor(0, 1);
  for (byte i = 0; i < 16; i++) {
    if (i == 1 && !isJumping) {
      // display dino left side
      if (leftFoot) {
        lcd.write(0);
      } else {
        lcd.write(4);
      }

    } else if (i == 2 && !isJumping) {
      // display dino right side
      if (leftFoot) {
        lcd.write(5);      
      } else {
        lcd.write(1);
      }
    } else {
      // display ground as usual
      if(ground[i]) {
        lcd.write(2);
      } else {
        lcd.write(3);
      }
    }
  }

  // display dino
  if (!isJumping) {
    // clear top row
    lcd.setCursor(1, 0);
    lcd.print("  ");    
  } else {
    // display on top row
    lcd.setCursor(1, 0);
    if (leftFoot) {
      lcd.write(0);
    } else {
      lcd.write(4);
    }
    lcd.setCursor(2, 0);
    if (leftFoot) {
      lcd.write(5);      
    } else {
      lcd.write(1);
    }
  }
  // display score
  lcd.setCursor(16 - getDigitCount(score), 0);
  lcd.print(score);

  // detect collision
  detectCollision();

  // decrease jump counter
  if (isJumping) {
    jumpCounter--;
    if (jumpCounter == 0) {
      isJumping = false; // End the jump after the counter reaches 0
    }
  }

  // Shift ground to the left
  for (byte i = 0; i < 15; i++) {
    ground[i] = ground[i + 1];
  }
  // Generate cactus on rightmost position
  if (cactusSpawnCounter >= cactusSpacing) {
    ground[15] = (random(5) == 0);
    if (ground[15]) {
      cactusSpawnCounter = 0;
    }
  } else {
    ground[15] = false;
  }

  // increment score
  score++;
  if (score >= 9999) {
    // safeguard against ridiculous high score
    lcd.clear();
    state = GAME_OVER;
  }
  cactusSpawnCounter++;


  if (leftFoot) {
    leftFoot = false;
  } else {
    leftFoot = true;
  }

  // increase difficulty
  difficultyIncreaseCounter++;
  if (difficultyIncreaseCounter >= 10) {
    difficultyIncreaseCounter = 0;
    difficulty++;
  }

  // periodically decrease spacing between cacti
  if (score % 50 == 0 && cactusSpacing > 4) {
    cactusSpacing--;
  }
  
  UPlastButtonState = UPbuttonState;
  delay(180 - difficulty);
}