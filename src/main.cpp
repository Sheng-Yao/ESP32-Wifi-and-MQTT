#include <Arduino.h>

//include library for wifi functionalities
#include <WiFi.h>

//MQTT library
#include <PubSubClient.h>

const char* ssid = "XXX"; //wifi name
const char* password = "XXX"; //wifi password
const char* serverAddress = "192.168.0.101";  //ip address for MQTT (target)

//ip address need to be up-to-date
const int serverPort = 1883;
//topic that message needed to publish into
const char* mqttPublishTopic = "testing";
//topic that messahe needed to be subscribe to (listening any input from this topic)
const char* mqttSubscribeTopic = "home";
//id for esp32 (must be unique for each client that connected to the same mqtt otherwise mqtt will crashed)
const char* mqttId = "ESP32-Client";

//create wifi instance for esp32
WiFiClient espClient;
//setup MQTT instance
PubSubClient mqttClient(espClient);

//intialize wifi connection function
void initWiFi();

//function that will be called when massage sent into esp32
void callback(String topic, byte *payload, unsigned int length) {

  // temperory message holder
  String payloadOutput = "";

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");

  //using the for loop to print out the payload of the topic that being published
  for (unsigned int i = 0; i < length; i++) {
    //record the message input
    payloadOutput += (char)payload[i];
  }

  Serial.println(payloadOutput);

  Serial.println();
  Serial.println("-----------------------");
}

void setup() {

  //initialize serial monitor for debuging
  Serial.begin(115200);
  while (!Serial);

  //initialize wifi
  initWiFi();

  // Connect to MQTT broker
  mqttClient.setServer(serverAddress, serverPort);

  //callback function being triggered if there are any message being send out
  mqttClient.setCallback(callback);

  while (!mqttClient.connected()) {

    //try to connect to mqtt
    if (mqttClient.connect(mqttId)) {
      Serial.println("Connected to MQTT broker!");
    }
    else {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.println(mqttClient.state());
      delay(500);
    }
  }

  //subsribe topic from mqtt
  mqttClient.subscribe(mqttSubscribeTopic);
}

//counter (to generate unique message output)
int counter = 0;
//timer
unsigned long duration = millis();

void loop() {

  if(millis()-duration >= 5000){

    //generating message output to mqtt
    String messageOutput = "Testing-" + counter;

    //sent message to particular mqtt topic
    mqttClient.publish(mqttPublishTopic, messageOutput.c_str());

    //counter increament
    counter++;

    //update duration variable
    duration = millis();
  }

  //keep on monitoring if any message being sent in
  mqttClient.loop();

  delay(5);
}


void initWiFi() {

  //set esp32 as station (to connect to access point)
  WiFi.mode(WIFI_STA);

  //start to connect to wifi
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi ..");

  //try to connect to access point
  while (WiFi.status() != WL_CONNECTED) {

    Serial.print('.');
    delay(250);
  }

  Serial.println(WiFi.localIP());
}