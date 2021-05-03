#define t   50
#define t2  100

int leds[] = {2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13};
int total_leds = 12;

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < total_leds; i++) {
    pinMode(leds[i], OUTPUT);
  }
}

void loop() {
  
  Serial.println("Effect 1");
  for (int i = 0; i < 3; i++) effect_1();
  Serial.println("Effect 2");
  for (int i = 0; i < 3; i++) effect_2();
  Serial.println("Effect 3");
  for (int i = 0; i < 20; i++) effect_3();
  Serial.println("Effect 4");
  for (int i = 0; i < 3; i++) effect_4();
  Serial.println("Effect 5");
  for (int i = 0; i < 3; i++) effect_5();
  Serial.println("Effect 6");
  for (int i = 0; i < 3; i++) effect_6();
  Serial.println("Effect 7");
  for (int i = 0; i < 3; i++) effect_7();
  Serial.println("Effect 8");
  for (int i = 0; i < 3; i++) effect_8();
  Serial.println("Effect 9");
  for (int i = 0; i < 10; i++) effect_9();
  Serial.println("Effect 10");
  for (int i = 0; i < 10; i++) effect_10();
  Serial.println("Effect 11");
  for (int i = 0; i < 5; i++) effect_11();
  Serial.println("Effect 12");
  for (int i = 0; i < 5; i++) effect_12();
  
  ledsOff();
  delay(2000);
}

//TURN ON ALL LEDs
void ledsOn() {
  for (int i = 0; i < total_leds; i++) {
    digitalWrite(leds[i], HIGH);
  }
}

//TURN OFF ALL LEDs
void ledsOff() {
  for (int i = 0; i < total_leds; i++) {
    digitalWrite(leds[i], LOW);
  }
}

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//LEFT TO RIGH FILL UNFILL - RIGHT TO LEFT FILL UNFIL   
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void effect_1() {
  int i=0;
  for(i=0; i<=total_leds; i++){
    digitalWrite(leds[i], HIGH);
    delay(t);
  }
  for(i=0; i<=total_leds; i++){
    digitalWrite(leds[i], LOW);
    delay(t);
  }
  
  for(i = total_leds; i>=0; i--){
    digitalWrite(leds[i], HIGH);
    delay(t);
  }
  for(i = total_leds; i>=0; i--){
    digitalWrite(leds[i], LOW);
    delay(t);
  }
}

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//SPRING
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void effect_2() {
  int i=0;
  for(i=0; i<=total_leds-1 ; i++)
  {
  digitalWrite(leds[i],HIGH);
  delay(t);
  }
  for(i=total_leds; i>=0 ; i--)
  {
  digitalWrite(leds[i],LOW);
  delay(t);
  }
  
  for(i=total_leds; i>=0 ; i--)
  {
  digitalWrite(leds[i],HIGH);
  delay(t);
  }
  for(i=0; i<=total_leds ; i++)
  {
  digitalWrite(leds[i],LOW);
  delay(t);
  }
}

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//RUN ODD LEDs
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void effect_3() {
  for (int i = 0; i < total_leds; i = i + 2) {
    digitalWrite(leds[i], HIGH);
    digitalWrite(leds[i + 1], LOW);
  }
  delay(t2);
  for (int i = 0; i < total_leds; i = i + 2) {
    digitalWrite(leds[i], LOW);
    digitalWrite(leds[i + 1], HIGH);
  }
  delay(t2);
}

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//RUN EVEN ODD LEDs
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void effect_4() {
for(int i = 0; i <= total_leds; i++){
  digitalWrite(leds[i], HIGH);
  delay(t2);
  i=i+1;
  }
for(int i = 0; i <= total_leds; i++){
  digitalWrite(leds[i], LOW);
  delay(t2);
  }

  for(int j = total_leds +1; j >= 0; j--){
  digitalWrite(leds[j], HIGH);
  delay(t2);
  j=j-1;
  }
for(int j = total_leds; j >= 0; j--){
  digitalWrite(leds[j], LOW);
  delay(t2);
  }
}

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//FROM LEFT AND RIGHT TO CENTER - AND VICE VERSA
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void effect_5() {
  int i=0;
  for(i=0; i<=total_leds/2 ; i++)
  {
  digitalWrite(i,HIGH);
  digitalWrite(leds[total_leds-i],HIGH);
  delay(t2);
  }
  for(i=0; i<=total_leds/2 ; i++)
  {
  digitalWrite(i,LOW);
  digitalWrite(leds[total_leds-i],LOW);
  delay(t2);
  }
  delay(t2);
  for(i=0; i<=total_leds/2 ; i++)
  {
  digitalWrite(i,HIGH);
  digitalWrite(leds[total_leds-i],HIGH);
  delay(t2);
  }
  for(i=total_leds/2; i>=0 ; i--)
  {
  digitalWrite(leds[i],LOW);
  digitalWrite(leds[total_leds-i],LOW);
  delay(t2);
  }
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//NIGHT RIDER LEFT TO RIGHT (ONE LED)
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void effect_6() {
  for (int i = 0; i < total_leds; i++) {
    digitalWrite(leds[i], HIGH);
    delay(t2);
    digitalWrite(leds[i], LOW);
  }
}

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//NIGHT RIDER RIGHT TO LEFT (ONE LED)
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void effect_7() {
  for (int i = total_leds; i > 0; i--) {
    digitalWrite(leds[i-1], HIGH);
    delay(t2);
    digitalWrite(leds[i-1], LOW);
  }
}

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//NIGHT RIDER BACK AND FORTH (ONE LED)
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void effect_8() {
  for(int i=0;i<=total_leds;i++){
    digitalWrite(leds[i],HIGH);
    delay(t2);
    digitalWrite(leds[i],LOW);
  }
  for(int i=total_leds-2;i>=0;i--){
    digitalWrite(leds[i],HIGH);
    delay(t2);
    digitalWrite(leds[i],LOW);
  }
}

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//INSIDE TO CENTER
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void effect_9() {
  for (int i = 0; i < total_leds / 2; i++) {
    digitalWrite(leds[i], HIGH);
    digitalWrite(leds[total_leds - 1 - i], HIGH);
    delay(t2);
    digitalWrite(leds[i], LOW);
    digitalWrite(leds[total_leds - 1 - i], LOW);
  }
}

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//OUTSIDE FROM CENTER
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void effect_10() 
{
  for (int i = (total_leds / 2) - 1; i >= 0 ; i--)
  {
    digitalWrite(leds[i], HIGH);
    digitalWrite(leds[total_leds - 1 - i], HIGH);
    delay(t2);
    digitalWrite(leds[i], LOW);
    digitalWrite(leds[total_leds - 1 - i], LOW);
  }
}

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//RUN 2 LEDs FROM LEFT TO RIGHT
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void effect_11() {
  for (int i = 0; i < total_leds + 3; i++) {
    if (i <= total_leds) {
      digitalWrite(leds[i], HIGH);
    }
    if (i > 1) {
      digitalWrite(leds[i - 2], LOW);
    }
    delay(t2);
  }
}

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//RUN 2 LEDS BACK AND FORTH
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void effect_12() {
  for (int i = 2; i < total_leds; i++) {
    if (i == 2) {
      digitalWrite(leds[0], HIGH);
      //digitalWrite(leds[1], HIGH);
    }
    digitalWrite(leds[i], HIGH);
    digitalWrite(leds[i - 2], LOW);
    delay(t2);
  }
  for (int i = total_leds - 3; i > -1; i--) {
    digitalWrite(leds[i], HIGH);
    digitalWrite(leds[i + 2], LOW);
    delay(t2);
  }
}
