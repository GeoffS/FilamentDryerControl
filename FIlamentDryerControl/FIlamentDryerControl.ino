const int RELAY_PIN = 5;
const int LED_PIN = 13;
const int delay_ms = 5000;

#include "max6675.h"

int ktcSO = 8;
int ktcCS = 9;
int ktcCLK = 10;

float setPoint_C = 65;
float hysterisis_C = 1;

float upperSetPoint_C = setPoint_C + hysterisis_C/2;
float mediumLowSetPoint_C = setPoint_C - hysterisis_C/2;
float lowerSetPoint_C = setPoint_C - hysterisis_C;

float lowDutyCycle = 0.5;
float equlibDutyCycle = 0.3;
float mediumLowEqulibDutyCycle = 0.15;

bool startupCtr;
float startupTemp_C = 45;

unsigned long equilibOnTime_ms = delay_ms * equlibDutyCycle;
unsigned long lowOnTime_ms = delay_ms * lowDutyCycle;
unsigned long mediumLowEquilibOnTime_ms = delay_ms * mediumLowEqulibDutyCycle;

MAX6675 ktc(ktcCLK, ktcCS, ktcSO);

void setup() 
{
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

   printVar("equilibOnTime_ms", equilibOnTime_ms);
   printVar("mediumLowEquilibOnTime_ms", mediumLowEquilibOnTime_ms);
   printVar("lowOnTime_ms", lowOnTime_ms);
  
  delay(500);

  startupCtr = 0;
}

void loop() 
{
  float deg_C = ktc.readCelsius();
  Serial.print(deg_C);

//  if(startupCtr < 10)
//  {
//    heaterOn();
//    startupCtr++;
//    if(deg_C > startupTemp_C)
//    {
//      heaterOff();
//      startupCtr = 100;
//      Serial.println("Too Hot");
//      return;
//    }
//    delay(2000);
//  }

  if((deg_C <= upperSetPoint_C) && (deg_C >= lowerSetPoint_C))
  {
    // Near the setpoint; pulse the heater:
    float onTime_ms = equilibOnTime_ms;
    if(deg_C >= mediumLowSetPoint_C)
    {
      // Just a little low...
      onTime_ms = mediumLowEquilibOnTime_ms;
    }
    Serial.print(",");
    Serial.println(onTime_ms);
    heaterOn();
//    digitalWrite(RELAY_PIN, HIGH);
//    digitalWrite(LED_PIN, HIGH);
    delay(onTime_ms);
    heaterOff();
//    digitalWrite(RELAY_PIN, LOW);
//    digitalWrite(LED_PIN, LOW);
    delay(delay_ms-onTime_ms);
    return;
  }
  
  // Too hot or too cold; Full on or off:
  if(deg_C < upperSetPoint_C)
  {
    // Too cold:
    //Serial.println(",20");
    Serial.print(",");
    float onTime_ms = lowOnTime_ms;
    Serial.println(onTime_ms);
    heaterOn();
    delay(onTime_ms);
    heaterOff();
    delay(delay_ms-onTime_ms);
    return;
//    digitalWrite(RELAY_PIN, HIGH);
//    digitalWrite(LED_PIN, HIGH);
  }
  
  if(deg_C > setPoint_C-hysterisis_C)
  {
    // Too hot:
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    Serial.println(",0");
  }

  delay(delay_ms);
}

void heaterOn()
{
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(LED_PIN, HIGH);
}

void heaterOff()
{
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
}

void printVar(String name, unsigned long value)
{
  Serial.print(name+" ");
  Serial.println(value);
}

