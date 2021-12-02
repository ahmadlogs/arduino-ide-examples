/************************************************************************************
 *  Created By: Tauseef Ahmad
 *  Created on: 28 November, 2021
 *  Updated on: 2 Decenmer, 2021
 *  YouTube Video: https://youtu.be/IiheQAMT3kA
 *  My Channel: https://www.youtube.com/channel/UCOXYfOHgu-C-UfGyDcu5sYw/
 *  
 **********************************************************************************/

#include <SoftwareSerial.h>

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
#include <EEPROM.h>
//sender phone number with country code
//const String PHONE = "ENTER_PHONE_HERE";
const int totalPhoneNo = 5;
String phoneNo[totalPhoneNo] = {"","","","",""};
int offsetPhone[totalPhoneNo] = {0,13,26,39,52};
String tempPhone = "";
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

//GSM Module RX pin to Arduino 3
//GSM Module TX pin to Arduino 2
#define rxPin 2
#define txPin 3
SoftwareSerial sim800(rxPin,txPin);
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
#define RELAY_1 7
#define RELAY_2 8
#define RELAY_3 9
#define RELAY_4 10

boolean STATE_RELAY_1 = 0;
boolean STATE_RELAY_2 = 0;
boolean STATE_RELAY_3 = 0;
boolean STATE_RELAY_4 = 0;
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
String smsStatus,senderNumber,receivedDate,msg;
boolean isReply = false;
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

boolean DEBUG_MODE = 1;


/*******************************************************************************
 * setup function
 ******************************************************************************/
void setup() {
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  Serial.begin(115200);
  Serial.println("Arduino serial initialize");
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  sim800.begin(9600);
  Serial.println("SIM800L software serial initialize");
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  pinMode(RELAY_1, OUTPUT); //Relay 1
  pinMode(RELAY_2, OUTPUT); //Relay 2
  pinMode(RELAY_3, OUTPUT); //Relay 3
  pinMode(RELAY_4, OUTPUT); //Relay 4

  digitalWrite(RELAY_1, LOW);
  digitalWrite(RELAY_2, LOW);
  digitalWrite(RELAY_3, LOW);
  digitalWrite(RELAY_4, LOW);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  smsStatus = "";
  senderNumber="";
  receivedDate="";
  msg="";
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  Serial.println("List of Registered Phone Numbers");
  for (int i = 0; i < totalPhoneNo; i++){
    phoneNo[i] = readFromEEPROM(offsetPhone[i]);
    if(phoneNo[i].length() != 13)
      {phoneNo[i] = "";Serial.println(String(i+1)+": empty");}
    else
      {Serial.println(String(i+1)+": "+phoneNo[i]);}
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  delay(7000);
  sim800.print("AT+CMGF=1\r"); //SMS text mode
  delay(1000);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
}




/*******************************************************************************
 * Loop Function
 ******************************************************************************/
void loop() {
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
while(sim800.available()){
  parseData(sim800.readString());
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
while(Serial.available())  {
  sim800.println(Serial.readString());
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
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

  unsigned int len, index;
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //Remove sent "AT Command" from the response string.
  index = buff.indexOf("\r");
  buff.remove(0, index+2);
  buff.trim();
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  
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
      
      //----------------------------------------------------------------------------
      if(msg.equals("r") && phoneNo[0].length() != 13) {
        writeToEEPROM(offsetPhone[0],senderNumber);
        phoneNo[0] = senderNumber;
        String text = "Number is Registered: ";
        text = text + senderNumber;
        debugPrint(text);
        Reply("Number is Registered", senderNumber);
      }
      //----------------------------------------------------------------------------
      if(comparePhone(senderNumber)){
        doAction(senderNumber);
      }
      //----------------------------------------------------------------------------
    }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  }
  else{
  //The result of AT Command is "OK"
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
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

    //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
    String tempcmd = msg.substring(0, 3);
    if(tempcmd.equals("r1=") || tempcmd.equals("r2=") ||
       tempcmd.equals("r3=") || tempcmd.equals("r4=") ||
       tempcmd.equals("r5=")){
        
        tempPhone = msg.substring(3, 16);
        msg = tempcmd;
        //debugPrint(msg);
        //debugPrint(tempPhone);
    }
    //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
       
}




/*******************************************************************************
 * doAction function:
 * Performs action according to the received sms
 ******************************************************************************/
void doAction(String phoneNumber){
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  if(msg == "a1"){  
    digitalWrite(RELAY_1, HIGH);
    STATE_RELAY_1 = 1;
    debugPrint("Relay 1 is ON");
    Reply("Relay 1 is ON", phoneNumber);
  }
  else if(msg == "a0"){
    digitalWrite(RELAY_1, LOW);
    STATE_RELAY_1 = 0;
    debugPrint("Relay 1 is OFF");
    Reply("Relay 1 is OFF", phoneNumber);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  else if(msg == "b1"){  
    digitalWrite(RELAY_2, HIGH);
    STATE_RELAY_2 = 1;
    debugPrint("Relay 2 is ON");
    Reply("Relay 2 is ON", phoneNumber);
  }
  else if(msg == "b0"){
    digitalWrite(RELAY_2, LOW);
    STATE_RELAY_2 = 0;
    debugPrint("Relay 2 is OFF");
    Reply("Relay 2 is OFF", phoneNumber);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  else if(msg == "c1"){  
    digitalWrite(RELAY_3, HIGH);
    STATE_RELAY_3 = 1;
    debugPrint("Relay 3 is ON");
    Reply("Relay 3 is ON", phoneNumber);
  }
  else if(msg == "c0"){
    digitalWrite(RELAY_3, LOW);
    STATE_RELAY_3 = 0;
    debugPrint("Relay 3 is OFF");
    Reply("Relay 3 is OFF", phoneNumber);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  else if(msg == "d1"){  
    digitalWrite(RELAY_4, HIGH);
    STATE_RELAY_4 = 1;
    debugPrint("Relay 4 is ON");
    Reply("Relay 4 is ON", phoneNumber);
  }
  else if(msg == "d0"){
    digitalWrite(RELAY_4, LOW);
    STATE_RELAY_4 = 0;
    debugPrint("Relay 4 is OFF");
    Reply("Relay 4 is OFF", phoneNumber);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  else if(msg == "stat=1"){
    String text = (STATE_RELAY_1)? "ON" : "OFF";
    debugPrint("Relay 1 is "+text);
    Reply("Relay 1 is "+text, phoneNumber);
  }
  else if(msg == "stat=2"){
    String text = (STATE_RELAY_2)? "ON" : "OFF";
    debugPrint("Relay 2 is "+text);
    Reply("Relay 2 is "+text, phoneNumber);
  }
  else if(msg == "stat=3"){
    String text = (STATE_RELAY_3)? "ON" : "OFF";
    debugPrint("Relay 3 is "+text);
    Reply("Relay 3 is "+text, phoneNumber);
  }
  else if(msg == "stat=4"){
    String text = (STATE_RELAY_4)? "ON" : "OFF";
    debugPrint("Relay 4 is "+text);
    Reply("Relay 4 is "+text, phoneNumber);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  else if(msg == "r2="){  
      writeToEEPROM(offsetPhone[1],tempPhone);
      phoneNo[1] = tempPhone;
      String text = "Phone2 is Registered: ";
      text = text + tempPhone;
      debugPrint(text);
      Reply(text, phoneNumber);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  else if(msg == "r3="){  
      writeToEEPROM(offsetPhone[2],tempPhone);
      phoneNo[2] = tempPhone;
      String text = "Phone3 is Registered: ";
      text = text + tempPhone;
      Reply(text, phoneNumber);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  else if(msg == "r4="){  
      writeToEEPROM(offsetPhone[3],tempPhone);
      phoneNo[3] = tempPhone;
      String text = "Phone4 is Registered: ";
      text = text + tempPhone;
      Reply(text, phoneNumber);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  else if(msg == "r5="){  
      writeToEEPROM(offsetPhone[4],tempPhone);
      phoneNo[4] = tempPhone;
      String text = "Phone5 is Registered: ";
      text = text + tempPhone;
      Reply(text, phoneNumber);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  else if(msg == "list"){  
      String text = "";
      if(phoneNo[0])
        text = text + phoneNo[0]+"\r\n";
      if(phoneNo[1])
        text = text + phoneNo[1]+"\r\n";
      if(phoneNo[2])
        text = text + phoneNo[2]+"\r\n";
      if(phoneNo[3])
        text = text + phoneNo[3]+"\r\n";
      if(phoneNo[4])
        text = text + phoneNo[4]+"\r\n";
        
      debugPrint("List of Registered Phone Numbers: \r\n"+text);
      Reply(text, phoneNumber);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  else if(msg == "del=1"){  
      writeToEEPROM(offsetPhone[0],"");
      phoneNo[0] = "";
      Reply("Phone1 is deleted.", phoneNumber);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  else if(msg == "del=2"){  
      writeToEEPROM(offsetPhone[1],"");
      phoneNo[1] = "";
      debugPrint("Phone2 is deleted.");
      Reply("Phone2 is deleted.", phoneNumber);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  else if(msg == "del=3"){  
      writeToEEPROM(offsetPhone[2],"");
      phoneNo[2] = "";
      debugPrint("Phone3 is deleted.");
      Reply("Phone3 is deleted.", phoneNumber);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  else if(msg == "del=4"){  
      writeToEEPROM(offsetPhone[3],"");
      phoneNo[3] = "";
      debugPrint("Phone4 is deleted.");
      Reply("Phone4 is deleted.", phoneNumber);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  else if(msg == "del=5"){  
      writeToEEPROM(offsetPhone[4],"");
      phoneNo[4] = "";
      debugPrint("Phone5 is deleted.");
      Reply("Phone5 is deleted.", phoneNumber);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM  
  if(msg == "del=all"){  
      writeToEEPROM(offsetPhone[0],"");
      writeToEEPROM(offsetPhone[1],"");
      writeToEEPROM(offsetPhone[2],"");
      writeToEEPROM(offsetPhone[3],"");
      writeToEEPROM(offsetPhone[4],"");
      phoneNo[0] = "";
      phoneNo[1] = "";
      phoneNo[2] = "";
      phoneNo[3] = "";
      phoneNo[4] = "";
      offsetPhone[0] = "";
      offsetPhone[1] = "";
      offsetPhone[2] = "";
      offsetPhone[3] = "";
      offsetPhone[4] = "";
      debugPrint("All phone numbers are deleted.");
      Reply("All phone numbers are deleted.", phoneNumber);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  smsStatus = "";
  senderNumber="";
  receivedDate="";
  msg="";
  tempPhone = "";
}





/*******************************************************************************
 * Reply function
 * Send an sms
 ******************************************************************************/
void Reply(String text, String Phone)
{
    sim800.print("AT+CMGF=1\r");
    delay(1000);
    sim800.print("AT+CMGS=\""+Phone+"\"\r");
    delay(1000);
    sim800.print(text);
    delay(100);
    sim800.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
    delay(1000);
    Serial.println("SMS Sent Successfully.");
}





/*******************************************************************************
 * writeToEEPROM function:
 * Store registered phone numbers in EEPROM
 ******************************************************************************/
void writeToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = 13; //strToWrite.length();
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + i, strToWrite[i]);
  }
}






/*******************************************************************************
 * readFromEEPROM function:
 * Store phone numbers in EEPROM
 ******************************************************************************/
String readFromEEPROM(int addrOffset)
{
  int len = 13;
  char data[len + 1];
  for (int i = 0; i < len; i++)
  {
    data[i] = EEPROM.read(addrOffset + i);
  }
  data[len] = '\0';
  return String(data);
}




/*******************************************************************************
 * comparePhone function:
 * compare phone numbers stored in EEPROM
 ******************************************************************************/
boolean comparePhone(String number)
{
  boolean flag = 0;
  //--------------------------------------------------
  for (int i = 0; i < totalPhoneNo; i++){
    phoneNo[i] = readFromEEPROM(offsetPhone[i]);
    if(phoneNo[i].equals(number)){
      flag = 1;
      break;
    }
  }
  //--------------------------------------------------
  return flag;
}





/*******************************************************************************
 * debugPrint function:
 * compare phone numbers stored in EEPROM
 ******************************************************************************/
 void debugPrint(String text){
  if(DEBUG_MODE == 1)
    Serial.println(text);
 }
