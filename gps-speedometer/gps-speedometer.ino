#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//On an arduino UNO: A4(SDA), A5(SCL)
#define OLED_RESET -1 //Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C //See datasheet for Address
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define rxPin 2
#define txPin 3
SoftwareSerial neogps(rxPin,txPin);

TinyGPSPlus gps;

void setup() {
  Serial.begin(115200);
  //Begin serial communication Arduino IDE (Serial Monitor)

  //Begin serial communication Neo6mGPS
  neogps.begin(9600);
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.display();
  delay(2000);

}

void loop() {
    
  boolean newData = false;
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (neogps.available())
    {
      if (gps.encode(neogps.read()))
      {
        newData = true;
      }
    }
  }

  //If newData is true
  if(newData == true)
  {
    newData = false;
    print_speed();
  }
  else
  {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.setTextSize(3);
    display.print("No Data");
    display.display();
  }  
  
}

void print_speed()
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
       
  if (gps.location.isValid() == 1)
  {
   //String gps_speed = String(gps.speed.kmph());
    display.setCursor(0, 0);
    display.setTextSize(3);
    display.print(gps.speed.kmph());
    
    display.setCursor(75, 20);
    display.setTextSize(2);
    display.print("km/h");

    display.setTextSize(1);
    display.setCursor(0, 50);
    display.print("SAT:");
    display.setCursor(25, 50);
    display.print(gps.satellites.value());

    display.setTextSize(1);
    display.setCursor(70, 50);
    display.print("ALT:");
    display.setCursor(95, 50);
    display.print(gps.altitude.meters(), 0);

    display.display();
    
  }

}
