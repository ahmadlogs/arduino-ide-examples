String serial_data = "";

#define relay1 8
#define relay2 9


void setup() {
  //start serial communication with Android
  //baudrate = 9600, it must be same in
  //MIT App and here
  Serial.begin(9600);
  
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
}

void loop() {
  while(Serial.available() > 0)
  {
    serial_data = Serial.readString();
    
    //use either 0x0A or '\n' for new line
    //if (data == '\n')
      //{Serial.println("All Data Received");}

    serial_data.toLowerCase();
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    if(serial_data == "relay1 on"){
      digitalWrite(relay1, HIGH);
      Serial.println("Relay1 is ON");
    }
    else if(serial_data == "relay1 off"){
      digitalWrite(relay1, LOW);
      Serial.println("Relay1 is OFF");
    }
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    else if(serial_data == "relay2 on"){
      digitalWrite(relay2, HIGH);
      Serial.println("Relay2 is ON");
    }
    else if(serial_data == "relay2 off"){
      digitalWrite(relay2, LOW);
      Serial.println("Relay2 is OFF");
    }
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  }
}
