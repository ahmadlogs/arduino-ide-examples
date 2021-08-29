/*
 * Created By: Tauseef Ahmad
 * YouTube Channel: https://www.youtube.com/channel/UCOXYfOHgu-C-UfGyDcu5sYw
 * 
*/

#include <SoftwareSerial.h>

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
#define RAIN_SENSOR A0
//----------------------------------------------------------------
//when rain is detected, then rain_value will increase
int rain_value = 0;
//----------------------------------------------------------------
//when rain value is equal or above 10 then sends rain alarm
int rain_default=10;
//----------------------------------------------------------------
//rain_flag = 0 means rain not detected
//rain_flag = 1 means rain is detected
boolean rain_flag = 0;
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//Alarm reciever's phone number with country code
//NOTE: Must be your personal phone number here
//this is not gsm module phone number

const String PHONE = "ENTER_YOUR_PERSONAL_PHONE_NUMBER";
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
#define rxPin 2
#define txPin 3
SoftwareSerial sim800L(rxPin,txPin);
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//#define BUZZER_PIN 8
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

//______________________________________________________________________________________
void setup()
{
  //----------------------------------------------------------------
  //Begin serial communication: Arduino IDE (Serial Monitor)
  Serial.begin(115200);
  //----------------------------------------------------------------
  //Begin serial communication: SIM800L
  sim800L.begin(9600);
  //----------------------------------------------------------------
  pinMode(RAIN_SENSOR,INPUT);
  //----------------------------------------------------------------
  //pinMode(BUZZER_PIN, OUTPUT);
  //by default the BUZZER is OFF
  //digitalWrite(BUZZER_PIN,LOW);
  //----------------------------------------------------------------
  Serial.println("Initializing...");
  //Once the handshake test is successful, it will back to OK
  sim800L.println("AT");
  delay(1000);
  sim800L.println("AT+CMGF=1");
  delay(1000);
  //----------------------------------------------------------------
}
//______________________________________________________________________________________

//______________________________________________________________________________________
void loop()
{
  while(sim800L.available()){
  Serial.println(sim800L.readString());
  }
  
  rain_value = analogRead(RAIN_SENSOR);
  rain_value = map(rain_value,0,1023,225,0);
  
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //The rain is detected, trigger Alarm and send sms
  if(rain_value>=rain_default) {
    //digitalWrite(BUZZER_PIN,HIGH);
    //----------------------------------------------------------------
    if(rain_flag == 0) {
      Serial.println("Rain is Detected.");
      rain_flag == 1;
      send_sms();
      make_call();
    }
    //----------------------------------------------------------------
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //No rain is detected, turn OFF Alarm
  else {
    //digitalWrite(BUZZER_PIN,LOW);
    if(rain_flag == 1) {Serial.println("Rain is not Detected.");}  
    rain_flag = 0;
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
}
//______________________________________________________________________________________

//______________________________________________________________________________________
void make_call()
{
    Serial.println("calling....");
    sim800L.println("ATD"+PHONE+";");
    delay(20000); //20 sec delay
    sim800L.println("ATH");
    delay(1000); //1 sec delay
}
//______________________________________________________________________________________

//______________________________________________________________________________________
void send_sms()
{
    Serial.println("sending sms....");
    delay(50);
    sim800L.print("AT+CMGF=1\r");
    delay(1000);
    sim800L.print("AT+CMGS=\""+PHONE+"\"\r");
    delay(1000);
    sim800L.print("This is a rain Alarm");
    delay(100);
    sim800L.write(0x1A); //ascii code for ctrl-26 //Serial2.println((char)26); //ascii code for ctrl-26
    delay(5000);
}
//______________________________________________________________________________________
