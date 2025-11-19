/*
 * Smart Shield Demo for Calliope mini
 * 
 * This example demonstrates the Smart Shield's capabilities:
 * - 160x120 indexed color display
 * - 7-button gamepad (LEFT, UP, RIGHT, DOWN, A, B, MENU)
 * - Text rendering and graphics
 * 
 * Hardware: Calliope mini with Smart Shield attached
 */

#include <Calliope_SmartShield.h>

Calliope_SmartShield shield;

void setup() {
  Serial.begin(115200);
  Serial.println("=== Smart Shield Demo ===");
  
  // Initialize the Smart Shield
  shield.begin();
  
  // Wait for shield to connect
  while (!shield.isConnected()) {
    shield.loop();
    delay(100);
  }
  
  Serial.println("Shield connected!");
  
  // Configure 16-color palette
  uint32_t palette[16] = {
    0x000000, 0xFFFFFF, 0xFF0000, 0x00FF00,  // Black, White, Red, Green
    0x0000FF, 0xFFFF00, 0xFF00FF, 0x00FFFF,  // Blue, Yellow, Magenta, Cyan
    0x808080, 0xC0C0C0, 0x800000, 0x008000,  // Gray, Silver, Maroon, Dark Green
    0x000080, 0x808000, 0x800080, 0x008080   // Navy, Olive, Purple, Teal
  };
  shield.setPalette(palette);
  shield.setBrightness(255);
  
  // Show welcome screen
  shield.clearFramebuffer(0);
  shield.drawText(20, 10, "Calliope Smart", 1, 0, 1);
  shield.drawText(20, 25, "Shield Demo", 1, 0, 1);
  shield.drawText(20, 45, "Press any", 2, 0, 1);
  shield.drawText(20, 60, "button!", 2, 0, 1);
  shield.transmitFramebuffer();
  
  delay(2000);
}

void loop() {
  shield.loop();
  
  // Update every frame
  shield.clearFramebuffer(0);
  
  // Title
  shield.drawText(5, 5, "Button Test", 1, 0, 1);
  
  // Show button states
  char buf[30];
  sprintf(buf, "L:%d U:%d R:%d D:%d", 
          shield.leftPressed(), shield.upPressed(),
          shield.rightPressed(), shield.downPressed());
  shield.drawText(5, 25, buf, 3, 0, 1);
  
  sprintf(buf, "A:%d B:%d M:%d", 
          shield.aPressed(), shield.bPressed(), shield.menuPressed());
  shield.drawText(5, 40, buf, 3, 0, 1);
  
  // Visual feedback - color blocks for pressed buttons
  // D-pad on the left
  int dpadX = 30, dpadY = 75;
  if (shield.leftPressed()) shield.fillRect(dpadX - 20, dpadY, 15, 10, 5);
  if (shield.rightPressed()) shield.fillRect(dpadX + 5, dpadY, 15, 10, 5);
  if (shield.upPressed()) shield.fillRect(dpadX - 5, dpadY - 12, 10, 12, 5);
  if (shield.downPressed()) shield.fillRect(dpadX - 5, dpadY + 10, 10, 12, 5);
  // Center dot
  shield.fillRect(dpadX - 2, dpadY + 3, 4, 4, 1);
  
  // Action buttons on the right
  if (shield.aPressed()) {
    shield.fillRect(100, 60, 30, 20, 2);  // Red
    shield.drawText(105, 65, "A", 1, 2, 1);
  }
  
  if (shield.bPressed()) {
    shield.fillRect(100, 85, 30, 20, 4);  // Blue
    shield.drawText(105, 90, "B", 1, 4, 1);
  }
  
  if (shield.menuPressed()) {
    shield.fillRect(100, 110, 40, 10, 6);  // Magenta
    shield.drawText(102, 111, "MENU", 1, 6, 1);
  }
  
  // Send framebuffer to display
  shield.transmitFramebuffer();
  
  delay(50);  // ~20 FPS
}