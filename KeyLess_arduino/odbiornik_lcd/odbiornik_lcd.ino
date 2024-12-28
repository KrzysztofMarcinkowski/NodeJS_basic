#include <RF24.h>
#include <LiquidCrystal.h>
#include <SPI.h>

#define CE_PIN 9
#define CSN_PIN 10

// LCD pin configuration
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = {0xF0, 0xF0, 0xF0, 0xF0, 0xE1}; // Address for communication

struct Payload {
  int data1;
  int data2;
};
Payload payload;

const int ledPin = 8; // LED connected to pin 8
unsigned long lastDataTime = 0; // Store the time of the last radio data reception
const unsigned long timeout = 500; // 500 milliseconds (half a second)

void setup() {
  lcd.begin(16, 2); // Initialize the LCD's columns and rows
  Serial.begin(9600); // Initialize serial communication

  pinMode(ledPin, OUTPUT); // Set LED pin as an output

  if (!radio.begin()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Radio Error");
    while (true); // Halt execution if radio initialization fails
  }

  // Radio configuration
  radio.openReadingPipe(1, address); // Set the receiver address
  radio.setPALevel(RF24_PA_MIN);     // Set low power mode
  radio.setDataRate(RF24_250KBPS);     // Set data rate
  radio.startListening();            // Start listening for incoming data

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Receiver Ready");
  delay(2000); // Brief display of initialization message
}

void loop() {
  if (radio.available()) {
    radio.read(&payload, sizeof(payload)); // Read incoming data

    // Display data on Serial Monitor
    Serial.print("Data1: ");
    Serial.println(payload.data1);
    Serial.print("Data2: ");
    Serial.println(payload.data2);

    // Display data on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Data1: ");
    lcd.print(payload.data1);
    lcd.setCursor(0, 1);
    lcd.print("Data2: ");
    lcd.print(payload.data2);

    // Update last data time and turn LED on
    lastDataTime = millis();
    digitalWrite(ledPin, HIGH); // Turn on LED when data is received
  }

  // Check if more than half a second has passed since last data reception
  if (millis() - lastDataTime > timeout) {
    digitalWrite(ledPin, LOW); // Turn off LED if no data received for half a second
  }
}
