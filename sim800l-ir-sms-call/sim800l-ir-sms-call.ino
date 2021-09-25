/*
 * ******************************************************************
 * Created By: Tauseef Ahmad
 * Created On: September 25, 2021
 * Tutorial Link: https://youtu.be/uLJqNfX81C4
 * My Channel: https://www.youtube.com/channel/UCOXYfOHgu-C-UfGyDcu5sYw/
 * 
 * ******************************************************************
 * IR Remote Version 3.3.0
 * https://github.com/Arduino-IRremote/Arduino-IRremote
 * ******************************************************************
 */

#include <SoftwareSerial.h>
#include <IRremote.h>


//sender phone number with country code
const String PHONE = "ENTER_YOUR_PERSONAL_PHONE_NUMBER";

String smsStatus,senderNumber,receivedDate,msg,buff;
boolean isReply = false;
String dtmf_cmd;
boolean is_call = false;


#define rxPin 2
#define txPin 3
SoftwareSerial sim800L(rxPin,txPin); 

const byte IR_RECEIVE_PIN = 4;

#define button1_pin 5
#define button2_pin 6
#define button3_pin 7
#define button4_pin 8

#define relay1_pin 9
#define relay2_pin 10
#define relay3_pin 11
#define relay4_pin 12

boolean relay1_state = 0;
boolean relay2_state = 0;
boolean relay3_state = 0;
boolean relay4_state = 0;

void setup()
{
  pinMode(button1_pin, INPUT_PULLUP);
  pinMode(button2_pin, INPUT_PULLUP);
  pinMode(button3_pin, INPUT_PULLUP);
  pinMode(button4_pin, INPUT_PULLUP);
    
  pinMode(relay1_pin, OUTPUT);
  pinMode(relay2_pin, OUTPUT);
  pinMode(relay3_pin, OUTPUT);
  pinMode(relay4_pin, OUTPUT);

 Serial.begin(115200);
 
 Serial.println("IR Receive test");
 IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

 sim800L.begin(9600);
 sim800L.print("AT+CMGF=1\r");
 delay(1000);

  sim800L.println("AT+DDET=1"); //Enable DTMF
  delay(500);

  
  smsStatus = "";
  senderNumber="";
  receivedDate="";
  msg="";
   
}

void loop()
{
  //****************************************
  while(sim800L.available()){
    buff = sim800L.readString();
    handle_sim800_response();
  }
  //****************************************
  while(Serial.available())  {
    sim800L.println(Serial.readString());
  }
  //****************************************

  listen_ir();

  listen_push_buttons();

}

void handle_sim800_response()
{
  Serial.println(buff);
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    if(is_call == true){
      //HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
      if(int index = buff.indexOf("+DTMF:") > -1 ){
        index = buff.indexOf(":");
        dtmf_cmd = buff.substring(index+1, buff.length());
        dtmf_cmd.trim();
        Serial.println("dtmf_cmd: "+dtmf_cmd);

        if(dtmf_cmd == "1")      control_relay(1);
        else if(dtmf_cmd == "2") control_relay(2);
        else if(dtmf_cmd == "3") control_relay(3);
        else if(dtmf_cmd == "4") control_relay(4);
      }    
      //HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
      if(buff.indexOf("NO CARRIER") > -1){
        sim800L.println("ATH");
        is_call = false;
      }
      //HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
    }
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    else if(buff.indexOf("RING") > -1)
    {
      delay(2000);
      sim800L.println("ATA");
      is_call = true;
    }
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    else if(buff.indexOf("+CMTI") > -1)
    {
        //get newly arrived sms memory location and store it in temp
        unsigned int index = buff.indexOf(",");
        String temp = buff.substring(index+1, buff.length()); 
        temp = "AT+CMGR=" + temp + "\r"; 
        //get the message stored at memory location "temp"
        sim800L.println(temp);
    }
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM         
    else if(buff.indexOf("+CMGR") > -1){
      extractSms();
      
      if(msg == "1")      control_relay(1);
      else if(msg == "2") control_relay(2);
      else if(msg == "3") control_relay(3);
      else if(msg == "4") control_relay(4);
      else if(msg.indexOf(".status") > -1) send_relay_status(msg.substring(0,1));
      else if(msg.indexOf("del all") > -1) delete_all_sms();
    }
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM 
}


void extractSms(){
     unsigned int len, index;
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    //Remove sent "AT Command" from the response string.
    index = buff.indexOf("\r");
    buff.remove(0, index+2);
    buff.trim();
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    //Remove response "AT Command" from the response string.
    index = buff.indexOf(":");
    buff.substring(0, index);
    buff.remove(0, index+2);
   //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
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

void listen_ir()
{
   if (IrReceiver.decode())
   {
      String ir_code = String(IrReceiver.decodedIRData.command, HEX);
      if(ir_code != "0"){Serial.println(ir_code);}

      if(ir_code == "2")      {delay(200);control_relay(1);}
      else if(ir_code == "3") {delay(200);control_relay(2);}
      else if(ir_code == "4") {delay(200);control_relay(3);}
      else if(ir_code == "5") {delay(200);control_relay(4);}
      
      IrReceiver.resume();
   }
}

void listen_push_buttons(){
    if(digitalRead(button1_pin) == LOW){
      delay(200);
      control_relay(1);
    }
    else if (digitalRead(button2_pin) == LOW){
      delay(200);
      control_relay(2);
    }
    else if (digitalRead(button3_pin) == LOW){
      delay(200);
      control_relay(3);
    }
    else if (digitalRead(button4_pin) == LOW){
      delay(200);
      control_relay(4);
    }
}
void control_relay(int relay){
  //------------------------------------------------
  if(relay == 1){
    relay1_state = !relay1_state;
    digitalWrite(relay1_pin, relay1_state);
    Serial.print("RelayState = ");
    Serial.println(relay1_state);
    delay(50);
  }
  //------------------------------------------------
  else if(relay == 2){
    relay2_state = !relay2_state;
    digitalWrite(relay2_pin, relay2_state);
    delay(50);
  }
  //------------------------------------------------
  else if(relay == 3){
    relay3_state = !relay3_state;
    digitalWrite(relay3_pin, relay3_state);
    delay(50);
  }
  //------------------------------------------------
  else if(relay == 4){
    relay4_state = !relay4_state;
    digitalWrite(relay4_pin, relay4_state);
    delay(50);
  }
  //------------------------------------------------
}

void send_relay_status(String relay)
{
  Serial.println("Relay Number: "+relay);
  //(relay1_state):"ON"?"OFF"
  String sms_text = "";
  //------------------------------------------------
  if(relay == "1"){
    sms_text = (relay1_state) ? "ON" : "OFF";
    sms_text = "Relay 1 is " + sms_text;
  }
  //------------------------------------------------
  else if(relay == "2"){
    sms_text = (relay2_state) ? "ON" : "OFF";
    sms_text = "Relay 2 is " + sms_text;
  }
  //------------------------------------------------
  else if(relay == "3"){
    sms_text = (relay3_state) ? "ON" : "OFF";
    sms_text = "Relay 3 is " + sms_text;
  }
  //------------------------------------------------
  else if(relay == "4"){
    sms_text = (relay4_state) ? "ON" : "OFF";
    sms_text = "Relay 4 is " + sms_text;
  }
  //------------------------------------------------  
  Reply(sms_text);
}

void delete_all_sms()
{
  sim800L.println("AT+CMGD=1,4");
  delay(5000);
  while(sim800L.available()){
    String response = sim800L.readString();
    if(response.indexOf("OK") > -1 ){
      Reply("All sms are deleted");
    } else {
      Reply(response);
    }
  }
}


void Reply(String text)
{
  Serial.println(text);
    sim800L.print("AT+CMGF=1\r");
    delay(1000);
    sim800L.print("AT+CMGS=\""+PHONE+"\"\r");
    delay(1000);
    sim800L.print(text);
    delay(100);
    sim800L.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
    delay(1000);
    Serial.println("SMS Sent Successfully.");

    msg = "";
}
