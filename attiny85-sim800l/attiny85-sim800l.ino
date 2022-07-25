/* ------------------------------------------------------------------------
 * Created by: Tauseef Ahmad
 * Created on: 23 July, 2022
 *  
 * Tutorial: https://youtu.be/pCpNk2ox5M4
 * ------------------------------------------------------------------------
 * ATTiny Core URL
 * https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json
 * ------------------------------------------------------------------------*/
 
//---------------------------------------
#include <SoftwareSerial.h>
#define rx 3 
#define tx 4
SoftwareSerial SIM800L(rx, tx);
//---------------------------------------
#define BUTTON_CALL 0
#define BUTTON_SMS  1
#define LED_PIN     2
//---------------------------------------
int button_state = 0;
const String AT = "AT+C";
//---------------------------------------
const String PHONE = "+923004691334";
//---------------------------------------

void setup() {
  //_________________________________
  pinMode(BUTTON_CALL, INPUT);
  pinMode(BUTTON_SMS,  INPUT);
  pinMode(LED_PIN, OUTPUT);
  //_________________________________
  SIM800L.begin(9600);
  delay(11000); 
  //_________________________________
  SIM800L.println(AT+"MGF=1");
  delay(1000);
  SIM800L.println(AT+"NMI=1,2,0,0,0");
  delay(1000);
  SIM800L.println("ATE0");
  delay(1000);
  //_________________________________
}

void loop() {
  //------------------------------------------
  while(SIM800L.available()){
    String buff = SIM800L.readString();
    if(buff.indexOf(PHONE) > -1){
      //_____________________________________
      if(buff.indexOf("L1") > -1){
        buff = "";
        digitalWrite(LED_PIN, HIGH);
        Reply("ON");
      }
      //_____________________________________
      else if(buff.indexOf("L0") > -1){
        buff = "";
        digitalWrite(LED_PIN, LOW);
        Reply("OFF");
      }
      //_____________________________________
    }
  }
  //---------------------------------------------
  button_state = digitalRead(BUTTON_CALL);
  //---------------------------------------------
  if(button_state == HIGH){
    delay(100);
    SIM800L.println("ATD"+PHONE+";");
  }
  //---------------------------------------------
  button_state = digitalRead(BUTTON_SMS);
  if(button_state == HIGH){
    delay(100);
    Reply("Hi");
  }
  //---------------------------------------------
}

void Reply(String text)
{
  SIM800L.println(AT+"MGF=1");
  delay(1000);
  SIM800L.println(AT+"MGS=\""+PHONE+"\"");
  delay(1000);
  SIM800L.print(text);
  delay(100);
  SIM800L.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
  delay(1000);
}
