void setColor(int red, int green, int blue);

#define PinR  14
#define PinG  5
#define PinB  4

char R = 100, G = 100, B = 100;

void setup()
{
  Serial.begin(115200);
  pinMode(PinR, OUTPUT);
  pinMode(PinG, OUTPUT);
  pinMode(PinB, OUTPUT);
  analogWriteRange(255);
  analogWriteFreq(1000);
  setColor(255, 150, 0);
}


void loop()
{

  R += 5;
  B += rand() * 10;
  G += rand() * 10;
  setColor(R, G, B);
  delay(100);
}

void setColor(int red, int green, int blue)
{
  red = 255 - red;
  green = 255 - green;
  blue = 255 - blue;
  analogWrite(PinR, red);
  analogWrite(PinG, green);
  analogWrite(PinB, blue);
}
