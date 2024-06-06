// #if defined(ESP8266)
// #include <ESP8266WiFi.h>
// #elif defined(ESP32)
#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h> 
LiquidCrystal_I2C lcd(0x27, 16, 2);


const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "broker.mqtt.cool";

const char* controlling1 = "uas_iot/control/buzzer";
const char* controlling2 = "uas_iot/control/led";

const int buzz_pin = 16;
const int led1 = 5;

WiFiClient espClient;
PubSubClient client(espClient);

void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(led1, OUTPUT);
  pinMode(buzz_pin, OUTPUT);
  lcd.init();
  lcd.backlight();

  // Connect to WiFi
  connectToWiFi();

  // Set up MQTT client
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(2000);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      client.subscribe(controlling1);
      client.subscribe(controlling2);
      // client.subscribe(controlling3);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  payload[length] = '\0'; // Ensure payload is null-terminated
  String message = String((char*)payload);

  if (String(topic) == controlling1) {
    if (message == "1") {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("BUZZER ON!");
      tone(buzz_pin, 100);
    } else {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("BUZZER OFF!");
      noTone(buzz_pin);
    }
  }

  if (strcmp(topic, controlling2) == 0) {
    if (message == "1") {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("LED ON!");
      digitalWrite(led1, HIGH);
    } else {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("LED OFF!");
      digitalWrite(led1, LOW);
    }
  }
}
