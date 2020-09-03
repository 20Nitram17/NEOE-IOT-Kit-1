/***************************************************************************
  This is a library for the MQ135 air quality sensor. Developed for the
  NEOE-IOT-Kit "Room Air Monitoring with MQ135 & NodeMCU"
  https://www.neoe.io/iot-kit-2_136_1067/
  The library is still under construction and only a beta release.
  Developed with input and suggestions from the community.
  Please share your feedback, thougths and suggestions with us.
  https://www.facebook.com/groups/neoe.io/
  Date of last change: May 5th, 2020
**************************************************************************/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/* Parameters for average calculation */
const int numReadings = 10;
int readings[numReadings]; // the readings from the analog input
int readIndex = 0; // the index of the current reading
float total = 0; // the running total
float average = 0; // the average

/* The load resistance on the board */
#define RLOAD 10.0

/* Parameters for calculating ppm of CO2 from sensor resistance */
#define PARA 116.6020682
#define PARB 2.769034857

/* Atmospheric CO2 level for calibration purposes */
#define ATMOCO2 397.13

/* Analog PIN */
uint8_t _pin = 0;
int pinValue;

/* Parameters for Calibration */
float RCurrent;
float RMax;

/* Parameters for indicator LED */
int PINblue = 12;
int PINgreen = 13;
int PINred = 15;
float LEDgreen;
float LEDred;
float LEDblue;

/* Parameter for notification status */
int notified;

// Update these with values suitable for your network.
const char* ssid = "ADD THE SSID OF YOUR WLAN HERE";
const char* password = "ADD THE PASSWORD OF YOUR WLAN HERE";
const char* mqtt_server = "ADD THE IP ADDRESS OF YOUR MQTT-SERVER HERE";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  randomSeed(micros());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      client.subscribe("inTopic");
    } else {
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {

  /* Prepare array for average calculation */
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  setup_wifi();
  client.setServer(mqtt_server, 1883);

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  /* Read sensor data and create average */ total = total - readings[readIndex]; readings[readIndex] = analogRead(_pin); total = total + readings[readIndex]; readIndex = readIndex + 1; if (readIndex >= numReadings) {
    readIndex = 0;
  }
  average = total / numReadings;

  /* Sensor very volatile, smooth calibration, consider RCurrent with only 0,1% */
  float RCurrent = getRZero();
  if ((((999 * RMax) + RCurrent) / 1000) > RMax) {
    RMax = (((999 * RMax) + RCurrent) / 1000); //Aufgrund der hohen Ausschläge des Sensors behutsames Anpassen - RCurrent zu 0,1% berücksichtigen
  }

  /* Calculate PPM */
  float ppm = getPPM();

  /* Send data to MQTT */
  client.publish("outTopic", String(ppm).c_str());

  /* Indicator LED */
  LEDblue = 0;
  LEDred = (1024 * ((ppm - 500) / 500)) / 3;
  LEDgreen = 1024 - ((1024 * ((ppm - 500) / 500)) / 3);
  if (LEDred > 1024) LEDred = 1024;
  if (LEDred < 0) LEDred = 0; if (LEDgreen > 1024)LEDgreen = 1024;
  if (LEDgreen < 0) LEDgreen = 0; 

  /* Calibration LED indicator. Switch already from blue to green when within 95% range. */
  if (ppm < (0.95 * ATMOCO2)) {
    LEDgreen = 0;
    LEDred = 0;
    LEDblue = 1024;
  } /* After calibration, one measurement per second is enough */ if (ppm > ATMOCO2) delay(1000);

  /* Send values to LEDs */
  analogWrite(PINgreen, LEDgreen);
  analogWrite(PINred, LEDred);
  analogWrite(PINblue, LEDblue);
 
  delay(1000);
}

/* Get the resistance of the sensor, ie. the measurement value. Return the sensor resistance in kOhm */
float getResistance() {
  int val = average;
  return ((1023. / (float)val) - 1.) * RLOAD;
}

/* Get the ppm of CO2 sensed (assuming only CO2 in the air. Return The ppm of CO2 in the air */
float getPPM() {
  return PARA * pow((getResistance() / RMax), -PARB);
}

/* Get the resistance RZero of the sensor for calibration purposes. Return the sensor resistance RZero in kOhm */
float getRZero() {
  return getResistance() * pow((ATMOCO2 / PARA), (1. / PARB));
}
