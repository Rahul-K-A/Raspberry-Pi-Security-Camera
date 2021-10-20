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
const char *Mqtt_Username = "Your MQTT Server username here (not mandatory if you are just publishing)";
const char *Mqtt_Password = "Your MQTT Server password here (not mandatory if you are just publishing) ";
const char *Topic = "HomeSecurity/Motion";
const int Mqtt_Port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
   Serial.begin(9600); // Starting Serial Terminal
   //Connect to Wifi
    WiFi.begin(ssid, Password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Done");
   
    
    //Connect to Broker
    client.setServer(Mqtt_Broker, Mqtt_Port);
    while (!client.connected())
    {
      String client_id = "esp8266-client-";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str(), Mqtt_Username, Mqtt_Password)) {} 
      else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
    }
}

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
   if(cm<=50.0f)
   {
     Serial.println("MOTION DETECTED");
     client.publish(Topic, "MOTION DETECTED");
     //delay for 10 seconds to allow camera to finish capturing video
     delay(10000);
   }
   else{
    Serial.println("NO MOTION DETECTED");
    client.publish(Topic, "NO MOTION DETECTED");
    }
   delay(100);
}



long GetDistanceCm(long microseconds) {
   return microseconds / (29*2);
}
