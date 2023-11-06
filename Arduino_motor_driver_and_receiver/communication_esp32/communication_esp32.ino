#include "Arduino.h"
#include <WiFi.h>
#include "esp_log.h"
#include "esp_system.h"
#include "esp_event.h"
#include "mqtt_client.h"

#define SECURE_MQTT // Comment this line if you are not using MQTT over SSL

#ifdef SECURE_MQTT
#include "esp_tls.h"

// Let's Encrypt CA certificate. Change with the one you need
static const unsigned char DSTroot_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";
#endif // SECURE_MQTT

esp_mqtt_client_config_t mqtt_cfg;
esp_mqtt_client_handle_t client;

const char* WIFI_SSID = "NTB-DELL-G5 6364";
const char* WIFI_PASSWD = "deltarobot";

const char* MQTT_HOST = "Your_broker_address296e152addcb45699e5e68d8e8926a66.s1.eu.hivemq.cloud";
#ifdef SECURE_MQTT
const uint32_t MQTT_PORT = 8883;
#else
const uint32_t MQTT_PORT = 1883;
#endif // SECURE_MQTT
const char* MQTT_USER = "battlerobot";
const char* MQTT_PASSWD = "Abc12345";

static esp_err_t mqtt_event_handler (esp_mqtt_event_handle_t event) {
	if (event->event_id == MQTT_EVENT_CONNECTED) {
		ESP_LOGI ("TEST", "MQTT msgid= %d event: %d. MQTT_EVENT_CONNECTED", event->msg_id, event->event_id);
		esp_mqtt_client_subscribe (client, "test/hello", 0);
		esp_mqtt_client_publish (client, "test/status", "1", 1, 0, false);
	} 
	else if (event->event_id == MQTT_EVENT_DISCONNECTED) {
		ESP_LOGI ("TEST", "MQTT event: %d. MQTT_EVENT_DISCONNECTED", event->event_id);
		//esp_mqtt_client_reconnect (event->client); //not needed if autoconnect is enabled
	} else  if (event->event_id == MQTT_EVENT_SUBSCRIBED) {
		ESP_LOGI ("TEST", "MQTT msgid= %d event: %d. MQTT_EVENT_SUBSCRIBED", event->msg_id, event->event_id);
	} else  if (event->event_id == MQTT_EVENT_UNSUBSCRIBED) {
		ESP_LOGI ("TEST", "MQTT msgid= %d event: %d. MQTT_EVENT_UNSUBSCRIBED", event->msg_id, event->event_id);
	} else  if (event->event_id == MQTT_EVENT_PUBLISHED) {
		ESP_LOGI ("TEST", "MQTT event: %d. MQTT_EVENT_PUBLISHED", event->event_id);
	} else  if (event->event_id == MQTT_EVENT_DATA) {
		ESP_LOGI ("TEST", "MQTT msgid= %d event: %d. MQTT_EVENT_DATA", event->msg_id, event->event_id);
		ESP_LOGI ("TEST", "Topic length %d. Data length %d", event->topic_len, event->data_len);
		ESP_LOGI ("TEST","Incoming data: %.*s %.*s\n", event->topic_len, event->topic, event->data_len, event->data);

	} else  if (event->event_id == MQTT_EVENT_BEFORE_CONNECT) {
		ESP_LOGI ("TEST", "MQTT event: %d. MQTT_EVENT_BEFORE_CONNECT", event->event_id);
	}
	return ESP_OK;
}

void setup () {
	mqtt_cfg.host = MQTT_HOST;
	mqtt_cfg.port = MQTT_PORT;
	mqtt_cfg.username = MQTT_USER;
	mqtt_cfg.password = MQTT_PASSWD;
	mqtt_cfg.keepalive = 15;
#ifdef SECURE_MQTT
	mqtt_cfg.transport = MQTT_TRANSPORT_OVER_SSL;
#else
	mqtt_cfg.transport = MQTT_TRANSPORT_OVER_TCP;
#endif // SECURE_MQTT
	mqtt_cfg.event_handle = mqtt_event_handler;
	mqtt_cfg.lwt_topic = "test/status";
	mqtt_cfg.lwt_msg = "0";
	mqtt_cfg.lwt_msg_len = 1;
	
	Serial.begin (115200);

	WiFi.mode (WIFI_MODE_STA);
	WiFi.begin (WIFI_SSID, WIFI_PASSWD);
	while (!WiFi.isConnected ()) {
		Serial.print ('.');
		delay (100);
	}
	Serial.println ();
#ifdef SECURE_MQTT
	esp_err_t err = esp_tls_set_global_ca_store (DSTroot_CA, sizeof (DSTroot_CA));
	ESP_LOGI ("TEST","CA store set. Error = %d %s", err, esp_err_to_name(err));
#endif // SECURE_MQTT
	client = esp_mqtt_client_init (&mqtt_cfg);
	//esp_mqtt_client_register_event (client, ESP_EVENT_ANY_ID, mqtt_event_handler, client); // not implemented in current Arduino core
	err = esp_mqtt_client_start (client);
	ESP_LOGI ("TEST", "Client connect. Error = %d %s", err, esp_err_to_name (err));
}

void loop () {	
	esp_mqtt_client_publish (client, "test/bye", "data", 4, 0, false);
	delay (2000);
}