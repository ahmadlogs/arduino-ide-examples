/* ------------------------------------------------------------------------
 * Created by: Tauseef Ahmad
 * Created on: 12 May, 2022
 *  
 * Tutorial: https://youtu.be/B_5nn7URZTk
 * ------------------------------------------------------------------------*/

#include<SoftwareSerial.h>

SoftwareSerial sim800l(2, 3);
#define RESET_PIN 4
unsigned long previousMillis = 0;
//---------------------------------------------------------------------------------------------
boolean getResponse(String expected_answer, unsigned int timeout=1000,boolean reset=false){
  boolean flag = false;
  String response = "";
  unsigned long previous;
  //*************************************************************
  for(previous=millis(); (millis() - previous) < timeout;){
    while(sim800l.available()){
      response = sim800l.readString();
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
//---------------------------------------------------------------------------------------------
boolean tryATcommand(String cmd, String expected_answer, int timeout, int total_tries){
  TryAgain:
  //*************************************************************
  for(int i=1; i<=total_tries; i++){
    sim800l.println(cmd);
    if(getResponse(expected_answer, timeout) == true){
      break;
    }
    else {
      Serial.print(".");
    }    
    if(i == total_tries){
      Serial.println("Faild! Resetting the Module");
      digitalWrite(RESET_PIN, LOW);
      delay(100);
      digitalWrite(RESET_PIN, HIGH);
      goto TryAgain;
    }
  }
  //*************************************************************
}
//---------------------------------------------------------------------------------------------
boolean resetSIM800L(){
  digitalWrite(RESET_PIN, LOW);
  delay(100);
  digitalWrite(RESET_PIN, HIGH);
  
  boolean flag = false;
  for(int i=1; i<=20; i++){
    sim800l.println("ATE");
    Serial.print(".");
    if(getResponse("NULL", 1000, true) == true)
      {flag = true; break;}
  }
  sim800l.println("ATE");
  if(flag == true){
    for(int i=1; i<=20; i++){
      if(getResponse("SMS Ready", 15000) == true)
        {flag = true; break;}
    }
  }
  
}
//---------------------------------------------------------------------------------------------

void setup() {
  pinMode(RESET_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  sim800l.begin(9600);

  Serial.begin(9600);
  
  resetSIM800L();
  tryATcommand("AT","OK",1000,20);
  tryATcommand("AT+CMGF=1","OK",1000,20);
  tryATcommand("AT+CNMI=1,2,0,0,0","OK",1000,20);
}

void loop() {

  while(sim800l.available()){
    String response = sim800l.readString();
    Serial.println(response);
    if(response.indexOf("+CMT:") > 0){
      if(response.indexOf("ledon") > 0){
        digitalWrite(LED_BUILTIN, HIGH);
      }
      if(response.indexOf("ledoff") > 0){
        digitalWrite(LED_BUILTIN, LOW);
      }
    }
    //Serial.println(buff);
  }
  while(Serial.available()){
    sim800l.println(Serial.readString());
  }
  //1 * 60 * 1000  = 60000  = 1 Minutes
  //15 * 60 * 1000 = 900000 = 15 Minutes
  if(millis() - previousMillis > 900000) { //15 Minutes interval
    tryATcommand("AT","OK",1000,15);
    previousMillis = millis();
  }

}










boolean getFirstResponse(unsigned int timeout = 1000){
  boolean flag = false;
  String response = "";
  unsigned long previous;
  for(previous=millis(); (millis() - previous) < timeout;){
    while(sim800l.available()){
      response = sim800l.readString();
      if(response != ""){
        Serial.println(response);
        return true;
      }
    }
  }
  return false;
}
