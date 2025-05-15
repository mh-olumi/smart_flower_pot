#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// ============================
//       Configuration
// ============================

// Wi-Fi credentials for your home/network
const char* WIFI_SSID = "Arm";
const char* WIFI_PASSWORD = "123456abc";

// Liara.ir Server URL (replace with your actual Liara app URL)
const char* SERVER_URL = "https://qomiot1.liara.run/api/sensor-data";

// DHT11 Sensor setup
#define DHTPIN 27        // GPIO27
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Light sensor (Photocell) setup
#define LIGHT_PIN 34     // GPIO34 (analog input)

// LED pins
#define LED_LIGHT 33     // LED for Light Level sabz
#define LED_TEMP 25      // LED for Temperature Range ghermez 
#define LED_HUMIDITY 26  // LED for Humidity Range abi

// Threshold constants
const float TEMP_MIN = 20.0;
const float TEMP_MAX = 28.0;
const float HUMIDITY_MIN = 30.0;
const float HUMIDITY_MAX = 35.0;
const int LIGHT_THRESHOLD = 300;

// Sensor read interval (milliseconds)
#define SENSOR_READ_INTERVAL 5000  // 5 seconds

// ============================
//         Globals
// ============================

unsigned long lastUpdate = 0;

// ============================
//          Setup
// ============================

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(1000); // Allow time for Serial Monitor to initialize

  Serial.println("Smart Flower Pot ESP32 Initialization...");

  // Initialize DHT11 sensor
  dht.begin();
  Serial.println("DHT11 Sensor Initialized.");

  // Initialize LEDs as OUTPUT
  pinMode(LED_LIGHT, OUTPUT);
  pinMode(LED_TEMP, OUTPUT);
  pinMode(LED_HUMIDITY, OUTPUT);
  Serial.println("LEDs Initialized.");

  // Connect to Wi-Fi
  connectToWiFi();
}

// ============================
//           Loop
// ============================

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastUpdate >= SENSOR_READ_INTERVAL) {
    lastUpdate = currentMillis;
    readAndSendSensorData();
  }
}

// ============================
//      Function Definitions
// ============================

/**
 * @brief Connects the ESP32 to the specified Wi-Fi network.
 */
void connectToWiFi() {
  Serial.printf("Connecting to Wi-Fi SSID: %s\n", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED && retryCount < 20) {
    delay(500);
    Serial.print(".");
    retryCount++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to Wi-Fi.");
  }
}

/**
 * @brief Reads sensor data and sends it to the server.
 */
void readAndSendSensorData() {
  // Read sensors
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int lightValue = analogRead(LIGHT_PIN);

  // Check if any reads failed
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    // Optionally, indicate sensor failure with LEDs
    digitalWrite(LED_TEMP, HIGH);
    digitalWrite(LED_HUMIDITY, HIGH);
    return;
  }

  // Log sensor data
  Serial.printf("Temperature: %.2f°C, Humidity: %.2f%%, Light Level: %d\n", temperature, humidity, lightValue);

  // Control LEDs based on thresholds
  controlLEDs(temperature, humidity, lightValue);

  // Send data to server
  sendSensorData(temperature, humidity, lightValue);
}

/**
 * @brief Controls LEDs based on sensor readings.
 */
void controlLEDs(float temperature, float humidity, int lightValue) {
  // Light LED
  if (lightValue < LIGHT_THRESHOLD) {
    digitalWrite(LED_LIGHT, HIGH);
  } else {
    digitalWrite(LED_LIGHT, LOW);
  }

  // Temperature LED
  if (temperature < TEMP_MIN || temperature > TEMP_MAX) {
    digitalWrite(LED_TEMP, HIGH);
  } else {
    digitalWrite(LED_TEMP, LOW);
  }

  // Humidity LED
  if (humidity < HUMIDITY_MIN || humidity > HUMIDITY_MAX) {
    digitalWrite(LED_HUMIDITY, HIGH);
  } else {
    digitalWrite(LED_HUMIDITY, LOW);
  }
}

/**
 * @brief Sends sensor data to the Liara.ir server via HTTP POST.
 */
void sendSensorData(float temperature, float humidity, int lightValue) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(SERVER_URL);
    http.addHeader("Content-Type", "application/json");

    // Create JSON payload
    String jsonPayload = "{\"temperature\":" + String(temperature, 2) +
                         ",\"humidity\":" + String(humidity, 2) +
                         ",\"lightValue\":" + String(lightValue) + "}";

    // Send POST request
    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.printf("Server Response Code: %d\n", httpResponseCode);
      Serial.printf("Server Response: %s\n", response.c_str());
    } else {
      Serial.printf("Error sending POST request: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    http.end();
  } else {
    Serial.println("Wi-Fi not connected. Cannot send data.");
  }
}
