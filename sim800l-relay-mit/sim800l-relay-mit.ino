#include<SoftwareSerial.h>

//sender phone number with country code
const String PHONE = "+92300123456789";

//GSM Module RX pin to Arduino 3
//GSM Module TX pin to Arduino 2
#define rxPin 2
#define txPin 3
SoftwareSerial sim800(rxPin,txPin);

#define RELAY_1 7
#define RELAY_2 8

String smsStatus,senderNumber,receivedDate,msg;

//if true then send "ussd code response message" 
//to the above mension phone number
//used in parseData() function
boolean ussdCmd = false;

//not implemented yet
boolean isReply = false;

void setup() {
  digitalWrite(RELAY_1, HIGH);
  digitalWrite(RELAY_2, HIGH);
  delay(7000);
  
  Serial.begin(115200);
  Serial.println("Arduino serial initialize");
  
  sim800.begin(9600);
  Serial.println("SIM800L software serial initialize");

  pinMode(RELAY_1, OUTPUT); //Relay 1
  pinMode(RELAY_2, OUTPUT); //Relay 2

  smsStatus = "";
  senderNumber="";
  receivedDate="";
  msg="";

  sim800.print("AT+CMGF=1\r"); //SMS text mode
  delay(1000);
}

void loop() {
//////////////////////////////////////////////////
while(sim800.available()){
  parseData(sim800.readString());
}
//////////////////////////////////////////////////
while(Serial.available())  {
  sim800.println(Serial.readString());
}
//////////////////////////////////////////////////
} //main loop ends

//***************************************************
void parseData(String buff){
  Serial.println(buff);

  unsigned int len, index;
  //////////////////////////////////////////////////
  //Remove sent "AT Command" from the response string.
  index = buff.indexOf("\r");
  buff.remove(0, index+2);
  buff.trim();
  //////////////////////////////////////////////////
  
  //////////////////////////////////////////////////
  if(buff != "OK"){
    index = buff.indexOf(":");
    String cmd = buff.substring(0, index);
    cmd.trim();
    
    buff.remove(0, index+2);
    
    if(cmd == "+CMTI"){
      //get newly arrived memory location and store it in temp
      index = buff.indexOf(",");
      String temp = buff.substring(index+1, buff.length()); 
      temp = "AT+CMGR=" + temp + "\r"; 
      //get the message stored at memory location "temp"
      sim800.println(temp); 
    }
    else if(cmd == "+CMGR"){
      extractSms(buff);
      
      //if received sms have at command
      if(msg.startsWith("at+") && senderNumber == PHONE){
        //if received sms have ussd code
        if(msg.startsWith("at+cusd=1"))
          {ussdCmd = true;Serial.println("startsWith");sim800.println(msg);}
        //if received sms have delete command 
        else if(msg.startsWith("at+cmgd"))
          {sendRseponseATcommand(msg, true);}
        else 
          {sendRseponseATcommand(msg, false);}
      }
      else if(senderNumber == PHONE){
          doAction();
      }
      else if(ussdCmd == true){
        ussdCmd = false;
        Serial.println("startsWith1");
        Reply(msg);
      }

    }
  //////////////////////////////////////////////////
  }
  else{
  //The result of AT Command is "OK"
  }
}

//************************************************************
void sendRseponseATcommand(String ATcommand, boolean deleted){

    if (ATcommand.equals("")){
      return;
    }
    
    uint8_t answer=0;
    unsigned int timeout = 1000;
    String response;
    unsigned long previous;

    delay(100);
    
    //Clean the input buffer
    while(sim800.available() > 0) sim800.read();

    //Send the AT command 
    sim800.println(ATcommand);
    
    previous = millis();

    //this loop waits for the answer with time out
    do{
        //if there are data in the UART input buffer, reads it.
        if(sim800.available() != 0){
          response = sim800.readString();
          
          if(response != "")
            answer == 1;
        }
    }while((answer == 0) && ((millis() - previous) < timeout));

  //Remove sent "AT Command" from the response string.
  int index = response.indexOf("\r");
  response.remove(0, index+2);
  response.trim();
  //Remove OK from the response string.
  index =response.indexOf("\n\r");
  response = response.substring(0, index);
  response.trim();
  Serial.println("---------------------");
  Serial.println(response);
  Serial.println("---------------------");
  
  if(deleted == true)
    {Reply("Delete: "+response);}
  //if received sms have delete command 
  else
    {Reply(response);}

}
//************************************************************

//************************************************************
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

void doAction(){
  if(msg == "relay1 off"){  
    digitalWrite(RELAY_1, HIGH);
    Reply("Relay 1 has been OFF");
  }
  else if(msg == "relay1 on"){
    digitalWrite(RELAY_1, LOW);
    Reply("Relay 1 has been ON");
  }
  else if(msg == "relay2 off"){
    digitalWrite(RELAY_2, HIGH);
    Reply("Relay 2 has been OFF");
  }
  else if(msg == "relay2 on"){
    digitalWrite(RELAY_2, LOW);
    Reply("Relay 2 has been ON");
  }

  
  smsStatus = "";
  senderNumber="";
  receivedDate="";
  msg="";  
}

void Reply(String text)
{
    sim800.print("AT+CMGF=1\r");
    delay(1000);
    sim800.print("AT+CMGS=\""+PHONE+"\"\r");
    delay(1000);
    sim800.print(text);
    delay(100);
    sim800.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
    delay(1000);
    Serial.println("SMS Sent Successfully.");
}
