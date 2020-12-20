# NEOE-IOT-Kit-1
Arduino-Sketch-Sammlung für Luftqualitätssensor mit MQ-135 und NodeMCU. Reagiert auf CO2 und andere Gase. 

Es gibt unterschiedliche Varianten des Sketches:

## NEOE-IOT-Kit-1-Home-Assistant.ino
- MQTT-kompatibel zur Anbindung an Home Assistant
- Dies ist die **Hauptvariante**, sie wird laufend weiterentwickelt
- Dieser Arduino-Sketch wird in folgendem Tutorial verwendet: https://www.neoe.io/blogs/tutorials/luftqualitatssensor-reagiert-auf-co2-mqtt-kompatibel-aufbau-variante-leiterplatte

## NEOE-IOT-Kit-1-Blynk.ino
- Blynk-kompatibel.
- Vorteil: wenn keine Smarthome-Zentrale (z.B. Home Assistant) in Betrieb ist, kann das NEOE-IOT-Kit-1 trotzdem genutzt werden.
- Dies ist eine **Nebenvariante**, sie wird aktuell nicht  weiterentwickelt.
- Dieser Arduino-Sketch wird in folgendem Tutorial verwendet: https://www.neoe.io/blogs/tutorials/luftqualitatssensor-reagiert-auf-co2-blynk-kompatibel-aufbau-variante-leiterplatte

## NEOE-IOT-Kit-1-Standalone.ino
- Diese Variante kann genutzt werden, wenn kein WLAN zur Verfügung ist.
- Darüber hinaus ändert die Indikator-LED hier nicht fließend die Farbe. Die Farbe "springt" je nach Messwert zwischen "rot", "gelb", "grün".
- Dies ist eine **Nebenvariante**, sie wird aktuell nicht  weiterentwickelt.

# Fragen und Anregungen? 
Bitte in unserer Facebook-Gruppe adressieren, damit die gesamte Community davon profitiert.
https://www.facebook.com/groups/neoe.io/
