#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LedControl.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
LedControl matrix = LedControl(51, 52, 53, 4);

Servo myServo;

#define TRIG_PIN 9
#define ECHO_PIN 10
#define SERVO_PIN 6

const int minAngle = 0;
const int maxAngle = 180;
const int stepAngle = 5;
const int maxDistance = 100;

float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) return -1;

  float distance = duration * 0.0343 / 2.0;
  return distance;
}

void clearAllMatrices() {
  for (int i = 0; i < 4; i++) {
    matrix.clearDisplay(i);
  }
}

void showOnePoint(int angle, float distance) {
  clearAllMatrices();

  if (distance < 0 || distance > maxDistance) {
    return;
  }

  int totalCols = 32;

  int x = map(angle, 0, 180, totalCols - 1, 0);
  int y = map((int)distance, 0, maxDistance, 7, 0);
  y = 7 - y;

  int device= x / 8;
  int col = x % 8;
  col = 7 - col;

  matrix.setLed(device, y, col, true);
}

void showLCD(int angle, float distance) {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Angle:");
  lcd.print(angle);

  lcd.setCursor(0, 1);
  lcd.print("Dist:");

  if (distance < 0) {
    lcd.print("No Echo");
  } else {
    lcd.print(distance, 1);
    lcd.print("cm");
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  myServo.attach(SERVO_PIN);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Radar Start");
  delay(1000);

  for (int i = 0; i < 4; i++) {
    matrix.shutdown(i, false);
    matrix.setIntensity(i, 3);
    matrix.clearDisplay(i);
  }
}

void loop() {
  for (int angle = minAngle; angle <= maxAngle; angle += stepAngle) {
    myServo.write(angle);
    delay(120);

    float distance = measureDistance();

    showLCD(angle, distance);
    showOnePoint(angle, distance);

    Serial.print("Angle: ");
    Serial.print(angle);
    Serial.print("  Distance: ");
    Serial.println(distance);

    delay(50);
  }

  for (int angle = maxAngle; angle >= minAngle; angle -= stepAngle) {
    myServo.write(angle);
    delay(120);

    float distance = measureDistance();

    showLCD(angle, distance);
    showOnePoint(angle, distance);

    Serial.print("Angle: ");
    Serial.print(angle);
    Serial.print("  Distance: ");
    Serial.println(distance);

    delay(50);
  }
}