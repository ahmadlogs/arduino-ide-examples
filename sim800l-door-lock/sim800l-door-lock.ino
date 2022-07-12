 /************************************************************************************
 *  Created By: Tauseef Ahmad
 *  Created On: 26 June, 2022
 *  
 *  YouTube Video: https://youtu.be/iESO8ae9cA0
 *  GitHub Account: https://www.github.com/ahmadlogs
 *  Arduino IDE: 1.8.20
 **********************************************************************************/
//------------------------------------------------------------------------------

#include <SoftwareSerial.h>
//---------------------------------------------------------
//Sender phone number with country code.
//Don't enter the phone number of the 
//sim card inserted into SIM800L
const String PHONE = "ENTER_PHONE_WITH_COUNTRY_CODE";
//---------------------------------------------------------
long unlock_duration      = 20000; //20 seconds
boolean lock_status       = false;
unsigned long lock_timer  = 0;
//---------------------------------------------------------
//GSM Module RX pin to Arduino 3
//GSM Module TX pin to Arduino 2
#define rxPin 2
#define txPin 3
SoftwareSerial sim800(rxPin,txPin);
//---------------------------------------------------------
#define RELAY_1 4
//---------------------------------------------------------
String smsStatus,senderNumber,receivedDate,msg;
//---------------------------------------------------------


/****************************************************************************************************
 * setup Function
*****************************************************************************************************/
void setup() {
  //---------------------------------------------------------
  Serial.begin(9600);
  Serial.println("Arduino serial initialize");
  //---------------------------------------------------------
  sim800.begin(9600);
  Serial.println("SIM800L software serial initialize");
  //---------------------------------------------------------
  pinMode(RELAY_1, OUTPUT); //Relay 1
  digitalWrite(RELAY_1, HIGH);
  //delay(7000);
  //---------------------------------------------------------
  smsStatus = "";
  senderNumber="";
  receivedDate="";
  msg="";
  //---------------------------------------------------------
  sim800.println("ATE"); //Echo ON
  delay(1000);
  sim800.println("AT+CMGF=1"); //SMS text mode
  delay(1000);
  //delete all sms from sim800l
  delAll();
  //---------------------------------------------------------
}


/****************************************************************************************************
 * make_call Function
*****************************************************************************************************/
void loop() {
  //---------------------------------------------------------
  while(sim800.available()){
    parseData(sim800.readString());
  }
  //---------------------------------------------------------
  while(Serial.available())  {
    sim800.println(Serial.readString());
  }
  //---------------------------------------------------------
  if (lock_status == true) {
    if (millis() - lock_timer > unlock_duration) {
      digitalWrite(RELAY_1, HIGH);
      lock_status = false;
      lock_timer = 0;
      Reply("Door is locked now");
    }
  }
  //---------------------------------------------------------
} //main loop ends


/****************************************************************************************************
 * parseData Function
*****************************************************************************************************/
void parseData(String buff){
  Serial.println(buff);

  unsigned int len, index;
  //---------------------------------------------------------
  //Remove sent "AT Command" from the response string.
  index = buff.indexOf("\r");
  buff.remove(0, index+2);
  buff.trim();
  //---------------------------------------------------------
  if(buff != "OK"){
    index = buff.indexOf(":");
    String cmd = buff.substring(0, index);
    cmd.trim();
    buff.remove(0, index+2);
    //-----------------------------------------------------------------
    if(cmd == "+CMTI"){
      //get newly arrived memory location and store it in temp
      index = buff.indexOf(",");
      String temp = buff.substring(index+1, buff.length()); 
      temp = "AT+CMGR=" + temp + "\r"; 
      //get the message stored at memory location "temp"
      sim800.println(temp); 
    }
    //______________________________________________________
    else if(cmd == "+CMGR"){
      extractSms(buff);
      if(senderNumber == PHONE){
        doAction();
        //delete all sms
        delAll();
      }
      else{ Serial.println("Phone number is not registered");}
    }
    //______________________________________________________
  }
  //-----------------------------------------------------------------
}


/****************************************************************************************************
 * extractSms Function
*****************************************************************************************************/
void extractSms(String buff){
   unsigned int index;
   
    index = buff.indexOf(",");
    smsStatus = buff.substring(1, index-1); 
    buff.remove(0, index+2);
    
    senderNumber = buff.substring(0, 13);
    buff.remove(0,19);
   
    receivedDate = buff.substring(0, 20);
    buff.remove(0,buff.indexOf("\r"));
    buff.trim();
    
    index =buff.indexOf("\n\r");
    buff = buff.substring(0, index);
    buff.trim();
    msg = buff;
    buff = "";
    msg.toLowerCase();
}


/****************************************************************************************************
 * doAction Function
*****************************************************************************************************/
void doAction(){
  if(msg == "unlock"){
    digitalWrite(RELAY_1, LOW);
    lock_status = true;
    lock_timer = millis();
    Reply("Door is unlocked for "+String(unlock_duration/1000)+" seconds");
  }
  else{ Reply("Invalid command"); }
    
  smsStatus = "";
  senderNumber="";
  receivedDate="";
  msg="";  
}


/****************************************************************************************************
 * Reply Function
*****************************************************************************************************/
void Reply(String text){
  text.trim();
  Serial.println(text);
  //return;
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\""+PHONE+"\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
  delay(1000);
  Serial.println("Sending SMS....");
}

/****************************************************************************************************
 * Delete all SMS Function
*****************************************************************************************************/
void delAll(){
  //return;
  sim800.println("AT+CMGD=1,4");
  delay(1000);
  sim800.println("AT+CMGDA= \"DEL ALL\"");
  delay(1000);
}
