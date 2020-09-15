
void initWiFi(){
  Serial.println();
  Serial.println();
  Serial.println("Startup");
  
  // test esid 
  WiFi.disconnect();
  delay(100);
  WiFi.mode(WIFI_STA);
  Debug("Connecting to WiFi: ");
  Debugln(esid);
  Debugln(epass);
  
  WiFi.begin((char*)esid.c_str(), (char*)epass.c_str());
  
  if ( testWifi() == 20 ) {   //Kết nối wifi OK khi testWifi()=20  ->launchWeb(0)
      launchWeb(0);
      return;
  }                          //Kết nối wifi Error testWifi==10 ->Khởi tạo AP
  
  Serial.println("Opening AP");
  setupAP();   
}

int testWifi(void) {
  int c = 0;
  Debugln("Wifi test..."); 
  
 if (staticip) {                           //Static IP active
    WiFi.config(ip, gateway, subnet);
  } 
  while ( c < 50 ) {
    if (WiFi.status() == WL_CONNECTED) {
        return(20); 
      } //Kết nối OK trả về 20
    delay(500);
    Serial.print(".");    
    c++;
  }
  Serial.println("WiFi Connect timed out!");
  return(10);                                          //Kết nối Error trả về 10
} 

//Khởi tạo AP
void setupAP(void) {
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done....");
  if (n == 0){
    Serial.println("no networks found");
    st ="<b>No networks found:</b>";
  } else {
    Serial.print(n);
    Serial.println(" Networks found");
    st = "<ul>";
    for (int i = 0; i < n; ++i)
     {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" (OPEN)":"*");
      
      // Print to web SSID and RSSI for each network found
      st += "<li>";
      st +=i + 1;
      st = st + ": " + WiFi.SSID(i)+ " (" + WiFi.RSSI(i) + ")";
      st += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" (OPEN)":"*";
      st += "</li>";
      delay(10);
     }
    st += "</ul>";
  }
  Serial.println(""); 
  WiFi.disconnect();
  delay(100);             //Khởi tạo chế độ AP -> launchWeb(1); cấu hình và inApMode=1;
  iotMode=0; staticip=false;
  WiFi.mode(WIFI_AP);

  WiFi.softAP(host);
  WiFi.begin(host); // not sure if need but works
  Serial.print("Access point started with name "); Serial.println(host);
  inApMode=1;
  launchWeb(1);
}

void launchWeb(int webtype) {
    Serial.println("");
    Serial.print("WiFi connected - lunchWeb : ");Serial.println(webtype);
    
    //Start the web server or MQTT
    if(otaFlag==1 && !inApMode){                                   //Vào chế độ OTA flash
      Serial.println("Starting OTA mode - Vào chế độ OTA");    
      Serial.printf("Sketch size: %u\n", ESP.getSketchSize());
      Serial.printf("Free size: %u\n", ESP.getFreeSketchSpace());
      MDNS.begin(host);
      server.on("/", HTTP_GET, [](){
        server.sendHeader("Connection", "close");
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(200, "text/html", otaServerIndex);
      });
      server.on("/update", HTTP_POST, [](){
        server.sendHeader("Connection", "close");
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
        setOtaFlag(0); 
        ESP.restart();
      },[](){                                       
        HTTPUpload& upload = server.upload();
        if(upload.status == UPLOAD_FILE_START){
          //Serial.setDebugOutput(true);
          WiFiUDP::stopAll();
          Serial.printf("Update: %s\n", upload.filename.c_str());
          otaCount=300;
          uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
          if(!Update.begin(maxSketchSpace)){//start with max available size
            Update.printError(Serial);
          }
        } else if(upload.status == UPLOAD_FILE_WRITE){
          if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
            Update.printError(Serial);
          }
        } else if(upload.status == UPLOAD_FILE_END){
          if(Update.end(true)){ //true to set the size to the current progress
            Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
          } else {
            Update.printError(Serial);
          }
          Serial.setDebugOutput(false);
        }
        yield();
      });
      server.begin();
      Serial.printf("Ready! Open http://%s.local in your browser\n", host);
      MDNS.addService("http", "tcp", 80);
      otaTickLoop.attach(1, otaCountown);
    } else {                                           
      //setOtaFlag(1); 
      if (webtype==1 || iotMode==0){ //in config mode or WebControle
          if (webtype==1) {                                         //Chế độ APP setup
            webtypeGlob == 1;
            Serial.println(WiFi.softAPIP());
            server.on("/", webHandleConfig);
            server.on("/a", webHandleConfigSave); 
            server.on("/gpio", webHandleGpio);         
          } else {
            //setup DNS since we are a client in WiFi net
            if (!MDNS.begin(host)) {
              Serial.println("Error setting up MDNS responder!");
            } else {
              Serial.println("mDNS responder started");
              MDNS.addService("http", "tcp", 80);
            }          
            Serial.println(WiFi.localIP());                         //Chế độ chuẩn iotMode=0; webtypeGlob=0; webtype=1
            server.on("/", webHandleRoot);  
            server.on("/gpio", webHandleGpio);
            server.on("/a", webHandleConfigSave); 
            server.on("/b", webHandleConfigKNX);
            server.on("/cfgwifi", webCfgWifi);
            server.on("/cfgknx", webCfgKNX);
            server.on("/cleareeprom", webHandleClearRom);
          }
          //server.onNotFound(webHandleRoot);
          server.begin();
          Serial.println("Web server started");   
          webtypeGlob=webtype;              //Store global to use in loop()
        } else if(webtype!=1 && iotMode==1){                      // in KNX (MQTT) and not in config mode     
          
          //mqttClient.setBrokerDomain((char*) mqttServer.c_str());//naren
          //mqttClient.setPort(1883);//naren
          mqttClient.setServer((char*) mqttServer.c_str(),1883);
          mqttClient.setCallback(mqtt_arrived);
          mqttClient.setClient(wifiClient);
          
          if (WiFi.status() == WL_CONNECTED){
              if (!connectMQTT()){
                delay(2000);
                if (!connectMQTT()){                            
                  Serial.println("Could not connect MQTT.");
                  Serial.println("Starting web server instead.");
                  iotMode=0;
                  launchWeb(0);
                  webtypeGlob=webtype;
                }
              }                    
            }
      }
    }
}

void webHandleConfig(){
  IPAddress ips = WiFi.softAPIP();
  String ipStr = String(ips[0]) + '.' + String(ips[1]) + '.' + String(ips[2]) + '.' + String(ips[3]);
  String s;
 
  s = "Configuration of " + hostName + " at "+ ipStr;
  s += "<p><a href=\"/gpio\">Control GPIO</a><br/><br/>";
  s += "<label>List wifi AP : </label>";
  s += st;
 
  s += "<form method='get' action='a'>";
  s += "<label>SSID: </label><input name='ssid' length=32></br>";
  s += "<label> Pass: </label><input name='pass' type='password' length=64></br>";
  s += "</br>";
  
  s += "<input type='submit'></form></p>";
  s += "\r\n\r\n";
  Serial.println("Sending 200");  
  server.send(200, "text/html", s); 
}

void webHandleConfigSave(){

  String s;
  s = "<p>Settings saved to eeprom and reset to boot into new settings</p>";
  s +="<p>  You must reset device from RESET button OR Off/On power 24vDC.</p>\r\n\r\n";
  server.send(200, "text/html", s); 
  Serial.println("clearing EEPROM.");
  clearConfig();   //Xóa tệp json
  
  esid=jsonStproc("ssid");
  epass=jsonStproc("pass");
  
  String qiot;
  qiot= server.arg("iot");
  Serial.println("POST iot mode: " + qiot);
  qiot=="0"? iotMode = 0 : iotMode = 1 ;
 
  /*String qstaticip;
  qstaticip= server.arg("staticip");
  Serial.println("Static IP: " + qstaticip);
  qstaticip=="0"? staticip = 0 : staticip = 1 ;*/
  
  staticip =jsonBoproc("staticip");
  ip=Stringtoip(jsonStproc("ipC"));
  subnet=Stringtoip(jsonStproc("subnetC"));
  gateway=Stringtoip(jsonStproc("gatewayC"));
  
  Serial.print("Settings written ");  //Tạo Json mới
  saveConfig()? Serial.println("sucessfully.") : Serial.println("not succesfully!");;
  Serial.println("Restarting!"); 
  delay(1000);
  ESP.reset();
}
void webHandleConfigKNX(){
  
  String s;
  s = "<p>Settings saved to eeprom and reset to boot into new settings</p>";
  s +="<p>  You must reset device from RESET button OR Off/On power 24vDC.</p>\r\n\r\n";
  server.send(200, "text/html", s); 
  Serial.println("clearing EEPROM.");
  clearConfig();   //Xóa tệp json

  isKNX =jsonBoproc("isKNX");
  udpServer =jsonStproc("udpserver");
  Serial.print("POST UDP Server: ");Serial.println(udpServer);
  
  String udpPortS=jsonStproc("udpport");
  udpPort= udpPortS.toInt();
  Serial.print("POST UDP Port: ");Serial.print(udpPortS);Serial.print("->");Serial.println(udpPort);

  groupknx_O1=jsonStproc("groupknx_O1");groupknx_O2=jsonStproc("groupknx_O2");
  groupknx_O3=jsonStproc("groupknx_O3");groupknx_O4=jsonStproc("groupknx_O4");
  groupknx_I1=jsonStproc("groupknx_I1");groupknx_I2=jsonStproc("groupknx_I2");
  groupknx_I3=jsonStproc("groupknx_I3");groupknx_I4=jsonStproc("groupknx_I4");

  unlinkknx1=jsonBoproc("unlinkknx1");unlinkknx2=jsonBoproc("unlinkknx2");
  unlinkknx3=jsonBoproc("unlinkknx1");unlinkknx4=jsonBoproc("unlinkknx4");
  
  mqttServer=jsonStproc("mqttserver");
  pubTopic = jsonStproc("pubtop");
  subTopic = jsonStproc("subtop");
  
  Serial.print("Settings written ");  //Tạo Json mới
  saveConfig()? Serial.println("sucessfully.") : Serial.println("not succesfully!");;
  Serial.println("Restarting!"); 
  delay(1000);
  ESP.reset();
}
void webHandleRoot(){
  String s;
  s = "<head><meta charset='UTF-8'></head>";
  s=s+ "<SCRIPT LANGUAGE='JavaScript'> function confirmAction() { return confirm('Bạn có chắc chắn muốn xóa cấu hình ?')}</SCRIPT>";
  s=s+ "<style> p.alignRight {text-align: right}</style><br/>";
  s += "<p><b>Wireless Switch IoT (ID:";
  s += EspID;
  s += ")</b></p>";
  s +="<br/>";
  s += "<a href=\"/gpio\">Kiểm tra đầu ra</a><br/>";
  s += "<a href=\"/cfgwifi\">Cấu hình Wifi</a><br/>";
  s += "<a href=\"/cfgknx\">Cấu hình kết nối (KNX, MQTT)</a><br/>";
  s += "<br/>";
  s += "<a>----------------------------</a><br/>";
  s +="<a href=\"gpio?reboot=1\">Khởi động lại (Reset device)</a><br/>";
  //s += "<a href=\"/cleareeprom\";style='color:Red;'>Xóa toàn bộ cài đặt (Set default factory)</a><br/>";
  s=s+ "<a href=\"/cleareeprom\" title='Cảnh báo !' onclick='return confirmAction()'>Xóa toàn bộ cài đặt (Set default factory)</a><br/>";
  s= s+"<br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>";
  s+= "<p class='alignRight'><font size='2'><i>" + CopyRight + " " + VersionAF + "</i></p>";
  s += "\r\n\r\n";
  Serial.println("Sending 200");  
  server.send(200, "text/html", s); 
}
//======================================
void webCfgWifi(){
 
  String s;
  buildJavascript();
  s = "<head><meta charset='UTF-8'></head>";
  s += "<b>Cấu hình Wifi </b> cho " + hostName; 
  s += "</br></br>"; 
  s += "<form method='get' action='a'>";
  s += "<label><b>* Thông số kết nối Wifi  </b></label></br>";
  s = s + "<label>Tên SSID: </label><input name='ssid' length=32 value='" + esid + "'></br>";
  s += "<label>Mật khẩu: </label><input name='pass' type='password' length=64 value='" + epass + "'></br>";
  s = s + "<label>IOT mode: </label><input type='radio' name='iot' value='0'" + ((iotMode)?"":"checked") + "> HTTP<input type='radio' name='iot' value='1'" + ((iotMode)?"checked":"") +"> MQTT</br>";
  s += "</br>";
  s = s + "<label>Static IP ? </label><input type='radio' name='staticip' value='1'" + ((staticip)?"checked":"") + ">Yes<input type='radio' name='staticip' value='0' " + ((staticip)?"":"checked") + ">No</br>";
  s += "</br>";
  s += "<label><b>* Thiết lập IP tĩnh  </b></label></br>";
  s += "<label>IP address: </label><input name='ipC' length=25 value='" + ipToString(ip) + "'></br>";
  s += "<label>Subnet Mac: </label><input name='subnetC' length=25 value='" + ipToString(subnet) + "'></br>";
  s += "<label>IP Gateway:</label><input name='gatewayC' length=25 value='" + ipToString(gateway) + "'></br>";
  
  s += "<input type='submit'></form></p>";

  s += "\r\n\r\n";
  Serial.println("Sending 200");  
  server.send(200, "text/html", s); 
}

void webCfgKNX(){
  
  String s;
  buildJavascript();
  s = "<head><meta charset='UTF-8'></head>";
  s += "<b>Cấu hình kết nối (KNX, MQTT)</b> cho " + hostName;
  s += "</br></br>";
  s += "<form method='get' action='b'>";
  s += "<label><b>* Thông số KNX Server  </b></label></br>";
  s = s + "<label>KNX connect ? </label><input type='checkbox' name='isKNX' value='1'" + ((isKNX)? "Checked":"") + "></br></br>";
   
  s = s + "<label>UDP Server IP/DNS: </label><input name='udpserver' length=20 value='" + udpServer + "'></br>";
  s = s + "<label>UDP Port: </label><input name='udpport' length=10 value='" + udpPort + "'></br>";
  s = s + "<label>Address Out 1: </label><input name='groupknx_O1' length=10 value='" + groupknx_O1 + "'></br>";
  s = s + "<label>Address Out 2: </label><input name='groupknx_O2' length=10 value='" + groupknx_O2 + "'></br>";
  s = s + "<label>Address Out 3: </label><input name='groupknx_O3' length=10 value='" + groupknx_O3 + "'></br>";
  s = s + "<label>Address Out 4: </label><input name='groupknx_O4' length=10 value='" + groupknx_O4 + "'></br>";
  s = s + "<label>Disable link In&Out:</label><input type='checkbox' name='unlinkknx1' value='1'" + ((unlinkknx1)? "Checked":"") + ">Out1&In1<input type='checkbox' name='unlinkknx2' value='1'" + ((unlinkknx2)? "Checked":"") + ">Out1&In2";
  s = s + "<input type='checkbox' name='unlinkknx3' value='1'" + ((unlinkknx3)? "Checked":"") + ">Out3&In3<input type='checkbox' name='unlinkknx4' value='1'" + ((unlinkknx4)? "Checked":"") + ">Out4&In4</br>";
  s = s + "<label>Address Input 1: </label><input name='groupknx_I1' length=10 value='" + groupknx_I1 + "'></br>";
  s = s + "<label>Address Input 2: </label><input name='groupknx_I2' length=10 value='" + groupknx_I2 + "'></br>";
  s = s + "<label>Address Input 3: </label><input name='groupknx_I3' length=10 value='" + groupknx_I3 + "'></br>";
  s = s + "<label>Address Input 4: </label><input name='groupknx_I4' length=10 value='" + groupknx_I4 + "'></br>";
  s += "</br>";
  s += "<label><b>* Thông số MQTT Server  </b></label></br>";
  s = s+ "<label>MQTT Broker IP/DNS: </label><input name='mqttserver' length=20  value='" + mqttServer + "'></br>";
  s = s+ "<label>MQTT Publish topic: </label><input name='pubtop' length=64  value='" + pubTopic + "'></br>";
  s = s+ "<label>MQTT Subscribe topic: </label><input name='subtop' length=64  value='" + subTopic + "'></br>";
  s += "</br>";
  s += "<input type='submit'></form></p>";
 
  s += "\r\n\r\n";
  Serial.println("Sending 200");  
  server.send(200, "text/html", s); 
}

//======================================
void webHandleClearRom(){
  String s;
  s = "<head><meta charset='UTF-8'></head>";
  s += "<p><b>Xóa thông tin cấu hình và khởi tạo lại để định cấu hình wifi mới</b><p>";
  s += "<label> Thiết bị sẽ khởi động lại và vào chế độ AP</label></br>";
  s += "<label> Bạn cần kết nối tới AP, truy cập 192.168.4.1 để cấu hình lại theo các bước</label></br>";
  s += "<label>   + Cấu hình kết nối wifi.</label></br>";
  s += "<label>   + Thiết bị khởi động lại.</label></br>";
  s += "<label>   + Cần đăng nhập wifi để truy cập lại thiết bị.</label></br>";
  s += "<label>   + Cấu hình kết nối KNX hoặc MQTT.</label></br>";
  s += "</html>\r\n\r\n";
  Serial.println("Sending 200"); 
  server.send(200, "text/html", s); 
  Serial.println("Erase config");
  //**AF**** clearConfig();
  delay(10);
  Serial.println("Done, restarting!");
  ESP.reset();
}


void webHandleGpio(){
  String s;
   // Set GPIO according to the request
    if (server.arg("state_1")=="1" || server.arg("state_1")=="0" ) {
      state_1 = server.arg("state_1").toInt();
      digitalWrite(OUTPIN_1, !state_1);
      sendto_udpServer(groupknx_O1,state_1);
      Serial.print("R_1  switched via web request to  ");      
      Serial.println(state_1);      
    }
    if (server.arg("state_2")=="1" || server.arg("state_2")=="0" ) {
      state_2 = server.arg("state_2").toInt();
      digitalWrite(OUTPIN_2, !state_2);
      sendto_udpServer(groupknx_O2,state_2);
      Serial.print("R_2 switched via web request to  ");      
      Serial.println(state_2);      
    }
    if (server.arg("state_3")=="1" || server.arg("state_3")=="0" ) {
      state_3 = server.arg("state_3").toInt();
      digitalWrite(OUTPIN_3, !state_3);
      sendto_udpServer(groupknx_O3,state_3);
      Serial.print("R_3 switched via web request to  ");      
      Serial.println(state_3);      
    }
    if (server.arg("state_4")=="1" || server.arg("state_4")=="0" ) {
      state_4 = server.arg("state_4").toInt();
      digitalWrite(OUTPIN_4, !state_4);
      sendto_udpServer(groupknx_O4,state_4);
      Serial.print("R_4 switched via web request to  ");      
      Serial.println(state_4);      
    }
//    if (server.arg("state_05")=="1" || server.arg("state_05")=="0" ) {
//      int state_05 = server.arg("state_05").toInt();
//      digitalWrite(OUTLED, !state_05);
//      Serial.print("Led switched via web request to  ");      
//      Serial.println(state_05);      
//    }
    if(server.arg("reboot")=="1")
    {
     ESP.reset(); 
    }

    buildJavascript(); 
   
    s="<!DOCTYPE HTML>\n";
    s+=javaScript;
    s+="<BODY onload='process()'>\n";
    s+="<BR>Wifi Single Relay Board<BR>\n";
    s+="<BR><BR>\n";
    s+="<p>Light is <A id='runtime'></A>\n</p>";
    s+="</BODY>\n";
    
    s = "Output_01 ";
    s += (digitalRead(OUTPIN_1))?"OFF":"ON";
    s += "<p><form action='gpio'><input type='radio' name='state_1' value='1' ";
    s += (state_1)?"checked":"";
    s += ">On<input type='radio' name='state_1' value='0' ";
    s += (state_1)?"":"checked";
    s += ">Off <input type='submit' value='Submit'></form></p>"; 

    s += "Output_02 ";
    s += (digitalRead(OUTPIN_2))?"OFF":"ON";
    s += "<p><form action='gpio'><input type='radio' name='state_2' value='1' ";
    s += (state_2)?"checked":"";
    s += ">On<input type='radio' name='state_2' value='0' ";
    s += (state_2)?"":"checked";
    s += ">Off <input type='submit' value='Submit'></form></p>"; 

    s += "Output_03 ";
    s += (digitalRead(OUTPIN_3))?"OFF":"ON";
    s += "<p><form action='gpio'><input type='radio' name='state_3' value='1' ";
    s += (state_3)?"checked":"";
    s += ">On<input type='radio' name='state_3' value='0' ";
    s += (state_3)?"":"checked";
    s += ">Off <input type='submit' value='Submit'></form></p>"; 

    s += "Output_04 ";
    s += (digitalRead(OUTPIN_4))?"OFF":"ON";
    s += "<p><form action='gpio'><input type='radio' name='state_4' value='1' ";
    s += (state_4)?"checked":"";
    s += ">On<input type='radio' name='state_4' value='0' ";
    s += (state_4)?"":"checked";
    s += ">Off <input type='submit' value='Submit'></form></p>";

//    s += "LED is ";
//    s += (!digitalRead(OUTLED))?"ON":"OFF";
//    s += "<p><form action='gpio'><input type='radio' name='state_05' value='1' ";
//    s += (!digitalRead(OUTLED))?"checked":"";
//    s += ">LED_On <input type='radio' name='state_05' value='0' ";
//    s += (!digitalRead(OUTLED))?"":"checked";
//    s += ">LED_OFF <input type='submit' value='Submit'></form></p>";
//    s +="<p><a href=\"gpio?reboot=1\">Reboot</a></p>";  
    
     
    server.send(200, "text/html", s);    
}

void buildJavascript(){
  javaScript="<SCRIPT>\n";
  javaScript+="var xmlHttp=createXmlHttpObject();\n";

  javaScript+="function createXmlHttpObject(){\n";
  javaScript+=" if(window.XMLHttpRequest){\n";
  javaScript+="    xmlHttp=new XMLHttpRequest();\n";
  javaScript+=" }else{\n";
  javaScript+="    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');\n";
  javaScript+=" }\n";
  javaScript+=" return xmlHttp;\n";
  javaScript+="}\n";

  javaScript+="function process(){\n";
  javaScript+=" if(xmlHttp.readyState==0 || xmlHttp.readyState==4){\n";
  javaScript+="   xmlHttp.open('PUT','xml',true);\n";
  javaScript+="   xmlHttp.onreadystatechange=handleServerResponse;\n"; // no brackets?????
  javaScript+="   xmlHttp.send(null);\n";
  javaScript+=" }\n";
  javaScript+=" setTimeout('process()',1000);\n";
  javaScript+="}\n";
 
  javaScript+="function handleServerResponse(){\n";
  javaScript+=" if(xmlHttp.readyState==4 && xmlHttp.status==200){\n";
  javaScript+="   xmlResponse=xmlHttp.responseXML;\n";
  javaScript+="   xmldoc = xmlResponse.getElementsByTagName('response');\n";
  javaScript+="   message = xmldoc[0].firstChild.nodeValue;\n";
  javaScript+="   document.getElementById('runtime').innerHTML=message;\n";
  javaScript+=" }\n";
  javaScript+="}\n";
  javaScript+="</SCRIPT>\n";
}

void buildXML(){
  XML="<?xml version='1.0'?>";
  XML+="<response>";
  XML+=millis2time();
 // XML+=(digitalRead(OUTPIN))?"ON":"OFF";
  XML+="</response>";
}

String millis2time(){
  String Time="";
  unsigned long ss;
  byte mm,hh;
  ss=millis()/1000;
  hh=ss/3600;
  mm=(ss-hh*3600)/60;
  ss=(ss-hh*3600)-mm*60;
  if(hh<10)Time+="0";
  Time+=(String)hh+":";
  if(mm<10)Time+="0";
  Time+=(String)mm+":";
  if(ss<10)Time+="0";
  Time+=(String)ss;
  return Time;
}

void handleXML(){
  buildXML();
  server.send(200,"text/xml",XML);
}
