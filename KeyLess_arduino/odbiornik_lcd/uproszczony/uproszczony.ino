#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>

#define CE_PIN 9
#define CSN_PIN 10

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = {0xF0, 0xF0, 0xF0, 0xF0, 0xE1}; // Taki sam adres jak w nadajniku

struct Payload {
  int code1;
  int code2;
};
Payload payload;

void setup() {
  Serial.begin(9600);

  if (!radio.begin()) {
    Serial.println("Radio initialization failed.");
    while (true);
  }

  radio.openReadingPipe(1, address); // Otwórz adres jako odbiornik
  radio.setPALevel(RF24_PA_MIN);     // Niski poziom mocy
  radio.setDataRate(RF24_1MBPS);     // Ustaw prędkość 1Mbps
  radio.startListening();            // Przełącz na tryb odbioru

  Serial.println("Receiver ready, waiting for data...");
}

void loop() {
  if (radio.available()) {
    radio.read(&payload, sizeof(payload)); // Odczytaj dane do struktury payload
    Serial.print("Received code1: ");
    Serial.println(payload.code1);
    Serial.print("Received code2: ");
    Serial.println(payload.code2);
  }
}