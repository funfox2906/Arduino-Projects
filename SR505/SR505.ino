#define SIGNAL_PIN 5
#define LED 4

void setup() {
  Serial.begin(9600);
  pinMode(SIGNAL_PIN, INPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(SIGNAL_PIN, LOW);
}

void loop() {
  if ( digitalRead(SIGNAL_PIN) == HIGH){
    Serial.println("Movement detected");
    digitalWrite(LED, HIGH);
  }
  if (digitalRead (SIGNAL_PIN) == LOW) 
  {
    Serial.println("No movement");
    digitalWrite(LED, LOW);
  }
delay(1000);
}
