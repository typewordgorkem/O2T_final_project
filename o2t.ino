#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define PIN           6
#define MATRIX_WIDTH  8
#define MATRIX_HEIGHT 8
#define CHAR_WIDTH    8 // The width of one character in pixels, including the space between characters

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(MATRIX_WIDTH, MATRIX_HEIGHT, PIN, NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG, NEO_GRB + NEO_KHZ800);

String incomingCommand = "";

// Snake game variables
int snakeX[64], snakeY[64];
int foodX, foodY;
int snakeLength;
int snakeDir; // 0: up, 1: right, 2: down, 3: left
bool gameRunning;
bool gamePaused = false;


void setup() {
  Serial.begin(9600);
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(40);
  matrix.setTextColor(matrix.Color(0, 0, 255)); // Set default text color
  matrix.fillScreen(1);
  matrix.show();
}

void loop() {
  if (Serial.available() > 0) {
    incomingCommand = Serial.readStringUntil('\n');
    if (incomingCommand == "all_on") {
      setAllPixels(75, 0, 130);
    } else if (incomingCommand == "all_off") {
      setAllPixels(0, 0, 0);
    } else if (incomingCommand.startsWith("display_text:")) {
      int separatorIndex = incomingCommand.indexOf(":", 13);
      String textToDisplay = incomingCommand.substring(13, separatorIndex);
      String colorHex = incomingCommand.substring(separatorIndex + 1);
      uint32_t color = (uint32_t)strtol(colorHex.c_str() + 1, NULL, 16);
      setColor(color);
      displayTextOnLEDs(textToDisplay);
    } else if (incomingCommand == "start_animation") {
      startAnimation();
    } else if (incomingCommand == "snake_start") {
      startSnakeGame();
    } else if (incomingCommand.startsWith("snake_")) {
      handleSnakeCommand(incomingCommand);
    }
  }

  updateSnakeGame();
}

void setAllPixels(uint8_t red, uint8_t green, uint8_t blue) {
  for (int i = 0; i < MATRIX_WIDTH * MATRIX_HEIGHT; i++) {
    matrix.setPixelColor(i, red, green, blue);
  }
  matrix.show();
}

void setColor(uint32_t color) {
  uint8_t red = (color >> 16) & 0xFF;
  uint8_t green = (color >> 8) & 0xFF;
  uint8_t blue = color & 0xFF;
  matrix.setTextColor(matrix.Color(red, green, blue));
}

void displayTextOnLEDs(String text) {
  int textLength = text.length() * CHAR_WIDTH;
  for (int xPos = MATRIX_WIDTH; xPos > -textLength; xPos--) {
    matrix.fillScreen(1);
    matrix.setCursor(xPos, 0);
    matrix.print(text);
    matrix.show();
    delay(80);
  }
}

void startAnimation() {
  // Add your animation logic here.
  // Example: Cycle through all colors
  for (int colorIndex = 0; colorIndex < 256; colorIndex++) {
    uint32_t color = matrix.Color(colorIndex, 230, 255 - colorIndex);
   for (int i = 0; i < MATRIX_WIDTH * MATRIX_HEIGHT; i++) {
      matrix.setPixelColor(i, color);
    }
    matrix.show();
    delay(20);
  }
}

void startSnakeGame() {
  snakeX[0] = 3;
  snakeY[0] = 3;
  snakeLength = 1;
  snakeDir = 0; // Start moving up
  generateFood();
  gameRunning = true;
}


void updateSnakeGame() {
  if (!gameRunning || gamePaused) return;

  moveSnake();
  drawSnake();
  checkCollision();
  matrix.show();
  delay(500);
}


void handleSnakeCommand(String command) {
  if (incomingCommand == "snake_pause") {
      gamePaused = true;
    } else if (incomingCommand == "snake_start") {
      gamePaused = false;
    } else if (incomingCommand == "snake_up") {
      snakeDir = 0;
    } else if (incomingCommand == "snake_down") {
      snakeDir = 2;
    } else if (incomingCommand == "snake_right") {
      snakeDir = 1;
    } else if (incomingCommand == "snake_left") {
      snakeDir = 3;
    }
  }


void generateFood() {
  foodX = random(0, MATRIX_WIDTH);
  foodY = random(0, MATRIX_HEIGHT);
}

void moveSnake() {
  for (int i = snakeLength - 1; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }

  switch (snakeDir) {
    case 0: // up
      snakeY[0]--;
      break;
    case 1: // right
      snakeX[0]++;
      break;
    case 2: // down
      snakeY[0]++;
      break;
    case 3: // left
      snakeX[0]--;
      break;
  }

  // Wrap around when hitting the wall
  if (snakeX[0] < 0) {
    snakeX[0] = MATRIX_WIDTH - 1;
  } else if (snakeX[0] >= MATRIX_WIDTH) {
    snakeX[0] = 0;
  }

  if (snakeY[0] < 0) {
    snakeY[0] = MATRIX_HEIGHT - 1;
  } else if (snakeY[0] >= MATRIX_HEIGHT) {
    snakeY[0] = 0;
  }
}

void drawSnake() {
  matrix.fillScreen(0);
  for (int i = 0; i < snakeLength; i++) {
    matrix.drawPixel(snakeX[i], snakeY[i], matrix.Color(0, 255, 0));
  }
  matrix.drawPixel(foodX, foodY, matrix.Color(255, 0, 0));
}

void checkCollision() {
  // Check for food collision
  if (snakeX[0] == foodX && snakeY[0] == foodY) {
    snakeLength++;
    generateFood();
  }

 

  // Check for self collision
  for (int i = 1; i < snakeLength; i++) {
    if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
      gameRunning = false;
    }
  }
}
