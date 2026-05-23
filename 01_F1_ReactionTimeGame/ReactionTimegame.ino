#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize the LCD (Address 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

int ledPin[4] = {5, 4, 3, 2};
#define pbPin 6
#define buzzerPin 9

unsigned long start;
unsigned long finalTime;
int randomDelay;

int previousButtonState = HIGH;
int currentButtonState;

// Note frequencies for the waiting song (C4, E4, G4, C5)
int melody[] = {262, 330, 392, 523, 392, 330};
int melodyLength = 6;

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  for(int i = 0; i < 4; i++){
    pinMode(ledPin[i], OUTPUT);
  }

  pinMode(pbPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);

  randomSeed(analogRead(A0));
}

void loop() {
  bool waitingNext = true;
  bool reacting = false;

  // 1. Get Ready phase
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Get Ready...");
  for(int i = 0; i < 4; i++){
    digitalWrite(ledPin[i], HIGH);
    tone(buzzerPin, 440, 200);
    delay(1000);
  }

  // 2. Random delay
  randomDelay = random(2000, 6000);
  delay(randomDelay);

  // 3. Lights out & GO beep!
  for(int i = 0; i < 4; i++){
    digitalWrite(ledPin[i], LOW);
  }
  tone(buzzerPin, 1000, 500);

  // 4. Reacting phase
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PRESS BUTTON!");
  reacting = true;
  start = millis();
  unsigned long lastDisplayUpdate = 0;

  while(reacting) {
    currentButtonState = digitalRead(pbPin);
    unsigned long currentMillis = millis();
    unsigned long elapsed = currentMillis - start;

    if(previousButtonState == HIGH && currentButtonState == LOW) {
      finalTime = elapsed;
      reacting = false;
      tone(buzzerPin, 1500, 100);
    }

    if (reacting && (currentMillis - lastDisplayUpdate >= 50)) {
      lcd.setCursor(0, 1);
      lcd.print(elapsed);
      lcd.print(" ms ");
      lastDisplayUpdate = currentMillis;
    }

    previousButtonState = currentButtonState;
  }

  // 5. Show Final Time
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Reaction Time:");
  lcd.setCursor(0, 1);
  lcd.print(finalTime);
  lcd.print(" ms");
  Serial.print("Reaction Time: ");
  Serial.println(finalTime);
  delay(2000);

  // 6. Restart phase with Dance and Music!
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press button");
  lcd.setCursor(0, 1);
  lcd.print("to play again!");
  unsigned long lastDanceUpdate = 0;
  int danceStep = 0;

  while (waitingNext) {
    currentButtonState = digitalRead(pbPin);

    if(previousButtonState == HIGH && currentButtonState == LOW){
      waitingNext = false;
      noTone(buzzerPin);
      for(int i = 0; i < 4; i++){
        digitalWrite(ledPin[i], LOW);
      }
    }

    if (waitingNext && (millis() - lastDanceUpdate >= 200)) {
      lastDanceUpdate = millis();
      tone(buzzerPin, melody[danceStep], 100);
      for(int i = 0; i < 4; i++){
        digitalWrite(ledPin[i], LOW);
      }
      int activeLed = danceStep;
      if (activeLed > 3) {
        activeLed = 6 - activeLed;
      }
      digitalWrite(ledPin[activeLed], HIGH);
      danceStep++;
      if (danceStep >= melodyLength) {
        danceStep = 0;
      }
    }

    previousButtonState = currentButtonState;
  }
}
