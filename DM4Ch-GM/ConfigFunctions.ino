bool loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  // Phân bổ một bộ đệm để lưu trữ nội dung của tệp tin.
  std::unique_ptr<char[]> buf(new char[size]);

  // Không sử dụng String ở đây vì thư viện ArduinoJson yêu cầu bộ đệm đầu được phép thay đổi. 
  // Nếu bạn không sử dụng ArduinoJson, bạn cũng có thể sử dụng configFile.readString thay thế.
  
  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<800> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("Failed to parse config file");
    return false;
  }

  int otaFlagC = json["otaFlag"];
  String esidC = json["esid"];
  String epassC = json["epass"];
  int iotModeC = json["iotMode"];
  boolean staticipC= json["staticip"];
  //Serial.print("Static IP load config: ");Serial.println(staticipC);
  
  String ipCC = json["ip"];           
  String subnetCC = json["subnet"];   
  String gatewayCC = json["gateway"]; 
  
  boolean isKNXC= json["isKNX"];
  //Serial.print("isKNX load config: ");Serial.println(isKNXC);
  String udpServerC = json["udpServer"];
  int udpPortC = json["udpPort"];
  String groupknx_O1C = json["groupknx_O1"];
  String groupknx_O2C = json["groupknx_O2"];
  String groupknx_O3C = json["groupknx_O3"];
  String groupknx_O4C = json["groupknx_O4"];
    
  String groupknx_I1C = json["groupknx_I1"];
  String groupknx_I2C = json["groupknx_I2"];
  String groupknx_I3C = json["groupknx_I3"];
  String groupknx_I4C = json["groupknx_I4"];

  boolean unlinkknx1C = json["unlinkknx1"];
  boolean unlinkknx2C = json["unlinkknx2"];
  boolean unlinkknx3C = json["unlinkknx3"];
  boolean unlinkknx4C = json["unlinkknx4"];
  
  String mqttServerC = json["mqttServer"];
  String pubTopicC = json["pubTopic"];
  String subTopicC = json["subTopic"];

  
  // Real world application would store these values in some variables for later use.
  otaFlag = otaFlagC;
  esid = esidC;
  epass = epassC;
  iotMode = iotModeC;
  
  staticip= staticipC;
  
  ip=Stringtoip(ipCC);
  subnet=Stringtoip(subnetCC);
  gateway=Stringtoip(gatewayCC);
  
  mqttServer = mqttServerC;
  pubTopic = pubTopicC;
  subTopic = subTopicC;
  
  isKNX= isKNXC;
  udpServer=udpServerC;
  udpPort=udpPortC;
  groupknx_O1=groupknx_O1C;groupknx_O2=groupknx_O2C;groupknx_O3=groupknx_O3C;groupknx_O4=groupknx_O4C;
  unlinkknx1=unlinkknx1C;unlinkknx2=unlinkknx2C;unlinkknx3=unlinkknx3C;unlinkknx4=unlinkknx4C;
  groupknx_I1=groupknx_I1C;groupknx_I2=groupknx_I2C;groupknx_I3=groupknx_I3C;groupknx_I4=groupknx_I4C;
  Serial.println("----Load config paramate-----");
  Serial.print("otaFlag: "); Serial.println(otaFlag);
  Serial.print("esid: ");    Serial.println(esid);
  Serial.print("epass: ");   Serial.println(epass);
  Serial.print("iotMode: "); Serial.println(iotMode);
  Serial.print("Static IP: "); Serial.println(staticip);
  Serial.print("is KNX: "); Serial.println(isKNX);
  /*Serial.print("IP Static: "); Serial.println(ipCC);
  Serial.print("UnlnkKNXCC: ");Serial.print(unlinkknx1C);Serial.print(unlinkknx2C);Serial.print(unlinkknx3C);Serial.println(unlinkknx4C);
  Serial.print("UnlnkKNX : ");Serial.print(unlinkknx1);Serial.print(unlinkknx2);Serial.print(unlinkknx3);Serial.println(unlinkknx4);
  
  Serial.print("mqttServer: ");Serial.println(mqttServer);
  Serial.print("pubTopic: ");Serial.println(pubTopic);
  Serial.print("subTopic: ");Serial.println(subTopic);*/

  //staticip=false;isKNX=false;
  
  return true;
}

bool saveConfig() {
  StaticJsonBuffer<800> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  
  json["otaFlag"] = otaFlag;
  json["esid"] = esid;
  json["epass"] = epass;
  json["iotMode"] = iotMode;
  //Static IP
  json["staticip"] = staticip;
  json["ip"] = ipToString(ip);
  json["subnet"] = ipToString(subnet);
  json["gateway"] = ipToString(gateway);
  //UDP server  
  json["isKNX"] = isKNX;
  
  json["udpServer"] = udpServer;
  json["udpPort"] = udpPort;
  
  //KNX group address
  json["groupknx_O1"] = groupknx_O1;json["groupknx_O2"] = groupknx_O2;json["groupknx_O3"] = groupknx_O3;json["groupknx_O4"] = groupknx_O4;
  json["groupknx_I1"] = groupknx_I1;json["groupknx_I2"] = groupknx_I2;json["groupknx_I3"] = groupknx_I3;json["groupknx_I4"] = groupknx_I4;
  
  json["unlinkknx1"] = unlinkknx1;json["unlinkknx2"] = unlinkknx2;json["unlinkknx3"] = unlinkknx3;json["unlinkknx4"] = unlinkknx4;
  json["mqttServer"] = mqttServer;
  json["pubTopic"] = pubTopic;
  json["subTopic"] = subTopic;
  
  Serial.println("----Save config paramate -----");
  Serial.print("UnlnkKNX : ");Serial.print(unlinkknx1);Serial.print(unlinkknx2);Serial.print(unlinkknx3);Serial.println(unlinkknx4);
  Serial.print("MQTT Server : ");Serial.print(mqttServer);Serial.print("-");Serial.print(pubTopic);Serial.print("-");Serial.println(subTopic);

  
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  delay(200);
  return true;
}


void setOtaFlag(int intOta){
  otaFlag=intOta;
  saveConfig();
  yield();
}

bool clearConfig(){
    Debugln("DEBUG: In config clear!");
    return SPIFFS.format();  
}

