#include <SoftwareSerial.h>

//Alarm reciever's phone number with country code
const String PHONE = "Enter_Your_Phone";

//GSM Module RX pin to Arduino 3
//GSM Module TX pin to Arduino 2
#define rxPin 2
#define txPin 3
SoftwareSerial sim800(rxPin,txPin);

//the pin that the pir sensor is atteched to
int pir_sensor = 11;

void setup() {
  pinMode(pir_sensor, INPUT);    // initialize sensor as an input
  Serial.begin(115200);          // initialize serial

  sim800.begin(9600);
  Serial.println("SIM800L software serial initialize");

  sim800.println("AT");
  delay(1000);
}

void loop(){

//////////////////////////////////////////////////
while(sim800.available()){
  Serial.println(sim800.readString());
}
//////////////////////////////////////////////////
while(Serial.available())  {
  sim800.println(Serial.readString());
}
//////////////////////////////////////////////////
  int val = digitalRead(pir_sensor);  // read sensor value
  if (val == HIGH) {                  // check if the sensor is HIGH
    //state = HIGH;
    Serial.println("Motion detected!");
    Serial.println("calling....");
    delay(1000);
    sim800.println("ATD"+PHONE+";");
    delay(20000); //20 sec delay
  }
}
