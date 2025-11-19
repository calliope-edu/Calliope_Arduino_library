/*!
 * @file Calliope_SmartShield.h
 *
 * Smart Shield driver for Calliope mini
 * Provides display and gamepad support via JACDAC-SPI protocol
 */

#ifndef _CALLIOPE_SMARTSHIELD_H_
#define _CALLIOPE_SMARTSHIELD_H_

#include <Arduino.h>
#include <SPI.h>

// ========== JACDAC-SPI Protocol ==========
#define JDSPI_MAGIC      0x7ACD
#define JDSPI_MAGIC_NOOP 0xB3CD

// JACDAC service classes
#define JD_SERVICE_CLASS_INDEXED_SCREEN 0x16fa36e5
#define JD_SERVICE_CLASS_ARCADE_GAMEPAD 0x1deaa06e

// JACDAC commands
#define JD_CMD_GET_REG 0x1000
#define JD_CMD_SET_REG 0x2000
#define JD_GET(reg) (JD_CMD_GET_REG | (reg))
#define JD_SET(reg) (JD_CMD_SET_REG | (reg))

// Display registers & commands
#define JD_INDEXED_SCREEN_REG_PALETTE      0x80
#define JD_INDEXED_SCREEN_REG_BRIGHTNESS   0x01
#define JD_INDEXED_SCREEN_CMD_START_UPDATE 0x81
#define JD_INDEXED_SCREEN_CMD_SET_PIXELS   0x83

// Gamepad register
#define JD_REG_READING 0x101

// ========== Button Definitions ==========
#define SHIELD_BUTTON_LEFT   0x02
#define SHIELD_BUTTON_UP     0x04
#define SHIELD_BUTTON_RIGHT  0x08
#define SHIELD_BUTTON_DOWN   0x10
#define SHIELD_BUTTON_A      0x20
#define SHIELD_BUTTON_B      0x40
#define SHIELD_BUTTON_MENU   0x80

// ========== Display Configuration ==========
#define SHIELD_WIDTH  160
#define SHIELD_HEIGHT 120
#define SHIELD_TRANSPARENT_BG 255  // Use as bg parameter for transparent text

// ========== JACDAC Protocol Structures ==========

struct __attribute__((packed)) jd_packet_t {
    uint8_t service_size;
    uint8_t service_number;
    uint16_t service_command;
    uint8_t data[0];
};

struct __attribute__((packed)) jd_frame_t {
    uint16_t crc;
    uint8_t size;
    uint8_t flags;
    uint64_t device_identifier;
    uint8_t data[240];
};

struct __attribute__((packed)) jd_indexed_screen_start_update_t {
    uint16_t x, y, width, height;
};

/** Class for Smart Shield display and gamepad control */
class Calliope_SmartShield {
public:
    Calliope_SmartShield();
    ~Calliope_SmartShield();
    
    // Hardware initialization
    void begin();
    void loop();
    bool isConnected();
    
    // Display control
    void setPalette(uint32_t* palette);
    void setBrightness(uint8_t brightness);
    
    // Framebuffer operations
    void clearFramebuffer(uint8_t color);
    void setPixel(uint16_t x, uint16_t y, uint8_t color);
    uint8_t getPixel(uint16_t x, uint16_t y);
    void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color);
    void transmitFramebuffer(bool verbose = false);
    
    // Text rendering (5x7 font)
    // bg: Background color, or SHIELD_TRANSPARENT_BG (255) for transparent
    void drawText(int16_t x, int16_t y, const char* text, uint8_t color, 
                  uint8_t bg = SHIELD_TRANSPARENT_BG, uint8_t size = 1);
    void drawChar(int16_t x, int16_t y, char c, uint8_t color, uint8_t bg, 
                  uint8_t size = 1);
    
    // Button/Gamepad functions
    uint8_t readButtons();
    bool isButtonPressed(uint8_t mask);
    bool isButtonDown(uint8_t mask);
    bool isButtonUp(uint8_t mask);
    
    // Individual button state
    bool leftPressed();
    bool upPressed();
    bool rightPressed();
    bool downPressed();
    bool aPressed();
    bool bPressed();
    bool menuPressed();
    
    // Edge detection
    bool leftDown();
    bool upDown();
    bool rightDown();
    bool downDown();
    bool aDown();
    bool bDown();
    bool menuDown();
    
private:
    jd_frame_t txFrame, rxFrame;
    uint8_t displayServiceNum;
    uint8_t gamepadServiceNum;
    uint64_t deviceId;
    bool connected;
    
    uint8_t framebuffer[9600];  // 160×120 pixels, 4-bit indexed
    uint8_t buttonState;
    uint8_t lastButtonState;
    
    void* pushPacket(uint8_t service_num, uint16_t service_cmd, uint8_t size);
    void transmitFrame();
    void processResponse();
};

#endif // _CALLIOPE_SMARTSHIELD_H_
