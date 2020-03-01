/*
 * Description:  Control LED PWM with AskSensors and ESP8266 dev board over MQTT
 *  Author: https://asksensors.com, 2020
 *  github: https://github.com/asksensors
 */
 

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

//TODO: ESP8266 MQTT user config
const char* ssid = ".............."; // Wifi SSID
const char* password = ".............."; // Wifi Password
const char* username = ".............."; // my AskSensors username
const char* subTopic = "actuator/............../.............."; // actuator/username/apiKeyOut
// PWM config
const int PWM_pin = 2;   // D4
float pwm_cmd = 0;

//AskSensors MQTT config
const char* mqtt_server = "mqtt.asksensors.com";
unsigned int mqtt_port = 1883;

WiFiClient askClient;
PubSubClient client(askClient);

void setup() {
  Serial.begin(115200);
  Serial.println("*****************************************************");
  Serial.println("********** Program Start : Controling ESP32 PWM with AskSensors over MQTT");
  Serial.println("PWM setup");
  pinMode(PWM_pin,OUTPUT);
  
  Serial.print("********** connecting to WIFI : ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("->WiFi connected");
  Serial.println("->IP address: ");
  Serial.println(WiFi.localIP());
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
    if (!client.connected()) 
    reconnect();
  Serial.print("********** Subscribing to AskSensors actuator topic:");
  Serial.print(subTopic);
  // susbscribe
  client.subscribe(subTopic);
  
}

void loop() {

  client.loop();
  analogWrite(PWM_pin, pwm_cmd);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("\nCommand received from AskSensors[");
  Serial.print(topic);
  Serial.print("] ");
  
   char command_str[8];
   char module_str[length-8];
  Serial.print("\nModule: ");
  for (int i = 0; i < 8; i++) {
    module_str[i] = (char)payload[i];
      Serial.print((char)module_str[i]);
  }
    Serial.print("\ncommand: ");
    for (int i = 0; i < length-8; i++) {
    command_str[i] = (char)payload[i+8];
   Serial.print((char)command_str[i]);
  }
  
  Serial.println("\n********** Parsing Actuator command");
  if(strncmp((char *)module_str, (char*) "module1=",8)== 0){ 
    pwm_cmd = (float)atoi((char*)command_str);
    // conversion from max 100% = 1023 PWM to 10-bits output
    pwm_cmd *= 1023.0/100;
    Serial.print("PWM Ratio:");
    Serial.print(pwm_cmd);
    Serial.println("%");
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("********** Attempting MQTT connection...");
    // Attempt to connect
if (client.connect("ESP8266Client", username, "")) { 
      Serial.println("-> MQTT client connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("-> try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
