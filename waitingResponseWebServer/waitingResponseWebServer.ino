#include <ESP8266WiFi.h>

const char SSID[] = "myssid";
const char WiFiPassword[] = "mypassword";


const byte LEDpin = LED_BUILTIN;
int ledStatus = LOW;

WiFiServer server(80);
WiFiClient client;

void setup() {
  Serial.begin(115200);
  Serial.print(F("\n\n\n\n"));
  Serial.print(F("Web Server Test:"));

  pinMode(LEDpin, OUTPUT);
  digitalWrite(LEDpin, ledStatus);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(SSID);
  WiFi.begin(SSID, WiFiPassword);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println(F("WiFi connected"));

  // Start the server
  server.begin();
  Serial.println(F("Server started"));

  // Print the IP address
  Serial.print(F("Web server IP address: "));
  Serial.println(WiFi.localIP());
}

void loop() {
  if (!client) {  //no client is currently connected
    client = server.available();  //check if there is a client available
  }
  if (client && client.available()) {  //there is a client connected and there is data available
    HTMLrequestHandler();
    sendHTMLresponse();
  }
}


void HTMLrequestHandler() {
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  if (req.indexOf("/off") != -1) {
    ledStatus = HIGH;
  }
  else if (req.indexOf("/on") != -1) {
    ledStatus = LOW;
  }
  digitalWrite(LEDpin, ledStatus);
  client.flush();
}


void sendHTMLresponse() {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  client.print("The LED is ");
  if (ledStatus == HIGH) {
    client.print("off");
  }
  else {
    client.print("on");
  }
  client.println("<br>");
  client.println("<br>");
  client.println("Click <a href=\"/off\">here</a> turn the LED off<br>");
  client.println("Click <a href=\"/on\">here</a> turn the LED on<br>");
  client.println();
  client.stop();
}
