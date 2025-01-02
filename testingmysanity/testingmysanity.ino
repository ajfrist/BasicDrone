int LED = 2;
int BUTTON = 4;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  pinMode(BUTTON, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(digitalRead(BUTTON));
  digitalWrite(LED, digitalRead(BUTTON));
  delay(10);
}
