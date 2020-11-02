/**********************************************************************************************************************************
  Arduino-Sketch für das NEOE-IOT-Kit-1, "Smart Home Luftqualitätssensor mit MQ-135 und NodeMCU. Reagiert auf CO2 und andere Gase.
  Arduino-Programmierung. MQTT-kompatibel zur Anbindung an Home Assistant."
  Dieser Arduino-Sketch wird in den folgenden beiden Tutorials verwendet:
  https://www.neoe.io/blogs/tutorials/luftqualitatssensor-reagiert-auf-co2-mqtt-kompatibel-aufbau-variante-breadboard
  https://www.neoe.io/blogs/tutorials/luftqualitatssensor-reagiert-auf-co2-mqtt-kompatibel-aufbau-variante-leiterplatte
  Fragen und Anregungen bitte in unserer Facebook-Gruppe adressieren, damit die gesamte Community davon profitiert.
  https://www.facebook.com/groups/neoe.io/
  Datum der letzten Änderung: 27. Oktober, 2020
**********************************************************************************************************************************/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Parameter zur Berechnung der Durchschnittswerte
const int numReadings = 10;
int readings[numReadings]; // Array zur Berechnung der Durchschnittswerte
int readIndex = 0; // Index eines einzelnen Wertes
float total = 0; // Summe aller Werte
float average = 0; // Durchschnittswert

// Lastwiderstand des MQ-135
#define RLOAD 10.0

// Parameter zur CO2 Berechnung in ppm auf Basis des Sensor-Widerstands
#define PARA 116.6020682
#define PARB 2.769034857

// CO2-Wert in der Atmosphäre, als Basis für die Kalibrierung
#define ATMOCO2 397.13

// Alle 10 Sekunden eine MQTT-Nachricht senden
int delay_time = 10000;

// Analog PIN
uint8_t _pin = 0;
int pinValue;

// Für die Kalibrierung erforderliche Parameter
float RCurrent;
float RMax;

// Parameter für die Indikator-LED
int PINblue = 12;
int PINgreen = 13;
int PINred = 15;
float LEDgreen;
float LEDred;
float LEDblue;

// WLAN-Zugangsdaten hier hinterlegen
const char* ssid = "NAME DES WLAN NETZWERKS"; // Anführungszeichen beibehalten
const char* password = "WLAN-PASSWORT"; // Anführungszeichen beibehalten, also z.B. so: "Geheim"

// Die für den MQTT-Server erforderlichen Daten hier hinterlegen
const char* mqtt_client = "NEOE-IOT-KIT-1-1"; // Wenn mehrere "NEOE IOT-Kits 1" im Einsatz sind, einfach mit der letzten Zahl durchnummerieren
const char* mqttServer = "IP-ADRESSE DES MQTT-SERVERS"; // Anführungszeichen beibehalten, also z.B. so: "192.168.0.236"
const uint16_t mqtt_port = 1883;
const char* mqtt_user = "BENUTZERNAME"; // Anführungszeichen beibehalten
const char* mqtt_password = "PASSWORT"; // Anführungszeichen beibehalten, also z.B. so: "Geheim"

// MQTT-Topic für Home Assistant MQTT Auto Discovery
const char* mqtt_config_topic = "homeassistant/sensor/co2-esszimmer/config"; // Name des Zimmers bei Bedarf ändern
const char* mqtt_state_topic = "homeassistant/sensor/co2-esszimmer/state"; // Name des Zimmers bei Bedarf ändern

// Speicher-Reservierung für JSON-Dokument, kann mithilfe von arduinojson.org/v6/assistant noch optimiert werden
StaticJsonDocument<512> doc_config;
StaticJsonDocument<512> doc_state;

char mqtt_config_data[512];
char mqtt_state_data[512];

bool configured = false;

WiFiClient espClient;
PubSubClient client(espClient);

// Funktion um CO2-Werte per MQTT zu übermitteln
void publishData(float p_co2) {
  doc_state["co2"] = round(p_co2);
  serializeJson(doc_state, mqtt_state_data);
  client.publish(mqtt_state_topic, mqtt_state_data);
}

// Funktion zur WLAN-Verbindung
void setup_wifi() {
  delay(10);
  /* Mit WLAN verbinden */
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

// Funktion zur MQTT-Verbindung
void reconnect() {
  while (!client.connected()) {
    if (client.connect(mqtt_client, mqtt_user, mqtt_password)) {
    } else {
      delay(5000);
    }
  }
}

// Funktion für Home Assistant MQTT Auto Discovery
void configMqtt() {
  doc_config["name"] = "CO2-Wert Esszimmer";  // Name des Zimmers bei Bedarf ändern
  doc_config["state_topic"] = mqtt_state_topic;
  doc_config["unit_of_measurement"] = "ppm";
  doc_config["value_template"] = "{{ value_json.co2}}";
  serializeJson(doc_config, mqtt_config_data);
  client.publish(mqtt_config_topic, mqtt_config_data, true);
  delay(1000);
}

// Widerstand des Sensors (in Kiloohm) berechnen
float getResistance() {
  int val = average;
  return ((1023. / (float)val) - 1.) * RLOAD;
}

// CO2 Wert in ppm berechnen. Auf Basis der Vereinfachung, dass sich nur CO2 in der Luft befindet.
float getPPM() {
  return PARA * pow((getResistance() / RMax), -PARB);
}

// RZero Widerstand des Sensors (in Kiloohm) für die Kalibrierung berechnen
float getRZero() {
  return getResistance() * pow((ATMOCO2 / PARA), (1. / PARB));
}

void setup() {
  // Array zur Berechnung der Durchschnittswerte aufbauen
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setBufferSize(512);
  if (!client.connected()) reconnect();
  configMqtt();
}

void loop() {

  if (!client.connected()) reconnect();

  // Sensor auslesen und Durchschnittswerte berechnen
  total = total - readings[readIndex]; readings[readIndex] = analogRead(_pin); total = total + readings[readIndex]; readIndex = readIndex + 1; if (readIndex >= numReadings) {
    readIndex = 0;
  }
  average = total / numReadings;

  // Volatiler Sensor, daher sanfte Kalibrierung, RCurrent nur mit 0,001%
  float RCurrent = getRZero();
  if ((((9999 * RMax) + RCurrent) / 10000) > RMax) {
    RMax = (((9999 * RMax) + RCurrent) / 10000);
  }

  // ppm berechnen
  float ppm = getPPM();

  // Nach Abschluss der Kalibierung beginnen, Daten via MQTT zu übermitteln
  if (ppm > ATMOCO2)  publishData(ppm);

  // Indikator-LED
  LEDblue = 0;
  LEDred = (1024 * ((ppm - 500) / 500)) / 3;
  LEDgreen = 1024 - ((1024 * ((ppm - 500) / 500)) / 3);
  if (LEDred > 1024) LEDred = 1024;
  if (LEDred < 0) LEDred = 0; if (LEDgreen > 1024)LEDgreen = 1024;
  if (LEDgreen < 0) LEDgreen = 0;

  // Indikator-LED schaltet von Blau (Kalibrierung) auf Grün, sobald Daten im 95% Bereich
  if (ppm < ATMOCO2) {
    LEDgreen = 0;
    LEDred = 0;
    LEDblue = 1024;
  }

  // Nach erfolgter Kalibrierung Anzahl der Messungen reduzieren, da sonst zu viele MQTT-Meldungen versendet werden
  if (ppm > ATMOCO2) delay(delay_time);

  // Werte an Indikator-LED senden
  analogWrite(PINgreen, LEDgreen);
  analogWrite(PINred, LEDred);
  analogWrite(PINblue, LEDblue);

}
