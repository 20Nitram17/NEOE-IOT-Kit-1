# NEOE-IOT-Kit-1
Arduino-Sketch-Sammlung für das NEOE-IOT-Kit-1: *Smart Home Luftqualitätssensor mit MQ-135 und NodeMCU. Reagiert auf CO2 und andere Gase. Arduino-Programmierung. MQTT-kompatibel zur Anbindung an Home Assistant. Aufbau-Variante "Leiterplatte"*.

Siehe: https://www.neoe.io/products/smarthome-iot-bausatz-luftqualitatssensor-mq-135-nodemcu-arduino-pcb

Es gibt unterschiedliche Varianten des Sketches:

## In diesem Ordner

### NEOE-IOT-Kit-1-Home-Assistant.ino
- MQTT-kompatibel zur Anbindung an Home Assistant
- Dieser Arduino-Sketch wird in folgendem Tutorial verwendet: https://www.neoe.io/blogs/tutorials/luftqualitatssensor-reagiert-auf-co2-mqtt-kompatibel-aufbau-variante-leiterplatte

## In dem Ordner "Varianten"

### NEOE-IOT-Kit-1-Blynk.ino
- Blynk-kompatibel.
- Vorteil: wenn keine Smarthome-Zentrale (z.B. Home Assistant) in Betrieb ist, kann das NEOE-IOT-Kit-1 trotzdem genutzt werden.
- Dieser Arduino-Sketch wird in folgendem Tutorial verwendet: https://www.neoe.io/blogs/tutorials/luftqualitatssensor-reagiert-auf-co2-blynk-kompatibel-aufbau-variante-leiterplatte
- Dies ist eine veraltete Variante, sie wird aktuell nicht  weiterentwickelt.

### NEOE-IOT-Kit-1-Standalone.ino
- Diese Variante kann genutzt werden, wenn kein WLAN zur Verfügung ist.
- Darüber hinaus ändert die Indikator-LED hier nicht fließend die Farbe. Die Farbe "springt" je nach Messwert zwischen "rot", "gelb", "grün".
- Dies ist eine experimentelle Variante, sie wird aktuell nicht  weiterentwickelt.

## Fragen und Anregungen? 
Bitte in unserer Facebook-Gruppe adressieren, damit die gesamte Community davon profitiert.
https://www.facebook.com/groups/neoe.io/
