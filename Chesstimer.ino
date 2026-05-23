#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize the LCD (Address is usually 0x27 or 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin Definitions
const int pinR = 11;
const int pinG = 10;
const int pinB = 9;
const int btn1 = 5; // Player 1 (Red)
const int btn2 = 4; // Player 2 (Blue)
const int speaker = 8;

// Game States
enum GameState { SETUP, READY, P1_TURN, P2_TURN, GAME_OVER };
GameState state = SETUP;

// Time Settings (in seconds)
const int timeModes[] = {60, 180, 300, 600}; // 1 min, 3 min, 5 min, 10 min
int modeIdx = 0;

// Game Variables
long p1Time = 0;
long p2Time = 0;
unsigned long lastTick = 0;

// Button Debounce & Edge Detection
bool lastBtn1State = HIGH;
bool lastBtn2State = HIGH;

void setup() {
  // Setup Pins
  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(speaker, OUTPUT);

  // Setup LCD
  lcd.init();
  lcd.backlight();
  
  // Enter Setup Mode
  updateSetupDisplay();
  setLED(255, 255, 255); // White LED for setup
}

void loop() {
  // Read Buttons (LOW means pressed because of INPUT_PULLUP)
  bool b1Reading = digitalRead(btn1);
  bool b2Reading = digitalRead(btn2);
  
  // Detect button presses (transitions from HIGH to LOW)
  bool b1Pressed = (b1Reading == LOW && lastBtn1State == HIGH);
  bool b2Pressed = (b2Reading == LOW && lastBtn2State == HIGH);
  
  lastBtn1State = b1Reading;
  lastBtn2State = b2Reading;

  unsigned long now = millis();

  switch (state) {
    
    // --- MODE 1: SELECTING TIME ---
    case SETUP:
      if (b1Pressed) {
        modeIdx = (modeIdx + 1) % 4; // Cycle through 0, 1, 2, 3
        updateSetupDisplay();
        beep(50, 1000);
      }
      if (b2Pressed) {
        p1Time = timeModes[modeIdx];
        p2Time = timeModes[modeIdx];
        state = READY;
        updateGameDisplay();
        setLED(0, 255, 0); // Green LED for Ready
        beep(150, 1500);
      }
      break;

    // --- MODE 2: WAITING FOR FIRST MOVE ---
    case READY:
      if (b1Pressed) { 
        state = P2_TURN; 
        lastTick = now; 
        setLED(0, 0, 255); // Blue for P2
        beep(100, 1200); 
      }
      else if (b2Pressed) { 
        state = P1_TURN; 
        lastTick = now; 
        setLED(255, 0, 0); // Red for P1
        beep(100, 1200); 
      }
      break;

    // --- MODE 3: PLAYER 1 COUNTDOWN ---
    case P1_TURN:
      if (now - lastTick >= 1000) {
        p1Time--;
        lastTick += 1000;
        updateGameDisplay();
        if (p1Time <= 0) { endGame(2); } // Time's up, Player 2 wins
      }
      if (b1Pressed) {
        state = P2_TURN;
        setLED(0, 0, 255); // Switch to P2's color (Blue)
        beep(80, 800);
      }
      break;

    // --- MODE 4: PLAYER 2 COUNTDOWN ---
    case P2_TURN:
      if (now - lastTick >= 1000) {
        p2Time--;
        lastTick += 1000;
        updateGameDisplay();
        if (p2Time <= 0) { endGame(1); } // Time's up, Player 1 wins
      }
      if (b2Pressed) {
        state = P1_TURN;
        setLED(255, 0, 0); // Switch to P1's color (Red)
        beep(80, 800);
      }
      break;

    // --- MODE 5: GAME OVER ---
    case GAME_OVER:
      if (b1Pressed || b2Pressed) {
        state = SETUP;
        updateSetupDisplay();
        setLED(255, 255, 255);
        beep(100, 1000);
      }
      break;
  }
  
  delay(10); // Very brief delay to debounce button noise
}

// ==========================================
// Helper Functions
// ==========================================

void setLED(int r, int g, int b) {
  analogWrite(pinR, r);
  analogWrite(pinG, g);
  analogWrite(pinB, b);
}

void beep(int duration, int freq) {
  tone(speaker, freq, duration);
}

void updateSetupDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("P1:Cycle P2:Pick");
  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  lcd.print(timeModes[modeIdx] / 60);
  lcd.print(" min");
}

void updateGameDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Player 1: "); formatTime(p1Time);
  lcd.setCursor(0, 1);
  lcd.print("Player 2: "); formatTime(p2Time);
}

void formatTime(long t) {
  int m = t / 60;
  int s = t % 60;
  lcd.print(m);
  lcd.print(":");
  if (s < 10) lcd.print("0");
  lcd.print(s);
}

void endGame(int winner) {
  state = GAME_OVER;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TIME UP!");
  lcd.setCursor(0, 1);
  
  if (winner == 1) {
    lcd.print("Player 1 Wins!");
    setLED(255, 0, 0); // Red
  } else {
    lcd.print("Player 2 Wins!");
    setLED(0, 0, 255); // Blue
  }
  
  // Play game over melody
  tone(speaker, 400, 500); delay(500);
  tone(speaker, 300, 1000);
}