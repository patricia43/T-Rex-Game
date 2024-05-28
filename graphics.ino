#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "common.h"

void graphics() {
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
   byte g;
   //draw dino positions
   for (g = 0; g < 7; g++) {
       lcd.createChar(g, &dino[g*8]);
   }
}