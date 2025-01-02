#include <SPI.h>
#include <LoRa.h>
//int pot = A0;
int LED = 5;
int num = 0;
int sign = 1;

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  while (!Serial);
  
  Serial.println("LoRa Sender");
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting Lora failed!");
    while (1);
  }
}

void loop() {
  LoRa.beginPacket();
  LoRa.write(num);
  LoRa.endPacket();
  
  num+=sign*3;
  if (num >= 255 || num <= 0){
    sign *= -1;
  }

  analogWrite(LED, num);
  Serial.println(num);
  
}
