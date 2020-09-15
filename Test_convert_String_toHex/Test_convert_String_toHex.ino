  #include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRremoteESP8266.h>
#define IR_SEND_PIN 15
IRsend irsend(IR_SEND_PIN);
//100000110111110001000011101111 == 20DF10EF

String ConvertBINtoHEX(String s){
  int value;
  String tempString = "";
  for (int i=0; i< s.length(); i++)  // for every character in the string  strlen(s) returns the length of a char array
  {
    value *= 2; // double the result so far
    if (s[i] == '1') value++; 
  } 
  tempString+="0x";
  s= String(value,HEX);
  s.toUpperCase();
  tempString+=s;
  s = tempString;
  return s;
}

unsigned long int ConvertBINtoHEXtoUint64(String s){
  char charArray[11];
  int value;
  String tempString = "";
  for (int i=0; i< s.length(); i++) 
  {
    value *= 2; // double the result so far
    if (s[i] == '1') value++; 
  } 
  tempString+="0x";
  s= String(value,HEX);
  s.toUpperCase();
  tempString+=s;
  s = tempString;
  s.toCharArray(charArray, sizeof(charArray));
  unsigned long int result = strtoul(charArray,0,16);
  return result;
}

  
//String temp = String(s,HEX);
void setup() {
  Serial.begin(115200);
//  SS = ConvertBINtoHEX("100000110111110001000011101111");
irsend.sendNEC(0x20DF906F, 32);
}

void loop() {
 Serial.println(ConvertBINtoHEXtoUint64("100000110111110001000011101111"));
 

delay(5000);
}
