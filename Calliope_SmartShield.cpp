/*!
 * @file Calliope_SmartShield.cpp
 *
 * Smart Shield driver implementation for Calliope mini
 */

#include "Calliope_SmartShield.h"

// Pin definitions for Calliope mini (micro:bit v2 compatible)
#define PIN_MOSI   15    // P15 - SPI Data Out
#define PIN_MISO   14    // P14 - SPI Data In
#define PIN_SCK    13    // P13 - SPI Clock
#define PIN_DC     8     // P8  - Flow Control
#define PIN_RST    17    // P16 - STM32 Reset

// 5x7 bitmap font for text rendering
static const uint8_t font5x7[95][5] PROGMEM = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x5F, 0x00, 0x00},
    {0x00, 0x07, 0x00, 0x07, 0x00}, {0x14, 0x7F, 0x14, 0x7F, 0x14},
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, {0x23, 0x13, 0x08, 0x64, 0x62},
    {0x36, 0x49, 0x55, 0x22, 0x50}, {0x00, 0x05, 0x03, 0x00, 0x00},
    {0x00, 0x1C, 0x22, 0x41, 0x00}, {0x00, 0x41, 0x22, 0x1C, 0x00},
    {0x08, 0x2A, 0x1C, 0x2A, 0x08}, {0x08, 0x08, 0x3E, 0x08, 0x08},
    {0x00, 0x50, 0x30, 0x00, 0x00}, {0x08, 0x08, 0x08, 0x08, 0x08},
    {0x00, 0x60, 0x60, 0x00, 0x00}, {0x20, 0x10, 0x08, 0x04, 0x02},
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, {0x00, 0x42, 0x7F, 0x40, 0x00},
    {0x42, 0x61, 0x51, 0x49, 0x46}, {0x21, 0x41, 0x45, 0x4B, 0x31},
    {0x18, 0x14, 0x12, 0x7F, 0x10}, {0x27, 0x45, 0x45, 0x45, 0x39},
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, {0x01, 0x71, 0x09, 0x05, 0x03},
    {0x36, 0x49, 0x49, 0x49, 0x36}, {0x06, 0x49, 0x49, 0x29, 0x1E},
    {0x00, 0x36, 0x36, 0x00, 0x00}, {0x00, 0x56, 0x36, 0x00, 0x00},
    {0x00, 0x08, 0x14, 0x22, 0x41}, {0x14, 0x14, 0x14, 0x14, 0x14},
    {0x41, 0x22, 0x14, 0x08, 0x00}, {0x02, 0x01, 0x51, 0x09, 0x06},
    {0x32, 0x49, 0x79, 0x41, 0x3E}, {0x7E, 0x11, 0x11, 0x11, 0x7E},
    {0x7F, 0x49, 0x49, 0x49, 0x36}, {0x3E, 0x41, 0x41, 0x41, 0x22},
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, {0x7F, 0x49, 0x49, 0x49, 0x41},
    {0x7F, 0x09, 0x09, 0x01, 0x01}, {0x3E, 0x41, 0x41, 0x51, 0x32},
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, {0x00, 0x41, 0x7F, 0x41, 0x00},
    {0x20, 0x40, 0x41, 0x3F, 0x01}, {0x7F, 0x08, 0x14, 0x22, 0x41},
    {0x7F, 0x40, 0x40, 0x40, 0x40}, {0x7F, 0x02, 0x04, 0x02, 0x7F},
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, {0x3E, 0x41, 0x41, 0x41, 0x3E},
    {0x7F, 0x09, 0x09, 0x09, 0x06}, {0x3E, 0x41, 0x51, 0x21, 0x5E},
    {0x7F, 0x09, 0x19, 0x29, 0x46}, {0x46, 0x49, 0x49, 0x49, 0x31},
    {0x01, 0x01, 0x7F, 0x01, 0x01}, {0x3F, 0x40, 0x40, 0x40, 0x3F},
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, {0x7F, 0x20, 0x18, 0x20, 0x7F},
    {0x63, 0x14, 0x08, 0x14, 0x63}, {0x03, 0x04, 0x78, 0x04, 0x03},
    {0x61, 0x51, 0x49, 0x45, 0x43}, {0x00, 0x00, 0x7F, 0x41, 0x41},
    {0x02, 0x04, 0x08, 0x10, 0x20}, {0x41, 0x41, 0x7F, 0x00, 0x00},
    {0x04, 0x02, 0x01, 0x02, 0x04}, {0x40, 0x40, 0x40, 0x40, 0x40},
    {0x00, 0x01, 0x02, 0x04, 0x00}, {0x20, 0x54, 0x54, 0x54, 0x78},
    {0x7F, 0x48, 0x44, 0x44, 0x38}, {0x38, 0x44, 0x44, 0x44, 0x20},
    {0x38, 0x44, 0x44, 0x48, 0x7F}, {0x38, 0x54, 0x54, 0x54, 0x18},
    {0x08, 0x7E, 0x09, 0x01, 0x02}, {0x0C, 0x52, 0x52, 0x52, 0x3E},
    {0x7F, 0x08, 0x04, 0x04, 0x78}, {0x00, 0x44, 0x7D, 0x40, 0x00},
    {0x20, 0x40, 0x44, 0x3D, 0x00}, {0x00, 0x7F, 0x10, 0x28, 0x44},
    {0x00, 0x41, 0x7F, 0x40, 0x00}, {0x7C, 0x04, 0x18, 0x04, 0x78},
    {0x7C, 0x08, 0x04, 0x04, 0x78}, {0x38, 0x44, 0x44, 0x44, 0x38},
    {0x7C, 0x14, 0x14, 0x14, 0x08}, {0x08, 0x14, 0x14, 0x18, 0x7C},
    {0x7C, 0x08, 0x04, 0x04, 0x08}, {0x48, 0x54, 0x54, 0x54, 0x20},
    {0x04, 0x3F, 0x44, 0x40, 0x20}, {0x3C, 0x40, 0x40, 0x20, 0x7C},
    {0x1C, 0x20, 0x40, 0x20, 0x1C}, {0x3C, 0x40, 0x30, 0x40, 0x3C},
    {0x44, 0x28, 0x10, 0x28, 0x44}, {0x0C, 0x50, 0x50, 0x50, 0x3C},
    {0x44, 0x64, 0x54, 0x4C, 0x44}, {0x00, 0x08, 0x36, 0x41, 0x00},
    {0x00, 0x00, 0x7F, 0x00, 0x00}, {0x00, 0x41, 0x36, 0x08, 0x00},
    {0x08, 0x04, 0x08, 0x10, 0x08}
};

Calliope_SmartShield::Calliope_SmartShield() {
    deviceId = 0x123456789ABCDEF0ULL;
    displayServiceNum = 1;
    gamepadServiceNum = 2;
    connected = false;
    buttonState = 0;
    lastButtonState = 0;
    memset(framebuffer, 0, sizeof(framebuffer));
}

Calliope_SmartShield::~Calliope_SmartShield() {
}

void Calliope_SmartShield::begin() {
    pinMode(PIN_DC, INPUT_PULLDOWN);
    pinMode(PIN_RST, OUTPUT);
    
    // Initialize SPI at 16MHz
    SPI.begin();
    SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));
    
    // Reset STM32
    digitalWrite(PIN_RST, LOW);
    delay(20);
    digitalWrite(PIN_RST, HIGH);
    delay(500);
}

void Calliope_SmartShield::loop() {
    if (digitalRead(PIN_DC) == HIGH && !connected) {
        connected = true;
    }
}

bool Calliope_SmartShield::isConnected() {
    return connected;
}

void* Calliope_SmartShield::pushPacket(uint8_t service_num, uint16_t service_cmd, uint8_t size) {
    uint8_t* dst = txFrame.data + txFrame.size;
    *dst++ = size;
    *dst++ = service_num;
    *dst++ = service_cmd & 0xFF;
    *dst++ = service_cmd >> 8;
    txFrame.size += ((size + 4 + 3) & ~3);
    return dst;
}

void Calliope_SmartShield::transmitFrame() {
    txFrame.crc = JDSPI_MAGIC;
    for (int i = 0; i < sizeof(jd_frame_t); i++) {
        ((uint8_t*)&rxFrame)[i] = SPI.transfer(((uint8_t*)&txFrame)[i]);
    }
}

void Calliope_SmartShield::processResponse() {
    if (rxFrame.crc == JDSPI_MAGIC && rxFrame.size > 0) {
        jd_packet_t* pkt = (jd_packet_t*)rxFrame.data;
        
        if (pkt->service_number == gamepadServiceNum && 
            pkt->service_command == JD_GET(JD_REG_READING)) {
            
            lastButtonState = buttonState;
            buttonState = 0;
            
            int numButtons = pkt->service_size / 2;
            for (int i = 0; i < numButtons; i++) {
                uint8_t buttonIdx = pkt->data[i * 2];
                uint8_t pressure = pkt->data[i * 2 + 1];
                
                if (pressure > 0 && buttonIdx >= 1 && buttonIdx <= 7) {
                    buttonState |= (1 << buttonIdx);
                }
            }
        }
    }
}

void Calliope_SmartShield::setPalette(uint32_t* palette) {
    if (!connected) return;
    
    memset(&txFrame, 0, sizeof(txFrame));
    txFrame.device_identifier = deviceId;
    txFrame.size = 0;
    
    void* data = pushPacket(displayServiceNum, 
                           JD_SET(JD_INDEXED_SCREEN_REG_PALETTE), 64);
    memcpy(data, palette, 64);
    transmitFrame();
}

void Calliope_SmartShield::setBrightness(uint8_t brightness) {
    if (!connected) return;
    
    memset(&txFrame, 0, sizeof(txFrame));
    txFrame.device_identifier = deviceId;
    txFrame.size = 0;
    
    uint8_t* data = (uint8_t*)pushPacket(
        displayServiceNum, JD_SET(JD_INDEXED_SCREEN_REG_BRIGHTNESS), 1);
    *data = brightness;
    transmitFrame();
}

void Calliope_SmartShield::clearFramebuffer(uint8_t color) {
    uint8_t packed = (color << 4) | color;
    memset(framebuffer, packed, sizeof(framebuffer));
}

void Calliope_SmartShield::setPixel(uint16_t x, uint16_t y, uint8_t color) {
    if (x >= SHIELD_WIDTH || y >= SHIELD_HEIGHT) return;
    
    int byteIdx = x * 60 + (y / 2);
    
    if (y % 2 == 0) {
        framebuffer[byteIdx] = (framebuffer[byteIdx] & 0xF0) | (color & 0x0F);
    } else {
        framebuffer[byteIdx] = (framebuffer[byteIdx] & 0x0F) | (color << 4);
    }
}

uint8_t Calliope_SmartShield::getPixel(uint16_t x, uint16_t y) {
    if (x >= SHIELD_WIDTH || y >= SHIELD_HEIGHT) return 0;
    
    int byteIdx = x * 60 + (y / 2);
    return (y % 2 == 0) ? (framebuffer[byteIdx] & 0x0F) : (framebuffer[byteIdx] >> 4);
}

void Calliope_SmartShield::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color) {
    for (uint16_t py = y; py < y + h && py < SHIELD_HEIGHT; py++) {
        for (uint16_t px = x; px < x + w && px < SHIELD_WIDTH; px++) {
            setPixel(px, py, color);
        }
    }
}

void Calliope_SmartShield::transmitFramebuffer(bool verbose) {
    if (!connected) return;
    
    // Send START_UPDATE command
    memset(&txFrame, 0, sizeof(txFrame));
    txFrame.device_identifier = deviceId;
    txFrame.size = 0;
    
    jd_indexed_screen_start_update_t* cmd = (jd_indexed_screen_start_update_t*)pushPacket(
        displayServiceNum, JD_INDEXED_SCREEN_CMD_START_UPDATE, 8);
    cmd->x = 0;
    cmd->y = 0;
    cmd->width = SHIELD_WIDTH;
    cmd->height = SHIELD_HEIGHT;
    transmitFrame();
    processResponse();
    
    // Send pixel data in batches of 3 columns
    for (int startCol = 0; startCol < SHIELD_WIDTH; startCol += 3) {
        while (digitalRead(PIN_DC) == LOW) {}
        
        memset(&txFrame, 0, sizeof(txFrame));
        txFrame.device_identifier = deviceId;
        txFrame.size = 0;
        
        for (int batch = 0; batch < 3 && (startCol + batch) < SHIELD_WIDTH; batch++) {
            int col = startCol + batch;
            void* data = pushPacket(displayServiceNum, 
                                   JD_INDEXED_SCREEN_CMD_SET_PIXELS, 60);
            memcpy(data, &framebuffer[col * 60], 60);
        }
        
        transmitFrame();
        processResponse();
    }
}

void Calliope_SmartShield::drawChar(int16_t x, int16_t y, char c, uint8_t color, uint8_t bg, uint8_t size) {
    if (c < 32 || c > 126) return;
    
    int charIndex = c - 32;
    
    for (uint8_t col = 0; col < 5; col++) {
        uint8_t line = pgm_read_byte(&font5x7[charIndex][col]);
        for (uint8_t row = 0; row < 8; row++) {
            if (line & 0x1) {
                if (size == 1) {
                    setPixel(x + col, y + row, color);
                } else {
                    fillRect(x + col * size, y + row * size, size, size, color);
                }
            } else if (bg != color) {
                if (size == 1) {
                    setPixel(x + col, y + row, bg);
                } else {
                    fillRect(x + col * size, y + row * size, size, size, bg);
                }
            }
            line >>= 1;
        }
    }
    
    if (bg != color) {
        for (uint8_t row = 0; row < 8 * size; row++) {
            if (size == 1) {
                setPixel(x + 5, y + row, bg);
            } else {
                fillRect(x + 5 * size, y + row, size, 1, bg);
            }
        }
    }
}

void Calliope_SmartShield::drawText(int16_t x, int16_t y, const char* text, uint8_t color, uint8_t bg, uint8_t size) {
    if (!text) return;
    
    int16_t cursorX = x;
    int16_t cursorY = y;
    int charWidth = 6 * size;
    int charHeight = 8 * size;
    bool drawBg = (bg != 255);
    
    while (*text) {
        if (*text == '\n') {
            cursorY += charHeight;
            cursorX = x;
        } else if (*text == '\r') {
            cursorX = x;
        } else {
            if (cursorX + charWidth > SHIELD_WIDTH) {
                cursorX = x;
                cursorY += charHeight;
            }
            if (cursorY + charHeight > SHIELD_HEIGHT) {
                break;
            }
            
            drawChar(cursorX, cursorY, *text, color, drawBg ? bg : color, size);
            cursorX += charWidth;
        }
        text++;
    }
}

// Button functions
uint8_t Calliope_SmartShield::readButtons() { return buttonState; }

bool Calliope_SmartShield::isButtonPressed(uint8_t mask) { 
    return (buttonState & mask) != 0; 
}

bool Calliope_SmartShield::isButtonDown(uint8_t mask) { 
    return (buttonState & mask) && !(lastButtonState & mask); 
}

bool Calliope_SmartShield::isButtonUp(uint8_t mask) { 
    return !(buttonState & mask) && (lastButtonState & mask); 
}

bool Calliope_SmartShield::leftPressed() { return isButtonPressed(SHIELD_BUTTON_LEFT); }
bool Calliope_SmartShield::upPressed() { return isButtonPressed(SHIELD_BUTTON_UP); }
bool Calliope_SmartShield::rightPressed() { return isButtonPressed(SHIELD_BUTTON_RIGHT); }
bool Calliope_SmartShield::downPressed() { return isButtonPressed(SHIELD_BUTTON_DOWN); }
bool Calliope_SmartShield::aPressed() { return isButtonPressed(SHIELD_BUTTON_A); }
bool Calliope_SmartShield::bPressed() { return isButtonPressed(SHIELD_BUTTON_B); }
bool Calliope_SmartShield::menuPressed() { return isButtonPressed(SHIELD_BUTTON_MENU); }

bool Calliope_SmartShield::leftDown() { return isButtonDown(SHIELD_BUTTON_LEFT); }
bool Calliope_SmartShield::upDown() { return isButtonDown(SHIELD_BUTTON_UP); }
bool Calliope_SmartShield::rightDown() { return isButtonDown(SHIELD_BUTTON_RIGHT); }
bool Calliope_SmartShield::downDown() { return isButtonDown(SHIELD_BUTTON_DOWN); }
bool Calliope_SmartShield::aDown() { return isButtonDown(SHIELD_BUTTON_A); }
bool Calliope_SmartShield::bDown() { return isButtonDown(SHIELD_BUTTON_B); }
bool Calliope_SmartShield::menuDown() { return isButtonDown(SHIELD_BUTTON_MENU); }
