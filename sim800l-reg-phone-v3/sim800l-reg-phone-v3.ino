#include <SoftwareSerial.h>
#include <EEPROM.h>

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
const int totalPhoneNo = 5;
const byte phoneLength = 13;
//total memory consumed by phone numbers in EEPROM
const byte totalMemory = (phoneLength * 5) - 1;
//set starting address of each phone number in EEPROM
int offsetPhone[totalPhoneNo] = {0,phoneLength,phoneLength*2,phoneLength*3,phoneLength*4};
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

//GSM Module RX pin to Arduino 3
//GSM Module TX pin to Arduino 2
#define rxPin 2
#define txPin 3
#define RESET_PIN 4
SoftwareSerial sim800(rxPin,txPin);
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
#define BUTTON_1 9
#define BUTTON_2 10
#define BUTTON_3 11
#define BUTTON_4 12

#define RELAY_1 5
#define RELAY_2 6
#define RELAY_3 7
#define RELAY_4 8

boolean STATE_RELAY_1 = 0;
boolean STATE_RELAY_2 = 0;
boolean STATE_RELAY_3 = 0;
boolean STATE_RELAY_4 = 0;
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
boolean isReply = false;
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

/*******************************************************************************
 * getResponse function
 ******************************************************************************/
boolean getResponse(String expected_answer, unsigned int timeout=1000,boolean reset=false){
  boolean flag = false;
  String response = "";
  unsigned long previous;
  //*************************************************************
  for(previous=millis(); (millis() - previous) < timeout;){
    while(sim800.available()){
      response = sim800.readString();
      //----------------------------------------
      //Used in resetSIM800L function
      //If there is some response data
      if(response != ""){
        Serial.println(response);
        if(reset == true)
          return true;
      }
      //----------------------------------------
      if(response.indexOf(expected_answer) > -1){
        return true;
      }
    }
  }
  //*************************************************************
  return false;
}

/*******************************************************************************
 * resetSIM800L function
 ******************************************************************************/
boolean resetSIM800L(){
  digitalWrite(RESET_PIN, LOW);
  delay(100);
  digitalWrite(RESET_PIN, HIGH);
  
  boolean flag = false;
  for(int i=1; i<=20; i++){
    sim800.println("ATE");
    Serial.print(".");
    if(getResponse("NULL", 1000, true) == true)
      {flag = true; break;}
  }
  sim800.println("ATE");
  if(flag == true){
    for(int i=1; i<=20; i++){
      if(getResponse("SMS Ready", 15000) == true)
        {return true;}
    }
  }
  return false;
}

/*******************************************************************************
 * setup function
 ******************************************************************************/
void setup() {
  //-----------------------------------------------------
  Serial.begin(9600);
  Serial.println("Arduino serial initialize");
  //-----------------------------------------------------
  sim800.begin(9600);
  Serial.println("SIM800L software serial initialize");
  //-----------------------------------------------------
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);
  pinMode(BUTTON_3, INPUT_PULLUP);
  pinMode(BUTTON_4, INPUT_PULLUP);
  
  pinMode(RELAY_1, OUTPUT); //Relay 1
  pinMode(RELAY_2, OUTPUT); //Relay 2
  pinMode(RELAY_3, OUTPUT); //Relay 3
  pinMode(RELAY_4, OUTPUT); //Relay 4

  digitalWrite(RELAY_1, LOW);
  digitalWrite(RELAY_2, LOW);
  digitalWrite(RELAY_3, LOW);
  digitalWrite(RELAY_4, LOW);
  
  pinMode(RESET_PIN, OUTPUT);
  delay(100);
  //-------------------------------------------------------------------
  LoadStateEEPROM();
  //-------------------------------------------------------------------
  if(resetSIM800L() == false)
  { Serial.println("SIM800L Not Ready"); }
  sim800.println("AT");
  getResponse("OK",1000);
  sim800.println("AT+CMGF=1");
  getResponse("OK",1000);
  sim800.println("AT+CNMI=1,2,0,0,0");
  getResponse("OK",1000);
  //-------------------------------------------------------------------
  Serial.println(GetRegisteredPhoneNumbersList());
  //-------------------------------------------------------------------
}


/*******************************************************************************
 * Loop Function
 ******************************************************************************/
void loop() {
//------------------------------------------------------------------------------
while(sim800.available()){
  String response = sim800.readString();
  Serial.println(response);
  //__________________________________________________________________________
  //if there is an incoming SMS
  if(response.indexOf("+CMT:") > -1){
    String callerID = getCallerID(response);
    String cmd = getMsgContent(response);
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    //this if statement will execute only if sim800l received "r" command 
    //and there will be no any phone number stored in the EEPROM
    if(cmd.equals("r")){
      String admin_phone = readFromEEPROM(offsetPhone[0]);
      if(admin_phone.length() != phoneLength){
        RegisterPhoneNumber(1, callerID, callerID);
        break;
      }
      else {
        String text = "Error: Admin phone number is failed to register";
        Serial.println(text);
        Reply(text, callerID);
        break;
      }
    }
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    //The action is performed only if the caller ID will matched with
    //any one of the phone numbers stored in the eeprom.
    if(comparePhone(callerID)){
      doAction(cmd, callerID);
    }
    else {
      String text = "Error: Please register your phone number first";
      Serial.println(text);
      Reply(text, callerID);
    }
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  }
  //__________________________________________________________________________
}
//------------------------------------------------------------------------------
while(Serial.available())  {
  String response = Serial.readString();
  if(response.indexOf("clear") > -1){
    Serial.println(response);
    DeletePhoneNumberList();
    Serial.println(GetRegisteredPhoneNumbersList());
    break;
  }
  sim800.println(response);
}
//------------------------------------------------------------------------------
listen_push_buttons();
//------------------------------------------------------------------------------
} //main loop ends

/*******************************************************************************
 * GetRegisteredPhoneNumbersList function:
 ******************************************************************************/
String GetRegisteredPhoneNumbersList(){
  String text = "Registered Numbers List: \r\n";
  String temp = "";
  for(int i = 0; i < totalPhoneNo; i++){
    temp = readFromEEPROM(offsetPhone[i]);
    if(temp == "")
      { text = text + String(i+1)+". Empty\r\n"; }
    else if(temp.length() != phoneLength)
      { text = text + String(i+1)+". Wrong Format\r\n"; }
    else
      {text = text + String(i+1)+". "+temp+"\r\n";}
  }

  return text;
}
  
/*******************************************************************************
 * RegisterPhoneNumber function:
 ******************************************************************************/
void RegisterPhoneNumber(int index, String eeprom_phone, String caller_id){
  if(eeprom_phone.length() == phoneLength){
    writeToEEPROM(offsetPhone[index-1],eeprom_phone);
    String text = "Phone"+String(index)+" is Registered: ";
    //text = text + phoneNumber;
    Serial.println(text);
    Reply(text, caller_id);
  }
  else {
    String text = "Error: Phone number must be "+String(phoneLength)+" digits long";
    Serial.println(text);
    Reply(text, caller_id);
  }
}

/*******************************************************************************
 * UnRegisterPhoneNumber function:
 ******************************************************************************/
void DeletePhoneNumber(int index, String caller_id){
  writeToEEPROM(offsetPhone[index-1],"");
  String text = "Phone"+String(index)+" is deleted";
  Serial.println(text);
  Reply(text, caller_id);
}
/*******************************************************************************
 * DeletePhoneNumberList function:
 ******************************************************************************/
void DeletePhoneNumberList(){
  for (int i = 0; i < totalPhoneNo; i++){
    writeToEEPROM(offsetPhone[i],"");
  }
}
  
/*******************************************************************************
 * doAction function:
 * Performs action according to the received sms
 ******************************************************************************/
void doAction(String cmd, String caller_id){
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  if(cmd == "1on"){
    controlRelayGSM(1, RELAY_1,STATE_RELAY_1 = HIGH,caller_id);
  }
  else if(cmd == "1off"){
    controlRelayGSM(1, RELAY_1,STATE_RELAY_1 = LOW,caller_id);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  else if(cmd == "2on"){  
    controlRelayGSM(2, RELAY_2,HIGH,caller_id);
    STATE_RELAY_2 = HIGH;
  }
  else if(cmd == "2off"){
    controlRelayGSM(2, RELAY_2,LOW,caller_id);
    STATE_RELAY_2 = LOW;
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  else if(cmd == "3on"){  
    controlRelayGSM(3, RELAY_3,HIGH,caller_id);
    STATE_RELAY_3 = HIGH;
  }
  else if(cmd == "3off"){
    controlRelayGSM(3, RELAY_3,LOW,caller_id);
    STATE_RELAY_3 = LOW;
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  else if(cmd == "4on"){  
    controlRelayGSM(4, RELAY_4,HIGH,caller_id);
    STATE_RELAY_4 = HIGH;
  }
  else if(cmd == "4off"){
    controlRelayGSM(4, RELAY_4,LOW,caller_id);
    STATE_RELAY_4 = LOW;
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  else if(cmd == "stat=1"){
    sendStatus(STATE_RELAY_1,caller_id);
  }
  else if(cmd == "stat=2"){
    sendStatus(STATE_RELAY_2,caller_id);
  }
  else if(cmd == "stat=3"){
    sendStatus(STATE_RELAY_3,caller_id);
  }
  else if(cmd == "stat=4"){
    sendStatus(STATE_RELAY_4,caller_id);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  else if(cmd.indexOf("r2=") > -1){
    RegisterPhoneNumber(2, getNumber(cmd), caller_id);
  }
  else if(cmd.indexOf("r3=") > -1){
    RegisterPhoneNumber(3, getNumber(cmd), caller_id);
  }
  else if(cmd.indexOf("r4=") > -1){ 
    RegisterPhoneNumber(4, getNumber(cmd), caller_id);
  }
  else if(cmd.indexOf("r5=") > -1){
    RegisterPhoneNumber(5, getNumber(cmd), caller_id);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  else if(cmd == "list"){
    String list = GetRegisteredPhoneNumbersList();
    Serial.println(list);
    Reply(list, caller_id);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //else if(cmd == "del=1"){  
    //DeletePhoneNumber(1, caller_id);
  //}
  else if(cmd == "del=2"){  
    DeletePhoneNumber(2, caller_id);
  }
  else if(cmd == "del=3"){  
    DeletePhoneNumber(3, caller_id);
  }
  else if(cmd == "del=4"){  
    DeletePhoneNumber(4, caller_id);
  }
  else if(cmd == "del=5"){  
    DeletePhoneNumber(5, caller_id);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  else if(cmd == "del=all"){
    DeletePhoneNumberList();
    String text = "All the phone numbers are deleted.";
    Serial.println(text);
    Reply(text, caller_id);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  else{
    String text = "Error: Unknown command: "+cmd;
    Serial.println(text);
    Reply(text, caller_id);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
}

/*******************************************************************************
 * listen_push_buttons function:
 ******************************************************************************/
void listen_push_buttons(){
    if(digitalRead(BUTTON_1) == LOW){
      control_relay(1, RELAY_1, STATE_RELAY_1);
    }
    else if (digitalRead(BUTTON_2) == LOW){
      control_relay(2, RELAY_2, STATE_RELAY_2);
    }
    else if (digitalRead(BUTTON_3) == LOW){
      control_relay(3, RELAY_3, STATE_RELAY_3);
    }
    else if (digitalRead(BUTTON_4) == LOW){
      control_relay(4, RELAY_4, STATE_RELAY_4);
    }
}
void control_relay(int relay_no, int relay_pin, boolean &status){
  delay(200);
  String text = (status)? "ON" : "OFF";
  status = !status;
  digitalWrite(relay_pin, status);
  Serial.println("Relay"+String(relay_no)+" is "+text);
  delay(50);
  EEPROM.update(totalMemory+relay_no,status);
}

/*******************************************************************************
 * getCallerID function:
 ******************************************************************************/
String getCallerID(String buff){
//+CMT: "+923001234567","","22/05/20,11:59:15+20"
//Hello
  unsigned int index, index2;
  index = buff.indexOf("\"");
  index2 = buff.indexOf("\"", index+1);
  String callerID = buff.substring(index+1, index2);
  callerID.trim();
  //Serial.print("index+1= "); Serial.println(index+1);
  //Serial.print("index2= "); Serial.println(index2);
  //Serial.print("length= "); Serial.println(callerID.length());
  Serial.println("Caller ID: "+callerID);
  return callerID;
}
/*******************************************************************************
 * getMsgContent function:
 ******************************************************************************/
String getMsgContent(String buff){
  //+CMT: "+923001234567","","22/05/20,11:59:15+20"
  //Hello  
  unsigned int index, index2;
  index = buff.lastIndexOf("\"");
  index2 = buff.length();
  String command = buff.substring(index+1, index2);
  command.trim();
  command.toLowerCase();
  //Serial.print("index+1= "); Serial.println(index+1);
  //Serial.print("index2= "); Serial.println(index2);
  //Serial.print("length= "); Serial.println(msg.length());
  Serial.println("Command:"+command);
  return command;
}
/*******************************************************************************
 * getNumber function:
 ******************************************************************************/
String getNumber(String text){
  //r=+923001234567
  String temp = text.substring(3, 16);
  //Serial.println(temp);
  temp.trim();
  return temp;
}


/*******************************************************************************
 * controlRelayGSM function:
 ******************************************************************************/
void controlRelayGSM(int relay_no, int relay_pin, boolean status, String caller_id){
  digitalWrite(relay_pin, status);
  EEPROM.update(totalMemory+relay_no,status);
  String text = "Relay "+String(relay_no)+" is ON";
  Serial.println(text);
  Reply(text, caller_id);
}

/*******************************************************************************
 * sendStatus function:
 ******************************************************************************/
void sendStatus(boolean status, String caller_id){
  String text = (status)? "ON" : "OFF";
  Serial.println("Relay is "+text);
  Reply("Relay is "+text, caller_id);
}

/*******************************************************************************
 * Reply function
 * Send an sms
 ******************************************************************************/
void Reply(String text, String caller_id)
{
  //return;
    sim800.print("AT+CMGF=1\r");
    delay(1000);
    sim800.print("AT+CMGS=\""+caller_id+"\"\r");
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
  //byte phoneLength = strToWrite.length();
  //EEPROM.write(addrOffset, phoneLength);
  for (int i = 0; i < phoneLength; i++)
    { EEPROM.write(addrOffset + i, strToWrite[i]); }
}

/*******************************************************************************
 * readFromEEPROM function:
 * Store phone numbers in EEPROM
 ******************************************************************************/
String readFromEEPROM(int addrOffset)
{
  //byte phoneLength = strToWrite.length();
  char data[phoneLength + 1];
  for (int i = 0; i < phoneLength; i++)
    { data[i] = EEPROM.read(addrOffset + i); }
  data[phoneLength] = '\0';
  return String(data);
}




/*******************************************************************************
 * comparePhone function:
 * compare phone numbers stored in EEPROM
 ******************************************************************************/
boolean comparePhone(String number)
{
  boolean flag = 0;
  String tempPhone = "";
  //--------------------------------------------------
  for (int i = 0; i < totalPhoneNo; i++){
    tempPhone = readFromEEPROM(offsetPhone[i]);
    if(tempPhone.equals(number)){
      flag = 1;
      break;
    }
  }
  //--------------------------------------------------
  return flag;
}



void LoadStateEEPROM()
{
  digitalWrite(RELAY_1, EEPROM.read(totalMemory+1)); delay(200);
  digitalWrite(RELAY_2, EEPROM.read(totalMemory+2)); delay(200);
  digitalWrite(RELAY_3, EEPROM.read(totalMemory+3)); delay(200);
  digitalWrite(RELAY_4, EEPROM.read(totalMemory+4)); delay(200);
}  
