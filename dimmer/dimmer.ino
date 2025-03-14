const int TRIG_PIN = 5;
const int ECHO_PIN = 4;
const int RED_LED = 10;      // the pin that the LED is attached to

void setup() {
  // initialize the serial communication:
  Serial.begin(9600);

  // initialize the PINS:
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RED_LED, OUTPUT);
}

long readUltrasonicDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  return pulseIn(ECHO_PIN, HIGH);
}


void loop() {
  // Calcula distância uma única vez por loop
  float distancia_cm = 0.01723 * readUltrasonicDistance();
  byte brightness = 255 * max(0, min(70 - distancia_cm, 67)) / 67;
  // Debug no Serial Monitor
  Serial.println("Distância: " + String(distancia_cm) + " cm");
  Serial.println("Brightness: " + String(brightness) + " cm");

  analogWrite(RED_LED, brightness);

  delay(40);
}

