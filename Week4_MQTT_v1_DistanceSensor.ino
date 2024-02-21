#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include "Adafruit_VL53L0X.h"

WiFiClient wifi;
MqttClient mqtt(wifi);

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
int measurement;
const int sendInterval = 2;

//WiFi network info: ssid and password
const char wifi_ssid[] = "sandbox370";
const char wifi_pass[] = "+s0a+s03!2gether?";

// //MQTT broker info: url and port (1883 default for MQTT)
// const char broker[] = "andrenet3000.cloud.shiftr.io";
// const int  port = 1883;

// //if needed: broker authentication credentials
// const char mqtt_user[] = "";
// const char mqtt_pass[] = "";

//MQTT broker info: url and port (1883 default for MQTT)
const char broker[] = "theyonetwork.cloud.shiftr.io";
const int  port = 1883;

//if needed: broker authentication credentials
const char mqtt_user[] = "theyonetwork";
const char mqtt_pass[] = "ConnDevSP24";

//the topic this device will publish messages to
const char pubTopic[] = "TOF_Sensor_Data";

void setup() {
  Serial.begin(9600);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    WiFi.begin(wifi_ssid, wifi_pass);
    delay(1000);
  }
  Serial.println("\nWiFi connected!");

//Sensor debugging stuff
    Serial.println("Adafruit VL53L0X test");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  } 
  Serial.println(F("VL53L0X API Simple Ranging example\n\n")); 

  //give your device any name, to use for identification
  mqtt.setId("AndreArduino");

  //set mqtt credentials, if needed
  mqtt.setUsernamePassword(mqtt_user, mqtt_pass);

  if (!mqtt.connect(broker, port)) {
    //error codes
    //  -1: credentials rejected
    //  -2: can't connect to broker
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqtt.connectError());
    while (true) {}; //do nothing forever
  } else Serial.println("MQTT connected.");
}

void loop() {

    VL53L0X_RangingMeasurementData_t measure;

      // Serial.print("Reading a measurement... ");
    lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

    if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    measurement = int(measure.RangeMilliMeter);

    } else {
      //Serial.println(" out of range ");
      //measurement = 0;
    }

    Serial.println(measurement);

  if (millis() % sendInterval < 10) {
    mqtt.beginMessage(pubTopic);
    mqtt.print(measurement);
    mqtt.endMessage();
    delay(10);
  }
  
}