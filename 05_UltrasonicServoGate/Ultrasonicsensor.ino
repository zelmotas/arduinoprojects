#include <Servo.h>

// Pin Definitions
const int trigPin  = 3;
const int echoPin  = 2;
const int servoPin = 7;

// Variables
long duration;
int distance;
Servo myServo;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  myServo.attach(servoPin);
  myServo.write(180); // Ensure it starts at "Open" (180 degrees)
}

void loop() {
  // 1. Get distance reading
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // 2. Trigger at 23cm or closer
  if (distance > 0 && distance <= 23) {
    Serial.println("Object detected! Closing...");
    delay(1000);
    myServo.write(75);   // Close gate
    delay(7000);         // Stay closed for 7 seconds
    Serial.println("Reopening...");
    myServo.write(180);  // Reopen
  }

  delay(100); // Stabilize sensor readings
}
