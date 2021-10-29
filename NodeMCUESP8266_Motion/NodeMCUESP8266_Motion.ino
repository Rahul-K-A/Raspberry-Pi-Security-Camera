#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//IMPORTANT: THIS CODE IS SPECIFICALLY WRITTEN FOR THE NODEMCUESP8266 MCU.
//THE CODE MAY HAVE TO BE MODIFIED TO MAKE It COMPATIBLE WITH OTHER MICROCONTROLLERS.

//This project uses an ultrasonic sensor.
//Other sensors are compatible with some slight modifications to the code
const int TriggerPin = 13; // Trigger Pin of Ultrasonic Sensor
const int EchoPin = 12; // Echo Pin of Ultrasonic Sensor

// WiFi
const char *ssid = "YourWifiNameHere"; // Enter your WiFi name
const char *Password = "YourWifiPasswordHere";  // Enter WiFi password

// MQTT Broker
const char *Mqtt_Broker = "Your broker IP Adress here";
const char *Mqtt_Username = "Your MQTT Server username here";
const char *Mqtt_Password = "Your MQTT Server password here";
const char *Topic = "HomeSecurity/Motion";
const int Mqtt_Port = 1883;

void loop() {
   long duration, cm;
   //Send pulse
   pinMode(TriggerPin, OUTPUT);
   digitalWrite(TriggerPin, LOW);
   delayMicroseconds(2);
   digitalWrite(TriggerPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(TriggerPin, LOW);
   //Recieve pulse
   pinMode(EchoPin, INPUT);
   //Get duration of pulse in microseconds
   duration = pulseIn(EchoPin, HIGH);
   //Get distance to object in centi meters
   cm = GetDistanceCm(duration);
   Serial.println(cm);
   delay(100);
}



long GetDistanceCm(long microseconds) {
  //Eqn to convert microseconds to cms
   return microseconds / 58;
}
