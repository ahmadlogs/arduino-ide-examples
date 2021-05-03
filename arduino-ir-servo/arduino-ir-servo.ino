#include <IRremote.h> // add IR remote library
#include <Servo.h> // add servo motor library

const byte IR_RECEIVE_PIN = 2;   // pin for the IR sensor

Servo servo1; 
int motor_angle=0;

void setup()
{
  Serial.begin(115200); // initialize serial communication
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  servo1.attach(9); // declare servo motor pin
  servo1.write(motor_angle); // move the motor to 0 deg
  Serial.println("Servo motor angle 0 degree");
  delay(2000);
}
void loop() 
{
  if(IrReceiver.decode()) // when button is pressed and code is received
  {
      String ir_code = String(IrReceiver.decodedIRData.command, HEX);
      if(ir_code != "0"){
        Serial.println(ir_code);
      }

    //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
    // check if digit1 button is pressed
    //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
    if(ir_code=="c")
    {
      Serial.println("servo motor angle 30 deg");
      motor_angle = 30;
      servo1.write(motor_angle); // move the motor to 30 deg
    }
    //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
    // check if digit2 button is pressed
    //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
    else if(ir_code=="1")
    {
      Serial.println("servo motor angle 60 deg");
      motor_angle = 60;
      servo1.write(motor_angle); // move the motor to 60 deg
    }  
    //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
    // check if digit3 button is pressed
    //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
    else if(ir_code=="4")
    {
      Serial.println("servo motor angle 90 deg");
      motor_angle = 90;          
      servo1.write(motor_angle);
    }  
    //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
    // check if digit4 button is pressed
    //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
    else if(ir_code=="7")
    {
      Serial.println("servo motor angle 120 deg");
      motor_angle = 120;
      servo1.write(motor_angle);
    }
    //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
    // check if digit5 button is pressed
    //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
    else if(ir_code=="e")
    {
      Serial.println("servo motor angle 0 deg");
      motor_angle = 0;
      servo1.write(motor_angle);
    }
    //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
    // check if digit UP button is pressed
    //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN 
    else if(ir_code=="20")
    {
      if(motor_angle<150) motor_angle+=10; // increase motor angle
      Serial.print("Motor angle is ");
      Serial.println(motor_angle);
      servo1.write(motor_angle);         // and move the motor to that angle
    }
    //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
    // check if digit DOWN button is pressed
    //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
    else if(ir_code=="21")
    {
      if(motor_angle>0) motor_angle-=10; // decrease motor angle
      Serial.print("Motor angle is ");
      Serial.println(motor_angle);
      servo1.write(motor_angle);      // and move the motor to that angle
    }
    delay(250);
    IrReceiver.resume();     // again be ready to receive next code
  }
}
