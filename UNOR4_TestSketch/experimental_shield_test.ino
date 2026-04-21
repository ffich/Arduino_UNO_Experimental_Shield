/*
 * experimental_shield_test.ino
 *
 * Full test suite for the Arduino UNO Experimental Shield on Arduino UNO R4.
 * 
 * Manual Mode: Select test using buttons D16, D17, D18, D19.
 * 
 * Logic:
 * 1. LED Chase (Startup)
 * 2. Dashboard (Idle) - Live sensors, wait for button
 * 3. Prep - Show Jumper warnings, wait for button
 * 4. Run - Execute test at 1s rate until button pressed
 *
 * Libraries: Adafruit SSD1306, Adafruit GFX, Arduino_CAN, SPI, Wire
 */

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino_CAN.h>

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

AppState currentState = PHASE_DASHBOARD;
int activeTestIndex = 0; // 0=EEPROM, 1=CAN, 2=RS485, 3=USB, 4=SUPERCAR, 5=GAME
unsigned long lastUpdate = 0;
const char* testNames[] = {"EEPROM", "CAN", "RS485", "USB-CH340", "SUPERCAR", "GAME"};
const int NUM_TESTS = 6;

// PERSISTENT DATA
int sessionHighScore = 0;

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

// Robust button polling (D16-D19)
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
  char tStr[10];
  dtostrf(t, 4, 1, tStr);
  snprintf(buf, size, "%s: P1=%d P2=%d T=%s", label, p1, p2, tStr);
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

  // Initialize LED pins
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }
}

// ─── MAIN LOOP ──────────────────────────────────────────────────────────────
void loop() {
  unsigned long now = millis();

  switch (currentState) {
    
    case PHASE_LEDS: {
      drawHeader("UNO-EXP MANUAL");
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
      if (activeTestIndex == 1) display.println("SET JP1,2 -> CAN");
      if (activeTestIndex == 2) display.println("SET JP3,4 -> RS485");
      if (activeTestIndex == 3) display.println("CHECK SHIELD USB");
      if (activeTestIndex == 4) display.println("OBSERVE LEDS");
      if (activeTestIndex == 5) display.println("D16:L  D17:R");
      
      display.setCursor(0, 55);
      display.println("Press ANY to start");
      display.display();

      if (getButtonPressed() > 0) {
        if (activeTestIndex == 1) CAN.begin(CanBitRate::BR_500k);
        if (activeTestIndex == 0) SPI.begin();
        currentState = PHASE_TEST_RUN;
      }
      break;
    }

    case PHASE_TEST_RUN: {
      static unsigned long holdStart = 0;
      bool isAnyBtnDown = (digitalRead(BTN_PINS[0]) == LOW || digitalRead(BTN_PINS[1]) == LOW);

      // Standard tests update (1s)
      if (activeTestIndex < 4 && (now - lastUpdate >= 1000)) {
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
        else if (activeTestIndex == 1) { // CAN
          uint8_t msgData[8];
          snprintf((char*)msgData, 8, "P1:%d", analogRead(PIN_POT1));
          CAN.write(CanMsg(0x10, 8, msgData));
          drawHeader("CAN RUNNING");
          display.setCursor(0,20);
          display.println("TX ID 0x10\nSensor data out");
        }
        else if (activeTestIndex == 2) { // RS485
          getSensorString(data, sizeof(data), "RS485");
          digitalWrite(PIN_RS485_CTRL, HIGH);
          Serial1.println(data);
          Serial1.flush();
          digitalWrite(PIN_RS485_CTRL, LOW);
          drawHeader("RS485 RUNNING");
          display.setCursor(0,20);
          display.println("TX Active\nCheck A/B lines");
        }
        else if (activeTestIndex == 3) { // USB
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

      // Fast update for SUPERCAR effect (frequency controlled by A0 Pot)
      int scanSpeed = 100;
      if (activeTestIndex == 4) {
        scanSpeed = map(analogRead(PIN_POT1), 0, 1023, 20, 500);
      }

      if (activeTestIndex == 4 && (now - lastUpdate >= scanSpeed)) {
        lastUpdate = now;
        static int pos = 0;
        static int dir = 1;
        
        // Clear all LEDs
        for(int i=0; i<NUM_LEDS; i++) digitalWrite(LED_PINS[i], LOW);
        
        // Scan effect
        digitalWrite(LED_PINS[pos], HIGH);
        pos += dir;
        if(pos >= NUM_LEDS-1) { pos = NUM_LEDS-1; dir = -1; }
        if(pos <= 0) { pos = 0; dir = 1; }
        
        drawHeader("SUPERCAR EFFECT");
        display.setCursor(0,20);
        display.print("Speed: "); display.print(scanSpeed); display.println("ms");
        display.println("Adjust A0 Pot");
        display.setCursor(0, 55);
        display.println("Hold ANY to exit");
        display.display();
      }

      // Fast update for GAME (30ms refresh)
      if (activeTestIndex == 5 && (now - lastUpdate >= 30)) {
        lastUpdate = now;
        static int playerX = 64;
        static float obsX = 64, obsY = 0;
        static float speed = 1.0;
        static int score = 0;
        static bool isDead = false;
        static bool resetNeeded = true;

        if (resetNeeded) {
          playerX = 64; obsX = random(10, 110); obsY = 0; speed = 1.5; score = 0; isDead = false; resetNeeded = false;
        }

        if (!isDead) {
          // Movement - check buttons directly
          pinMode(BTN_PINS[0], INPUT_PULLUP);
          pinMode(BTN_PINS[1], INPUT_PULLUP);
          if (digitalRead(BTN_PINS[0]) == LOW) playerX -= 4;
          if (digitalRead(BTN_PINS[1]) == LOW) playerX += 4;
          playerX = constrain(playerX, 0, 118);

          // Obstacle logic
          obsY += speed;
          if (obsY > 64) {
            obsY = 0;
            obsX = random(5, 120);
            score++;
            speed += 0.05; // Difficulty increase
          }

          // Collision (Player at y=55, h=5, w=10. Obstacle 5x5)
          if (obsY > 50 && obsY < 60 && obsX > playerX - 5 && obsX < playerX + 10) {
            isDead = true;
            if (score > sessionHighScore) sessionHighScore = score;
          }
        }

        display.clearDisplay();
        if (isDead) {
          display.setCursor(35, 15); display.println("GAME OVER");
          display.setCursor(35, 30); display.print("Score: "); display.println(score);
          display.setCursor(20, 50); display.println("Hold ANY to exit");
          
          if (isAnyBtnDown) {
             if (holdStart == 0) holdStart = millis();
             if (millis() - holdStart > 1000) resetNeeded = true; // 1s hold to retry
          } else {
             holdStart = 0;
          }
        } else {
          // Draw Player
          display.fillRect(playerX, 55, 10, 5, SSD1306_WHITE);
          // Draw Obstacle
          display.fillRect((int)obsX, (int)obsY, 5, 5, SSD1306_WHITE);
          // HUD
          display.setCursor(0, 0); display.print("S:"); display.print(score);
          display.setCursor(50, 0); display.println("Hold:Exit");
          display.setCursor(95, 0); display.print("H:"); display.print(sessionHighScore);
        }
        display.display();
      }

      // ─── TEST EXIT LOGIC ──────────────────────────────────────────────────

      if (activeTestIndex >= 4) { // SUPERCAR (4) and GAME (5) use Long Press
        if (isAnyBtnDown) {
          if (holdStart == 0) holdStart = millis();
          if (millis() - holdStart > 2000) { // 2 second hold to exit
            for(int i=0; i<NUM_LEDS; i++) digitalWrite(LED_PINS[i], LOW);
            currentState = PHASE_DASHBOARD;
            holdStart = 0;
          }
        } else {
          holdStart = 0;
        }

        // Periodically update the UI to show hold progress if needed (optional)
        if (activeTestIndex == 4 || activeTestIndex == 5) {
          if (isAnyBtnDown && holdStart != 0) {
             display.fillRect(0, 62, map(millis() - holdStart, 0, 2000, 0, 128), 2, SSD1306_WHITE);
             display.display();
          }
        }
      } 
      else { // Standard tests (0-3) use short press release
        if (getButtonPressed() > 0) {
          for(int i=0; i<NUM_LEDS; i++) digitalWrite(LED_PINS[i], LOW);
          currentState = PHASE_DASHBOARD;
        }
      }
      break;
    }
  }
}
