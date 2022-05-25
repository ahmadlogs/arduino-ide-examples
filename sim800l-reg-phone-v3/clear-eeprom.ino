#include <EEPROM.h>
int phoneLength = 13;

void setup() {
  Serial.begin(9600);
  for(int i = 0; i < 5; i++)
    {writeToEEPROM(phoneLength*i, "");}
    
  Serial.println("Phone numbers list deleted successfully");
}

void loop() {

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
