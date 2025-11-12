/*
 * Smart Shield Pong Game
 * 
 * A simple Pong-style game demonstrating:
 * - Graphics rendering
 * - Button input handling
 * - Game loop with state management
 * 
 * Controls:
 * - UP/DOWN: Move paddle
 * - A: Start game / Reset
 * - MENU: Pause
 */

#include <Calliope_SmartShield.h>

Calliope_SmartShield shield;

// Game state
int ballX = 80, ballY = 60;
int ballVX = 2, ballVY = 1;
int paddleY = 50;
int score = 0;
bool gameActive = false;
bool paused = false;

void setup() {
  Serial.begin(115200);
  
  shield.begin();
  
  while (!shield.isConnected()) {
    shield.loop();
    delay(100);
  }
  
  // Setup palette
  uint32_t palette[16] = {
    0x000000, 0xFFFFFF, 0xFF0000, 0x00FF00,
    0x0000FF, 0xFFFF00, 0xFF00FF, 0x00FFFF,
    0x808080, 0xC0C0C0, 0x800000, 0x008000,
    0x000080, 0x808000, 0x800080, 0x008080
  };
  shield.setPalette(palette);
  shield.setBrightness(255);
  
  showTitleScreen();
}

void showTitleScreen() {
  shield.clearFramebuffer(0);
  shield.drawText(45, 30, "PONG", 5, 0, 2);
  shield.drawText(20, 60, "Press A to start", 1, 0, 1);
  shield.transmitFramebuffer();
}

void resetBall() {
  ballX = 80;
  ballY = 60;
  ballVX = (random(2) == 0) ? -2 : 2;
  ballVY = random(-2, 3);
  if (ballVY == 0) ballVY = 1;
}

void loop() {
  shield.loop();
  
  // Check for start/reset
  if (shield.aDown() && !gameActive) {
    gameActive = true;
    paused = false;
    score = 0;
    resetBall();
  }
  
  // Check for pause
  if (shield.menuDown() && gameActive) {
    paused = !paused;
  }
  
  if (!gameActive) {
    showTitleScreen();
    delay(100);
    return;
  }
  
  if (paused) {
    shield.clearFramebuffer(0);
    shield.drawText(45, 50, "PAUSED", 5, 0, 2);
    shield.drawText(20, 80, "Press MENU", 1, 0, 1);
    shield.transmitFramebuffer();
    delay(100);
    return;
  }
  
  // Move paddle
  if (shield.upPressed() && paddleY > 0) {
    paddleY -= 3;
  }
  if (shield.downPressed() && paddleY < 90) {
    paddleY += 3;
  }
  
  // Move ball
  ballX += ballVX;
  ballY += ballVY;
  
  // Ball collision with top/bottom
  if (ballY <= 0 || ballY >= 115) {
    ballVY = -ballVY;
    ballY = constrain(ballY, 0, 115);
  }
  
  // Ball collision with right wall (bounce back)
  if (ballX >= 155) {
    ballVX = -ballVX;
    ballX = 155;
  }
  
  // Ball collision with paddle
  if (ballX <= 15 && ballX >= 10 && ballY >= paddleY && ballY <= paddleY + 30) {
    ballVX = abs(ballVX);  // Bounce right
    ballX = 15;
    score++;
    
    // Speed up slightly
    if (ballVX < 4) ballVX++;
  }
  
  // Ball missed - game over
  if (ballX < 0) {
    gameActive = false;
    shield.clearFramebuffer(0);
    shield.drawText(30, 40, "GAME OVER", 2, 0, 2);
    char buf[20];
    sprintf(buf, "Score: %d", score);
    shield.drawText(40, 65, buf, 5, 0, 1);
    shield.drawText(20, 90, "Press A", 1, 0, 1);
    shield.transmitFramebuffer();
    delay(100);
    return;
  }
  
  // Draw game
  shield.clearFramebuffer(0);
  
  // Draw paddle (left side)
  shield.fillRect(5, paddleY, 5, 30, 3);
  
  // Draw ball
  shield.fillRect(ballX - 2, ballY - 2, 4, 4, 1);
  
  // Draw center line
  for (int y = 0; y < 120; y += 10) {
    shield.fillRect(78, y, 4, 5, 8);
  }
  
  // Draw score
  char buf[10];
  sprintf(buf, "%d", score);
  shield.drawText(5, 5, buf, 5, 0, 1);
  
  shield.transmitFramebuffer();
  
  delay(30);  // ~33 FPS
}
