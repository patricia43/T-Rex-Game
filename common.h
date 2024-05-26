#ifndef COMMON_H
#define COMMON_H

// Global variables
  extern bool DOWNbuttonState = 0;
  extern bool DOWNlastButtonState = 0;
  extern bool UPbuttonState = 0;
  extern bool UPlastButtonState = 0;
  extern bool OKbuttonState = 0;
  extern bool OKlastButtonState = 0;

  extern byte state = 0;

  extern int score = 0;
  extern byte difficulty = 180;
  extern byte difficultyIncreaseCounter = 0;
  extern byte scoreDigitsCount = 1;

  extern bool leftFoot = true;

  // byte selection = 0;
  extern bool cursorOnTopPos = true;

  extern bool ground[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};

  // jump
  extern bool isJumping = false; // Flag to track if the dino is jumping
  extern byte jumpCounter = 0; // Counter to manage the duration of the jump

  // cactus spawn
  extern byte cactusSpawnCounter = 0;
  extern byte cactusSpacing = 6;

  // file from SD Card

  extern byte crtLine = 1;
  extern byte crtLine2 = 0;
  extern byte leaderBoardEntriesCount = 0;
  extern byte lineCount = 0;

  // user and high score
  extern String crtUser = "";
  extern int crtUsersHighScore = 0;

#endif // COMMON_H