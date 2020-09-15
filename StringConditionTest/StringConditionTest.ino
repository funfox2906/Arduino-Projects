String line = "[Power]S111000001110000001000000101111110";
 
void setup() {
  // Mở cổng Serial với mức baudrate là 9600
  Serial.begin(115200);
String namefunction="";
String data="";
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
Serial.println("Name: ");
Serial.println(namefunction);
Serial.println("Data: ");
Serial.println(data);
  
}
 
void loop() {
  
}
