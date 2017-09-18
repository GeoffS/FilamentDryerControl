const int RELAY_PIN = 6;
const int LED_PIN = 13;
const int delay_ms = 5000;

#include "max6675.h"

int ktcSO = 8;
int ktcCS = 9;
int ktcCLK = 10;

float setPoint_C = 35;
float hysterisis_C = 1;

float upperSetPoint_C = setPoint_C + hysterisis_C/2;
float lowerSetPoint_C = setPoint_C - hysterisis_C/2;

float equlibDutyCycle = 0.215;

unsigned long equilibOnTime_ms = delay_ms * equlibDutyCycle;
unsigned long equilibOffTime_ms = delay_ms - equilibOnTime_ms;

bool heaterStatus = false;

MAX6675 ktc(ktcCLK, ktcCS, ktcSO);

void setup() 
{
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  heaterStatus = false;
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  
  delay(500);
}

void loop() 
{
  float deg_C = ktc.readCelsius();
  //Serial.print("Deg C = "); 
  Serial.print(deg_C);

  if((deg_C <= upperSetPoint_C) && (deg_C >= lowerSetPoint_C))
  {
    // Near the setpoint; pulse the heater:
    Serial.println(",10");
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
    delay(equilibOnTime_ms);
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    delay(equilibOffTime_ms);
    return;
  }
  
  // Too hot or too cold; Full on or off:
  if(heaterStatus)
  {
    // Heating up
    if(deg_C > setPoint_C) heaterStatus = false;
  }
  else
  {
    // Cooling down
    if(deg_C < setPoint_C-hysterisis_C) heaterStatus = true;
  }
  
  
  if(heaterStatus)
  {
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
    Serial.println(",20");
  }
  else
  {
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    Serial.println(",0");
  }
  delay(delay_ms);
}
