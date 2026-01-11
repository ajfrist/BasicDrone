#include <SPI.h>
#include <LoRa.h>
int LED = 5;
int val = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);

  while (!Serial);
  Serial.println("LoRa Receiver");
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting Lora failed!");
    while (1);
  }

  //LoRa.onReceive(onReceive);

  LoRa.receive();
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      int c = (int) LoRa.read();
      Serial.print(c);

      analogWrite(LED, 100);
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
  
  digitalWrite(LED, HIGH);
}

//void onReceive(int PacketSize){
//  //int packetSize = LoRa.parsePacket();
//  //if (packetSize) {
//    //while (LoRa.available()){
//  int inChar = LoRa.read();
//  //Serial.print("new ");
//  //Serial.print(packetSize+", "+inChar);
//  Serial.println(inChar);
//  //inString += (char)inChar;
//  //val = inString.toInt();
//  val = inChar;
//  //}
//  //inString = "";
//  //LoRa.packetRssi();
//  if (val > 0){
//    digitalWrite(LED, HIGH);
//  }
//  else if (val == 0){
//    digitalWrite(LED, LOW);
//  }
//  //}
//  Serial.print("hi");
//  //delay(100);
//}
