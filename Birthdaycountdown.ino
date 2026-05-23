#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// 1. CREATE THE LCD OBJECT
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setColor(int red, int blue, int green);
void playHappyBirthday(void);

#define redPin 5
#define greenPin 3
#define bluePin 4
#define buzzerPin 8
#define buttonPin 2  // <--- NEW: Button connected to Pin 2

int timer = 0;            // Start at 0 seconds
int initialTimer = 0;     // Remembers the starting time for the color fade
unsigned long elapsedSecond = 0;

bool isRunning = false;   // <--- NEW: Tracks if we are in Setup Mode or Countdown Mode

// Button tracking variables
int lastButtonState = HIGH;
unsigned long lastClickTime = 0;

void setup() {
  lcd.init();       
  lcd.backlight();  

  lcd.setCursor(0, 0);          
  lcd.print("Set Time:      "); 

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
  // <--- NEW: Activates the internal pull-up resistor for the button!
  pinMode(buttonPin, INPUT_PULLUP); 
}

void loop() {
  
  // ==========================================
  // MODE 1: SETUP MODE (Waiting for button)
  // ==========================================
  if (isRunning == false) {
    lcd.setCursor(0, 1);
    lcd.print(timer);
    lcd.print(" Secs  ");
    
    int currentButtonState = digitalRead(buttonPin);
    
    // Check if button was just pressed (went from HIGH to LOW)
    if (lastButtonState == HIGH && currentButtonState == LOW) {
      unsigned long pressTime = millis();
      
      // If the last click was less than 400ms ago, it's a double-click!
      if (pressTime - lastClickTime < 400) {
        isRunning = true;         // Start the timer!
        initialTimer = timer;     // Lock in the total time for the fade math
        
        lcd.setCursor(0, 0);          
        lcd.print("Time Remaining:"); 
        elapsedSecond = millis(); // Reset the 1-second tracker
        setColor(0, 255, 0);      // Turn on initial Green light
      } 
      else {
        // Single click - add 5 seconds
        timer += 5; 
      }
      
      lastClickTime = pressTime;
      delay(50); // Simple debounce to prevent accidental double-clicks
    }
    
    lastButtonState = currentButtonState;
  } 
  
  // ==========================================
  // MODE 2: COUNTDOWN MODE
  // ==========================================
  else {
    lcd.setCursor(0, 1);
    lcd.print(timer);
    lcd.print(" Secs  "); 

    if (timer == 0){
      lcd.setCursor(0, 0);
      lcd.print("Timer is up!   ");

      while (true) {
        playHappyBirthday(); 
        delay(2000);
      }
    }

    if (millis() - elapsedSecond > 1000){
      timer--;
      elapsedSecond = millis();
    }

    // --- UPDATED GRADUAL FADE LOGIC ---
    // Now dynamically calculates based on whatever time you set!
    int halfTime = initialTimer / 2;

    if (timer > halfTime) {
      int redBrightness = map(timer, initialTimer, halfTime, 0, 255);
      setColor(redBrightness, 255, 0); 
    } 
    else if (timer > 0) {
      int greenBrightness = map(timer, halfTime, 0, 255, 0);
      setColor(255, greenBrightness, 0);
    }
    else {
      setColor(255, 0, 0); // Solid Red
    }
  }
}

void setColor(int red, int green, int blue){
  analogWrite(redPin, red);
  analogWrite(bluePin, blue);
  analogWrite(greenPin, green);
}

void playHappyBirthday(void) {
  int melody[] = {
    262, 262, 294, 262, 349, 330,
    262, 262, 294, 262, 392, 349,
    262, 262, 523, 440, 349, 330, 294,
    466, 466, 440, 349, 392, 349
  };

  int noteDurations[] = {
    250, 250, 500, 500, 500, 1000,
    250, 250, 500, 500, 500, 1000,
    250, 250, 500, 500, 500, 500, 1000,
    250, 250, 500, 500, 500, 1000
  };

  for (int i = 0; i < 25; i++) {
    if (i % 2 == 0) {
      lcd.backlight();
      setColor(255, 0, 0); 
    } else {
      lcd.noBacklight();
      setColor(0, 0, 0);   
    }

    int duration = noteDurations[i];
    tone(buzzerPin, melody[i], duration);
    
    int pauseBetweenNotes = duration * 1.30;
    delay(pauseBetweenNotes);
    
    noTone(buzzerPin);
  }

  lcd.backlight(); 
  setColor(255, 0, 0);
}