/* ------------------------------------------------------------------------
 * Created by: Tauseef Ahmad
 * Created on: 12 May, 2022
 *  
 * Tutorial: https://youtu.be/B_5nn7URZTk
 * ------------------------------------------------------------------------*/

#include<SoftwareSerial.h>

SoftwareSerial sim800l(2, 3);
//---------------------------------------------------------------------------------------------
boolean getResponse(String expected_answer, unsigned int timeout = 1000){
  boolean flag = false;
  String response = "";
  unsigned long previous;
  //*************************************************************
  for(previous=millis(); (millis() - previous) < timeout;){
    while(sim800l.available()){
      response = sim800l.readString();
      if(response.indexOf(expected_answer) > 0){
        flag = true;
        goto OUTSIDE;
      }
    }
  }
  //*************************************************************
  OUTSIDE:
  if(response != ""){Serial.println(response);}
  return flag;
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
#define RESET_PIN 4
unsigned long previousMillis = 0;
//---------------------------------------------------------------------------------------------

void setup() {
  pinMode(RESET_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  sim800l.begin(9600);

  Serial.begin(9600);

  tryATcommand("ATE","OK",1000,20);
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




/*
bool tryATcommand(String cmd, String expected_answer, int timeout, int total_tries) {
  int num_attempts = 0;
  bool response_received = false;
  while (!response_received && num_attempts < total_tries) {
    sim800l.println(cmd);
    response_received = getResponse(expected_answer, timeout);
    if (!response_received) {
      Serial.print(".");
      num_attempts++;
    }
  }
  if (!response_received) {
    Serial.println("Failed to get response after multiple attempts. Resetting the module...");
    digitalWrite(RESET_PIN, LOW);
    delay(100);
    digitalWrite(RESET_PIN, HIGH);
  }
  return response_received;
}
*/
