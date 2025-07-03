#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <DHT11.h>

// LCD Initialization
LiquidCrystal_PCF8574 lcd(0x27);

// Dust Sensor Pins  
#define VOUT1_PIN 34
#define VOUT2_PIN 35
#define BUZZER_PIN 27

// DHT Sensor Configuration
DHT11 dht11(2);

// WiFi and ThingSpeak Config
const char* host = "api.thingspeak.com";
const int httpPort = 80;
const String channelID = "2005329";
const String writeApiKey = "M3I8SW1ZZ86YBGYN";

// Threshold values for coffee storage
const float TEMP_WARNING = 25.0;
const float TEMP_CRITICAL = 27.0;
const float HUMIDITY_WARNING = 65.0;
const float HUMIDITY_CRITICAL = 75.0;
const float DUST_WARNING = 25.0;
const float DUST_CRITICAL = 35.0;

// Sensor data variables
float concentration1 = 0.0, concentration2 = 0.0;
float temperature = 0.0, humidity = 0.0;
String latitude = "0.3476";
String longitude = "32.5825";

// Display management
unsigned long lastDisplayChange = 0;
const unsigned long displayInterval = 8000;  // 8 seconds per display mode
bool showMeasurements = true;

WiFiClient client;

void setup() {
  Serial.begin(115200);
  pinMode(VOUT1_PIN, INPUT);
  pinMode(VOUT2_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initialize LCD
  Wire.begin();
  int error;
  Wire.beginTransmission(0x27);
  error = Wire.endTransmission();

  if (error == 0) {
    Serial.println("LCD found.");
    lcd.begin(20, 4);  // Initialize the LCD
    lcd.setBacklight(255);
    lcd.setCursor(1, 0);
    lcd.print("COFFEE STORAGE MONITOR");
    lcd.setCursor(2, 1);
    lcd.print("MEASUREMENT SYSTEM");
    lcd.setCursor(1, 2);
    lcd.print("AIR QUALITY & ENVIRONMENT");
    lcd.setCursor(4, 3);
    lcd.print("Initializing...");
    delay(3000);
    lcd.clear();
  } else {
    Serial.println("LCD not found. Check connections.");
    while (1);  // Stop if LCD not found
  }

  // Connect to WiFi
  lcd.setCursor(0, 0);
  lcd.print("Connecting to WiFi...");
  connectToStrongestWiFi();
  lcd.clear();
  lcd.setCursor(2, 1);
  lcd.print("WiFi CONNECTED!");
  delay(1000);
}

void loop() {
  checkWiFi();

  // Read sensor values
  dustConc();
  readDHTSensor();
  soundBuzzer(concentration1, concentration2);

  Serial.print("Concentration 1: ");
  Serial.print(concentration1);
  Serial.print(" | Concentration 2: ");
  Serial.print(concentration2);
  Serial.print(" | Temp: ");
  Serial.print(temperature);
  Serial.print("°C | Humidity: ");
  Serial.print(humidity);
  Serial.print("% | Location: ");
  Serial.print(latitude);
  Serial.print(", ");
  Serial.println(longitude);

  // Update display (toggle between measurements and predictions)
  if (millis() - lastDisplayChange >= displayInterval) {
    showMeasurements = !showMeasurements;
    lastDisplayChange = millis();
    lcd.clear();
  }

  if (showMeasurements) {
    displayMeasurements();
  } else {
    displayEnvironmentStatus(); // Changed from displayPredictions
  }

  // Send Data to ThingSpeak
  sendDataToThingSpeak();

  delay(1000);
}



// [Rest of your existing functions remain the same: dustConc(), readDHTSensor(), 
// soundBuzzer(), displayMeasurements(), sendDataToThingSpeak(), getGeoLocation(), 
// connectWifi(), connectToStrongestWiFi(), checkWiFi()]

// Updated advice functions for coffee storage context
