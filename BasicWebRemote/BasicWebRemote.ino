#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRremoteESP8266.h>
#define IR_SEND_PIN 15
#define DELAY_BETWEEN_COMMANDS 1000
IRsend irsend(IR_SEND_PIN);
const char* ssid = "AfonHOME";
const char* password = "0101010101";

ESP8266WebServer server(80);

//---TEST
unsigned long int ConvertBINtoHEXtoUint64(String s){
  char charArray[11];
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
  s.toCharArray(charArray, sizeof(charArray));
  unsigned long int result = strtoul(charArray,0,16);
  return result;
}

//-----end test

String rowDiv = "    <div class=\"row\" style=\"padding-bottom:1em\">\n";
String endDiv = "    </div>\n";

// Buttons are using the bootstrap grid for sizing - http://getbootstrap.com/css/#grid
String generateButton(String colSize, String id, String text, String url) {

  return  "<div class=\"" + colSize + "\" style=\"text-align: center\">\n" +
          "    <button id=\"" + id + "\" type=\"button\" class=\"btn btn-default\" style=\"width: 100%\" onclick='makeAjaxCall(\"" + url + "\")'>" + text + "</button>\n" +
          "</div>\n";
}

void handleRoot1() {

  String website = "<!DOCTYPE html>\n";
  website = website + "<html>\n";
  website = website + "  <head>\n";
  website = website + "    <meta charset=\"utf-8\">\n";
  website = website + "    <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n";
  website = website + "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  website = website + "    <link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\">\n";
  website = website + "  </head>\n";
  website = website + "  <body>\n";
  website = website + "    <div class=\"container-fluid\">\n";
  // ------------------------- Power Controls --------------------------
  website += "<div id=\"button\"><a href=\"login.html\">Log In</a></div>";

  
  website = website + rowDiv;
  website = website + generateButton("col-xs-4", "tvpower","TV Power", "tvpower");
  website = website + generateButton("col-xs-4", "ACoff","ACoff", "ACoff");
  website = website + endDiv;
  // ------------------------- Channel Controls --------------------------
  website = website + rowDiv;
  website = website + generateButton("col-xs-5", "tvchannelup","TVchannelup", "tvchannelup");
  website = website + generateButton("col-xs-5", "tvchanneldown","TVchanneldown", "tvchanneldown");
  //website = website + generateButton("col-xs-3", "sschannel3","---", "sschannel3");
  
  website = website + generateButton("col-xs-5", "sstvsound","MuteTV", "sstvsound");
  website = website + endDiv;
  // ------------------------- Volume Controls --------------------------
  website = website + rowDiv;
  website = website + generateButton("col-xs-12", "up","Vol Up", "up");
  website = website + endDiv;
  website = website + rowDiv;
  website = website + generateButton("col-xs-12", "down","Vol Down", "down");
  website = website + endDiv;
//--------------------------------------------------
  website = website + "    <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js\"></script>\n";
  website = website + "    <script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\"></script>\n";
  website = website + "    <script> function makeAjaxCall(url){$.ajax({\"url\": url})}</script>\n";
  website = website + "  </body>\n";
  website = website + "</html>\n";

  server.send(200, "text/html", website);
}



void handleRoot2() {

  String website = "<!DOCTYPE html>\n";
  website = website + "<html>\n";
  website = website + "  <head>\n";
  website = website + "    <meta charset=\"utf-8\">\n";
  website = website + "    <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n";
  website = website + "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  website = website + "    <link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\">\n";
  website = website + "  </head>\n";
  website = website + "  <body>\n";
  website = website + "    <div class=\"container-fluid\">\n";
  // ------------------------- Power Controls --------------------------
  website += "<div><h1>Hello day la trang test login</h1></div>";
  website += "<button type=\"button\" > ButtonTest</button>";
//--------------------------------------------------
  website = website + "    <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js\"></script>\n";
  website = website + "    <script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\"></script>\n";
  website = website + "    <script> function makeAjaxCall(url){$.ajax({\"url\": url})}</script>\n";
  website = website + "  </body>\n";
  website = website + "</html>\n";

  server.send(200, "text/html", website);
}



void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}


uint16_t ACoff[307] = {3158, 1618,  362, 432,  362, 1228,  362, 432,  364, 432,  364, 1226,  364, 432,  364, 1226,  390, 404,  362, 432,  388, 1202,  366, 1226,  364, 1228,  364, 432,  364, 1226,  388, 406,  364, 1226,  390, 1200,  366, 1226,  364, 430,  366, 430,  362, 432,  364, 432,  364, 1228,  362, 1228,  390, 404,  364, 1230,  362, 432,  362, 1226,  364, 1228,  388, 410,  362, 430,  364, 430,  362, 1230,  364, 430,  364, 1228,  364, 430,  364, 430,  364, 1226,  366, 1226,  364, 1228,  364, 430,  364, 1228,  364, 1228,  364, 1228,  364, 1228,  364, 1226,  390, 1202,  364, 1228,  364, 1228,  362, 432,  362, 434,  362, 432,  362, 434,  362, 432,  364, 432,  362, 432,  388, 1204,  364, 430,  364, 430,  366, 1228,  388, 1202,  366, 1224,  364, 1226,  390, 1204,  388, 406,  390, 1202,  362, 1230,  362, 432,  364, 432,  388, 406,  364, 434,  362, 434,  366, 1224,  388, 1202,  364, 1226,  364, 1226,  364, 1228,  390, 1202,  362, 1228,  390, 1200,  366, 430,  364, 430,  364, 430,  364, 432,  392, 402,  362, 436,  362, 430,  364, 432,  364, 1226,  364, 1226,  364, 1228,  388, 1202,  388, 1202,  390, 1202,  390, 406,  362, 430,  390, 406,  362, 432,  362, 432,  366, 430,  388, 408,  390, 406,  360, 1230,  374, 1216,  364, 1228,  366, 1226,  390, 1202,  364, 430,  364, 1228,  364, 1226,  364, 430,  364, 434,  388, 404,  362, 434,  362, 430,  364, 1228,  362, 432,  366, 430,  388, 1202,  364, 1226,  390, 1202,  388, 1202,  364, 1228,  364, 1226,  364, 1226,  366, 1228,  364, 1226,  364, 1226,  364, 432,  364, 432,  362, 432,  362, 432,  362, 432,  360, 436,  362, 432,  364, 432,  362, 1228,  362, 1226,  390, 1202,  366, 1224,  390, 1202,  364, 1226,  364, 1228,  388, 406,  388, 406,  364, 432,  364, 432,  362, 432,  364, 432,  364, 432,  362, 432,  364, 1226,  368};


//change channel up:20DF00FF
//change channel down:20DF807F
//mute: 20DF906F
void setup(void){
  irsend.begin();
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot1);
  server.on("/login.html", handleRoot2);
//volumn up TV: 
//volumn down TV: 20DFC03F
  server.on("/down", [](){
    Serial.println("Volumn Down");
    irsend.sendNEC(0x20DFC03F, 32);
    server.send(200, "text/plain", "Volume Down");
  });

  server.on("/up", [](){
    Serial.println("Volumn Up");
    irsend.sendNEC(0x20DF40BF, 32);
    server.send(200, "text/plain", "Volume Up");
  });

  server.on("/ACoff", [](){
    Serial.println("AC Off");
    irsend.sendRaw(ACoff, sizeof(ACoff), 38);  // Send a raw data capture at 38kHz.
     yield();  
    server.send(200, "text/plain", "AC Power OFF");
  });

  server.on("/tvchannelup", [](){
    Serial.println("TV channel up");
    irsend.sendNEC(0x20DF00FF, 32);
    server.send(200, "text/plain", "TV channel up");
  });

  server.on("/tvchanneldown", [](){
    Serial.println("TV channel down");
    irsend.sendNEC(0x20DF807F, 32);
    server.send(200, "text/plain", "TV channel down");
  });

  server.on("/sstvsound", [](){
    Serial.println(" Sound MUTE");
    irsend.sendNEC(0x20DF906F, 32);
    server.send(200, "text/plain", " Sound MUTE");
  });

  server.on("/tvpower", [](){
    Serial.println("TV power");
    Serial.println(ConvertBINtoHEXtoUint64("00100000110111110001000011101111"));
    irsend.sendNEC(ConvertBINtoHEXtoUint64("00100000110111110001000011101111"), 32);
    
    server.send(200, "text/plain", "TV Power");
  });

  
 
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP Server Started");
}

void loop(void){
  server.handleClient();
}
