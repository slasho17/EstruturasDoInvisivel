#include <Arduino.h>

const int TRIG_PIN = 5;
const int ECHO2_PIN = 12;
const int ECHO1_PIN = 4;
const int RED_LED = 10;      // the pin that the LED is attached to
int loops = 0;

void setup() {
  // initialize the serial communication:
  Serial.begin(9600);

  // initialize the PINS:
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO1_PIN, INPUT);
  pinMode(ECHO2_PIN, INPUT);
  pinMode(RED_LED, OUTPUT);
}

long readUltrasonicDistance1() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  int echo1 = pulseIn(ECHO1_PIN, HIGH);
  
  Serial.println("echo1: " + String(echo1));
  return echo1;
}

long readUltrasonicDistance2() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  int echo2 = pulseIn(ECHO2_PIN, HIGH);
  
  Serial.println("echo2: " + String(echo2));
  return echo2;
}
int delay_s = 1;
void loop() {
  // Calcula distância uma única vez por loop
  float distancia1_cm = 0.01723 * readUltrasonicDistance1();
  delay(delay_s);
  float distancia2_cm = 0.01723 * readUltrasonicDistance2();
  distancia2_cm > 1 ? delay_s-- : delay_s++;
  Serial.println("delay_s: " + String(delay_s));

  byte brightness = 255 * max(0, min(70 - min(distancia1_cm, distancia2_cm), 67)) / 67;
  // Debug no Serial Monitor
  Serial.println("Distância1: " + String(distancia1_cm) + " cm");
  Serial.println("Distância2: " + String(distancia2_cm) + " cm");
  Serial.println("Brightness: " + String(brightness));
  analogWrite(RED_LED, brightness);
  loops += 1;
  Serial.println("loops = " + String(loops));

  delay(2000);
}