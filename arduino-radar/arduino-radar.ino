#include <Servo.h>  

//-----------------------------------
//servo setup
//Servo Pin must have PWM
const int servoPin = 8;
Servo myServo;
//-----------------------------------

//-----------------------------------
//ultrasonic sensor setup
const int trigPin = 10;
const int echoPin = 11;
long duration;
int distance;
//-----------------------------------
 
void setup() {
  
  Serial.begin(9600);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  myServo.attach(servoPin);
}

void loop() {
 
  //-----------------------------------------------------------
  //servo move forward
  for(int i=10;i<=170;i++) // servo position 10-170
  {  
    myServo.write(i);
    delay(30);
    distance = measureDistance();
    Serial.print(i); 
    Serial.print(","); 
    Serial.print(distance ); 
    Serial.print("."); 
  }
  //-----------------------------------------------------------
  
  //-----------------------------------------------------------
  //servo move backward
  for(int i=170;i>10;i--) // servo position 170-10
  {  
    myServo.write(i);
    delay(40);
    distance = measureDistance();
    Serial.print(i);
    Serial.print(",");
    Serial.print(distance );
    Serial.print(".");
  }
  //-----------------------------------------------------------
}

int measureDistance(){ 
  
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
 
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); 
  distance= duration*0.034/2;
  return distance;
}
