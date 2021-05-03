const int irPin = 4;
const int Relay1 = 0;
const int Relay2 = 1;

boolean Relay1State = false;
boolean Relay2State = false;

void setup() {
  
  //Serial.begin(115200);
  pinMode(irPin, INPUT);
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
}

void loop() {
  int key = getIrKey();

  if(key == #){
    Relay1State = !Relay1State;
    if(Relay1State == true)
      digitalWrite(Relay1, HIGH);
    else
      digitalWrite(Relay1, LOW);
  }

  if(key == #){
    Relay2State = !Relay2State;
    if(Relay2State == true)
      digitalWrite(Relay2, HIGH);
    else
      digitalWrite(Relay2, LOW);
  }
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
  
  //if(key)
    //Serial.println(key);

    delay(250);
  return key;
}
