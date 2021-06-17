#include <SoftwareSerial.h>
#include <Wire.h>
//#include <SPI.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (A4 = SDA, A5 = SCL)
// Reset pin # (or -1 if sharing Arduino reset pin)
#define OLED_RESET     4
//See datasheet for Address
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#define rxPin 2
#define txPin 3
SoftwareSerial sim800L(rxPin,txPin); 


String network, battery;

void setup() {
  //Begin serial communication with Serial Monitor
  Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  
  //Begin serial communication with Arduino and SIM800L
  sim800L.begin(9600);

  sim800L.println("AT");
  delay(1000);
  sim800L.println("AT+CMGF=1");
  delay(1000);

}

void loop() {

      //network = "0";
      //battery = "0";

      network = get_network();
      battery = get_battery();
      
      display_battery();
      display_network();
      
}

int get_network()
{
  String buff;
  //buff = "+CSQ: 16,0";
  unsigned int result, index1, index2, timeout=0;

  sim800L.println("AT+CSQ");  
  for (unsigned long start = millis(); millis() - start < 2000;){
    while(sim800L.available())
    {
      buff = sim800L.readString();
      timeout = 1;
      break;
    }
  }

  if(timeout == 0)
    { return 0; }
    
  Serial.println(buff);
  
  //String network_status;
  //_____________________________________________________
  //Remove sent "AT Command" from the response string.
  index1 = buff.indexOf("\r");
  buff.remove(0, index1+2);
  buff.trim();
  //_____________________________________________________
  
  //_____________________________________________________
  
    index1 = buff.indexOf(":");
    index2 = buff.indexOf(",");
    buff = buff.substring(index1+1, index2);
    buff.trim();
    result = buff.toInt();

    if(result == 99)
    {
      //not known or not detectable
      return 0;
    }
    else if(result >= 2 && result <= 9)
    {
      //Signal Quality = Marginal
      return 20;
    }
    else if(result >= 10 && result <= 14)
    {
      //Signal Quality = OK
      return 40;
    }
    else if(result >= 15 && result <= 19)
    {
      //Signal Quality = Good
      return 60;
    }
    else if(result >= 20 && result <= 31)
    {
      //Signal Quality = Excellent
      return 99;
    }
    
  return 0;
}

int get_battery()
{
  String buff;
  //buff = "+CBC: 0,99,4190";
  unsigned int index1, index2, timeout=0;
    
  sim800L.println("AT+CBC");  
  for (unsigned long start = millis(); millis() - start < 2000;){
    while(sim800L.available())
    {
      buff = sim800L.readString();
      timeout = 1;
      break;
    }
  }

  if(timeout == 0)
    { return 0; }
    
  Serial.println(buff);
  
  //String network_status;
  //_____________________________________________________
  //Remove sent "AT Command" from the response string.
  index1 = buff.indexOf("\r");
  buff.remove(0, index1+2);
  buff.trim();
  //_____________________________________________________
  
  //_____________________________________________________
  
    //search for first comma (,) = index1
    //search for 2nd comma (,) = index2
    index1 = buff.indexOf(",");
    index2 = buff.indexOf(",", index1+1);
    String result = buff.substring(index1+1, index2);
    result.trim();
    
    Serial.println(result);
    //buff.remove(0, index+2);
  return result.toInt();
}

void display_battery()
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(20, 0);
  display.print("Bat:");
  display.print(battery);
  display.print("%");
  display.drawRect(14, 20, 80, 40, WHITE);
  display.drawRect(94, 30, 10, 20, WHITE);
  display.fillRect(14, 20, (int)(8 * (battery.toInt()) / 10), 40, WHITE);
  display.display();
  delay(2000);
}

void display_network()
{
  display.clearDisplay();
  display.drawLine(5, 15, 25, 15, WHITE);
  display.drawLine(5, 15, 14, 30, WHITE);
  display.drawLine(25, 15, 17, 30, WHITE);
  display.fillRect(14, 15, 4, 40, WHITE);
  int net = network.toInt() / 20;
  int x1 = 24, y1 = 50, x2 = 4, y2 = 5;
  for (int i = 1; i <= net; i++)
  {
    display.fillRect(x1, y1, x2, y2, WHITE);
    x1 += 10;
    y1 -= 5;
    y2 += 10;
    y2 -= 5;
  }
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(80, 34);
  display.print(network);
  display.setTextSize(1);
  display.setCursor(117, 44);
  display.println("%");
  display.display();
  delay(2000);
}
