#include <SoftwareSerial.h>

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//Alarm reciever's phone number with country code
const String PHONE_1 = "ENTER_PHONE_NUMBER_HERE";
const String PHONE_2 = ""; //optional
const String PHONE_3 = ""; //optional
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
#define rxPin 2
#define txPin 3
SoftwareSerial sim800L(rxPin,txPin);
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
#define flame_sensor_pin 5
//fire_flag = 0 means no fire detected
boolean fire_flag = 0;
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
#define buzzer_pin 4
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

void setup()
{
  //-----------------------------------------------------------------------------------
  //Begin serial communication: Arduino IDE (Serial Monitor)
  Serial.begin(115200);
  
  //-----------------------------------------------------------------------------------
  //Begin serial communication: SIM800L
  sim800L.begin(9600);
  
  //-----------------------------------------------------------------------------------
  pinMode(flame_sensor_pin, INPUT);
  
  //-----------------------------------------------------------------------------------
  pinMode(buzzer_pin, OUTPUT);
  digitalWrite(buzzer_pin,LOW);
  
  //----------------------------------------------------------------------------------
  Serial.println("Initializing...");
  //Once the handshake test is successful, it will back to OK
  sim800L.println("AT");
  delay(1000);
  sim800L.println("AT+CMGF=1");
  delay(1000);
  //-----------------------------------------------------------------------------------
}

void loop()
{
  while(sim800L.available()){
  Serial.println(sim800L.readString());
  }
  
  int flame_value = digitalRead(flame_sensor_pin);
  
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //The fire is detected, trigger Alarm and send sms
  if(flame_value == LOW)
  {
    digitalWrite(buzzer_pin,HIGH);
    //----------------------------------------------------------------
    if(fire_flag == 0)
    {
      Serial.println("Fire Detected.");
      fire_flag == 1;
      send_multi_sms();
      make_multi_call();
    }
    //----------------------------------------------------------------
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //No fire is detected, turn OFF Alarm
  else
  {
    digitalWrite(buzzer_pin,LOW);
    fire_flag = 0;
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
}

//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
void send_multi_sms()
{
  if(PHONE_1 != ""){
    Serial.print("Phone 1: ");
    send_sms("Fire is Detected", PHONE_1);
  }
  if(PHONE_2 != ""){
    Serial.print("Phone 2: ");
    send_sms("Fire is Detected", PHONE_2);
  }
  if(PHONE_3 != ""){
    Serial.print("Phone 3: ");
    send_sms("Fire is Detected", PHONE_3);
  }
}
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN

//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
void make_multi_call()
{
  if(PHONE_1 != ""){
    Serial.print("Phone 1: ");
    make_call(PHONE_1);
  }
  if(PHONE_2 != ""){
    Serial.print("Phone 2: ");
    make_call(PHONE_2);
  }
  if(PHONE_3 != ""){
    Serial.print("Phone 3: ");
    make_call(PHONE_3);
  }
}
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN

//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
void send_sms(String text, String phone)
{
    Serial.println("sending sms....");
    delay(50);
    sim800L.print("AT+CMGF=1\r");
    delay(1000);
    sim800L.print("AT+CMGS=\""+phone+"\"\r");
    delay(1000);
    sim800L.print(text);
    delay(100);
    sim800L.write(0x1A); //ascii code for ctrl-26 //Serial2.println((char)26); //ascii code for ctrl-26
    delay(5000);
}
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN

//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
void make_call(String phone)
{
    Serial.println("calling....");
    sim800L.println("ATD"+phone+";");
    delay(20000); //20 sec delay
    sim800L.println("ATH");
    delay(1000); //1 sec delay
}
