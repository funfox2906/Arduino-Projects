#include "FS.h"

long intSleep = 1800000;  // Sleep for 30 minutes 

unsigned long prvTime = 0; //last time we took action

void loopCount() {
   File f = SPIFFS.open("/TV_Samsung.rem", "r");
   String line;
   String namefunction="";
   String data="";
   int cnt=0;
   if (!f) {
    Serial.println("Count file open failed on read.");
   } else {
      while(f.available()) {
        line += f.readStringUntil('\n'); //doc tu file
       
        
      if (cnt>=1){
        for (int i =0 ; i<line.length();i++){
          if ( line[i]=='['){
            for (int j=i+1; j<line.length();j++){
              if (line[j]!=']') namefunction+=line[j]; else break;
              }
          } else if (line[i]==']'){
              for (int j=i+1; j<line.length();j++){
              data+=line[j];
              }
            }
        }
      Serial.println();
      Serial.println("Name: ");
      Serial.print(namefunction);
      Serial.println("Data: ");
      Serial.print(data);
      line ="";
      namefunction="";
      data="";
      
        delay(1000);
       
     }
        cnt++;
      }
      f.close();
    }
    
}

//
////opening "w" will truncate the file, so we know we only store our 1 value.
//    f = SPIFFS.open("/Tivi_LG.ram", "w"); 
//
//    if (!f) {
//      Serial.println("Count file open failed on update.");
//    } else {
//      f.println(curCnt + 1); 
//      f.close();
//      Serial.println("Adding one");
//    }
//
//    return curCnt;
//}
//
//int loopReset() {
//    
//    File f = SPIFFS.open("/Tivi_LG.ram", "w");
//
//    if (!f) {
//      Serial.println("Count file open failed on reset.");
//    } else {
//      f.println(0); 
//      f.close();
//    }
//}
//
//void takeAction() {
//  Serial.println("We did something!");
//  //reset our loop counter:
//  loopReset();


void setup() {
  
  Serial.begin(115200);
  delay(10);

  SPIFFS.begin();
 
  // Take some action if we've had 6 iterations (0 index)
//  if (loopCount() > 5) {
//    takeAction();
//  }
//  
//1,000,000 = 1 second
  //ESP.deepSleep(20000000); //20 seconds
  loopCount();
}


void loop() {
 
  //nothing to do here, since deep sleep prevents it from getting this far...
    
}
