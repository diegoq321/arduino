#include <Adafruit_AHTX0.h>
#include <AdafruitIO_WiFi.h>

/************************ Adafruit IO Config *******************************/
#define IO_USERNAME "tu_usuario"  // Reemplaza con tu nombre de usuario de Adafruit IO
#define IO_KEY "tu_clave"         // Reemplaza con tu clave de Adafruit IO

/******************************* WIFI **************************************/
#define WIFI_SSID "tu_red_wifi"    // Reemplaza con el nombre de tu red WiFi
#define WIFI_PASS "tu_contraseña"  // Reemplaza con la contraseña de tu red WiFi

Adafruit_IO_Arduino::AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

// Include the necessary libraries for the PIR motion sensor
const int motionSensorPin = 2; // Assuming the PIR sensor is connected to digital pin 2

Adafruit_AHTX0 aht;

void setup() {
  Serial.begin(115200);
  Serial.println("Adafruit AHT10/AHT20 demo!");

  if (!aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }
  Serial.println("AHT10 or AHT20 found");

  // Initialize the PIR motion sensor pin
  pinMode(motionSensorPin, INPUT);

  // Connect to Adafruit IO
  Serial.print("Connecting to Adafruit IO...");
  io.connect();
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected to Adafruit IO");
}

void loop() {
  // Read the motion sensor input
  int motionDetected = digitalRead(motionSensorPin);

  // If motion is detected, print a message
  if (motionDetected == HIGH) {
    Serial.println("Movimiento detectado!");
    // Publish motion event to Adafruit IO
    AdafruitIO_Feed *motionFeed = io.feed("movimiento");
    motionFeed->save("Motion Detected");
  } else {
    Serial.println("Sin movimiento!");
  }

  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp); // populate temp and humidity objects with fresh data
  Serial.print("Temperatura: ");Serial.print(temp.temperature);Serial.println(" °C");
  Serial.print("Humedad: ");Serial.print(humidity.relative_humidity);Serial.println("% rH");

  // Publish temperature and humidity to Adafruit IO
  AdafruitIO_Feed *temperatureFeed = io.feed("temperatura");
  temperatureFeed->save(temp.temperature);

  AdafruitIO_Feed *humidityFeed = io.feed("humedad");
  humidityFeed->save(humidity.relative_humidity);

  delay(10000);
}
