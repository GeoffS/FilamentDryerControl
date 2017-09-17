const int RELAY_PIN = 6;
const int LED_PIN = 13;
const int delay_ms = 2000;

#include "max6675.h"

int ktcSO = 8;
int ktcCS = 9;
int ktcCLK = 10;

float setPoint_C = 35;
float hysterisis_C = 1;

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
//    digitalWrite(RELAY_PIN, LOW);
//    digitalWrite(LED_PIN, LOW);
//    delay(delay_ms);
//    digitalWrite(RELAY_PIN, HIGH);
//    digitalWrite(LED_PIN, HIGH);
//    delay(delay_ms);

  float degC = ktc.readCelsius();
  if(degC > 35.0) heaterStatus = false;
  else            heaterStatus = true;
  
  Serial.print("Deg C = "); 
  Serial.print(degC);
  if(heaterStatus)
  {
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
    Serial.println(" Heater On");
  }
  else
  {
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    Serial.println(" Heater Off");
  }
  
  delay(500);
}
