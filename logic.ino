#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <SPI.h>
#include "common.h"

void switchUser(String userName) {
  crtUser = userName;
  crtUsersHighScore = getUserHighScore(userName);
}

int getUserHighScore(String userName) {
  // Open the leaderboard file for reading
  File file = SD.open("lead.csv");
  if (!file) {
    return -1;
  }

  // Iterate through the file line by line
  while (file.available()) {
    String line = readLine(file);
    byte commaIndex = line.indexOf(',');
    if (commaIndex == -1) {
      continue; // Skip lines without a comma
    }

    String playerName = line.substring(0, commaIndex);

    if (playerName == userName) {
      file.close();
      return line.substring(commaIndex + 1).toInt();
    }
  }

  file.close();
  return -1; // Return -1 if the user is not found
}

byte countLines(File &file) {
  byte lineCount = 0;
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
    return;
  }

  bool userUpdated = false;
  byte userPosition = -1;
  byte currentPosition = 0;

  while (originalFile.available()) {
    String line = readLine(originalFile);
    byte commaIndex = line.indexOf(',');
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

void InitializeCard() {
  //Serial.print("Beginning of init of SD card: ");
  if (!SD.begin(10)) {
    //Serial.print("fail\n");
  } else {
    //Serial.print("success\n");
    //Serial.print("\n");
  }
}

void readMyFile() {
  File file = SD.open("lead.csv");
  if (file) {
    while(file.available()) {
      Serial.write(file.read());
    }
    file.close();
  } else {
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

byte getDigitCount(int n) {
  if (n == 0) {
    return 1;
  }
  byte c = 0;
  while(n) {
    c++;
    n /= 10;
  }
  return c;
}