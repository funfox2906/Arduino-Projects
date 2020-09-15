#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include "EEPROM.h"
#include <NeoPixelBus.h>
#include <BlynkSimpleEsp8266.h>
const uint16_t PixelCount = 24; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 2; 
#define colorSaturation 128
#define Pin1 5
#define Pin2 4


#define PinR 14 
#define PinG 12
#define PinB 13

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);

ESP8266WebServer server(80);

RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

const char*     ssid           = "AP_test";
const char*     passphrase     = "12345678";
String          blynk          = "92f50f1621b640d28a10e866849b7cc5";
String          st;
String          content;
int             statusCode;

void setup()
{
    Serial.begin(115200);
    pinMode(Pin1,OUTPUT);
    pinMode(Pin2,OUTPUT);
    pinMode(PinR,OUTPUT);
    pinMode(PinG,OUTPUT);
    pinMode(PinB,OUTPUT);
    strip.Begin(); //setup led Init
    strip.Show();


   
    EEPROM.begin(512);
    delay(10);
//    
//    for (int i = 0; i < 128; ++i)
//            {
//                EEPROM.write(i, 0);
//            }
//    Serial.println("Clearing EEPROM ... ");
//    delay(5000);
//    


    Serial.println("Startup");
    Serial.println("Reading EEPROM ssid");
    String esid;
    for (int i = 0; i < 32; ++i)
    {
        esid += char(EEPROM.read(i));
    }
    Serial.print("SSID: ");
    Serial.println(esid.c_str());
    esid.trim();
 
    Serial.println("Reading EEPROM pass");
    String epass = "";
    for (int i = 32; i < 96; ++i)
    {
        epass += char(EEPROM.read(i));
    }
    Serial.print("PASS: ");
    Serial.println(epass.c_str());
    epass.trim();
    
  //Blynk
Serial.println("Reading EEPROM blynk");
    String eblynk = "";
    for (int i = 96; i < 128; ++i)
    {
        eblynk += char(EEPROM.read(i));
    }
    Serial.print("BLYNK: ");
    Serial.println(eblynk.c_str());
    eblynk.trim();
  //-------
 
    if ( esid.length() > 1 )
    {
        WiFi.begin(esid.c_str(), epass.c_str());
        if (testWifi())
        {
            launchWeb(0);
            WiFi.disconnect();
            //CHECK TOKEN
            char * auth_ = new char[eblynk.length() + 1];
            eblynk.toCharArray(auth_, eblynk.length() + 1);
            Blynk.begin(auth_, esid.c_str(), epass.c_str());
            //------
            EEPROM.end();
            return;
        }
    }
    setupAP();
    EEPROM.end();
}
 
bool testWifi(void)
{
    int c = 0;
    Serial.println("Xin vui long doi ket noi WIFI");
    while ( c < 20 )
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            return true;
        }
        delay(1000);
        Serial.print(".");
        c++;
    }
    Serial.println("");
    Serial.println("Ko co ket noi, Moi truy cap AP");
    return false;
}
 
void launchWeb(int webtype)
{
    Serial.println("");
    Serial.println("WiFi ket noi");
    Serial.print("Dia chi IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("SoftAP IP: ");
    Serial.println(WiFi.softAPIP());
    createWebServer(webtype);
    // Start the server
    server.begin();
    Serial.println("May chu bat dau");
}
 
void setupAP(void)
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    int n = WiFi.scanNetworks();
    Serial.println("\n\nWIFI LIST\n");
    if (n == 0)
    {
        Serial.println("no networks found");
    }
    else
    {
        for (int i = 0; i < n; ++i)
        {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
            delay(10);
        }
    }
    Serial.println("");
    st = "<ol>";
    for (int i = 0; i < n; ++i)
    {
        // Print SSID and RSSI for each network found
        st += "<li>";
        st += WiFi.SSID(i);
        st += " (";
        st += WiFi.RSSI(i);
        st += ")";
        st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
        st += "</li>";
    }
    st += "</ol>";
    delay(100);
    Serial.println("softap");
    Serial.println(ssid);
    Serial.println(passphrase);
    WiFi.softAP(ssid, passphrase, 6);
 
    launchWeb(1);
    Serial.println("..............................");
}
 
void createWebServer(int webtype)
{
    if ( webtype == 1 ) //Trang nhap wifi
    {
        server.on("/", []()
        {
            IPAddress ip = WiFi.softAPIP();
            String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
            content = "<!DOCTYPE HTML>\r\n<html><h2>AP Test</h2>";
          
          
            content += "<form method=\"get\" action=\"setting\">";
            content += "<div>Wifi</div>";
            content += "<div><input name=\"ssid\" size=\"40\"></div>";
            content += "<div>Mat Khau</div>";
            content += "<div><input name=\"pass\" size=\"40\"></div>";
            content += "<div>Blynk Token</div>";
            content += "<div><input name=\"blynk\" size=\"40\"></div>";
           
            content += "<div><input type='submit'></div>";
 
            content += "<p>";
            content += st;
            content += "</p>";
            content += "</html>";
            
            server.send(200, "text/html", content);
        });
        server.on("/setting", []()
        {
            String qsid = server.arg("ssid");
            String qpass = server.arg("pass");
            String qblynk = server.arg("blynk");
            
            if (qsid.length() > 0 && qpass.length() > 0)
            {
                EEPROM.begin(512);
                Serial.println("clearing eeprom");
                for (int i = 0; i < 128; ++i)
                {
                    EEPROM.write(i, 0);
                }
                
                EEPROM.commit();
                Serial.println(qsid);
                Serial.println("");
                Serial.println(qpass);
                Serial.println("");
                Serial.println(qblynk);
                Serial.println("");
               
 
                Serial.println("writing eeprom ssid:");
                for (int i = 0; i < qsid.length(); ++i)
                {
                    EEPROM.write(i, qsid[i]);
                    Serial.print("Wrote: ");
                    Serial.println(qsid[i]);
                }
 
                Serial.println("writing eeprom pass:");
                for (int i = 0; i < qpass.length(); ++i)
                {
                    EEPROM.write(32 + i, qpass[i]);
                    Serial.print("Wrote: ");
                    Serial.println(qpass[i]);
                }

                Serial.println("writing eeprom blynk token:");
                for (int i = 0; i < qblynk.length(); ++i)
                {
                    EEPROM.write(96 + i, qblynk[i]);
                    Serial.print("Wrote: ");
                    Serial.println(qblynk[i]);
                }
                
                EEPROM.commit();
                EEPROM.end();
                //Chop den xanh sau khu lam xong
                Serial.println("Check thiet bi");
                LedRunAround();               
                TurnOffLed();
                
                Serial.println("DONE");
                //---------
                content = "{\"Success\":\"Luu vao he thong. Khoi dong lai ten wifi moi\"}";
                statusCode = 200;
                
                delay(2000);
                //createWebServer(3);
            }
            else
            {
                content = "{\"Error\":\"404 not found\"}";
                statusCode = 404;
                Serial.println("Sending 404");
            }
            server.send(statusCode, "application/json", content);
        });
    }
    else if (webtype == 0) //trang hien thi da dang nhap
    {
        server.on("/", []()
        {
            IPAddress ip = WiFi.localIP();
            String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
            server.send(200, "application/json", "{\"IP\":\"" + ipStr + "\"}");
        });
//        server.on("/cleareeprom", []()
//        {
//            content = "<!DOCTYPE HTML>\r\n<html>";
//            content += "<h2>Clear EEPROM</h2><p>DONE !</p></html>";
//            server.send(200, "text/html", content);
//            Serial.println("clearing eeprom");
//            for (int i = 0; i < 128; ++i)
//            {
//                EEPROM.write(i, 0);
//               
//            }
//            delay(5000);
//            Serial.println("DONE !");
//            Serial.println("\nReset ESP pls");
//            
//            EEPROM.commit();
//        });
    }
    //-------------------------- Working on it-----
    else if (webtype == 3)
    {
      server.on("/ledcontrol", []()
        {
            IPAddress ip = WiFi.localIP();
            String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
//            content = "<!DOCTYPE HTML>\r\n<html><h2>Dieu Khien Led</h2>";
//          
//            content += "</html>";
//            server.send(200, "text/html", content);
//              LedControl();
        });
    }
    //------------------------------------------
}
 
void loop()
{
    server.handleClient();
    Blynk.run();
}
