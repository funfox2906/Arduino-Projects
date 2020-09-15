void TurnOffLed(){
  for (int i=0; i<24; i++){
    strip.SetPixelColor(i,black);
  }
  strip.Show();
}


void LedRunAround(){
  for (int i=0; i<24; i++){
    TurnOffLed();
    strip.SetPixelColor(i, green);
    strip.SetPixelColor(23-i, blue);
    if (i<12){
      strip.SetPixelColor(12+i,red);
    } else {
      strip.SetPixelColor(i-12, red);
    }
    strip.Show();
    delay(70);
  }
}

void LedRGB(String color){
  TurnOffLed();
  if (color == "green"){
    for (int i=0; i<24; i++){
      strip.SetPixelColor(i, green);
    }
  } else if(color == "red")
  {
    if (color == "red"){
    for (int i=0; i<24; i++){
      strip.SetPixelColor(i, red);
    }
   }
  }else if (color=="green")
  {
    if (color == "green"){
    for (int i=0; i<24; i++){
      strip.SetPixelColor(i, green);
    }
   }
  }
  strip.Show();
  delay(50);
}


