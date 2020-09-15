#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRremoteESP8266.h>
#include "FS.h"
#define IR_SEND_PIN 15
#define DELAY_BETWEEN_COMMANDS 1000
IRsend irsend(IR_SEND_PIN);
const char* ssid = "AfonHOME";
const char* password = "0101010101";

ESP8266WebServer server(80);

String rowDiv = "    <div class=\"row\" style=\"padding-bottom:1em\">\n";
String endDiv = "    </div>\n";

// Buttons are using the bootstrap grid for sizing - http://getbootstrap.com/css/#grid
String generateButton(String colSize, String id, String text, String url) {
  return  "<div class=\"" + colSize + "\" style=\"text-align: center\">\n" +
          "    <button id=\"" + id + "\" type=\"button\" class=\"btn btn-default\" style=\"width: 100%\" onclick='makeAjaxCall(\"" + url + "\")'>" + text + "</button>\n" +
          "</div>\n";
}



void handleRoot(){
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
  website += "<h1>Choose type of TV</h1>";
  website += rowDiv;
 
  website += "<div id=\"button\"><a href=\"samsungTV.html\">SamSung</a></div>";
  website += endDiv;
  website += rowDiv;
  website += "<div id=\"button\"><a href=\"lgTV.html\">LG</a></div>";
  website += endDiv;
website = website + "    <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js\"></script>\n";
  website = website + "    <script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\"></script>\n";
  website = website + "    <script> function makeAjaxCall(url){$.ajax({\"url\": url})}</script>\n";
  website = website + "  </body>\n";
  website = website + "</html>\n";
  server.send(200, "text/html", website);
}

void samsungtv() {
  File f = SPIFFS.open("/TV_Samsung.rem", "r");
  String website= "<!DOCTYPE html>\n";
  website = website + "<html>\n";
  website = website + "  <head>\n";
  website = website + "    <meta charset=\"utf-8\">\n";
  website = website + "    <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n";
  website = website + "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  website = website + "    <link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\">\n";
  website = website + "  </head>\n";
  website = website + "  <body>\n";
  website = website + "    <div class=\"container-fluid\">\n";

  website += ReadSPIFFSandCreateButton(f,website);
  website += rowDiv;
  website += "<div id=\"button\"><a href=\"root.html\">BACK</a></div>";
  website += endDiv;
  website = website + "    <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js\"></script>\n";
  website = website + "    <script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\"></script>\n";
  website = website + "    <script> function makeAjaxCall(url){$.ajax({\"url\": url})}</script>\n";
  website = website + "  </body>\n";
  website = website + "</html>\n";
  server.send(200, "text/html", website );
  
}



void lgtv(){
  File f = SPIFFS.open("/Tivi_LG.rem", "r");
  String website= "<!DOCTYPE html>\n";
  website = website + "<html>\n";
  website = website + "  <head>\n";
  website = website + "    <meta charset=\"utf-8\">\n";
  website = website + "    <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n";
  website = website + "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  website = website + "    <link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\">\n";
  website = website + "  </head>\n";
  website = website + "  <body>\n";
  website = website + "    <div class=\"container-fluid\">\n";
  

  website += ReadSPIFFSandCreateButton(f,website);


  
  website += rowDiv;
  website += "<div id=\"button\"><a href=\"root.html\">BACK</a></div>";
  website += endDiv;
  website = website + "    <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js\"></script>\n";
  website = website + "    <script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\"></script>\n";
  website = website + "    <script> function makeAjaxCall(url){$.ajax({\"url\": url})}</script>\n";
  website = website + "  </body>\n";
  website = website + "</html>\n";
  server.send(200, "text/html", website );
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


String ReadSPIFFSandCreateButton(File f,String website){
String line;
String namefunction="";
String data="";
int cnt=0;
if (!f) {
  Serial.println("Count file open failed on read ");
  
} else {
    while(f.available()) {
        line += f.readStringUntil('\r'); 
//        if (cnt>=1){
          for (int i =0 ; i<line.length();i++){
            if ( line[i]=='['){
              for (int j=i+1; j<line.length();j++){
                if (line[j]!=']') namefunction+=line[j]; else break;
                }
            } else if (line[i]==']'){
                for (int j=i+1; j<line.length();j++){
                data+=line[j];
                }
              }
          }
          website += generateButton("col-xs-4", namefunction, namefunction, namefunction);
          //TV power =551489775
          server.on("/"+namefunction, [namefunction,data](){
              Serial.println(namefunction);
              Serial.println(data);
              Serial.println(ConvertBINtoHEXtoUint64(data));            
              irsend.sendNEC(ConvertBINtoHEXtoUint64(data), 32);
              server.send(200, "text/plain", ""+namefunction);
          });
          line ="";
          namefunction="";
          data="";
     }
//     cnt++;
//  }
  f.close();
  }
  return website;
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

void setup() {
  irsend.begin();
  Serial.begin(115200);
  SPIFFS.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  server.on("/", handleRoot);
  server.on("/samsungTV.html", samsungtv);
  server.on("/lgTV.html", lgtv);
  server.on("/root.html", handleRoot);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP Server Started");  
}

void loop() {
  server.handleClient();

}
