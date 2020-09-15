#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NeoPixelBus.h>
#include <BlynkSimpleEsp8266.h>
const uint16_t PixelCount = 24; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 2; 
#define colorSaturation 128
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

const char* ssid =         "AFON-2.4GHZ";
const char* password =    "0101010101";
const char* mqtt_server = "192.168.1.108";   
const uint16_t mqtt_port = 1883;
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void setup_wifi() {
  strip.Begin(); //setup led Init
   strip.Show();
  delay(10);
  // Kết nối wifi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String RGBcolor ="";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  
//
// R = C/(256^2);
//
//G = (C/256) % 256;
//
//B = C%256;
//    String R;
//    String G;
//    String B;
//    int temp[2];
//    for (int i = 0; i < RGBcolor.length(); i++) {
//        if (RGBcolor.charAt(i) == ',') {
//            temp[i] = i; //Tìm vị trí của dấu ","
//        }
//    }
//    for (int i=0; i<=temp[0];i++){
//      if (isDigit(RGBcolor[i])) R+=RGBcolor[i];
//    }
//    for (int i=temp[0]; i<=temp[1];i++){
//      if (isDigit(RGBcolor[i])) G+=RGBcolor[i];
//    }
//    for (int i=temp[1]; i<=RGBcolor.length();i++){
//      if (isDigit(RGBcolor[i])) R+=RGBcolor[i];
//    }
//    Serial.println(R);
//    Serial.println(G);
//    Serial.println(B);
//  if (((char)payload[0]=='r')&&((char)payload[1]=='g')&&((char)payload[2]=='b')){
//    RgbColor color = RgbColor(R.toInt(),G.toInt(), B.toInt());
//    for (int i=0; i<24; i++){
//      strip.SetPixelColor(i, color);
//    }
//  }

  
  if ((char)payload[0] == '1') {
     Serial.println("ON");
    LedRunAround();   
  } 
 if ((char)payload[0] == '0') {
    Serial.println("OFF");
    TurnOffLed(); 
  }

if ((char)payload[0] == '2') {
    Serial.println("all green");
    LedRGB("green"); 
  }
  if ((char)payload[0] == '3') {
    Serial.println("all red");
    LedRGB("red");
  }
  if ((char)payload[0] == '4') {
    Serial.println("all blue");
    LedRGB("blue");
  }
}

void reconnect() {
  // Đợi tới khi kết nối
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      //topic event
      client.publish("event", "tin nhan");
      //sub lại thông tin
      client.subscribe("event");

      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
 // Thực hiện 2s gửi dữ liệu helloworld lên broker 1 lần
//  long now = millis();
//  if (now - lastMsg > 2000) {
//    lastMsg = now;
//    ++value;
//    snprintf (msg, 75, "Test MQTT #%ld", value);
//    Serial.print("Publish message: ");
//    Serial.println(msg);
//    client.publish("event", msg);
//  }
}
