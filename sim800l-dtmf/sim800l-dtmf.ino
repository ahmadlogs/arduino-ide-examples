#include <SoftwareSerial.h>

//GSM Module TX pin to Arduino 2
//GSM Module RX pin to Arduino 3
#define rxPin 2
#define txPin 3
SoftwareSerial sim800L(rxPin,txPin);

#define RELAY_1 8
#define RELAY_2 9
boolean relay1_state = false;
boolean relay2_state = false;

//stores incomming data from sim800l
String buff;
String dtmf_cmd;
boolean is_call = false;
 
void setup()
{
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  
  //Begin serial communication with (Serial Monitor)
  Serial.begin(115200);
  
  //Begin serial communication with (SIM800L)
  sim800L.begin(9600);
  Serial.println("Begin serial communication with (SIM800L)");
  delay(7000);

  sim800L.println("AT"); //send AT
  delay(500);

  sim800L.println("AT+DDET=1"); //Enable DTMF
  delay(500);
}

void loop()
{
  while(sim800L.available()){
    buff = sim800L.readString();
    Serial.println(buff);
    
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    if(is_call == true)
    {
      //HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
      if(int index = buff.indexOf("+DTMF:") > -1 )
      {
        index = buff.indexOf(":");
        dtmf_cmd = buff.substring(index+1, buff.length());
        dtmf_cmd.trim();
        Serial.println("dtmf_cmd: "+dtmf_cmd);
        doAction();
      }    
      //HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
      if(buff.indexOf("NO CARRIER") > -1)
      {
        sim800L.println("ATH");
        is_call = false;
      }
      //HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
    }
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    if(buff.indexOf("RING") > -1)
    {
      delay(2000);
      sim800L.println("ATA");
      is_call = true;
    }
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  }

  
  //-------------------------------------------------------
  while(Serial.available())  {
    sim800L.println(Serial.readString());
  }
  //-------------------------------------------------------
}


void doAction(){
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  if(dtmf_cmd == "1"){
    relay1_state = !relay1_state;  
    digitalWrite(RELAY_1, relay1_state);
    if(relay1_state == true)
      {Serial.println("Relay 1 has been ON");}
    else
      {Serial.println("Relay 1 has been OFF");}
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  else if(dtmf_cmd == "2"){
    relay2_state = !relay2_state;  
    digitalWrite(RELAY_2, relay2_state);
    if(relay2_state == true)
      {Serial.println("Relay 2 has been ON");}
    else
      {Serial.println("Relay 2 has been OFF");}
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
}

/*
AT
Check Sim is ready or not
AT+CPIN?
 */
