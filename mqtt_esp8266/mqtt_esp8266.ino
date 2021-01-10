#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "RONG";
const char* password = "0903760064";
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

const char humidityTopic[] = "/clothes/humidity";
const char buzzerStartupTopic[] = "/clothes/buzzer/startup";
const char buzzerOnTopic[] = "/clothes/buzzer/on";
const char buzzerPlayingTopic[] = "/clothes/buzzer/playing";

bool humidDecrease = true;  // TESTING, should be removed when done.

const int dryLimit = 5;  // TESTING, value should be changed when done.
const int maxHumid = 10;  // TESTING, value should be changed when done.
int humidity = maxHumid;

const int humidChangeDelay = 1000;  // TESTING, should be removed when done.
int lastMillisHumidChange = 0;  // TESTING, should be removed when done.

const int publishDelay = 1000;  // TESTING, value should be changed when done.
int lastMillisPublish = 0;

bool buzzerOn = false;
bool buzzerPlaying = false;

void switchBuzzerOnOff(String message) {
  if (message == "1") {
    buzzerOn = true;
  }
  else if (message == "0") {
    buzzerOn = false;
    buzzerPlaying = false;
  }
}

// TESTING, will be replaced by AutoConnect when done.
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
// End TESTING part.

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");

      client.subscribe(buzzerOnTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(topic);
  Serial.println(message);

  if (strcmp(topic, buzzerOnTopic) == 0) {
    switchBuzzerOnOff(message);
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi(); // TESTING, will be replaced by AutoConnect when done.
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
    client.publish(buzzerStartupTopic, "startup");
  }
  client.loop();

  int now = millis();

  if (now - lastMillisPublish > publishDelay) {
    lastMillisPublish = now;
    client.publish(humidityTopic, String(humidity).c_str());
    if (buzzerPlaying) {
      client.publish(buzzerPlayingTopic, "1");
    }
    else {
      client.publish(buzzerPlayingTopic, "0");
    }
  }

  if (humidity <= dryLimit && buzzerOn) {
    buzzerPlaying = true;
  }

  // Add humidity reading code here...

  // TESTING, will be replaced by humidity reading code when done.
  if (now - lastMillisHumidChange > humidChangeDelay) {
    lastMillisHumidChange = now;
    
    if (humidity == 0) {
      humidDecrease = false;
    }
    else if (humidity == maxHumid) {
      humidDecrease = true;
    }
    
    if (humidDecrease) {
      humidity--;
    }
    else {
      humidity++;
    }
  }
  // End TESTING part.
}
