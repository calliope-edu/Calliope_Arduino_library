
/*
 * Serial Port over BLE
 * Create UART service compatible with Nordic's *nRF Toolbox* and Adafruit's *Bluefruit LE* iOS/Android apps.
 *
 * Copyright (c) Sandeep Mistry. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 * BLESerial class implements same protocols as Arduino's built-in Serial class and can be used as it's wireless
 * replacement. Data transfers are routed through a BLE service with TX and RX characteristics. To make the
 * service discoverable all UUIDs are NUS (Nordic UART Service) compatible.
 *
 * Please note that TX and RX characteristics use Notify and WriteWithoutResponse, so there's no guarantee
 * that the data will make it to the other end. However, under normal circumstances and reasonable signal
 * strengths everything works well.
 */

// Only works with Calliope mini 1 & 2. Go to Menu -> Tools -> Softdevice, Select S110 or S130
// Calliope mini 3 is not supported due to a lack of a soft device implementation

#include <Calliope_Arduino.h>

Calliope calliope;

void setup() {
  Serial.begin(115200);

  Serial.println("Calliope ready!");
  
  // custom services and characteristics can be added as well
  calliope.BTLESerial.setLocalName("calliope");
  calliope.BTLESerial.begin();

  // Start LED matrix driver after radio (required)
  calliope.begin();
}

void loop() {
  calliope.BTLESerial.poll();

  forward();
  //loopback();
  spam();
}


// forward received from Serial to calliope.BTLESerial and vice versa
void forward() {
  if (calliope.BTLESerial && Serial) {
    int byte;
    if (calliope.BTLESerial.available()) {
      Serial.write(calliope.BTLESerial.read());
    }
    char buffer[10];
    memset(buffer, 0x0, 10);
    int idx = 0;
    
    while (Serial.available() && idx != 10) {
       buffer[idx] = Serial.read();
       idx++;
    }
    if (idx) {
      calliope.BTLESerial.write(buffer, idx);
    }
  }
  delay(1);
}

// echo all received data back
void loopback() {
  if (calliope.BTLESerial) {
    int byte;
    while ((byte = calliope.BTLESerial.read()) > 0) {
        calliope.BTLESerial.write(byte);
    }
  }
}

// periodically sent time stamps
void spam() {
  if (calliope.BTLESerial) {
    calliope.BTLESerial.print(millis());
    calliope.BTLESerial.println(" tick-tacks!");
    delay(1000);
  }
}
