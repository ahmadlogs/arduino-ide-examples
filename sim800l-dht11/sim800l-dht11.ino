#include <SoftwareSerial.h>
#include <DHT.h>

//sender phone number with country code
//i.e +923001234567
const String PHONE = "Enter_Your_Phone_Number";

//GSM Module RX pin to Arduino 3
//GSM Module TX pin to Arduino 2
#define rxPin 2
#define txPin 3
SoftwareSerial sim800(rxPin,txPin);

String smsStatus,senderNumber,receivedDate,msg;


#define DHT11_PIN 11
#define DHTTYPE DHT11

DHT dht(DHT11_PIN, DHTTYPE);

void setup() {
  
  Serial.begin(115200);
  
  Serial.println("initialize DHT11");
  dht.begin();

  Serial.println("SIM800L software serial initialize");
  sim800.begin(9600);
  sim800.println("AT+CMGF=1");

  delay(2000);
}

void loop()
{
  //Serial.println(get_temprature());
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
      Serial.println(msg);
      Serial.println(senderNumber);
      
      if(senderNumber == PHONE && msg == "get temperature"){
          Reply(get_temprature());
      }

    }
  //////////////////////////////////////////////////
  }
  else{
  //The result of AT Command is "OK"
  }
}

//************************************************************
String get_temprature()
{
  //Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();

  //Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  
  //Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  //°C
  String humidity = "Humidity: " + String(h) + "%";
  String temperature = "Temperature: " + String(t) + " Celsius";
  String heat_index = "Heat index: " + String(hic) + " Celsius";

  //Serial.println(humidity);
  //Serial.println(temperature);
  //Serial.println(heat_index);
  //Serial.println("------------------------------------------");

  return humidity + "\n" + temperature + "\n" + heat_index;
}

//************************************************************
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
