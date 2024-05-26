#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <SPI.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

#define START_MENU 0
#define GAME 1
#define GAME_OVER 2
#define LEADERBOARD 3
#define USER_SELECT 4

#define jump_duration 3

// const int downButton = 2;
// const int upButton =  7;
// const int okButton = 4;

#define downButton 2
#define upButton 7
#define okButton 4

bool DOWNbuttonState = 0;
bool DOWNlastButtonState = 0;
bool UPbuttonState = 0;
bool UPlastButtonState = 0;
bool OKbuttonState = 0;
bool OKlastButtonState = 0;

byte state = 4;

int score = 0;
int scoreDigitsCount = 1;

bool leftFoot = true;

byte selection = 0;
// int message = 0;
// int selection = 0;
bool ground[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};

// jump
bool isJumping = false; // Flag to track if the dino is jumping
byte jumpCounter = 0; // Counter to manage the duration of the jump

// cactus spawn
byte cactusSpawnCounter = 0;

// file from SD Card

// File leaderboardFile;
// File usersFile;
// const String fileName = "lead.csv";
// const String usersFileName = "users.txt";

byte crtLine = 1;
int crtLine2 = 0;
byte leaderBoardEntriesCount = 0;
byte lineCount = 0;

// user and high score
String crtUser = "";
int crtUsersHighScore = 0;

void graphics(){
   byte dino[56] = {
   //0- first half
   0b00000,
   0b00000,
   0b00010,
   0b00010,
   0b00011,
   0b00011,
   0b00001,
   0b00001,
   //1- second half
   0b00111,
   0b00111,
   0b00111,
   0b00100,
   0b11100,
   0b11100,
   0b11000,
   0b01000,
   //2- cactus 
   0b00100,
   0b10100,
   0b10100,
   0b10100,
   0b10100,
   0b11100,
   0b00100,
   0b00100,

   //3 - ground
   0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11111,

  // 4 - first half no leg
  0b00000,
  0b00000,
  0b00010,
  0b00010,
  0b00011,
  0b00011,
  0b00001,
  0b00000,
  // 5 - second half no leg
    0b00111,
   0b00111,
   0b00111,
   0b00100,
   0b11100,
   0b11100,
   0b11000,
   0b00000,
  // 6 - empty
   0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
   };
   int g;
   //draw dino positions
   for (g = 0; g < 7; g++) {
       lcd.createChar(g, &dino[g*8]);
   }
}

void initGame() {
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
}

void initMenu() {
  crtLine = 1;
}

void switchUser(String userName) {
  crtUser = userName;
  crtUsersHighScore = getUserHighScore(userName);
}

int getUserHighScore(String userName) {
  Serial.println("lead.csv");
  // Open the leaderboard file for reading
  File file = SD.open("lead.csv");
  if (!file) {
    Serial.println("Error opening file");
    return -1; // Return an error value if the file cannot be opened
  }

  // Iterate through the file line by line
  while (file.available()) {
    String line = readLine(file);
    int commaIndex = line.indexOf(',');
    if (commaIndex == -1) {
      continue; // Skip lines without a comma
    }

    String playerName = line.substring(0, commaIndex);
    // Serial.print("Player name: ");
    // Serial.println(playerName);
    // Serial.print("Player score string: ");
    // Serial.println(line);

    if (playerName == userName) {
      int playerScore = line.substring(commaIndex + 1).toInt();
      file.close();
      return playerScore;
    }
  }

  file.close();
  return -1; // Return -1 if the user is not found
}

void setup() {
  // Debugging 
  Serial.begin(9060);
  while(!Serial) {};

  // SD Card
  InitializeCard();

  // Print leaderboard file contents to serial monitor
  readMyFile();

  // LCD Screen
  lcd.init();         // initialize the lcd
  lcd.backlight();    // Turn on the LCD screen backlight

  // Buttons
  pinMode(downButton, INPUT); // initialize the pushbutton pin as an input
  // pinMode(buttonPin, INPUT_PULLUP); // enable the internal pull-up resistor
  pinMode(okButton, INPUT);
  pinMode(upButton, INPUT); // initialize the pushbutton pin as an input

  // Graphics
  graphics();

  // testing
  File file = SD.open("lead.csv");
  if (!file) {
    Serial.println("Failed to open file");
    return;
  }

  int numberOfLines = countLines(file);
  lineCount = numberOfLines;
  Serial.print("Number of lines in file: ");
  Serial.println(numberOfLines);

  file.close();

  crtUser = "Dino1";
  Serial.print("Current user: ");
  Serial.println(crtUser);

  crtUsersHighScore = getUserHighScore(crtUser);
  Serial.print("User and score: ");
  Serial.print(crtUser);
  Serial.print(", ");
  Serial.println(crtUsersHighScore);
}

int countLines(File &file) {
  int lineCount = 0;
  while (file.available()) {
    char c = file.read();
    if (c == '\n') {
      lineCount++;
    }
  }
  return lineCount;
}

void updateLeaderboard() {
  String tempFileName = "temp.csv";
  File originalFile = SD.open("lead.csv", FILE_READ);
  File tempFile = SD.open(tempFileName, FILE_WRITE);

  if (!originalFile || !tempFile) {
    Serial.println("Error opening file");
    return;
  }

  bool userUpdated = false;
  int userPosition = -1;
  int currentPosition = 0;

  while (originalFile.available()) {
    String line = readLine(originalFile);
    int commaIndex = line.indexOf(',');
    String playerName = line.substring(0, commaIndex);
    int playerScore = line.substring(commaIndex + 1).toInt();

    if (playerName == crtUser) {
      userPosition = currentPosition;
    }

    if (!userUpdated && crtUsersHighScore > playerScore) {
      tempFile.print(crtUser);
      tempFile.print(",");
      tempFile.println(crtUsersHighScore);
      userUpdated = true;
    }

    if (playerName != crtUser) {

      tempFile.print(playerName);
      tempFile.print(",");
      tempFile.println(playerScore);
    }

    currentPosition++;
  }

  // If the user has not been updated yet, it means their new score is the lowest
  if (!userUpdated) {
    tempFile.print(crtUser);
    tempFile.print(",");
    tempFile.println(crtUsersHighScore);
  }

  originalFile.close();
  tempFile.close();

  // Remove the original file
  SD.remove("lead.csv");

  // Rename the temp file to the original file name
  // Copy temp file to original file name
  tempFile = SD.open(tempFileName, FILE_READ);
  originalFile = SD.open("lead.csv", FILE_WRITE);

  if (!tempFile || !originalFile) {
    Serial.println("Error reopening files");
    return;
  }

  while (tempFile.available()) {
    originalFile.write(tempFile.read());
  }

  tempFile.close();
  originalFile.close();

  // Remove the temporary file
  SD.remove(tempFileName);
}

void startMenu() {
  
  DOWNbuttonState = digitalRead(downButton);
  UPbuttonState = digitalRead(upButton);
  OKbuttonState = digitalRead(okButton);

  if (DOWNbuttonState == HIGH && DOWNbuttonState != DOWNlastButtonState){
    if (selection == 0) {
      selection++;
    }
  }
  else {
      if (UPbuttonState == HIGH && UPbuttonState != UPlastButtonState){
        if (selection == 1) {
          selection--;
        }
    }
  }

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Start");
  lcd.setCursor(1, 1);
  lcd.print("Select player");
  
  // draw selection arrow
  if (selection == 0) {
    lcd.setCursor(0, 0);
  } else {
    lcd.setCursor(0, 1);
  }
  lcd.print(">");

  // play is selected
  if (selection == 0) {
    // init game variables
    initGame();
    if (OKbuttonState == HIGH && OKbuttonState != OKlastButtonState) {
      state = GAME;
      lcd.clear();
    }    
  }

  DOWNlastButtonState = DOWNbuttonState;
  UPlastButtonState = UPbuttonState;
  OKlastButtonState = OKbuttonState;

  delay(120);
}

void userSelect() {
  //Serial.println("Entering userSelect");

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
    crtLine2--;
    lcd.clear();
    if (crtLine2 < 0) {
      crtLine2 = lineCount - 1;
    }
  }

  // Displaying leaderboard entries

  // Open the leaderboard file
  File file = SD.open("lead.csv");
  if (!file) {
    Serial.println("Error opening file");
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
  int commaIndex;
  String playerName;
  if (crtLine2 > 0) {
    commaIndex = previousLine.indexOf(',');
    playerName = previousLine.substring(0, commaIndex);
    // int playerScore = previousLine.substring(commaIndex + 1).toInt();

    // Display top row:
    lcd.setCursor(0, 0);
    lcd.print(playerName);
  } else {
    lcd.setCursor(0, 0);
    lcd.print("SELECT:");
  }

  // Display the current line on the bottom row
  String line = readLine(file);
  commaIndex = line.indexOf(',');
  playerName = line.substring(0, commaIndex);
  // playerScore = line.substring(commaIndex + 1).toInt();
  lcd.setCursor(0, 1);
  lcd.print(">");
  lcd.print(playerName);

  file.close();

  if (OKbuttonState == HIGH && OKbuttonState != OKlastButtonState) {
    lcd.clear();
    state = START_MENU;
    switchUser(playerName);
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
  lcd.print("Game over!");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);

  if (OKbuttonState == HIGH && OKbuttonState != OKlastButtonState) {
      state = LEADERBOARD;
      initMenu();
      lcd.clear();
    }

  DOWNlastButtonState = DOWNbuttonState;
  UPlastButtonState = UPbuttonState;
  OKlastButtonState = OKbuttonState;
}

void InitializeCard() {
  Serial.print("Beginning of init of SD card: ");
  if (!SD.begin(10)) {
    Serial.print("fail\n");
  } else {
    Serial.print("success\n");
    Serial.print("\n");
  }
}

void readMyFile() {
  File file = SD.open("lead.csv");
  if (file) {
    Serial.print("Reading data stored in lead.csv: \n");

    while(file.available()) {
      Serial.write(file.read());
    }

    file.close();

  } else {
    Serial.println("Error opening file");
  }
}

String readLine(File &file) {
  String line = "";
  while (file.available()) {
    char c = file.read();
    if (c == '\n') {
      break;
    }
    line += c;
  }
  return line;
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
    Serial.println("Error opening file");
    return;
  }

  // Move to the correct line in the file
  String previousLine ="";

  for (int i = 0; i < crtLine; i++) {
    if (!file.available()) {
      // Reached the end of the file
      file.close();
      return;
    }
    // The previous line will be displayed on the top row
    previousLine = readLine(file);
  }
  int commaIndex = previousLine.indexOf(',');
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

  DOWNlastButtonState = DOWNbuttonState;
  UPlastButtonState = UPbuttonState;
  OKlastButtonState = OKbuttonState;

  delay(120);
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
    Serial.println(time_spent);
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

int getDigitCount(int n) {
  if (n == 0) {
    return 1;
  }
  int c = 0;
  while(n) {
    c++;
    n /= 10;
  }
  return c;
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
  for (int i = 0; i < 16; i++) {
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
  for (int i = 0; i < 15; i++) {
    ground[i] = ground[i + 1];
  }
  // Generate cactus on rightmost position
  if (cactusSpawnCounter >= 6) {
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
    state = GAME_OVER;
  }
  cactusSpawnCounter++;


  if (leftFoot) {
    leftFoot = false;
  } else {
    leftFoot = true;
  }
  
  UPlastButtonState = UPbuttonState;
  delay(120);
}

void loop() {
  
  // userSelect();
  
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