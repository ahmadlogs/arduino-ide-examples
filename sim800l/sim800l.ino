#include <SoftwareSerial.h>

#define rxPin 2
#define txPin 3
SoftwareSerial sim800L(rxPin,txPin); 

String buff;
void setup()
{
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  
  //Begin serial communication with Arduino and SIM800L
  sim800L.begin(9600);

  Serial.println("Initializing...");
  
  sim800L.println("AT");
  waitForResponse();

  sim800L.println("ATE1");
  waitForResponse();

  sim800L.println("AT+CMGF=1");
  waitForResponse();

  sim800L.println("AT+CNMI=1,2,0,0,0");
  waitForResponse();
}

void loop()
{
  while(sim800L.available()){
    buff = sim800L.readString();
    Serial.println(buff);
  }
  while(Serial.available())  {
    buff = Serial.readString();
    buff.trim();
    if(buff == "s")
      send_sms();
    else if(buff== "c")
      make_call();
    else
      sim800L.println(buff);
  }
}

void send_sms(){
  sim800L.print("AT+CMGS=\"+923001234567\"\r");
  waitForResponse();
  
  sim800L.print("Hello from SIM800L");
  sim800L.write(0x1A);
  waitForResponse();
}


void make_call(){
  sim800L.println("ATD+923001234567;");
  waitForResponse();
}

void waitForResponse(){
  delay(1000);
  while(sim800L.available()){
    Serial.println(sim800L.readString());
  }
  sim800L.read();
}



/* *************************************************
 * SMS SENT STATUS
 * SMS SENT     -> OK
 * SMS NOT SENT -> ERROR or +CMS ERROR
 * *************************************************
AT+CMGS="+85291234567"
> It is easy to send text messages.
+CMGS: 12

OK


The value in the information response, 12, is the message reference number allocated to the SMS 
text message. The final result code OK indicates the sending of the SMS text message was successful. 
If the execution of the +CMGS AT command fails, the GSM/GPRS modem or mobile phone will return 
either the final result code ERROR or +CMS ERROR.
 */
