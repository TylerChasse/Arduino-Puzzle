#include <Esplora.h>
#include <EEPROM.h>

int xPos;
int yPos;
int xAxisPos;
int yAxisPos;
bool complete = false;
bool part1Done = false;
bool part2Done = false;
int attempt = 1;

extern "C" {
  void asmDelay();
}

void setup() {
  Serial.begin(9600);
  Esplora.writeRGB(255, 0, 0);

  // set random seed - https://forum.arduino.cc/t/random-function-not-randomising-after-reset/1161545/7
  size_t const address{0};
  unsigned int seed{};
  EEPROM.get(address, seed);
  randomSeed(seed);
  EEPROM.put(address, seed + 1);

  setJoystickNums();
  setAxisNums();
}

void setJoystickNums() {
  // set to random coordinate along outer boundaries of joystick
  // - to be on outer boundary one coordinate must be 512 or -512
  int num = random(0, 3);
  if (num == 0) {
    xPos = 512;
    yPos = random(-512, 512);
  } else if (num == 1) {
    xPos = -512;
    yPos = random(-512, 512);
  } else if (num == 2) {
    yPos = 512;
    xPos = random(-512, 512);
  } else if (num == 3) {
    yPos = -512;
    xPos = random(-512, 512);
  }
}

void setAxisNums() {
  xAxisPos = random(-100, 100);
  yAxisPos = random(-100, 100);
}

void setColors(int* colorArray, int size) {
  // set colorArray to random numbers that correspond to colors
  for (int i = 0; i < size; i++) {
    colorArray[i] = random(1, 5);
  }
}

void showColors(int* colorArray, int size) {
  // show colors in corresponding to numbers in colorArray
  for (int i = 0; i < size; i++) {
    switch (colorArray[i]) {
      case 1:
        // red
        Esplora.writeRGB(255, 0, 0);
        break;
      case 2:
        // green
        Esplora.writeRGB(0, 255, 0);
        break;
      case 3:
        // blue
        Esplora.writeRGB(0, 0, 255);
        break;
      case 4:
        // purple
        Esplora.writeRGB(255, 0, 255);
        break;
    }
    myDelay(50);
    Esplora.writeRGB(0, 0, 0);
    myDelay(50);
  }
}

void checkGuesses(int* colorArray, int size) {
  bool solved = false;
  while (!solved) {
    // for each color in array
    for (int i = 0; i < size; i++) {
      while (Esplora.readButton(1) == 1 && Esplora.readButton(2) == 1 && Esplora.readButton(3) == 1 && Esplora.readButton(4) == 1 && Esplora.readJoystickButton() == 1) {}
      // if button pressed corresponds to correct color
      if (Esplora.readButton(colorArray[i]) == 0) {
        Esplora.tone(600);
        myDelay(20);
        Esplora.noTone();
        if (i == size - 1) {
          solved = true;
        }
      // if button pressed does not correspond to correct color
      } else {
        Esplora.tone(200);
        myDelay(20);
        Esplora.noTone();
        // show colors again and restart
        showColors(colorArray, size);
        break;
      }
    }
  }
}

void wrongGuess() {
  if (Esplora.readButton(1) == 0) {
    Esplora.tone(200);
    Esplora.noTone();
  }
}
  
void myDelay(uint8_t length) {
  asm volatile (
      "mov R21, %0\n"           // move length into R21 (%0 is a placeholder for input)
      :
      : "r" (length)            // signify length variable to be used as input 
    );
  asmDelay();                   // call asm function that will access length via R21
}

// run in a while loop in main function so things are constantly checked/updated
void part1() {
  int xValue = Esplora.readJoystickX();
  int yValue = Esplora.readJoystickY();
  int button = Esplora.readButton(1);

  // Joystick data output
  Serial.print("\nxPos: ");
  Serial.print(xPos);
  Serial.print("\tYPos: ");
  Serial.print(yPos);
  Serial.print("\tJoystick X: ");  
  Serial.print(xValue);
  Serial.print("\tY: ");           
  Serial.print(yValue);           
  Serial.print("\tButton: ");      
  Serial.print(button);
  Serial.print("\tAttempt:");
  Serial.print(attempt);

  // if correct and haven't completed 3 times, reset position
  if (complete == true && attempt < 4) {
    setJoystickNums();
    complete = false;
  // if completed all three
  } else if (attempt >= 4) {
    Esplora.writeRGB(0, 0, 255);
    Esplora.tone(800);
    unsigned long first = millis();
    myDelay(70);
    unsigned long second = millis();
    Serial.print(second-first);
    Esplora.noTone();
    part1Done = true;
    return;
  }

  // if in correct position (green)
  if ((xPos >= (xValue - 20) && xPos <= (xValue + 20)) && (yPos >= (yValue - 20) && yPos <= (yValue + 20))) {
    Esplora.writeRGB(0, 255, 0);
    if (button == 0) {
      Esplora.tone(600);
      myDelay(20);
      Esplora.noTone();
      complete = true;
      attempt += 1;
    }
  // if in yellow position
  } else if ((xPos >= (xValue - 250) && xPos <= (xValue + 250)) && (yPos >= (yValue - 250) && yPos <= (yValue + 250))) {
    Esplora.writeRGB(255, 255, 0);
    wrongGuess();
  // if in orange position
  } else if ((xPos >= (xValue - 500) && xPos <= (xValue + 500)) && (yPos >= (yValue - 500) && yPos <= (yValue + 500))) {
    Esplora.writeRGB(255, 50, 0);
    wrongGuess();
  // if in red position
  } else if (!part1Done) {
    Esplora.writeRGB(255, 0, 0);
    wrongGuess();
  }
}

// run in a while loop in main function so things are constantly checked/updated
void part2() {
  int xAxis = Esplora.readAccelerometer(X_AXIS);
  int yAxis = Esplora.readAccelerometer(Y_AXIS);
  int button = Esplora.readButton(1);

  // accelerometer data output
  Serial.print("\nxAxisPos: ");
  Serial.print(xAxisPos);
  Serial.print("\tYAxisPos: ");
  Serial.print(yAxisPos);
  Serial.print("\tAccelerometer X: ");
  Serial.print(xAxis);
  Serial.print("\tY: ");
  Serial.print(yAxis);
  Serial.print("\tButton: ");
  Serial.print(button);
  Serial.print("\tAttempt:");
  Serial.print(attempt);

  // if correct and haven't completed 3 times, reset position
  if (complete == true && attempt < 4) {
    setAxisNums();
    complete = false;
  // if completed all 3
  } else if (attempt >= 4) {
    Esplora.writeRGB(0, 0, 255);
    Esplora.tone(800);
    myDelay(70);
    Esplora.noTone();
    part2Done = true;
    return;
  }

  // if in correct position (green)
  if ((xAxisPos >= (xAxis - 14) && xAxisPos <= (xAxis + 14)) && (yAxisPos >= (yAxis - 14) && yAxisPos <= (yAxis + 14))) {
    Esplora.writeRGB(0, 255, 0);    // green light
    if (button == 0) {              // if button pressed to enter guess
      Esplora.tone(600);            // high pitch tone
      myDelay(20); 
      Esplora.noTone();
      complete = true;              // complete, next attempt
      attempt += 1;
    }
  // if in yellow position
  } else if ((xAxisPos >= (xAxis - 25) && xAxisPos <= (xAxis + 25)) && (yAxisPos >= (yAxis - 25) && yAxisPos <= (yAxis + 25))) {
    Esplora.writeRGB(255, 255, 0);  // yellow light
    wrongGuess();                   // if guess entered, low pitch tone
  // if in orange position
  } else if ((xAxisPos >= (xAxis - 50) && xAxisPos <= (xAxis + 50)) && (yAxisPos >= (yAxis - 50) && yAxisPos <= (yAxis + 50))) {
    Esplora.writeRGB(255, 50, 0);   // orange light
    wrongGuess();                   // if guess entered, low pitch tone
  // if in red position
  } else if (!part2Done) {
    Esplora.writeRGB(255, 0, 0);    // red light
    wrongGuess();                   // if guess entered, low pitch tone
  }
}

void part3() {
  // number of colors
  int size = 6;
  int colorArray[size];
  // set, show, and check colors three times
  setColors(colorArray, size);
  showColors(colorArray, size);
  checkGuesses(colorArray, size);
  setColors(colorArray, size);
  showColors(colorArray, size);
  checkGuesses(colorArray, size);
  setColors(colorArray, size);
  showColors(colorArray, size);
  checkGuesses(colorArray, size);
  // all three done/correct
  Esplora.tone(800);
  myDelay(70);
  Esplora.noTone();
}

void lightShow(int counter) {
  // play color sequence n amount of times
  while (counter > 0) {
    Esplora.writeRGB(0, 0, 255);
    myDelay(20);
    Esplora.writeRGB(0, 255, 255);
    myDelay(20);
    Esplora.writeRGB(0, 255, 0);
    myDelay(20);
    Esplora.writeRGB(255, 255, 0);
    myDelay(20);
    Esplora.writeRGB(255, 0, 0);
    myDelay(20);
    Esplora.writeRGB(255, 0, 255);
    myDelay(20);
    Esplora.writeRGB(0, 0, 255);
    myDelay(20);
    counter -= 1;
  }
}

void loop() {
  while (!part1Done) {
    part1();
  }
  complete = false;
  attempt = 1;
  while (!part2Done) {
    part2();
  }
  part3();
  lightShow(5);
  // reset all values
  part1Done = false;
  part2Done = false;
  complete = false;
  attempt = 1;
}
