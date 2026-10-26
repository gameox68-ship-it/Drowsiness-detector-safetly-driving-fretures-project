#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const int EYE_SENSOR_PIN = 2; // Green Ethernet wire
const int BUZZER_PIN     = 4; // Solid Blue Ethernet wire
RF24 radio(9, 10);            // CE -> D9, CSN -> D10
const byte address[6] = "00001";

const int EYES_CLOSED_STATE = LOW; 
const unsigned long DROWSY_DELAY_MS = 1500; 

unsigned long eyesClosedStartTime = 0;
bool isCurrentlyClosed = false;

void setup() {
  pinMode(EYE_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); 

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
}

void loop() {
  int sensorValue = digitalRead(EYE_SENSOR_PIN);
  bool sendGoSignal = true; 

  if (sensorValue == EYES_CLOSED_STATE) {
    if (!isCurrentlyClosed) {
      isCurrentlyClosed = true;
      eyesClosedStartTime = millis();
    }
    if (millis() - eyesClosedStartTime >= DROWSY_DELAY_MS) {
      sendGoSignal = false; 
      digitalWrite(BUZZER_PIN, HIGH); // Alarm ON
    }
  } else {
    isCurrentlyClosed = false;
    sendGoSignal = true; 
    digitalWrite(BUZZER_PIN, LOW); // Alarm OFF
  }

  uint8_t command = sendGoSignal ? 1 : 0;
  radio.write(&command, sizeof(command));
  delay(50);
}