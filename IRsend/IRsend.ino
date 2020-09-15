/* IRremoteESP8266: IRsendDemo - demonstrates sending IR codes with IRsend.
 *
 * Version 1.0 April, 2017
 * Based on Ken Shirriff's IrsendDemo Version 0.1 July, 2009,
 * Copyright 2009 Ken Shirriff, http://arcfn.com
 *
 * An IR LED circuit *MUST* be connected to the ESP8266 on a pin
 * as specified by IR_LED below.
 *
 * TL;DR: The IR LED needs to be driven by a transistor for a good result.
 *
 * Suggested circuit:
 *     https://github.com/markszabo/IRremoteESP8266/wiki#ir-sending
 *
 * Common mistakes & tips:
 *   * Don't just connect the IR LED directly to the pin, it won't
 *     have enough current to drive the IR LED effectively.
 *   * Make sure you have the IR LED polarity correct.
 *     See: https://learn.sparkfun.com/tutorials/polarity/diode-and-led-polarity
 *   * Typical digital camera/phones can be used to see if the IR LED is flashed.
 *     Replace the IR LED with a normal LED if you don't have a digital camera
 *     when debugging.
 *   * Avoid using the following pins unless you really know what you are doing:
 *     * Pin 0/D3: Can interfere with the boot/program mode & support circuits.
 *     * Pin 1/TX/TXD0: Any serial transmissions from the ESP8266 will interfere.
 *     * Pin 3/RX/RXD0: Any serial transmissions to the ESP8266 will interfere.
 *   * ESP-01 modules are tricky. We suggest you use a module with more GPIOs
 *     for your first time. e.g. ESP-12 etc.
 */

#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRsend.h>

#define IR_LED 15 
IRsend irsend(IR_LED);  
uint16_t OFF[307] = {3158, 1618,  362, 432,  362, 1228,  362, 432,  364, 432,  364, 1226,  364, 432,  364, 1226,  390, 404,  362, 432,  388, 1202,  366, 1226,  364, 1228,  364, 432,  364, 1226,  388, 406,  364, 1226,  390, 1200,  366, 1226,  364, 430,  366, 430,  362, 432,  364, 432,  364, 1228,  362, 1228,  390, 404,  364, 1230,  362, 432,  362, 1226,  364, 1228,  388, 410,  362, 430,  364, 430,  362, 1230,  364, 430,  364, 1228,  364, 430,  364, 430,  364, 1226,  366, 1226,  364, 1228,  364, 430,  364, 1228,  364, 1228,  364, 1228,  364, 1228,  364, 1226,  390, 1202,  364, 1228,  364, 1228,  362, 432,  362, 434,  362, 432,  362, 434,  362, 432,  364, 432,  362, 432,  388, 1204,  364, 430,  364, 430,  366, 1228,  388, 1202,  366, 1224,  364, 1226,  390, 1204,  388, 406,  390, 1202,  362, 1230,  362, 432,  364, 432,  388, 406,  364, 434,  362, 434,  366, 1224,  388, 1202,  364, 1226,  364, 1226,  364, 1228,  390, 1202,  362, 1228,  390, 1200,  366, 430,  364, 430,  364, 430,  364, 432,  392, 402,  362, 436,  362, 430,  364, 432,  364, 1226,  364, 1226,  364, 1228,  388, 1202,  388, 1202,  390, 1202,  390, 406,  362, 430,  390, 406,  362, 432,  362, 432,  366, 430,  388, 408,  390, 406,  360, 1230,  374, 1216,  364, 1228,  366, 1226,  390, 1202,  364, 430,  364, 1228,  364, 1226,  364, 430,  364, 434,  388, 404,  362, 434,  362, 430,  364, 1228,  362, 432,  366, 430,  388, 1202,  364, 1226,  390, 1202,  388, 1202,  364, 1228,  364, 1226,  364, 1226,  366, 1228,  364, 1226,  364, 1226,  364, 432,  364, 432,  362, 432,  362, 432,  362, 432,  360, 436,  362, 432,  364, 432,  362, 1228,  362, 1226,  390, 1202,  366, 1224,  390, 1202,  364, 1226,  364, 1228,  388, 406,  388, 406,  364, 432,  364, 432,  362, 432,  364, 432,  364, 432,  362, 432,  364, 1226,  368};
uint16_t coldmaxfan25temp[307] = {3186, 1592,  366, 430,  366, 1226,  392, 402,  390, 404,  366, 1226,  366, 432,  390, 1200,  364, 430,  366, 430,  390, 1200,  390, 1202,  390, 1200,  366, 430,  390, 1200,  366, 428,  390, 1202,  366, 1226,  390, 1202,  392, 404,  366, 428,  364, 432,  364, 428,  366, 1226,  390, 1202,  390, 404,  364, 1228,  364, 430,  366, 1226,  392, 1200,  390, 402,  366, 428,  390, 406,  390, 1202,  390, 404,  390, 1202,  366, 428,  390, 408,  390, 1200,  366, 1224,  394, 1198,  366, 428,  366, 1226,  390, 1200,  390, 406,  388, 1202,  366, 1226,  392, 1198,  390, 1202,  390, 1200,  390, 406,  390, 404,  392, 1200,  364, 430,  390, 404,  366, 430,  390, 406,  390, 1200,  390, 1200,  390, 1200,  364, 432,  390, 1200,  392, 1198,  366, 1226,  392, 1200,  366, 428,  364, 432,  392, 404,  388, 1202,  390, 406,  390, 406,  390, 404,  364, 432,  364, 1224,  392, 1200,  366, 428,  364, 1230,  390, 1198,  392, 1198,  364, 1228,  392, 1198,  390, 406,  366, 430,  388, 1202,  366, 428,  390, 404,  392, 404,  364, 430,  390, 404,  364, 1226,  392, 1198,  390, 1202,  390, 1198,  390, 1202,  366, 1224,  390, 406,  392, 402,  364, 430,  364, 430,  364, 432,  392, 402,  366, 428,  366, 428,  392, 1200,  390, 1200,  390, 1200,  390, 1202,  390, 1202,  390, 406,  392, 1200,  390, 1200,  366, 430,  392, 402,  364, 430,  390, 404,  390, 406,  368, 1226,  390, 406,  364, 430,  364, 1226,  364, 1226,  390, 1204,  366, 1222,  390, 1200,  366, 1226,  364, 1226,  390, 1204,  392, 1196,  392, 1202,  390, 404,  366, 430,  390, 404,  390, 406,  390, 404,  390, 406,  388, 406,  364, 432,  364, 1224,  392, 1198,  366, 1228,  364, 1226,  366, 1224,  366, 1226,  366, 1224,  366, 430,  366, 428,  366, 428,  366, 430,  366, 430,  364, 430,  364, 432,  364, 430,  366, 1224,  390};  // UNKNOWN 6BE437BB
uint16_t coldautofan25temp[307] = {3160, 1616,  390, 406,  364, 1226,  366, 430,  394, 400,  392, 1198,  392, 404,  366, 1226,  366, 428,  394, 402,  390, 1202,  392, 1198,  366, 1224,  394, 402,  390, 1202,  392, 402,  366, 1226,  390, 1200,  390, 1202,  392, 404,  392, 402,  390, 404,  390, 408,  364, 1226,  368, 1224,  392, 404,  392, 1198,  366, 432,  390, 1196,  366, 1228,  392, 402,  392, 402,  390, 406,  392, 1198,  392, 402,  366, 1224,  392, 404,  366, 430,  366, 1224,  394, 1198,  390, 1200,  390, 404,  392, 1200,  392, 1198,  392, 404,  392, 1200,  366, 1226,  392, 1198,  392, 1200,  366, 1226,  366, 430,  390, 404,  390, 1202,  366, 428,  390, 404,  392, 402,  366, 430,  366, 1226,  392, 1198,  392, 1198,  366, 428,  368, 1224,  364, 1226,  392, 1200,  394, 1198,  366, 428,  368, 428,  366, 432,  366, 1222,  390, 406,  390, 404,  390, 404,  390, 406,  392, 1198,  366, 1228,  392, 1198,  372, 1220,  392, 1200,  392, 1200,  392, 1198,  392, 1198,  390, 406,  366, 428,  392, 406,  366, 426,  368, 428,  392, 404,  366, 430,  390, 406,  366, 1224,  366, 1226,  390, 1198,  390, 1202,  366, 1224,  392, 1200,  364, 430,  366, 430,  392, 402,  392, 402,  366, 430,  390, 404,  392, 404,  364, 430,  390, 1200,  390, 1202,  390, 1200,  392, 1200,  392, 1198,  366, 430,  390, 1202,  392, 1198,  368, 428,  392, 404,  366, 426,  366, 430,  366, 430,  392, 1198,  390, 406,  394, 402,  364, 1226,  390, 1198,  392, 1200,  390, 1200,  366, 1226,  366, 1226,  392, 1200,  366, 1226,  390, 1200,  390, 1202,  392, 402,  368, 428,  364, 430,  390, 404,  364, 430,  392, 404,  366, 428,  392, 402,  392, 1200,  364, 1224,  366, 1224,  392, 1200,  368, 1224,  392, 1198,  390, 1202,  366, 428,  390, 406,  392, 404,  366, 430,  390, 404,  390, 406,  366, 428,  392, 402,  366, 1224,  392};  // UNKNOWN 5CC4D9FF
uint16_t autominifan[307] = {3184, 1590,  404, 392,  366, 1224,  392, 404,  392, 400,  366, 1226,  392, 404,  392, 1200,  390, 404,  364, 430,  390, 1202,  390, 1200,  366, 1226,  366, 430,  390, 1200,  364, 430,  368, 1224,  392, 1198,  366, 1226,  366, 428,  392, 402,  366, 430,  390, 406,  366, 1226,  366, 1224,  366, 428,  392, 1200,  392, 402,  390, 1200,  366, 1226,  366, 430,  390, 406,  366, 430,  390, 1200,  394, 402,  366, 1224,  392, 404,  392, 402,  394, 1196,  390, 1200,  364, 1226,  364, 1228,  390, 1200,  366, 1224,  390, 406,  390, 1200,  392, 1200,  390, 1200,  366, 1224,  392, 404,  390, 404,  392, 404,  364, 1226,  390, 406,  390, 404,  366, 430,  368, 426,  394, 1198,  388, 406,  362, 1228,  366, 428,  364, 1226,  392, 1200,  366, 1224,  392, 1200,  392, 404,  364, 1226,  392, 404,  364, 1228,  366, 430,  392, 404,  364, 430,  392, 404,  366, 430,  390, 1200,  392, 1200,  392, 1198,  394, 1196,  390, 1200,  364, 1228,  390, 1202,  392, 1198,  392, 402,  390, 404,  392, 402,  364, 432,  364, 432,  364, 430,  366, 430,  368, 1224,  364, 1226,  366, 1224,  364, 1228,  366, 1224,  366, 1228,  390, 402,  364, 432,  390, 404,  364, 432,  364, 430,  366, 430,  364, 432,  390, 406,  390, 1200,  364, 1228,  364, 1224,  394, 1198,  392, 1198,  366, 432,  390, 1198,  368, 1224,  366, 430,  366, 428,  392, 402,  392, 404,  388, 406,  364, 1226,  366, 430,  390, 404,  366, 1224,  390, 1200,  390, 1200,  390, 1202,  366, 1226,  390, 1200,  366, 1226,  390, 1200,  366, 1226,  366, 1226,  366, 428,  366, 430,  366, 430,  392, 402,  366, 430,  392, 402,  366, 430,  366, 430,  366, 1224,  390, 1200,  390, 1200,  392, 1198,  366, 1224,  366, 1226,  390, 1202,  368, 426,  392, 404,  364, 430,  368, 428,  390, 406,  368, 428,  390, 406,  364, 430,  390, 1202,  390};  // UNKNOWN 53152F89
void setup() {
  irsend.begin();
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
}

void loop() {

  Serial.println("Send Now: ");
  
  irsend.sendRaw(OFF, sizeof(OFF), 38);  // Send a raw data capture at 38kHz.
  yield();  
  delay(5000);
  
  irsend.sendRaw(coldmaxfan25temp, sizeof(coldmaxfan25temp), 38);
  yield();  
  delay(5000);
  irsend.sendRaw(autominifan, sizeof(autominifan), 38);
  yield();  
  
  delay(5000);
  irsend.sendRaw(coldautofan25temp, sizeof(coldautofan25temp), 38);
  yield();  
  delay(5000);
}
