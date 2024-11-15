#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include <LiquidCrystal.h>
#include <SPI.h>

#define CE_PIN 9
#define CSN_PIN 10
#define INTERVAL_MS_SIGNAL_LOST 3000 // Interval for lost connection (increased to 3 seconds)
#define CHECK_INTERVAL_MS 500 // Interval to check data availability

// LCD pin configuration
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";

struct Payload {
  int data1;
  int data2;
};
Payload payload;

unsigned long lastSignalMillis = 0;
unsigned long lastCheckMillis = 0;

void setup() {
  lcd.begin(16, 2); // Set up the LCD's columns and rows
  Serial.begin(9600); // Initialize serial communication

  // Attempt to initialize radio module
  if (radio.begin()) {
    // Display "Radio OK" on LCD if initialization is successful
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Radio OK");
    delay(5000); // Display the message for 5 seconds

    // Set additional radio configurations
    radio.setAutoAck(false);
    radio.setDataRate(RF24_250KBPS);
    radio.setPALevel(RF24_PA_MIN);
    radio.setPayloadSize(sizeof(payload));
    radio.openReadingPipe(0, address);
    radio.startListening();
  } else {
    // Display error message if initialization fails
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Radio Error");
    while (true) {
      // Halt execution if the radio failed to initialize
    }
  }
}

void loop() {
  unsigned long currentMillis = millis();

  // Check data availability at set intervals
  if (currentMillis - lastCheckMillis >= CHECK_INTERVAL_MS) {
    lastCheckMillis = currentMillis;

    if (radio.available()) {
      // Read data from radio
      radio.read(&payload, sizeof(payload));

      // Display received data on the serial monitor
      Serial.println("Received");
      Serial.print("Data1: ");
      Serial.println(payload.data1);
      Serial.print("Data2: ");
      Serial.println(payload.data2);

      // Display received data on the LCD
      lcd.clear();
      lcd.setCursor(0, 0); // Display data1 on the first line
      lcd.print("Data1: ");
      lcd.print(payload.data1);

      lcd.setCursor(0, 1); // Display data2 on the second line
      lcd.print("Data2: ");
      lcd.print(payload.data2);

      lastSignalMillis = currentMillis; // Update the last signal time
    }
  }

  // Check for connection loss if no signal for a defined interval
  if (currentMillis - lastSignalMillis > INTERVAL_MS_SIGNAL_LOST) {
    lostConnection();
  }
}

void lostConnection() {
  Serial.println("We have lost connection, preventing unwanted behavior");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connection Lost");
  delay(1000); // Brief delay before next check for connection
}
