/*
 * experimental_shield_test_no_can.ino
 *
 * Alternative test suite for the Arduino UNO Experimental Shield on Arduino UNO R4.
 * CAN test removed as requested.
 * 
 * Manual Mode: Select test using buttons D16, D17.
 * 
 * Logic:
 * 1. LED Chase (Startup)
 * 2. Dashboard (Idle) - Live sensors, wait for button
 * 3. Prep - Show Jumper warnings, wait for button
 * 4. Run - Execute test at 1s rate until button pressed
 *
 * Libraries: Adafruit SSD1306, Adafruit GFX, SPI, Wire
 */

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ─── PIN DEFINITIONS ────────────────────────────────────────────────────────
const int LED_PINS[] = {2, 3, 6, 7, 8, 9};
const int NUM_LEDS   = 6;

// Buttons connected to Analog Pins (A2=D16, A3=D17)
const int BTN_PINS[] = {16, 17}; 
const int NUM_BTNS   = 2;

const int PIN_POT1   = A0;
const int PIN_POT2   = A1;
const int PIN_RS485_CTRL = 5; // DE/RE
const int PIN_EEPROM_CS  = 10;

// ─── CONSTANTS ──────────────────────────────────────────────────────────────
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDR     0x3C
#define TMP102_ADDR   0x48

// ─── OBJECTS ────────────────────────────────────────────────────────────────
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ─── STATE ──────────────────────────────────────────────────────────────────
enum AppState {
  PHASE_LEDS,
  PHASE_DASHBOARD,
  PHASE_TEST_PREP,
  PHASE_TEST_RUN
};

AppState currentState = PHASE_LEDS;
int activeTestIndex = 0; // 0=EEPROM, 1=RS485, 2=USB
unsigned long lastUpdate = 0;
const char* testNames[] = {"EEPROM", "RS485", "USB-CH340"};
const int NUM_TESTS = 3;

// ─── HELPERS ────────────────────────────────────────────────────────────────

// Read TMP102 Temperature
float readTMP102() {
  Wire.beginTransmission(TMP102_ADDR);
  Wire.write(0x00);
  Wire.endTransmission();
  if (Wire.requestFrom(TMP102_ADDR, 2) == 2) {
    uint8_t msb = Wire.read();
    uint8_t lsb = Wire.read();
    int tempSum = ((msb << 8) | lsb) >> 4;
    return tempSum * 0.0625;
  }
  return 0.0;
}

void drawHeader(const char* title) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(title);
  display.drawFastHLine(0, 9, 128, SSD1306_WHITE);
}

// Robust button polling (D16-D17)
int getButtonPressed() {
  for (int i = 0; i < NUM_BTNS; i++) {
    pinMode(BTN_PINS[i], INPUT_PULLUP);
    if (digitalRead(BTN_PINS[i]) == LOW) {
      delay(20);
      if (digitalRead(BTN_PINS[i]) == LOW) {
        while(digitalRead(BTN_PINS[i]) == LOW); // Wait release
        return i + 1;
      }
    }
  }
  return 0;
}

void getSensorString(char* buf, int size, const char* label) {
  int p1 = analogRead(PIN_POT1);
  int p2 = analogRead(PIN_POT2);
  float t = readTMP102();
  
  // dtostrf is not always available on non-AVR boards like UNO R4.
  // Using String conversion as a more portable alternative.
  String tStr = String(t, 1);
  snprintf(buf, size, "%s: P1=%d P2=%d T=%s", label, p1, p2, tStr.c_str());
}

// ─── SETUP ──────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);
  Wire.begin();
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("OLED fail"));
  }
  display.clearDisplay();
  display.display();

  pinMode(PIN_RS485_CTRL, OUTPUT);
  digitalWrite(PIN_RS485_CTRL, LOW);
  pinMode(PIN_EEPROM_CS, OUTPUT);
  digitalWrite(PIN_EEPROM_CS, HIGH);
}

// ─── MAIN LOOP ──────────────────────────────────────────────────────────────
void loop() {
  unsigned long now = millis();

  switch (currentState) {
    
    case PHASE_LEDS: {
      drawHeader("UNO-EXP NO CAN");
      display.setCursor(0, 20);
      display.println("Phase 1: LED Chase");
      display.display();
      for (int i = 0; i < NUM_LEDS; i++) {
        pinMode(LED_PINS[i], OUTPUT);
        digitalWrite(LED_PINS[i], HIGH);
        delay(500);
        digitalWrite(LED_PINS[i], LOW);
      }
      currentState = PHASE_DASHBOARD;
      break;
    }

    case PHASE_DASHBOARD: {
      if (now - lastUpdate >= 100) {
        lastUpdate = now;
        drawHeader("DASHBOARD (IDLE)");
        int p1 = analogRead(PIN_POT1);
        int p2 = analogRead(PIN_POT2);
        float t = readTMP102();
        display.setCursor(0, 15);
        display.print("POT1: "); display.println(p1);
        display.print("POT2: "); display.println(p2);
        display.print("TEMP: "); display.print(t); display.println(" C");
        display.setCursor(0, 44);
        display.print("> "); display.println(testNames[activeTestIndex]);
        display.println("B1:Cycle  B2:Select");
        display.display();
      }

      int btn = getButtonPressed();
      if (btn == 1) { // D16 - Cycle
        activeTestIndex = (activeTestIndex + 1) % NUM_TESTS;
      } else if (btn == 2) { // D17 - Select
        currentState = PHASE_TEST_PREP;
      }
      break;
    }

    case PHASE_TEST_PREP: {
      drawHeader("TEST PREP");
      display.setCursor(0, 15);
      display.print("Test: "); display.println(testNames[activeTestIndex]);
      display.setCursor(0, 28);
      
      if (activeTestIndex == 0) display.println("SET JP2 -> CS");
      else if (activeTestIndex == 1) display.println("SET JP3,4 -> RS485");
      else if (activeTestIndex == 2) display.println("CHECK SHIELD USB");
      
      display.setCursor(0, 55);
      display.println("Press ANY to start");
      display.display();

      if (getButtonPressed() > 0) {
        if (activeTestIndex == 0) SPI.begin();
        currentState = PHASE_TEST_RUN;
      }
      break;
    }

    case PHASE_TEST_RUN: {
      if (now - lastUpdate >= 1000) {
        lastUpdate = now;
        char data[100];
        static uint8_t testVal = 0x42;
        testVal = (testVal == 0x42) ? 0xA5 : 0x42;
        
        if (activeTestIndex == 0) { // EEPROM
           digitalWrite(PIN_EEPROM_CS, LOW); SPI.transfer(0x06); digitalWrite(PIN_EEPROM_CS, HIGH); delay(1);
           digitalWrite(PIN_EEPROM_CS, LOW); SPI.transfer(0x02); SPI.transfer(0x00); SPI.transfer(0x00); SPI.transfer(testVal); digitalWrite(PIN_EEPROM_CS, HIGH);
           delay(10);
           digitalWrite(PIN_EEPROM_CS, LOW); SPI.transfer(0x03); SPI.transfer(0x00); SPI.transfer(0x00); uint8_t val = SPI.transfer(0x00); digitalWrite(PIN_EEPROM_CS, HIGH);
           drawHeader("EEPROM RUNNING");
           display.setCursor(0, 20);
           display.print("Tgt: 0x"); display.println(testVal, HEX);
           display.print("Read: 0x"); display.println(val, HEX);
           display.println(val == testVal ? "PASS" : "FAIL");
        }
        else if (activeTestIndex == 1) { // RS485
          getSensorString(data, sizeof(data), "RS485");
          digitalWrite(PIN_RS485_CTRL, HIGH);
          Serial1.println(data);
          Serial1.flush();
          digitalWrite(PIN_RS485_CTRL, LOW);
          drawHeader("RS485 RUNNING");
          display.setCursor(0,20);
          display.println("TX Active\nCheck A/B lines");
        }
        else if (activeTestIndex == 2) { // USB
          getSensorString(data, sizeof(data), "USB");
          Serial1.println(data);
          drawHeader("USB RUNNING");
          display.setCursor(0,20);
          display.println("TX Active\nCheck Shield USB");
        }

        display.setCursor(0, 55);
        display.println("Press ANY to exit");
        display.display();
      }

      if (getButtonPressed() > 0) {
        currentState = PHASE_DASHBOARD;
      }
      break;
    }
  }
}
