#include <SoftwareSerial.h>

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//Alarm reciever's phone number with country code
//NOTE: Must be your personal phone number here
//this is not gsm module phone number
const String PHONE = "+923451234567";
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
#define rxPin 2
#define txPin 3
SoftwareSerial sim800L(rxPin,txPin);
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
#define LASER_RECEIVER_PIN 4
#define LASER_TRANSMIT_PIN 5
//fire_flag = 0 means tripwire detected
boolean laser_flag = 0;
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
#define BUZZER_PIN 8
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

void setup()
{
  //----------------------------------------------------------------
  //Begin serial communication: Arduino IDE (Serial Monitor)
  Serial.begin(115200);
  //----------------------------------------------------------------
  //Begin serial communication: SIM800L
  sim800L.begin(9600);
  //----------------------------------------------------------------
  pinMode(LASER_RECEIVER_PIN,INPUT);
  pinMode(LASER_TRANSMIT_PIN,OUTPUT);
  //by default the LASER Transmitter is ON
  digitalWrite(LASER_TRANSMIT_PIN,HIGH);
  //----------------------------------------------------------------
  pinMode(BUZZER_PIN, OUTPUT);
  //by default the BUZZER is OFF
  digitalWrite(BUZZER_PIN,LOW);
  //----------------------------------------------------------------
  Serial.println("Initializing...");
  //Once the handshake test is successful, it will back to OK
  sim800L.println("AT");
  delay(1000);
  sim800L.println("AT+CMGF=1");
  delay(1000);
  //----------------------------------------------------------------
}

void loop()
{
  while(sim800L.available()){
  Serial.println(sim800L.readString());
  }
  
  int laser_value = digitalRead(LASER_RECEIVER_PIN);
  
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //The laser trip wire is detected, trigger Alarm and send sms
  if(laser_value == LOW) {
    digitalWrite(BUZZER_PIN,HIGH);
    //----------------------------------------------------------------
    if(laser_flag == 0) {
      Serial.println("Laser Tripwire Detected.");
      laser_flag == 1;
      make_call();
    }
    //----------------------------------------------------------------
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //No laser tripwire is detected, turn OFF Alarm
  else {
    //Serial.println("Laser Tripwire !Detected.");
    digitalWrite(BUZZER_PIN,LOW);
    laser_flag = 0;
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
}

//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
void make_call()
{
    Serial.println("calling....");
    sim800L.println("ATD"+PHONE+";");
    delay(20000); //20 sec delay
    sim800L.println("ATH");
    delay(1000); //1 sec delay
}
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
