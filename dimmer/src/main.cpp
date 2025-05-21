#include <Arduino.h>
#include <math.h>

#define MAX_DISTANCE  70
#define MIN_DISTANCE 3
#define RANGE (MAX_DISTANCE - MIN_DISTANCE)

#define LED_STRIP 3
#define TRIG_PIN 4
#define SENSOR_STARTING_PIN 8
#define N_SENSORS 5 + SENSOR_STARTING_PIN

#define DELAY 80
#define GAMMA 1.75
#define N_READS 10

byte last_reads[N_READS] = {0};
int loop_n = 0;

void setup() {
  // initialize the serial communication:
  Serial.begin(9600);

  // initialize the PINS:
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(LED_STRIP, OUTPUT);
  for (int sensor_pin = SENSOR_STARTING_PIN; sensor_pin < N_SENSORS; sensor_pin++) {
    pinMode(sensor_pin, INPUT);
  }
}

void trigger_pin() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
}

long readUltrasonicDistance() {
  int echo_value = 0;
  int lowest_echo = 0;

  for (int sensor_pin = SENSOR_STARTING_PIN; sensor_pin < N_SENSORS; sensor_pin++) {
    trigger_pin();
    echo_value = pulseIn(sensor_pin, HIGH, 40000ul);
    // Serial.println("echo_value[" + String(sensor_pin) + "] = " + String(echo_value));
    delayMicroseconds(DELAY);
    if (lowest_echo == 0 || (echo_value < lowest_echo && echo_value != 0))
      lowest_echo = echo_value;
  }
  return lowest_echo;
}

byte meanOfLastReads() {
  loop_n++;
  if (loop_n == N_READS) loop_n = 0;
  int soma = 0;
  for (int i = 0; i < N_READS; i++) {
    soma += last_reads[i];
  }
  return soma / N_READS;
}

byte getBrightness(double distancia_cm) {
  double clamped_value = constrain(MAX_DISTANCE - distancia_cm, 0, MAX_DISTANCE);
  double y = powf(clamped_value / MAX_DISTANCE, GAMMA);
  return y * 255.0f + 0.5f;
}

void loop() {
  // Calcula distância uma única vez por loop
  double distancia_cm = 0.01723 * readUltrasonicDistance();
  last_reads[loop_n] = getBrightness(distancia_cm);
  byte brightness = meanOfLastReads();
  analogWrite(LED_STRIP, brightness);
  
  // Debug no Serial Monitor
  // analogWrite(LED_STRIP, 255);
  // Serial.println("Distância: " + String(distancia_cm) + " cm");
  // Serial.println("Brightness: " + String(brightness));
  // delay(5000);
}