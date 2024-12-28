#include <RF24.h>
#include <nRF24L01.h>
#include <RF24_config.h>
#include <SPI.h>

#define CE_PIN 9
#define CSN_PIN 10

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = {0xF0, 0xF0, 0xF0, 0xF0, 0xE1}; // Address for communication

struct Payload {
  int code1;
  int code2;
};
Payload payload;

void setup() {
  Serial.begin(9600); // Initialize serial communication
  
  // Initialize the radio and check for success
  if (!radio.begin()) {
    Serial.println("Radio initialization failed. Please check connections.");
    while (true); // Stop the program if radio initialization fails
  }
  
  // Check if the chip is connected
  if (radio.isChipConnected()) {
    Serial.println("Radio is connected and working.");
  } else {
    Serial.println("Radio is not connected properly.");
    while (true); // Stop the program if radio is not connected
  }
  
  // Configure the radio
  radio.openWritingPipe(address); // Set the address
  radio.setPALevel(RF24_PA_MIN);  // Set power level to low for short range
  radio.setDataRate(RF24_250KBPS);    // Set data rate
  radio.stopListening();           // Set module to transmit mode
  
  // Debugging: Print current radio configuration
  Serial.print("Data Rate: ");
  Serial.println(radio.getDataRate()); 
  Serial.print("PA Level: ");
  Serial.println(radio.getPALevel());

  Serial.println("Enter code 1 and code 2 to transmit:");
}

void loop() {

payload.code1 = 5;
payload.code2 = 5;
  /*
  // Ask for code1
  Serial.print("Please enter code 1: ");
  while (Serial.available() == 0) {
    // Wait for input
  }
  payload.code1 = Serial.parseInt(); // Read code1 as an integer
  Serial.println(payload.code1); // Echo back the entered code1

  // Ask for code2
  Serial.print("Please enter code 2: ");
  while (Serial.available() == 0) {
    // Wait for input
  }
  payload.code2 = Serial.parseInt(); // Read code2 as an integer
  Serial.println(payload.code2); // Echo back the entered code2
*/
  // Transmit payload at least 5 times with a 3-second interval
  for (int i = 0; i < 100; i++) {
    bool success = radio.write(&payload, sizeof(payload));
    if (success) {
      Serial.println("Transmission successful!");
      payload.code1 = payload.code1 + 1;
      payload.code2 = payload.code2 + 3;
    } else {
      Serial.println("Transmission failed.");
    }
    delay(500); // Wait 3 seconds before next transmission
  }

  Serial.println("All transmissions completed.");
  delay(1000); // Wait before allowing new inputs and sending again
}
