int In1 = 5;  
int In2 = 4;
int val= 0;
void setup()
{
  Serial.begin(115200);
  pinMode(In1, OUTPUT);      
  pinMode(In2, OUTPUT);   
  //pinMode(congtac, INPUT);
       
}

void loop()
{
 
// 
//  val = digitalRead(congtac);
//  Serial.println(val);
//  if (val != 0 )
//  {
//  Serial.println("Cong tac da dc bat");
//  
//  digitalWrite(ledPin2, val);
//  } else 
//  digitalWrite(ledPin2,val);

 //  delay(1000);

 
  delay(200);
  digitalWrite(In1, LOW);
  delay(200);
  digitalWrite(In1,HIGH);
  delay(200);
  digitalWrite(In2,LOW);
  delay(200); 
digitalWrite(In2,HIGH);
  delay(200); 

  
}
