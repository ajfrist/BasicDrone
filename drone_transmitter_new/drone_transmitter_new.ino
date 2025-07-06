#include <SPI.h>
#include <LoRa.h>

#define DEBUG 0  // Set to 1 to enable debug prints
#if DEBUG
  #define DEBUG_PRINT(x)    Serial.print(x)
  #define DEBUG_PRINTLN(x)  Serial.println(x)
#else
  #define DEBUG_PRINT(x)    do {} while (0)
  #define DEBUG_PRINTLN(x)  do {} while (0)
#endif


#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

// int JOYSTICK_PIN_X1 = A1;
// int JOYSTICK_PIN_Y1 = A2;
// int JOYSTICK_PIN_X2 = A3;
// int JOYSTICK_PIN_Y2 = A4;
// int THROTTLE_PIN = A5;
// int SWITCH_PIN1 = 2;
// int SWITCH_PIN2 = 3;

// int throttle = 0;
// bool pressed = false;

int LEFT_PIN = A2;
int FORWARD_PIN = 2;
int RIGHT_PIN = 3;
int BACKWARD_PIN = 4;
int UP_PIN = 5;
int DOWN_PIN = 6;
int TRIM_TOGGLE_PIN = 7;
int TRIM_LED_PIN = 8;
int POWER_TOGGLE_PIN = A0;
int POWER_LED_PIN = A1;

bool isTrimming = false;
bool trimPressed = false;
bool isOn = false;
bool powerPressed = false;

long packetCount = 0;

void setup() {
  // pinMode(JOYSTICK_PIN_X1, INPUT);
  // pinMode(JOYSTICK_PIN_Y1, INPUT);
  // pinMode(JOYSTICK_PIN_X2, INPUT);
  // pinMode(JOYSTICK_PIN_Y2, INPUT);
  // pinMode(THROTTLE_PIN, INPUT);
  // pinMode(SWITCH_PIN1, INPUT_PULLUP);
  // pinMode(SWITCH_PIN2, INPUT_PULLUP);

  // initialThrottle = analogRead(THROTTLE_PIN);
  // throttle = initialThrottle;

  pinMode(LEFT_PIN, INPUT_PULLUP);
  pinMode(FORWARD_PIN, INPUT_PULLUP);
  pinMode(RIGHT_PIN, INPUT_PULLUP);
  pinMode(BACKWARD_PIN, INPUT_PULLUP);
  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);
  pinMode(TRIM_TOGGLE_PIN, INPUT_PULLUP);
  pinMode(TRIM_LED_PIN, OUTPUT);
  pinMode(POWER_TOGGLE_PIN, INPUT_PULLUP);
  pinMode(POWER_LED_PIN, OUTPUT);
  
  Serial.begin(9600);
  while (!Serial);
  
  DEBUG_PRINTLN("LoRa Sender");
  if (!LoRa.begin(915E6)) {
    DEBUG_PRINTLN("Starting Lora failed!");
    while (1);
  }
}

void loop() {
  DEBUG_PRINT("Free memory: ");
  DEBUG_PRINTLN(freeMemory());

  int throttle = 1023;
  int trimToggle = 0;
  int power = 0;
  int x1 = 511;
  int y1 = 511;
  int x2 = 511;
  int y2 = 511;

  // Handle toggle switch for trim
  // trim pressed
  if (digitalRead(TRIM_TOGGLE_PIN) == LOW){
    // flip trimming state if button was previously not being pressed
    if (!trimPressed){
      isTrimming = !isTrimming;
    }
    trimPressed = true; // set to being pressed
  }
  // trim not pressed 
  else {
    trimPressed = false; // set to not pressed
  }

  // enable trim LED state indicator
  if (isTrimming){
    digitalWrite(TRIM_LED_PIN, HIGH);
    trimToggle = 1;
  }
  else {
    digitalWrite(TRIM_LED_PIN, LOW);
  }

  // Handle toggle switch for power
  // power pressed
  if (digitalRead(POWER_TOGGLE_PIN) == LOW){
    // flip power state if button was previously not being pressed
    if (!powerPressed){
      isOn = !isOn;
    }
    powerPressed = true; // set to being pressed
  }
  // power not pressed 
  else {
    powerPressed = false; // set to not pressed
  }

  // enable power LED state indicator
  if (isOn){
    digitalWrite(POWER_LED_PIN, HIGH);
    power = 1;
  }
  else {
    digitalWrite(POWER_LED_PIN, LOW);
  }
  
  // Check pressed set standard max values
  if (digitalRead(LEFT_PIN) == LOW){
    x1 = 0;
  }
  if (digitalRead(FORWARD_PIN) == LOW){
    y1 = 1023;
  }
  if (digitalRead(RIGHT_PIN) == LOW){
    x1 = 1023;
  }
  if (digitalRead(BACKWARD_PIN) == LOW){
    y1 = 0;
  }
  if (digitalRead(UP_PIN) == LOW){
    y2 = 1023;
  }
  if (digitalRead(DOWN_PIN) == LOW){
    y2 = 0;
  }
  


  LoRa.beginPacket();
  // LoRa.write(analogRead(THROTTLE_PIN));
  // LoRa.write(analogRead(SWITCH_PIN1));
  // LoRa.write(analogRead(SWITCH_PIN2));
  // LoRa.write(analogRead(JOYSTICK_PIN_X1));
  // LoRa.write(analogRead(JOYSTICK_PIN_Y1));
  // LoRa.write(analogRead(JOYSTICK_PIN_X2));
  // LoRa.write(analogRead(JOYSTICK_PIN_Y2));
  // LoRa.write(throttle, 2);
  // LoRa.write(trimToggle, 2);
  // LoRa.write(switch2, 2);
  // LoRa.write(x1, 2);
  // LoRa.write(y1, 2);
  // LoRa.write(x2, 2);
  // LoRa.write(y2, 2);
  sendInt(throttle);
  sendInt(trimToggle);
  sendInt(power);
  sendInt(x1);
  sendInt(y1);
  sendInt(x2);
  sendInt(y2);
  // LoRa.print("hello");
  LoRa.endPacket();

  
  DEBUG_PRINT("Sent packet ");
  DEBUG_PRINTLN(packetCount);
  packetCount += 1;

//   int packetSize = LoRa.parsePacket();
//   if (packetSize) {
//     // received a packet
//     DEBUG_PRINTLN("  Received packet: ");

//     // read packet
//     while (LoRa.available()) {
//       DEBUG_PRINT("Throttle: ");
//       int val = (int) LoRa.read();
//       DEBUG_PRINTLN(val);
      
// //      DEBUG_PRINT("Motors adjusted: ");
// //      val = (int) LoRa.read();
// //      if (val & 0b0001){
// //        DEBUG_PRINT("1 ");
// //      }
// //      if (val & 0b0010){
// //        DEBUG_PRINT("2 ");
// //      }
// //      if (val & 0b0100){
// //        DEBUG_PRINT("3 ");
// //      }
// //      if (val & 0b1000){
// //        DEBUG_PRINT("4 ");
// //      }
// //      DEBUG_PRINTLN("");

//       DEBUG_PRINT("Current respective throttles: ");
//       DEBUG_PRINT((int)LoRa.read());
//       DEBUG_PRINT((int)LoRa.read());
//       DEBUG_PRINT((int)LoRa.read());
//       DEBUG_PRINTLN((int)LoRa.read());

//       DEBUG_PRINT("Receiver RSSI: ");
//       val = (int) LoRa.read();
//       DEBUG_PRINT(val);
//     }
      
//     DEBUG_PRINT(", Transmitter RSSI: ");
//     DEBUG_PRINTLN(LoRa.packetRssi());
//     DEBUG_PRINTLN("");
//   }
//   else {
//    DEBUG_PRINTLN("---no packet---");
// //    DEBUG_PRINTLN(analogRead(THROTTLE_PIN));
//   }
    

    // print RSSI of packet
//    DEBUG_PRINT("' with RSSI ");
//    DEBUG_PRINTLN(LoRa.packetRssi());


  
}

void sendInt(int val){
  char a[12] = {0};
  snprintf(a, sizeof(a), "%d", val);
  LoRa.print(String(a)+"_");
}
