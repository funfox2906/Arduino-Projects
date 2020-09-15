//-------------------------------- Help functions ---------------------------
// Quy đổi, OTA
//------------------------------------------------------------------
String jsonStproc(String keyst){  //Đọc và xử lý string json them khóa keyst
  String stt; 
  stt= server.arg(keyst);
  stt.replace("%2F","/");
  //Serial.println(stt);
  return (char*) stt.c_str();
}
boolean jsonBoproc(String keyst){  //Đọc và xử lý string json them khóa keyst
  String stt; 
  stt= server.arg(keyst);
 // Serial.print("jsonBoproc : "); Serial.println(stt);
  return (stt=="1"?1:0);
}
String ipToString(IPAddress ip){
  String s="";
  for (int i=0; i<4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  return s;
}
IPAddress Stringtoip(String strIP){                //????? Kiểm tra lại hàm convert này
  byte Parts[4] = {0,0,0,0};
  int Part = 0;
  for ( int i=0; i<strIP.length(); i++ )
   {
    char c = strIP[i];
    if ( c == '.' )
      {
       Part++;
       continue;
       }
    Parts[Part] *= 10;
    Parts[Part] += c - '0';
     }
    return (Parts);
}

String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 3; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}


void otaCountown(){
    if(otaCount>0 && otaFlag==1) {
      otaCount--;
      Serial.println(otaCount); 
    }
}
//Functin convert AFON-----------------
char* Charstring(String s){
   char st[50];
   s.toCharArray(st,s.length());
   strncpy(st, st+ 11, strlen(st)); 
   return st;
}
char* StrtoChar(String s){   //Check lai KQ
  char st[30];
  s.toCharArray(st,s.length());
  return st;
}
String Cutstring(String s){
   String st;
   //s.toCharArray(st,s.length());
   //strncpy(st, st+ 11, strlen(st));
   st=s.substring(11,s.length());
   return st;
}
int Intstring(String s){  //????
   char* st=Charstring(s); 
   //conver Char* to Int
   return atoi(st);
}
boolean Bolstring(String s){
   char* st=Charstring(s); 
   //conver Char* to Int
   if (atoi(st)==1) {
    return 1;
   }else return 0; 
}
#ifdef WEBOTA
void ota(){
   //Debugln("OTA."); 
   if (OTA.parsePacket()) {
    IPAddress remote = OTA.remoteIP();
    int cmd  = OTA.parseInt();
    int port = OTA.parseInt();
    int size   = OTA.parseInt();

    Serial.print("Update Start: ip:");
    Serial.print(remote);
    Serial.printf(", port:%d, size:%d\n", port, size);
    uint32_t startTime = millis();

    WiFiUDP::stopAll();

    if(!Update.begin(size)){
      Serial.println("Update Begin Error");
      return;
    }

    WiFiClient clientWiFi;
    if (clientWiFi.connect(remote, port)) {

      uint32_t written;
      while(!Update.isFinished()){
        written = Update.write(clientWiFi);
        if(written > 0) clientWiFi.print(written, DEC);
      }
      Serial.setDebugOutput(false);

      if(Update.end()){
        clientWiFi.println("OK");
        Serial.printf("Update Success: %u\nRebooting...\n", millis() - startTime);
        ESP.restart();
      } else {
        Update.printError(clientWiFi);
        Update.printError(Serial);
      }
    } else {
      Serial.printf("Connect Failed: %u\n", millis() - startTime);
    }
  }
  //IDE Monitor (connected to Serial)
  if (TelnetServer.hasClient()){
    if (!Telnet || !Telnet.connected()){
      if(Telnet) Telnet.stop();
      Telnet = TelnetServer.available();
    } else {
      WiFiClient toKill = TelnetServer.available();
      toKill.stop();
    }
  }
  if (Telnet && Telnet.connected() && Telnet.available()){
    while(Telnet.available())
      Serial.write(Telnet.read());
  }
  if(Serial.available()){
    size_t len = Serial.available();
    uint8_t * sbuf = (uint8_t *)malloc(len);
    Serial.readBytes(sbuf, len);
    if (Telnet && Telnet.connected()){
      Telnet.write((uint8_t *)sbuf, len);
      yield();
    }
    free(sbuf);
  }
}
#endif
