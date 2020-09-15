void Scan_Wifi_Networks()
{
  // Thiết lập WiFi để chế độ trạm và ngắt kết nối từ một AP nếu nó đã được kết nối trước đây.
  //Cần phải ở chế độ dicsonected để Chạy quét mạng!
 
 // WiFi.mode(WIFI_STA);
 // WiFi.disconnect();
//  delay(100);
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
 // Serial.println("Scaning Networks Complete..");
 // Serial.print(n); Serial.println(" Networks have been Found");

  if (n == 0)
  {
     Serial.println("no networks found");
  }
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      if (esid == WiFi.SSID(i)){
        Serial.print("My network has been Found! ");
       
        Do_Connect();                    // Khởi động lại để kết nối - sai MK sẽ ntn ?
        //Fl_MyNetwork = true;
      } 
      else
      {
        //Serial.print("Not my network... ");
      }
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
}


void Do_Connect()                  // Try to connect to the Found WIFI Network!
{
   ESP.reset();
}

