#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

#define IO_USERNAME  "diegoq321"
#define IO_KEY       "aio_BnBE498u7rCvzplEr1cvQXZooKay"
#define WIFI_SSID "nombre_red"
#define WIFI_PASS "contrasena_red"

WiFiClient wifiClient;

Adafruit_MQTT_Client mqtt(&wifiClient, "io.adafruit.com", 1883, IO_USERNAME, IO_KEY);

Adafruit_MQTT_Publish temperatureFeed = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/temperature");
Adafruit_MQTT_Publish humidityFeed = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/humidity");
Adafruit_MQTT_Publish motionFeed = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/motion");

#define MOTION_PIN 2 // Pin digital conectado al sensor de movimiento PIR

Adafruit_AHTX0 aht;

void connect() {
  Serial.print("Conectando a MQTT... ");
  int8_t ret;
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Reintentando conexión MQTT en 5 segundos...");
    mqtt.disconnect();
    delay(5000);
  }
  Serial.println("Conectado!");
}

void setup() {
  Serial.begin(115200);
  delay(10);

  Wire.begin();

  if (!aht.begin()) {
    Serial.println("Error al iniciar el sensor de temperatura y humedad!");
    while (1);
  }

  pinMode(MOTION_PIN, INPUT);

  Serial.println("Conectando a MQTT...");
  connect();
}

void loop() {
  // Reconectar si la conexión MQTT se pierde
  if (!mqtt.connected()) {
    connect();
  }

  // Lectura de temperatura y humedad
  sensors_event_t temp_event, humidity_event;
  aht.getEvent(&temp_event, &humidity_event);
  if (isnan(temp_event.temperature) || isnan(humidity_event.relative_humidity)) {
    Serial.println("Error al leer el sensor!");
  } else {
    float temperature = temp_event.temperature;
    float humidity = humidity_event.relative_humidity;
    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.print(" °C\tHumedad: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Enviar datos al feed de temperatura y humedad
    temperatureFeed.publish(temperature);
    humidityFeed.publish(humidity);
  }

  // Lectura del sensor de movimiento
  int motionDetected = digitalRead(MOTION_PIN);
  if (motionDetected == HIGH) {
    Serial.println("Movimiento detectado!");
    // Enviar datos al feed de movimiento
    motionFeed.publish("Movimiento detectado!");
  } else {
    Serial.println("Sin movimiento");
    // Enviar datos al feed de movimiento
    motionFeed.publish("Sin movimiento");
  }

  delay(5000); // Espera 5 segundos antes de volver a leer los sensores
}