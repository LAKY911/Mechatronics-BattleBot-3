#include <HardwareSerial.h>
#include <WiFi.h>
#include <PubSubClient.h>


// Update these with values suitable for your network.
const char* ssid = "wifi";
const char* password = "ahmed234";
const char* mqtt_server = "broker.mqtt-dashboard.com";
#define mqtt_port 8884
#define MQTT_USER "test"
#define MQTT_PASSWORD "test"
#define MQTT_SERIAL_PUBLISH_CH "testtopic"

WiFiClient wifiClient;

PubSubClient client(wifiClient);

void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    client.publish("outTopic", "hello world");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) {
      Serial.println("connected");
      //Once connected, publish an announcement...
      client.publish("testtopic", "hello world");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void setup() {
  Serial.begin(115200);
  Serial.setTimeout(500);// Set time out for 
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  reconnect();
}

void publishSerialData(char *serialData){
  if (!client.connected()) {
    reconnect();
  }
  client.publish(MQTT_SERIAL_PUBLISH_CH, serialData);
}
void loop() {
   client.loop();
   if (Serial.available() > 0) {
     char bfr[501];
     memset(bfr,0, 501);
     Serial.readBytesUntil( '\n',bfr,500);
     publishSerialData(bfr);
   }
 }