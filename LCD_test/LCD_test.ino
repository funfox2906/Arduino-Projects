#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// SCL = D1
// SDA = D2
const int trigPin = 14;  //D4
const int echoPin = 0;  //D3
long duration;
int distance;

Adafruit_SSD1306 display(-1);


void displayy (char n[] , int x){
  // Display Text
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,28);
  display.println(n);
  display.print(x);
  display.display();
  delay(1000);
  display.clearDisplay();
  }

  
  
void setup()   
{                
  // initialize with the I2C addr 0x3C
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  

  // Clear the buffer.
  display.clearDisplay();
  display.println("Setting up ...");
  delay(1000);
  display.clearDisplay();
  //Ultrasonicsensor
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input
Serial.begin(9600);
}



  
void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(trigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);

// Calculating the distance
distance= duration*0.034/2;
// Prints the distance on the Serial Monitor
displayy("Distance: ",distance);

delay(1000);

  
}
