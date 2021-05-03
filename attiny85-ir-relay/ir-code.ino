const int irPin = 4;

void setup() {
  
  Serial.begin(115200);
  pinMode(irPin, INPUT);
}

void loop() {
  int key = getIrKey();
  
  if(key != 0)
    Serial.println(key);
}

int getIrKey(){
  int len = pulseIn(irPin,LOW);
  int key, temp;
  key = 0;
  //Serial.print("len=");
  //Serial.println(len);
  if(len > 5000) {
    for(int i=1;i<=32;i++){
      temp = pulseIn(irPin,HIGH);
      if(temp > 1000)
        key = key + (1<<(i-17));
    }
  }
  if(key < 0 )
    key = -key;

  delay(250);
  return key;
}
