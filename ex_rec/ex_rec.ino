#include <SPI.h>
#include <LoRa.h>

#ifdef ARDUINO_SAMD_MKRWAN1300
#error "This example is not compatible with the Arduino MKR WAN 1300 board!"
#endif

void setup() {

  
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Receiver Callback");

  pinMode(5, OUTPUT);
  
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  int packetSize = LoRa.parsePacket();
  String tot = "";
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    //char thing = "";
    while (LoRa.available()) {
      char thing = (char)LoRa.read();
      Serial.print(thing);
      tot += thing;
    }

    if (tot.toInt() > 0){
      for (int i = 0; i < tot.toInt(); i++){
        digitalWrite(5, HIGH);
        delay(100);
        digitalWrite(5, LOW);
        delay(100);
      }
    }
    

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
    delay(100);
  }
}
