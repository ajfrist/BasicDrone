#include <SPI.h>
#include <LoRa.h>

int JOYSTICK_PIN_X1 = A1;
int JOYSTICK_PIN_Y1 = A2;
int JOYSTICK_PIN_X2 = A3;
int JOYSTICK_PIN_Y2 = A4;
int THROTTLE_PIN = A5;
int SWITCH_PIN1 = 2;
int SWITCH_PIN2 = 3;

int throttle = 0;
bool pressed = false;

void setup() {
  pinMode(JOYSTICK_PIN_X1, INPUT);
  pinMode(JOYSTICK_PIN_Y1, INPUT);
  pinMode(JOYSTICK_PIN_X2, INPUT);
  pinMode(JOYSTICK_PIN_Y2, INPUT);
  pinMode(THROTTLE_PIN, INPUT);
  pinMode(SWITCH_PIN1, INPUT_PULLUP);
  pinMode(SWITCH_PIN2, INPUT_PULLUP);

  initialThrottle = analogRead(THROTTLE_PIN);
  throttle = initialThrottle;
  
  Serial.begin(9600);
  while (!Serial);
  
  Serial.println("LoRa Sender");
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting Lora failed!");
    while (1);
  }
}

void loop() {
  LoRa.beginPacket();
  LoRa.write(analogRead(THROTTLE_PIN));
  LoRa.write(analogRead(SWITCH_PIN1));
  LoRa.write(analogRead(SWITCH_PIN2));
  LoRa.write(analogRead(JOYSTICK_PIN_X1));
  LoRa.write(analogRead(JOYSTICK_PIN_Y1));
  LoRa.write(analogRead(JOYSTICK_PIN_X2));
  LoRa.write(analogRead(JOYSTICK_PIN_Y2));
  LoRa.endPacket();



  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.println("  Received packet: ");

    // read packet
    while (LoRa.available()) {
      Serial.print("Throttle trim adjustment: ");
      int val = (int) LoRa.read();
      Serial.println(val);
      
//      Serial.print("Motors adjusted: ");
//      val = (int) LoRa.read();
//      if (val & 0b0001){
//        Serial.print("1 ");
//      }
//      if (val & 0b0010){
//        Serial.print("2 ");
//      }
//      if (val & 0b0100){
//        Serial.print("3 ");
//      }
//      if (val & 0b1000){
//        Serial.print("4 ");
//      }
//      Serial.println("");

      Serial.print("Current respective throttles: ");
      Serial.print((int)LoRa.read());
      Serial.print((int)LoRa.read());
      Serial.print((int)LoRa.read());
      Serial.println((int)LoRa.read());

      Serial.print("Receiver RSSI: ");
      val = (int) LoRa.read();
      Serial.print(val);
    }
      
    Serial.print(", Transmitter RSSI: ");
    Serial.println(LoRa.packetRssi());
    Serial.println("");
  }
  else {
//    Serial.println("---no packet---");
//    Serial.println(analogRead(THROTTLE_PIN));
  }
    

    // print RSSI of packet
//    Serial.print("' with RSSI ");
//    Serial.println(LoRa.packetRssi());


  
}
