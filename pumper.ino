#define MAX_DISTANCE 25

//pin declearation 
int relay = 10; 
int pin_trig = 8;
int pin_echo = 7;

float getDistance() {
  long long duration, distanceCm;
  digitalWrite(pin_trig, LOW);
  delayMicroseconds(2);
  digitalWrite(pin_trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin_trig, LOW);
  duration = pulseIn(pin_echo, HIGH, 7000);
  distanceCm = int(duration * 0.034 / 2);
  return distanceCm;
}

void setup() {
  Serial.begin (9600);
  pinMode(pin_trig, OUTPUT);
  pinMode(pin_echo, INPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
}

void loop() {
  delay(50);
  long distanceCm = getDistance();
  if (distanceCm < 5) {
    digitalWrite(relay, HIGH);
    Serial.println(distanceCm);
    Serial.println("FULL");
  }
  else if (distanceCm >=5 && distanceCm <= MAX_DISTANCE) {
    digitalWrite(relay, LOW);
    Serial.println(distanceCm);
    Serial.println("PUMPING");
  } 
}
