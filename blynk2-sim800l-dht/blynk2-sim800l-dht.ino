
/************************************************************************************
 * Created By: Tauseef Ahmad
 * Created On: December 29, 2021
 * 
 * Tutorial: https://youtu.be/qrxuAezlDyA
 *
 * ****************************************************************************
 * Download Resources
 * **************************************************************************** 
 *  Download latest Blynk library here:
 *  https://github.com/blynkkk/blynk-library/releases/latest
 * **************************************************************************** 
 *  DHT-sensor-library
 *  https://github.com/adafruit/DHT-sensor-library
 * ****************************************************************************
 *  Attention! Please check out TinyGSM guide:
 *  https://tiny.cc/tinygsm-readme
 *  Change GPRS apm, user, pass, and Blynk auth token to run :)
 *  Feel free to apply it to any other example. It's simple!
 **********************************************************************************/

//-----------------------------------------------------------------------
/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
//-----------------------------------------------------------------------
/* Fill-in your Template ID (only if using Blynk.Cloud) */
#define BLYNK_TEMPLATE_ID "ENTER_BLYNK_TEMPLATE_ID"
#define BLYNK_DEVICE_NAME "ENTER_BLYNK_DEVICE_NAME"
#define BLYNK_AUTH_TOKEN "ENTER_BLYNK_AUTH_TOKEN"
//-----------------------------------------------------------------------
// Select your modem:
#define TINY_GSM_MODEM_SIM800
//#define TINY_GSM_MODEM_SIM900
//#define TINY_GSM_MODEM_M590
//#define TINY_GSM_MODEM_A6
//#define TINY_GSM_MODEM_A7
//#define TINY_GSM_MODEM_BG96
//#define TINY_GSM_MODEM_XBEE
//-----------------------------------------------------------------------
// Default heartbeat interval for GSM is 60
// If you want override this value, uncomment and set this option:
//#define BLYNK_HEARTBEAT 30
//-----------------------------------------------------------------------
#include <TinyGsmClient.h>
#include <BlynkSimpleTinyGSM.h>
//-----------------------------------------------------------------------
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = BLYNK_AUTH_TOKEN;
//-----------------------------------------------------------------------
// Your GPRS credentials
// Leave empty, if missing user or pass
char apn[]  = "";
char user[] = "";
char pass[] = "";
//-----------------------------------------------------------------------
// Hardware Serial on Mega, Leonardo, Micro
//#define SerialAT Serial1
//-----------------------------------------------------------------------
// or Software Serial on Uno, Nano
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(2, 3); // RX, TX
TinyGsm modem(SerialAT);
//-----------------------------------------------------------------------
#include <DHT.h>
#define DHT11_PIN 4
#define DHTTYPE DHT11
DHT dht(DHT11_PIN, DHTTYPE);
//-----------------------------------------------------------------------
BlynkTimer timer;
#define INTERVAL 1000L
//-----------------------------------------------------------------------



/************************************************************************************
 *  This function sends Arduino's up time every second to Virtual Pin.
 *  In the app, Widget's reading frequency should be set to PUSH. This means
 *  that you define how often to send data to Blynk App.
 **********************************************************************************/
void SendDhtData()
{
  //-----------------------------------------------------------------------
  //Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  //-----------------------------------------------------------------------
  //Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  //-----------------------------------------------------------------------
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  //-----------------------------------------------------------------------
  //Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  //-----------------------------------------------------------------------
  Serial.print("Temperature: ");
  Serial.println(t);
  Serial.print("Humidity: ");
  Serial.println(h);
  Serial.println("------------------------------------------");
  //-----------------------------------------------------------------------
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  //-----------------------------------------------------------------------
}


/************************************************************************************
 *  setup() function
 **********************************************************************************/
void setup()
{
  //-----------------------------------------------------------
  // Debug console
  Serial.begin(115200);
  delay(10);
  //-----------------------------------------------------------
  // Set GSM module baud rate
  SerialAT.begin(9600);
  //SerialAT.begin(9600,SWSERIAL_8N1,D3,D4);
  delay(3000);
  //-----------------------------------------------------------
  dht.begin();
  //-----------------------------------------------------------
  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println("Initializing modem...");
  modem.restart();
  // Unlock your SIM card with a PIN
  //modem.simUnlock("1234");
  //-----------------------------------------------------------
  Blynk.begin(auth, modem, apn, user, pass);
  //-----------------------------------------------------------
  // Setup a function to be called every second
  timer.setInterval(INTERVAL, SendDhtData);
  //-----------------------------------------------------------  
}


/************************************************************************************
 *  loop() function
 **********************************************************************************/
void loop()
{
  Blynk.run();
  timer.run();
}
