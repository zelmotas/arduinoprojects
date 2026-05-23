#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Change 0x27 to 0x3F if screen is blank

// --- PIN DEFINITIONS ---
#define pirPin 2
#define armButton 4
#define disarmButton 5
#define sirenPin 7     
#define beepPin 8      
#define bluePin 9
#define greenPin 10
#define redPin 11
#define potPin A0       

// --- SYSTEM STATES ---
enum SystemState { DISARMED, ARMING, ARMED, ALARM };
SystemState currentState = DISARMED;

unsigned long stateTimer = 0;
unsigned long flashTimer = 0;
bool flashState = false;
int countdown = 10;

// --- PIR MOTION FILTER VARIABLES ---
unsigned long motionStartTime = 0;
bool isDetectingMotion = false;

// --- COMBINATION LOCK VARIABLES ---
int secretCode[3] = {4, 2, 7}; // <--- CHANGE YOUR PASSCODE HERE!
int enteredCode[3] = {0, 0, 0};
int currentDigit = 0;          
int previousDisarmState = HIGH; 

void setup() {
  Serial.begin(9600);
  
  lcd.init();
  lcd.backlight();

  pinMode(pirPin, INPUT);
  pinMode(armButton, INPUT_PULLUP);
  pinMode(disarmButton, INPUT_PULLUP);
  
  pinMode(sirenPin, OUTPUT);
  pinMode(beepPin, OUTPUT);
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  lcd.setCursor(0, 0);
  lcd.print("Calibrating PIR.");
  delay(3000); 
  setLED(0, 1, 0); 
}

void loop() {
  unsigned long currentMillis = millis();

  // Look for a single "click" of the Select button
  int disarmButtonState = digitalRead(disarmButton);
  bool selectPressed = (previousDisarmState == HIGH && disarmButtonState == LOW);
  previousDisarmState = disarmButtonState;

  // ---------------------------------------------------------
  // STATE 1: DISARMED
  // ---------------------------------------------------------
  if (currentState == DISARMED) {
    lcd.setCursor(0, 0);
    lcd.print("System Ready    ");
    lcd.setCursor(0, 1);
    lcd.print("Press to Arm    ");
    
    setLED(0, 1, 0); // Green
    digitalWrite(sirenPin, LOW); 

    if (digitalRead(armButton) == LOW) {
      currentState = ARMING;
      countdown = 10;           
      currentDigit = 0;         
      isDetectingMotion = false; // Reset motion tracker
      stateTimer = currentMillis;
      tone(beepPin, 1000, 200); 
      delay(300);               
      lcd.clear();
    }
  }

  // ---------------------------------------------------------
  // STATE 2: ARMING (Countdown)
  // ---------------------------------------------------------
  else if (currentState == ARMING) {
    lcd.setCursor(0, 0);
    lcd.print("Arming in: ");
    lcd.print(countdown);
    lcd.print("   "); 

    setLED(1, 1, 0); // Yellow

    if (currentMillis - stateTimer >= 1000) {
      stateTimer = currentMillis;
      countdown--;
      tone(beepPin, 600, 100); 
    }

    if (countdown <= 0) {
      currentState = ARMED;
      tone(beepPin, 1500, 600); 
      lcd.clear();
    }
  }

  // ---------------------------------------------------------
  // STATES 3 & 4: ARMED or ALARM (The Safe Dial Active!)
  // ---------------------------------------------------------
  else if (currentState == ARMED || currentState == ALARM) {
    
    // 1. Read the dial and shrink it to a number between 0 and 9
    int potValue = analogRead(potPin);
    int dialNumber = map(potValue, 0, 1023, 0, 9);

    // 2. Handle the Top Line of the LCD & Sensors
    if (currentState == ARMED) {
      setLED(1, 0, 0); // Solid Red

      // LIVE TIMER DISPLAY: Check if we are currently tracking motion
      if (isDetectingMotion) {
        unsigned long elapsed = currentMillis - motionStartTime;
        int msLeft = 2000 - elapsed;
        if (msLeft < 0) msLeft = 0; // Don't let it go below 0
        
        lcd.setCursor(0, 0);
        lcd.print("Motion! ");
        lcd.print(msLeft);
        lcd.print("ms  "); // Extra spaces to clear leftover characters
      } else {
        lcd.setCursor(0, 0);
        lcd.print("SYSTEM ARMED    ");
      }

      // --- THE 2-SECOND CONFIDENCE FILTER ---
      if (digitalRead(pirPin) == HIGH) {
        if (isDetectingMotion == false) {
          isDetectingMotion = true;
          motionStartTime = currentMillis; 
        } 
        else if (currentMillis - motionStartTime >= 2000) {
          // Time's up! Trigger the alarm!
          currentState = ALARM;
          currentDigit = 0; 
          Serial.println("NOTIFY_PHONE"); 
          isDetectingMotion = false; 
          lcd.clear();
        }
      } 
      else {
        isDetectingMotion = false; // Reset if motion stops before 2 seconds
      }
    } 
    else if (currentState == ALARM) {
      lcd.setCursor(0, 0);
      lcd.print("! INTRUDER !    ");
      digitalWrite(sirenPin, HIGH); // BLAST THE SIREN

      // Flash Police Lights
      if (currentMillis - flashTimer >= 100) {
        flashTimer = currentMillis;
        flashState = !flashState;
        if (flashState) setLED(1, 0, 0); 
        else setLED(0, 0, 1);            
      }
    }

    // 3. Draw the Code Interface on the Bottom Line
    lcd.setCursor(0, 1);
    lcd.print("Code: ");
    for (int i = 0; i < 3; i++) {
      if (i < currentDigit) {
        lcd.print("* "); 
      } else if (i == currentDigit) {
        lcd.print(dialNumber); 
        lcd.print(" ");
      } else {
        lcd.print("_ "); 
      }
    }
    lcd.print("  "); 

    // 4. Handle Password "Select" Button
    if (selectPressed) {
      enteredCode[currentDigit] = dialNumber; 
      currentDigit++;                         
      tone(beepPin, 1200, 100);               

      // If they finished entering 3 digits...
      if (currentDigit == 3) { 
        if (enteredCode[0] == secretCode[0] && 
            enteredCode[1] == secretCode[1] && 
            enteredCode[2] == secretCode[2]) {
          
          // CORRECT PASSCODE!
          currentState = DISARMED;
          isDetectingMotion = false; // Safety reset
          tone(beepPin, 2000, 500); 
          lcd.clear();
          
        } else {
          // WRONG PASSCODE!
          currentDigit = 0; 
          tone(beepPin, 150, 1000); 
          
          if (currentState == ARMED) {
            currentState = ALARM;
            Serial.println("NOTIFY_PHONE"); 
          }
        }
      }
    }
  }
}

// Helper function
void setLED(int r, int g, int b) {
  digitalWrite(redPin, r);
  digitalWrite(greenPin, g);
  digitalWrite(bluePin, b);
}