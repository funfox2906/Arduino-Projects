#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

#define receiverPin 14

IRrecv irrecv(receiverPin);
decode_results results;
void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn();
}

void loop() {
  if (irrecv.decode(&results)){
    Serial.println(results.value,HEX);
    delay(200);
    irrecv.resume();
  }
}
