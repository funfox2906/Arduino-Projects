// A UDP instance to let us send and receive packets over UDP
void sendto_udpServer(String groupknx,int value){
    char ipBuffer[255];
    String message;
    message="group " + groupknx +" ";
    message += (value)?"ON":"OFF"; 
    //message += (value)?"OFF":"ON";           //Đảo gá trị trước khi gửi
    message.toCharArray(ipBuffer, message.length()+1);
    
    Serial.println(message);
    if (isKNX) {
    //Udp.beginPacket(Udpserver_IP, udpPort);
    Udp.beginPacket(Stringtoip(udpServer), udpPort);
    Udp.write(ipBuffer);
    Udp.endPacket();
    }   
}
void send_udp(String message,IPAddress rIP,int rPort){
    char ipBuffer[255];
    message.toCharArray(ipBuffer, message.length());
    //Serial.println(ipBuffer);
    Udp.beginPacket(rIP, rPort);
    Udp.write(ipBuffer);
    Udp.endPacket();
    //Serial.print("Send register to Udp server : ");Serial.println(ipToString(rIP));
}

void init_udp(){    //Khởi tạo Udp và lắng nghe cổng udpPort_local; Gửi string cấu hình cho server
  Udp.begin(udpPort_local);   
 //Dieu khien:   group 1/1/1 on(off) hoac value  (03 cụm)
 //Dang ky   :   register chipID switch/blind addr1;addr2;addr3;addr4 (04 or 05 cụm)
  String stt="register ";
  stt = stt + typeDevice + ESP.getChipId();
  stt += " ";
  stt += ipToString(WiFi.localIP());
  stt = stt + " switch " + groupknx_O1 + ";"+ groupknx_O2 +";"+ groupknx_O3 + ";"+ groupknx_O4+ "; "+ groupknx_I1 + ";"+ groupknx_I2 +";"+ groupknx_I3 + ";"+ groupknx_I4+ ";";
  Serial.println(stt);
  send_udp(stt,Stringtoip(udpServer),udpPort);
  
}
void loop_udp(){
  int noBytes = Udp.parsePacket();
  String received_command = "";
  
  if (noBytes ) {
    Serial.print(millis() / 1000);
    Serial.print(":Packet of ");
    /*Serial.print(noBytes);
    Serial.print(" received from ");
    Serial.print(Udp.remoteIP());
    Serial.print(":");
    Serial.println(Udp.remotePort());*/
    // We've received a packet, read the data from it
    Udp.read(packetBuffer,noBytes); // read the packet into the buffer

    // display the packet contents in HEX
    for (int i=1;i<=noBytes;i++)
    {
      //Serial.print(packetBuffer[i-1],HEX);
      received_command = received_command + char(packetBuffer[i - 1]);
      if (i % 32 == 0)
      {
        Serial.println();
      }
      else Serial.print(' ');
    } // end for
    //Serial.println();
    Serial.println(received_command);
    Serial.println();
    String stt=received_command;

    //Xử lý lệnh Udp từ KNX
    //Cú pháp : a/b/c ON(OFF)
    String Addr=stt.substring(0, stt.indexOf(' '));
    String Comm=stt.substring(stt.indexOf(' ')+1,stt.length());
    
    int value=((Comm=="ON" or Comm=="on")?1:0);
    //int value=((Comm=="ON" or Comm=="on")?0:1);        //Đảo gá trị khi nhận về
   if (Addr==groupknx_O1){
      state_1 = value;
      digitalWrite(OUTPIN_1, !state_1);
   }else if (Addr==groupknx_O2){
      state_2 = value;
      digitalWrite(OUTPIN_2, !state_2);
    }else if (Addr==groupknx_O3){
      state_3 = value;
      digitalWrite(OUTPIN_3, !state_3);
    }else if (Addr==groupknx_O4){
      state_4 = value;
      digitalWrite(OUTPIN_4, !state_4);
    }
    //Phan hoi lai goi Udp
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    String stans="Answer ";
    stans = stans + typeDevice + ESP.getChipId();
    stans = stans + " " + ipToString(WiFi.localIP()) + " " + Addr + " " + value;
    Udp.println(stans);
    Udp.endPacket();

    
  }
}

