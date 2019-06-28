#include <LiquidCrystal.h>

double err = 0;
double err_last = 0;
int out = 0;
double T = 0.05; //50 milliseconds
int kp = 50;
int kd = 1;
int ki = 3;
int ref = 5; //in
double i = 0;
double pos;
const double v = 0.0133; //in/us
unsigned long startMillis;

LiquidCrystal lcd(6,7,8,11,12,13);

void setup() {
  // put your setup code here, to run once:
  pinMode(2, INPUT); //button
  attachInterrupt(digitalPinToInterrupt(2), button, FALLING);
  pinMode(3, OUTPUT); //PWM IA
  pinMode(4, OUTPUT); //PWM IB
  pinMode(9, OUTPUT); //Trig
  pinMode(10, INPUT); //Echo
  Serial.begin(9600);
  Serial.println("start");

  lcd.begin(16,2);
  lcd.print("start");
  startMillis = millis();
}

void button(){
  if(ref == 5)
    ref = 2;
  else
    ref = 5;
}

void loop() {
  // put your main code here, to run repeatedly:
  startMillis = millis();
  digitalWrite(9, LOW);
  delayMicroseconds(2);
  digitalWrite(9, HIGH);
  delayMicroseconds(10);
  digitalWrite(9, LOW);
  int t = pulseIn(10, HIGH);
  pos = (t/2)*v;
  if(pos < 0 || pos > 10)
    pos = 0;
  String str = "Pos: " + String(round(pos)) + " in";
  Serial.print(str);
  lcd.clear();
  lcd.print(str);
  //lcd.setCursor(0,0);
  err = ref - pos;
  double P = kp*err;
  i = i+(err*T);
  double I = ki*i;
  double d = (err-err_last)/T;
  double D = kd*d;
  out = round(P + I + D);

  if(out > 0){
    out+=150;

    if(out>255)
      out = 255;
    analogWrite(3, out);
    analogWrite(4, 0);
  }
  else{
    if(out<-127)
      out = -127;
    analogWrite(4, abs(out));
    analogWrite(3, 0);
  }
  
  Serial.println("  |  Out: " + String(out));
  analogWrite(5,abs(out));

  err_last = err;
  if(T*1000-int(millis()-startMillis)>0)
    delay(T*1000-int(millis()-startMillis));
}
