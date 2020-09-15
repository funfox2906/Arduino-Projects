#include "NeoPixelBus.h"
#include "ESP8266WiFi.h"

#define PixelCount 24 //number of Leds
#define PixelPin 2 //GPIO 05
const char* ssid ="AfonHOME";
const char* password = "0101010101";

int firstrun =0; //check if system was just powered on
int buttonpressed = 0;

//Initialze Library
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);

//define arrays for colors
long switchled00[] = 
{
  0x0E5219, 0x52160E,0x0E5219, 0x52160E,0x0E5219, 0x52160E,0x0E5219, 0x52160E,0x0E5219, 0x52160E,0x0E5219, 0x52160E,0x0E5219, 0x52160E,0x0E5219, 0x52160E,0x0E5219, 0x52160E,0x0E5219, 0x52160E,0x0E5219, 0x52160E,0x0E5219, 0x52160E
};

long switchled01[] = 
{
 0x19257B, 0x7B7A19,0x19257B, 0x7B7A19,0x19257B, 0x7B7A19,0x19257B, 0x7B7A19,0x19257B, 0x7B7A19,0x19257B, 0x7B7A19,0x19257B, 0x7B7A19,0x19257B, 0x7B7A19,0x19257B, 0x7B7A19,0x19257B, 0x7B7A19,0x19257B, 0x7B7A19,0x19257B, 0x7B7A19
};

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);
  strip.Begin(); //Init of Pixel Stick
  strip.Show(); //clears any lit up Leds

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status()!= WL_CONNECTED){
    delay(250);
    Serial.print(".");
  }
  //Confirmation of that wifi connected
  Serial.println("");
  Serial.println("WIFI CONNECTED");
  server.begin();
  Serial.println("Web Server Started");

  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Serial.println();  
}

void loop() {
  WiFiClient client = server.available();
  if (!client){
    return;
  }

  //read which button was pressed on the Web page
  String request = client.readStringUntil('\r');
  //light up leds based on the button pressed
  if (request.indexOf("/REDGREEN =1")!= -1) {
    for (int led =0; led<24; led++){
      strip.SetPixelColor(led, HtmlColor(switchled00[led]));
    }
    strip.Show();
    buttonpressed =LOW;
    firstrun =1 ;
  }

if (request.indexOf("/BLUEYELLOW =1")!= -1) {
    for (int led =0; led<24; led++){
      strip.SetPixelColor(led, HtmlColor(switchled00[led]));
    }
    strip.Show();
    buttonpressed =HIGH;
    firstrun =1 ;
  }


client.println("<HTTP/1.1 200 OK>"); //HTML HEADER
client.println("<html>");
client.println("<style>");
client.println("body {background-color: #ACAEAD;}");
client.println("</style>");
if (firstrun == 0){
  client.println("Please Click a Button");
}
else {
  client.println("Last Button Pressed: ");
}
if (buttonpressed ==LOW){
  client.println("Red&Green");
}
if (buttonpressed ==HIGH){
  client.println("Blue&Yellow");
}
client.println("<br><br>");
client.println("<a href=\"/REDGREEN=1\"\"><button>Red & Green </button></a>");  
client.println("<a href=\"/BLUEYELLOW=1\"\"><button>Blue & Yellow </button></a><br/>");
client.println("</html>");  
delay(10);
}
