
#include <WiFi.h>

#include <PubSubClient.h>

#define LIGHT_SENSOR_PIN 33



char mac[17];
const char* ssid = "GuestWLANPortal";
const char* mqtt_server = "142.93.174.193";



const char* topic1 = "zuerich/briefkasten/in";
const char* topic2 = "zuerich/briefkasten/out";





WiFiClient espClient;

PubSubClient client(espClient);



void setup_wifi() {

  Serial.print("Connecting to ");

  Serial.print(ssid);

  WiFi.begin(ssid);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");
  }

  Serial.println("done!");
}
void callback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, topic2) == 0) {
    char message[length];
    for (int i = 0; i < length; i++) { message[i] = (char)payload[i]; }
    Serial.print("Received Message: ");
    Serial.println(message);
  }
}






void reconnect() {
  Serial.print("Attempting MQTT connection...");
  while (!client.connected()) {
    if (client.connect(mac)) {
      Serial.println("done!");
      client.subscribe(topic1);
      client.subscribe(topic2);
    } else {
      delay(500);
      Serial.print(".");
    }
  }
}




void setup() {

  Serial.begin(115200);

  setup_wifi();

  client.setServer(mqtt_server, 1883);
}






// isopen= 1, open
//isopen = 0, closed

// ESP32 pin GIOP36 (ADC0)

void loop() {
  int isopen = 0;
  while (true) {
    if (!client.connected()) {

      reconnect();
    }
    // reads the input on analog pin (value between 0 and 4095)
    int analogValue = analogRead(LIGHT_SENSOR_PIN);

    Serial.print("Analog Value = ");
    Serial.print(analogValue);  // the raw analog reading

    // We'll have a few threshholds, qualitatively determined
    if (analogValue < 15 && isopen == 1) {
      Serial.println(" => letter box closed");
      isopen = 0;
    } else if (analogValue > 15 && isopen == 0) {
      Serial.println(" => letter box open");
      isopen = 1;
    }
    char lightBuffer[10];
    sprintf(lightBuffer, "%d", analogValue);
    client.publish(topic1, lightBuffer);

    delay(1000);
  }
}
