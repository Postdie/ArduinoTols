#include <FreqMeasure.h>

//https://github.com/PaulStoffregen/FreqMeasure
void setup() {

Serial.begin(57600);

Serial.println("hi");

FreqMeasure.begin();//Measures on pin 8 by default
}

double sum = 0;
int count = 0;
bool state=true;
float frequency;
int continuity = 0;
int id = 0;

void loop() {
if (FreqMeasure.available()) {

// average several reading together
sum = sum + FreqMeasure.read();
count = count +1;

if (count > 30){
frequency = FreqMeasure.countToFrequency(sum / count);
Serial.println(frequency); //we were using this during the testing phase
sum=0;
count =0;
}
}
if (frequency > 6800 && frequency < 7200)
{
id=1;
continuity ++;
// Serial print("Continuity ->");
// Serial printin{continuity);
frequency = 0;
}
if (frequency > 145 && frequency < 375)
{
  id = 2;
  continuity++;
  //Serial.print("Continuity ->");
  //Serial.print(continuity);
  frequency = 0;    
  }
  if(frequency > 800 && frequency < 1200)
  {
    id = 3;
    continuity++;
    //Serial.print("Continuity ->");
    //Serial.println(continuity);
    frequency = 0;
  }
  if(frequency > 5000 && frequency < 6500)
  {
    id = 4;
    continuity++;
    //Serial.print("Continuity ->");
    //Serial.println(continuity);
    frequency = 0;  
  }
  if(continuity >=3)
  {
    continuity = 0;
    switch (id)
  {
    case 1 : Serial.print(id); //noisy cricket
    break;
    case 2 : Serial.print(id); //mosquito
    break;
    case 3 : Serial.print(id); //honeybee
    break;
    case 4 : Serial.print(id); //chain saw
    break;
    default : Serial.print(0);  
  }
  id = 0;
  delay(2000);    
  }
}