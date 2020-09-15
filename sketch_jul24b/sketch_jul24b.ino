void setup() {
  Serial.println("clearing eeprom");
            for (int i = 0; i < 128; ++i)
            {
                EEPROM.write(i, 0);
            }

}

void loop() {
  // put your main code here, to run repeatedly:

}
