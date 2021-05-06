#include <SoftwareSerial.h>

#define rxPin 2
#define txPin 3
SoftwareSerial sim800L(rxPin,txPin); 

void setup()
{
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(115200);
  
  //Begin serial communication with Arduino and SIM800L
  sim800L.begin(9600);
  Serial.println("Initializing SIM800L...");
  delay(1000);

  sim800L.print("AT+CMGF=1\r"); //SMS text mode
  delay(1000);
}

void loop()
{
  while(sim800L.available()){
    Serial.println(sim800L.readString());
  }
  while(Serial.available())  {
    sim800L.println(Serial.readString());
  }
}

/*
AT
Check Sim is ready or not
AT+CPIN?
 */
