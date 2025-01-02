#include <SPI.h>
#include <LoRa.h>

int LED_PIN_1 = 18;
int LED_PIN_2 = 19;
int LED_PIN_3 = 2;
int LED_PIN_4 = 3;

//int INCREASE_INPUT_PIN = 4;
//int DECREASE_INPUT_PIN = 5;
int MOTOR_TOGGLE_PIN_1 = 4;
int MOTOR_TOGGLE_PIN_2 = 5;
int MOTOR_TOGGLE_PIN_3 = 6;
int MOTOR_TOGGLE_PIN_4 = 7;

int THROTTLE_PIN = A5;

int motorActive1 = 0;
int motorActive2 = 0;
int motorActive3 = 0;
int motorActive4 = 0;

int initialThrottle = 0;
int throttle = 0;
int throttleBuffer = 30;
bool pressed = false;

void setup() {
  // Init pins 1,2,3,4; set to default on technically 
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  pinMode(LED_PIN_4, OUTPUT);
  digitalWrite(LED_PIN_1, LOW);
  digitalWrite(LED_PIN_2, LOW);
  digitalWrite(LED_PIN_3, LOW);
  digitalWrite(LED_PIN_4, LOW);
  pinMode(MOTOR_TOGGLE_PIN_1, INPUT_PULLUP);
  pinMode(MOTOR_TOGGLE_PIN_2, INPUT_PULLUP);
  pinMode(MOTOR_TOGGLE_PIN_3, INPUT_PULLUP);
  pinMode(MOTOR_TOGGLE_PIN_4, INPUT_PULLUP);
  pinMode(THROTTLE_PIN, INPUT);

  initialThrottle = analogRead(THROTTLE_PIN);
  throttle = initialThrottle;
  
  Serial.begin(9600);
//  while (!Serial);
//  
//  Serial.println("LoRa Sender");
//  if (!LoRa.begin(915E6)) {
//    Serial.println("Starting Lora failed!");
//    while (1);
//  }
}

void loop() {
  // Read each button state for toggling trim
  int toggle1 = digitalRead(MOTOR_TOGGLE_PIN_1);
  int toggle2 = digitalRead(MOTOR_TOGGLE_PIN_2);
  int toggle3 = digitalRead(MOTOR_TOGGLE_PIN_3);
  int toggle4 = digitalRead(MOTOR_TOGGLE_PIN_4);

  // Toggle each motor's active state based on if pressed
  if (!toggle1 and not pressed){ // HIGH == off, LOW == on
    pressed = true;
    motorActive1 = !motorActive1;
  }
  else if (!toggle2){
    pressed = true;
    motorActive2 = !motorActive2;
  }
//  Serial.println(toggle);
  else if (!toggle3){
    pressed = true;
    motorActive3 = !motorActive3;
  }
  else if (!toggle4){
    pressed = true;
    motorActive4 = !motorActive4;
  }
  if (toggle1 and toggle2 and toggle3 and toggle4){
    pressed = false;
  }
  // Turn on LED light indicating state of active motor toggle value
  digitalWrite(LED_PIN_1, motorActive1);
  digitalWrite(LED_PIN_2, motorActive2);
  digitalWrite(LED_PIN_3, motorActive3);
  digitalWrite(LED_PIN_4, motorActive4);


  // Serial DEBUG info to computer Serial monitor for button states
  Serial.println("*** TRIM RECEIVER ***");
  Serial.print(analogRead(MOTOR_TOGGLE_PIN_1)+", ");
  Serial.print(analogRead(MOTOR_TOGGLE_PIN_2)+", ");
  Serial.print(analogRead(MOTOR_TOGGLE_PIN_3)+", ");
  Serial.println(analogRead(MOTOR_TOGGLE_PIN_4));
  

  // Transmit active motor and trim amount information
  LoRa.beginPacket();

  // Get current throttle position; if abs > threshold, start tracking
  throttle = analogRead(THROTTLE_PIN);
  // if (throttle > initialThrottle){
  //   throttle -= throttleBuffer;
  // } else {
  //   throttle += throttleBuffer;
  // }
  if (throttle > initialThrottle + throttleBuffer){
    LoRa.write(map(throttle, 1023-(initialThrottle+30)*2, 1023, -10, 10));
  }

  // Map trim value to (-10,10) if throttle exceeds buffer threshold, otherwise -100
  if(motorActive1 and abs(throttle - initialThrottle) > throttleBuffer){
    LoRa.write(map(throttle, 1023-initialThrottle*2, 1023, -10, 10));
  } else {LoRa.write(-100);}

  if(motorActive2 and abs(throttle - initialThrottle) > throttleBuffer){
    LoRa.write(map(throttle, 1023-initialThrottle*2, 1023, -10, 10));
  } else {LoRa.write(-100);}

  if(motorActive3 and abs(throttle - initialThrottle) > throttleBuffer){
    LoRa.write(map(throttle, 1023-initialThrottle*2, 1023, -10, 10));
  } else {LoRa.write(-100);}

  if(motorActive4 and abs(throttle - initialThrottle) > throttleBuffer){
    LoRa.write(map(throttle, 1023-initialThrottle*2, 1023, -10, 10));
  } else {LoRa.write(-100);}  

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
      
      Serial.print("Motors adjusted: ");
      val = (int) LoRa.read();
      if (val & 0b0001){
        Serial.print("1 ");
      }
      if (val & 0b0010){
        Serial.print("2 ");
      }
      if (val & 0b0100){
        Serial.print("3 ");
      }
      if (val & 0b1000){
        Serial.print("4 ");
      }
      Serial.println("");

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
      

      print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());


  
}
