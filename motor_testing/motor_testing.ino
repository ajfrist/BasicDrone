int OUT = 3;
int IN = 1;
int count = 1;
int inc = 1;

void setup() {
  Serial.begin(9600);
  pinMode(OUT, OUTPUT);
  //pinMode(IN, INPUT):
}

void loop() {
  analogWrite(OUT, count);
  count+=inc;
  if (count >= 255 || count <= 0){
    inc*=-1;
  }
  Serial.println(count);
  //Serial.println("Speed: ");
  //Serial.println(count);
 
  Serial.println(analogRead(IN));
  delay(100);
  
  /*analogWrite(OUT, 255);
  delay(5000);
  analogWrite(OUT, 0);
  delay(3000);*/
}
