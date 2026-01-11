#include <SPI.h>
#include <LoRa.h>
int pot = A0;

void setup() {
  Serial.begin(9600);
  pinMode(pot, INPUT);

  while (!Serial);
  Serial.println("LoRa Sender");
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting Lora failed!");
    while (1);
  }
}

void loop() {
  int val = map(
}
