

void setup() {
  // put your setup code here, to run once:
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  delay(5000);
  digitalWrite(6, 255);
  digitalWrite(9, 255);
  digitalWrite(10, 255);
  digitalWrite(11, 255);
  delay(2000);
  digitalWrite(6, 0);
  digitalWrite(9, 0);
  digitalWrite(10, 0);
  digitalWrite(11, 0);
}

void loop() {
  // put your main code here, to run repeatedly:

}
