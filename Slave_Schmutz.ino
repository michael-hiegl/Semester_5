#define RELAISPIN 16
#define POTI A0

#include <ESP8266WiFi.h>
#include <espnow.h>

uint8_t newMacAddress[] = { 0x96, 0x3B, 0xC7, 0x34, 0x69, 0x03 };
uint8_t receiverAddress[] = { 0x94, 0x3C, 0xC6, 0x33, 0x68, 0x02 };

// uint8_t newMacAddress[] = { 0x97, 0x3C, 0xC8, 0x35, 0x70, 0x03 };
// uint8_t receiverAddress[] = { 0x93, 0x3B, 0xC5, 0x32, 0x67, 0x01 };

// Functions ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void onoff(uint8_t* macAddr, uint8_t* incomingData, uint8_t len) {  // Nimmt empfangene Nachricht als Argument schaltet je nach Inhalt das Relais HIGH oder LOW
  if (incomingData[0] == 'E') {
    digitalWrite(RELAISPIN, HIGH);
  }
  if (incomingData[0] == 'A') {
    digitalWrite(RELAISPIN, LOW);
  }
}

void peercheck(uint8_t* mac_addr, uint8_t sendStatus) {  // ueberprueft, ob der Teilnehmer erreicht werden konnte. Schaltet das Relais ab, wenn dies nicht der Fall ist
  if (sendStatus != 0) {
    digitalWrite(RELAISPIN, LOW);
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  pinMode(RELAISPIN, OUTPUT);                   // Definieren von Relaispin als Output
  pinMode(LED_BUILTIN, OUTPUT);                 // Definieren von eingebauter LED als Output
  digitalWrite(RELAISPIN, LOW);                 // Zusperren im Fall eines Stromausfalls und Wiederanlaufs
  WiFi.mode(WIFI_STA);                          // Initialisieren des Boards als Wifi-Station
  wifi_set_macaddr(STATION_IF, newMacAddress);  // Vergeben einer MAC an das Board
  WiFi.disconnect();                            // Deinitialisieren des Boards als Standard-Wifi-Device

  if (esp_now_init() != 0) {  // Initialisieren des Boards als ESPnow-Wifi-Device und Abfrage ob erfolgreich durch Ansteuern der eingebauten LED
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }

  esp_now_add_peer(receiverAddress, ESP_NOW_ROLE_COMBO, 0, NULL, 0);  // ESP32 als Kommunikationspartner hinzufuegen. Dazu werden MAC, Rolle, Channel, Schluessel und Schluessellaenge uebergeben

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);  // ESP8266 zum Transciever machen

  esp_now_register_recv_cb(onoff);      // Fuehrt bei Empfangen einer Nachricht die Interrupt-Funktion onoff aus. Definiert in ESPnow erhaelt diese als Argumente die Sender-MAC, die Nachricht und die Laenge der Nachricht
  esp_now_register_send_cb(peercheck);  // Fuehrt bei Senden einer Nachricht die Interrupt-Funktion peercheck aus. Definiert in ESPnow erhaelt diese als Argumente die Empfaenger-MAC, und den Erfolgsstatus des Versendens
}

void loop() {
  uint8_t potiwert = analogRead(POTI) / 5;  // Einlesen des Potiwertes an Analogpin und runterskalieren des Wertes

  esp_now_send(receiverAddress, (uint8_t*)&potiwert, sizeof(potiwert));  // Versenden des Potiwertes

  delay(200);
}
