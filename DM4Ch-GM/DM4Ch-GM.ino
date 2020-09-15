 /*WIFI FOUR DIMMER 0-10v BOARD Virtual four way Switch code
    
    Physial Switch            SSR           MQTT ON      MQTT OFF            HTTP ON                         HTTP OFF  
   
    GPIO2              Relay 1(GPIO14)       R1_ON       R1_OFF     http://ip/gpio?state_1=1       http://ip/gpio?state_1=0 
    GPIO5              Relay 2(GPIO12)       R2_ON       R2_OFF     http://ip/gpio?state_2=1       http://ip/gpio?state_2=0 
    GPIO15             Relay 3(GPIO4)        R3_ON       R3_OFF     http://ip/gpio?state_3=1       http://ip/gpio?state_3=0 
    GPIO16             Relay 4(GPIO13)       R4_ON       R4_OFF     http://ip/gpio?state_4=1       http://ip/gpio?state_4=0 
*/
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
//#include <EEPROM.h>
#include <Ticker.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "FS.h"
#include <WiFiUDP.h>
#include "Arduino.h"

// Includes of Expressif SDK
extern "C"{
  #include "pwm.h"
  #include "user_interface.h"  //Needed for the reset command
}

#define DEBUG
//#define WEBOTA
//debug added for information, change this according your needs

#ifdef DEBUG
  #define Debug(x)    Serial.print(x)
  #define Debugln(x)  Serial.println(x)
  #define Debugf(...) Serial.printf(__VA_ARGS__)
  #define Debugflush  Serial.flush
#else
  #define Debug(x)    {}
  #define Debugln(x)  {}
  #define Debugf(...) {}
  #define Debugflush  {}
#endif



//***** Settings declare ********************************************************************************************************* 
String hostName ="Wireless"; //The MQTT ID -> MAC adress will be added to make it kind of unique
String typeDevice="DM";      //SW: Switch; SH: Shutter; BI: Binary Input; RGB: RGB Control; DM: Dimmer
int iotMode=0; //IOT mode: 0 = Web control, 1 = MQTT (No const since it can change during runtime)
String VersionAF="("+ hostName+typeDevice +" Version 1.0 - 04.2018)";
String CopyRight="Copyright (c) AF-GAMMAVIETNAM";
//GPIO's >> PWM

#define PWM_PERIOD 5000 // Period of PWM frequency -> default of SDK: 5000 -> * 200ns ^= 1 kHz 
#define PWM_CHANNELS 4  // PWM channels

// PWM setup (choice all pins that you use PWM D1 - D8)
uint32 io_info[PWM_CHANNELS][3] = {
// MUX, FUNC, PIN
//  {PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5,   5}, // D1
//  {PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4,   4}, // D2
//  {PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0,   0}, // D3
  {PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2,   2}, // D4
  {PERIPHS_IO_MUX_MTMS_U,  FUNC_GPIO14, 14}, // D5
  {PERIPHS_IO_MUX_MTDI_U,  FUNC_GPIO12, 12}, // D6
  {PERIPHS_IO_MUX_MTCK_U,  FUNC_GPIO13, 13}, // D7
//  {PERIPHS_IO_MUX_MTDO_U,  FUNC_GPIO15 ,15}, // D8
                         // D0 - not have PWM :-(
};
// PWM initial duty: all off
uint32 pwm_duty_init[PWM_CHANNELS];
// Period
uint32_t period = PWM_PERIOD;
// Dimmer variables
int16_t duty = 0;
int16_t step = 1;
const int16_t PMWmin=0;
const int16_t PMWmax=5000;


#define OUTPIN_1 2 //output pin D5 //relay1
#define OUTPIN_2 12 //output pin D6//relay2
#define OUTPIN_3 13 //output pin D7//relay3
#define OUTPIN_4 3  //output pin D9(RX)//relay4
#define INPIN 0     // input pin (push button)

#define SWITCH_INPIN1 2 //D1 input pin (Physical Switch)   //Intially while booting maintain this gpio2 to high to low by placing the physical switch in off condition //trigger relay1
#define SWITCH_INPIN2 4 //D2 input pin (Physical Switch)   //trigger relay2
#define SWITCH_INPIN3 5 //D3  //trigger relay3
#define SWITCH_INPIN4 1 //16 test - 1(RX) chuẩn - error debug input pin (Physical Switch)  //Intially while booting maintain this gpio15 to low by  placing the physical switch in off condition//trigger relay4
//R5 has to be removed from the SSR board,so that we will be able use it as virtual switch, its connected to the GPIO16 
//#define OUTLED 5 
#define RESTARTDELAY 3 //minimal time in sec for button press to reset
#define HUMANPRESSDELAY 50 // the delay in ms untill the press should be handled as a normal push by human. Button debounce. !!! Needs to be less than RESTARTDELAY & RESETDELAY!!!
#define RESETDELAY 10 //Minimal time in sec for button press to reset all settings and boot to config mode

//##### Object instances ##### 
MDNSResponder mdns;
ESP8266WebServer server(80);
WiFiClient wifiClient;
PubSubClient mqttClient;
WiFiUDP Udp;

Ticker btn_timer;            //Tạo nhịp ngắt 0.05s kiểm tra nút bấm flash On/Off test; 3s; 20s
Ticker otaTickLoop;          //Tạo nhịp ngắt 1s chờ cập nhật OTA

//##### Flags ##### They are needed because the loop needs to continue and cant wait for long tasks!
int rstNeed=0;   // Restart needed to apply new settings
int toPub=0; // determine if state should be published.
int configToClear=0; // determine if config should be cleared.
int otaFlag=0;
boolean inApMode=0;

//##### Global vars ##### 
int webtypeGlob;
int otaCount=300; //imeout in sec for OTA mode
int current; //Current state of the button
unsigned long count = 0; //Button press time counter
String st; //WiFi Stations HTML list
String state; //State of light
char buf[40]; //For MQTT data recieve
char* host; //The DNS hostname

//To be read from Config file
//String esid="BMTL-Inter";
//String epass = "BOMONTENLUA2015";
String esid="";
String epass = "";
boolean staticip=false;
IPAddress ip(192,168,1,90);   
IPAddress gateway(192,168,1,1);   
IPAddress subnet(255,255,255,0); 

//---Udp server with KNX
boolean isKNX=false;
String EspID=String(ESP.getChipId());
String udpServer="192.168.1.10";
int udpPort=12345;

String groupknx_O1="7/1/1";String groupknx_O2="7/1/2";String groupknx_O3="7/1/3";String groupknx_O4="7/1/4";
String groupknx_I1="7/4/1";String groupknx_I2="7/4/2";String groupknx_I3="7/4/4";String groupknx_I4="7/4/4";
boolean unlinkknx1=0,unlinkknx2=0,unlinkknx3=0,unlinkknx4=0;
  
String javaScript,XML;
int switch_status1, switch_status2, switch_status3, switch_status4; //Physical state of the switch
int16_t state_1=0, state_2=0, state_3=0, state_4=0;
int send_status_1, send_status_2, send_status_3, send_status_4;

String mqttServer,pubTopic,subTopic;

const char* otaServerIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

unsigned int udpPort_local = 12345;// local port to listen for UDP packets
byte packetBuffer[512];            //buffer to hold incoming and outgoing packets

void IntCallbackDo1(){
    if (unlinkknx1==0) {
      send_status_1=1;
    }else {
      sendto_udpServer(groupknx_I1,!digitalRead(SWITCH_INPIN1));
    }
}

//-------------- void's -------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(100);
  
  ////// Initialize the PWM
  // Initial duty -> all off
  for (uint8_t channel = 0; channel < PWM_CHANNELS; channel++) {
    pwm_duty_init[channel] = 0;
  }

  // Initialize
  pwm_init(period, pwm_duty_init, PWM_CHANNELS, io_info);

  // Commit
  pwm_start();
  
  //pinMode(OUTLED, OUTPUT);
  pinMode(INPIN, INPUT_PULLUP);
 
  //define manual switch
   pinMode(SWITCH_INPIN1, INPUT);pinMode(SWITCH_INPIN2, INPUT);
   pinMode(SWITCH_INPIN3, INPUT);pinMode(SWITCH_INPIN4, INPUT);
  
  btn_timer.attach(0.05, btn_handle);

  attachInterrupt(digitalPinToInterrupt(SWITCH_INPIN1), IntCallbackDo1, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(SWITCH_INPIN2), IntCallbackDo2, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(SWITCH_INPIN3), IntCallbackDo3, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(SWITCH_INPIN4), IntCallbackDo4, CHANGE);
  
  Debugln("DEBUG: Entering loadConfig()");
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
  }
  
  uint8_t mac[6];
  WiFi.macAddress(mac);
  hostName += typeDevice; hostName += "-"; hostName += EspID;
  host =(char*) hostName.c_str(); 
  
  loadConfig();

  Debugln("DEBUG: loadConfig() passed");
  
  // Connect to WiFi network
  Debugln("DEBUG: Entering initWiFi()");
  initWiFi();
  
  Debugln("DEBUG: initWiFi() passed");
  Debug("iotMode    :");Debugln(iotMode);
  Debug("staticIP   :");Debugln(staticip);
  
  Debug("webtypeGlob:");Debugln(webtypeGlob);
  Debug("otaFlag    :");Debugln(otaFlag);
  Debug("static IP  :");Debugln(staticip);
  Debug("isKNX      :");Debugln(isKNX);
  Debugln("DEBUG: Starting the main loop");
  
 // iotMode=1;
  if (isKNX==1) {
    init_udp();
  }
}


void btn_handle()
{
  if(!digitalRead(INPIN)){
    ++count; // one count is 50ms
  } else {
    if (count > 1 && count < HUMANPRESSDELAY/5) { //push between 50 ms and 1 sec - Check kênh 1 On/Off    
      Serial.print("button pressed "); 
      Serial.print(count*0.05); 
      Serial.println(" Sec."); 
    
      Serial.print("Light is "); Serial.println(digitalRead(OUTPIN_1));
      Serial.print("Switching light to "); Serial.println(!digitalRead(OUTPIN_1));

      //Test Ch 1
      digitalWrite(OUTPIN_1, !digitalRead(OUTPIN_1)); 
      state = digitalRead(OUTPIN_1);
      
      if(iotMode==1 && mqttClient.connected()){
        toPub=1;        
        Debugln("DEBUG: toPub set to 1");
      }
    } else if (count > (RESTARTDELAY/0.05) && count <= (RESETDELAY/0.05))
      {                                          //pressed 3-9 secs (60*0.05s) - Vào chế độ OTC flash
      Serial.print("button pressed "); 
      Serial.print(count*0.05); 
      Serial.println(" Sec. Restarting!"); 
      setOtaFlag(!otaFlag);      
      ESP.reset();
    } else if (count > (RESETDELAY/0.05)){         //pressed >10 secs - Vào chế độ Clear
      Serial.print("button pressed "); 
      Serial.print(count*0.05); 
      Serial.println(" Sec."); 
      Serial.println(" Clear settings and resetting!");       
      configToClear=1;
      }
    count=0; //reset since we are at high
  }
}


//-------------------------------- Main loop ---------------------------
void loop() {
 
  //proce_button_loop();
  if (send_status_1==1){
    if (state_1>0) {
       state_1=PMWmin;
    }else {state_1=PMWmax;}
    send_status_1=0;
    pwm_set_duty(state_1, 0);
    pwm_start();
    sendto_udpServer(groupknx_O1,state_1);
    Serial.print("Button 1 control Dim 1 : ");Serial.println(state_1);
  }
  if (send_status_2==1){
    state_2=!state_2;send_status_2=0;
    digitalWrite(OUTPIN_2, !state_2);
    sendto_udpServer(groupknx_O2,state_2);
    Serial.print("Button 2 control LED 2 : ");Serial.println((state_2==1)?"ON":"OFF");
  }
  if (send_status_3==1){
    state_3=!state_3;send_status_3=0;
    digitalWrite(OUTPIN_3, !state_3);
    sendto_udpServer(groupknx_O3,state_3);
    Serial.print("Button 3 control LED 3 : ");Serial.println((state_3==1)?"ON":"OFF");
  }
  if (send_status_4==1){
    state_4=!state_4;send_status_4=0;
    digitalWrite(OUTPIN_4, !state_4);
    sendto_udpServer(groupknx_O4,state_4);
    Serial.print("Button 4 control LED 4 : ");Serial.println((state_4==1)?"ON":"OFF");
  }

  //Debugln("DEBUG: loop() begin");
  if(configToClear==1){
    //Debugln("DEBUG: loop() clear config flag set!");
    clearConfig()? Serial.println("Config cleared!") : Serial.println("Config could not be cleared");
    delay(1000);
    ESP.reset();
  }
  //Debugln("DEBUG: config reset check passed");  
  if (WiFi.status() == WL_CONNECTED && otaFlag){
    if(otaCount<=1) {
      Serial.println("OTA mode time out. Reset!"); 
      setOtaFlag(0);
      ESP.reset();
      delay(100);
    }
    server.handleClient();
    delay(1);
  } else if (WiFi.status() == WL_CONNECTED || webtypeGlob == 1){   //??? Lam gi day
    //Debugln("DEBUG: loop() wifi connected & webServer ");
    if (iotMode==0 || webtypeGlob == 1){
      //Debugln("DEBUG: loop() Web mode requesthandling ");
      server.handleClient();
      delay(1);
      if (isKNX) {
            loop_udp();
          }
      if(esid != "" && WiFi.status() != WL_CONNECTED) //wifi reconnect part
      {
        Scan_Wifi_Networks();                      //Cố gắng quét, kiểm tra tên wifi nếu có trùng thì reset và connect lại
                                                   //Nếu bị đổi Pass Wifi cần : Ấn Flash >10s; Ấn Reset sau 5s; Kết nối AP  -> Thiết lập lại 
      }
    } else if (iotMode==1 && otaFlag !=1){         //Udp loop
          if (isKNX) {
            loop_udp();
          }
          
          if (!connectMQTT()){
              delay(200);          
          }                    
          if (mqttClient.connected()){            
              //Debugln("mqtt handler");
              mqtt_handler();
          } else {
              Debugln("mqtt Not connected!");
          }
    }
  } else{
    Debugln("DEBUG: loop - WiFi not connected");  
    delay(1000);
    initWiFi(); //Try to connect again
  }
    //Debugln("DEBUG: loop() end");
}
