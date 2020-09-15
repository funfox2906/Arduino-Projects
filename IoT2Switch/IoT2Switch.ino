#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h> 
#include <ArduinoJson.h>
#include <ESP8266mDNS.h>
#include <WiFiUDP.h>
#include "Arduino.h"
/* Set these to your desired credentials. */
char *ssid;
char *password;


ESP8266WebServer server(80);
int In1 = 5;
int In2= 4;
String esid;
String epass;
String status;


String jsonStproc(String keyst){ 
  String stt; 
  stt= server.arg(keyst);
  stt.replace("%2F","/");
  return (char*) stt.c_str();
}

void InputWifi(){   
 String s;
  s += "<form method='get' action='a'>";
  s += "<label>SSID: </label><input name='ssid' length=32></br>";
  s += "<label> Pass: </label><input name='pass' type='password' length=64></br>";
  s += "</br>";
  
  s += "<input type='submit'></form></p>";
  s += "\r\n\r\n";
  Serial.println("Sending 200");  
  server.send(200, "text/html", s); 
 
}
 void WifiConnect(){
  esid=jsonStproc("ssid");
  epass=jsonStproc("pass");
  Serial.println(esid);
  Serial.println(epass);
  WiFi.begin((char*)esid.c_str(),(char*)epass.c_str());
  String s;
  int count = 0;
  Serial.printf("\nConnection status: %d\n", WiFi.status());
//-------------------------  TAI SAO ???
while ((WiFi.status() != WL_CONNECTED))
    {
    delay(500);
    Serial.print(".");
    count+=500;
    Serial.println(count);
    if (count>= 10000) goto notconnected;
    }
    Serial.printf("\nConnection status: %d\n", WiFi.status());
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());


if ( WiFi.status() == WL_CONNECTED) { 
    Serial.print("WIFI CONNECTED TO:  ");
    Serial.println(esid);  
    s+= "<h1>WIFI CONNECT SUCCEDED</h1>";
  server.send(200, "text/html",s);
  Serial.print("Connected to wifi");
    }     
    else {
    notconnected:
      s+= "<h1>WIFI NOT CONNECTED</h1>";
      s+= "<h2>please enter infor again</h2>";
      Serial.println("NOT CONNECTED");   
  server.send(200, "text/html",s);
    }
}

//-----------------------------
// Dieu khien LED
void LedControl(){

// Kiem tra neu co client ket noi
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Doi client gui data
  Serial.println("Co client moi");
  while(!client.available()){
    delay(1);
  }

  // Doc yeu cau tu client
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Kiem tra yeu cau la gi
  if (req.indexOf("/on") != -1){
    digitalWrite(In1, 1);
  }
  else if (req.indexOf("/off") != -1) {
    digitalWrite(In1, 0);
  }

  client.flush();

  // Chuan bi thong tin phan hoi
  String s ;
//  s += "<head>";
//  s += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
//  s += "<script src=\"https://code.jquery.com/jquery-2.1.3.min.js\"></script>";
//  s += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css\">";
//  s += "</head>";
  
  s += "<div class=\"container\" action ='ledcontrol'> ";
  s += "<h1>Led Control</h1>";
  s += "<div class=\"row\">";
  s += "<div class=\"col-md-2\"><input type=\"button\" value=\"On\" onclick=\"on()\"></div>";
  s += "<div class=\"col-md-2\"><input type=\"button\" value=\"Off\" onclick=\"off()\"></div>";
  s += "</div></div>";
  s += "<script>function on() {$.get(\"/on\");}</script>";
  s += "<script>function off() {$.get(\"/off\");}</script>";
  client.print(s);
  delay(1);
  Serial.println("Ngat ket noi Client");




  
}



void setup() {
  pinMode(In1, OUTPUT);
  pinMode(In2,OUTPUT);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  server.on("/", InputWifi);
  server.on("/a", WifiConnect);
  server.on("/ledcontrol", LedControl);
  server.begin();
  

}
void loop() {
  server.handleClient();
 
  
  
}
