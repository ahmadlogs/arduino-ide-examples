#include <SoftwareSerial.h>
//------------------------------------------------
//GSM Module RX pin to Arduino 3
//GSM Module TX pin to Arduino 2
#define rxPin 2
#define txPin 3
SoftwareSerial sim800(rxPin,txPin);
//------------------------------------------------
#define RELAY_1 7
boolean STATE_RELAY_1 = 0;
//------------------------------------------------
String smsStatus,senderNumber,receivedDate,msg;
//------------------------------------------------
const unsigned long OneMinute = 60UL * 1000UL;
const unsigned long OneHour = 60UL * OneMinute;
const unsigned long TwentyFourHours = 24 * OneHour;
//The relay will turn OFF after stopTime end.
//stopTime can be set by sms.
//set the default stopTime to 1 hour
unsigned long stopTime = 10 * OneMinute;
unsigned long startTime = 0;
//------------------------------------------------


/*******************************************************************************
 * setup function
 ******************************************************************************/
void setup() {
  //--------------------------------------------------
  Serial.begin(9600);
  Serial.println("Arduino serial initialize");
  //--------------------------------------------------
  sim800.begin(9600);
  Serial.println("SIM800L software serial initialize");
  //--------------------------------------------------
  pinMode(RELAY_1, OUTPUT); //Relay 1

  digitalWrite(RELAY_1, LOW);
  //--------------------------------------------------
  smsStatus = "";
  senderNumber="";
  receivedDate="";
  msg="";
  //--------------------------------------------------
  delay(7000);
  sim800.print("ATE1\r\n"); //ECHO ON
  delay(500);
  //set up the fona to send a +CMTI notification when an SMS is received
  sim800.print("AT+CNMI=2,1\r\n");
  delay(500);
  sim800.print("AT+CMGF=1\r\n"); //SMS text mode
  delay(500);
  sim800.print("AT+CLIP=1\r\n"); //Enable Caller ID
  delay(500);
  //--------------------------------------------------
}




/*******************************************************************************
 * Loop Function
 ******************************************************************************/
void loop() {
//--------------------------------------------------
while(sim800.available()){
  parseData(sim800.readString());
}
//--------------------------------------------------
while(Serial.available())  {
  sim800.println(Serial.readString());
}
//--------------------------------------------------
//check if the relay is ON
if(STATE_RELAY_1 == 1 and startTime > 0){
  unsigned long currentMillis = millis();
  //____________________________________________
  if(currentMillis - startTime >= stopTime) {
    STATE_RELAY_1 = 0;
    digitalWrite(RELAY_1, LOW);
    startTime = 0;
  }
  //____________________________________________
}
//--------------------------------------------------

} //main loop ends





/*******************************************************************************
 * parseData function:
 * this function parse the incomming command such as CMTI or CMGR etc.
 * if the sms is received. then this function read that sms and then pass 
 * that sms to "extractSms" function. Then "extractSms" function divide the
 * sms into parts. such as sender_phone, sms_body, received_date etc.
 ******************************************************************************/
void parseData(String buff){
  Serial.println(buff);
  //---------------------------------------------------------------------
  if(buff.indexOf("RING") > -1)
  {
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    //+CLIP: "03001234567",129,"",0,"",0
    if(buff.indexOf("+CLIP:")){
     unsigned int index, index1;
      index = buff.indexOf("\"");
      index1 = buff.indexOf("\"", index+1);
      //get phone like this format 03001234567
      senderNumber = buff.substring(index+1, index1);
      senderNumber.trim();
      Serial.println("Caller ID: "+senderNumber);
    }
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    //terminate the call
    sim800.println("ATH");
    delay(100);
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    //turn on the relay1
    if(STATE_RELAY_1 == 0){
      STATE_RELAY_1 = 1;
      digitalWrite(RELAY_1, HIGH);
      startTime = millis();
      Serial.println("Relay 1 is ON");
      Reply("Relay 1 is ON", senderNumber);
    }
    //turn off the relay1
    else {
      STATE_RELAY_1 = 0;
      digitalWrite(RELAY_1, LOW);
      startTime = 0;
      Serial.println("Relay 1 is OFF");
      Reply("Relay 1 is OFF", senderNumber);
    }
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    
    return;
  }
  //---------------------------------------------------------------------
  unsigned int len, index;
  //---------------------------------------------------------------------
  //Remove sent "AT Command" from the response string.
  index = buff.indexOf("\r");
  buff.remove(0, index+2);
  buff.trim();
  //---------------------------------------------------------------------
  //do nothing
  if(buff == "OK")
    {return;}
  //---------------------------------------------------------------------
  index = buff.indexOf(":");
  String cmd = buff.substring(0, index);
  cmd.trim();
  
  buff.remove(0, index+2);
  //---------------------------------------------------------------------
  if(cmd == "+CMTI"){
    //get newly arrived memory location and store it in temp
    index = buff.indexOf(",");
    String temp = buff.substring(index+1, buff.length()); 
    temp = "AT+CMGR=" + temp + "\r"; 
    //get the message stored at memory location "temp"
    sim800.println(temp);
  }
  //---------------------------------------------------------------------
  else if(cmd == "+CMGR"){
    extractSms(buff);
    doAction(senderNumber);
  }
  //---------------------------------------------------------------------
}





/*******************************************************************************
 * extractSms function:
 * This function divide the sms into parts. such as sender_phone, sms_body, 
 * received_date etc.
 ******************************************************************************/
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




/*******************************************************************************
 * doAction function:
 * Performs action according to the received sms
 ******************************************************************************/
void doAction(String phoneNumber){
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  if(msg == "1on"){  
    digitalWrite(RELAY_1, HIGH);
    startTime = millis();
    STATE_RELAY_1 = 1;
    Serial.println("Relay 1 is ON");
    Reply("Relay 1 is ON", phoneNumber);
  }
  else if(msg == "1off"){
    digitalWrite(RELAY_1, LOW);
    startTime = 0;
    STATE_RELAY_1 = 0;
    Serial.println("Relay 1 is OFF");
    Reply("Relay 1 is OFF", phoneNumber);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //dr=50  (50 minutes)
  String temp_msg = msg.substring(0, 3);
  if(temp_msg.equals("dr=")){
    String temp_duration = msg.substring(3, msg.length());
    long duration = temp_duration.toInt();
    duration = duration * OneMinute;
    //-------------------------------------
    if(duration > OneMinute){
      if(duration > TwentyFourHours)
        {stopTime = TwentyFourHours;}
      stopTime = duration;
      String reply_msg = "Relay Stop Timer is set: "+String(stopTime/60000)+" Minutes";
      Serial.println(reply_msg);
      Reply(reply_msg, phoneNumber);
    }
    //-------------------------------------
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
}




/*******************************************************************************
 * Reply function
 * Send an sms
 ******************************************************************************/
void Reply(String text, String Phone)
{
    //return;
    sim800.print("AT+CMGF=1\r");
    delay(1000);
    sim800.print("AT+CMGS=\""+Phone+"\"\r");
    delay(1000);
    sim800.print(text);
    delay(100);
    sim800.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
    delay(1000);
    Serial.println("SMS Sent Successfully.");

    /*************************************************************
     * only upto 70 messages can be stored in sim800l memory
     * after the memory is full, then no new sms will be received
     * until you free up the sms memory by deleting some sms.
     * that's why the below written command deletes all the sms
     * from the memory automatically. 
     *************************************************************/
    //sim800.print("AT+CMGD=1,4");
}
