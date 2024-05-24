#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

#define START_MENU 0
#define GAME 1
#define GAME_OVER 2

#define jump_duration 3

const int downButton = 2;
const int upButton =  7;
const int okButton = 4;

int DOWNbuttonState = 0;
int DOWNlastButtonState = 0;
int UPbuttonState = 0;
int UPlastButtonState = 0;
int OKbuttonState = 0;
int OKlastButtonState = 0;

int state = 0;

int score = 0;
int scoreDigitsCount = 1;

bool leftFoot = true;

int selection = 0;
// int message = 0;
// int selection = 0;
bool ground[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};

// jump
bool isJumping = false; // Flag to track if the dino is jumping
int jumpCounter = 0; // Counter to manage the duration of the jump

// cactus spawn
int cactusSpawnCounter = 0;

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

void setup() {
  lcd.init();         // initialize the lcd
  lcd.backlight();    // Turn on the LCD screen backlight
  pinMode(downButton, INPUT); // initialize the pushbutton pin as an input
  // pinMode(buttonPin, INPUT_PULLUP); // enable the internal pull-up resistor
  pinMode(okButton, INPUT);
  pinMode(upButton, INPUT); // initialize the pushbutton pin as an input
  graphics();

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

void gameOver() {
  // lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game over!");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
  // delay(120);
}

void detectCollision() {
  // check for collision with cactus
  if (!isJumping && ground[2]) {
    // collision detected
    lcd.clear();
    state = GAME_OVER;
  }
}

int getDigitCount(int n) {
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

  // decrease jump counter
  if (isJumping) {
    jumpCounter--;
    if (jumpCounter == 0) {
      isJumping = false; // End the jump after the counter reaches 0
    }
  }

  // detect collision
  detectCollision();

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
    // score = 0;
  }
  cactusSpawnCounter++;
  if (leftFoot) {
    leftFoot = false;
  } else {
    leftFoot = true;
  }
  
  UPlastButtonState = UPbuttonState;
  delay(180);
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
  }
}
