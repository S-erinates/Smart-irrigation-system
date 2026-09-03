#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ==========================================
// 1. PIN AND THRESHOLD DEFINITIONS
// ==========================================
#define DHTPIN 2
#define DHTTYPE DHT11
#define SOIL_PIN A0
#define PUMP_PIN 5

// --- SETTINGS (Change these values according to your sensor) ---
const int WATERING_START_VALUE = 460;  // If value goes ABOVE this threshold (DRY), the pump turns on
const int WATERING_STOP_VALUE = 320;   // If value goes BELOW this threshold (WET), the pump turns off

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 20, 4);

unsigned long previousTime = 0;
const long MEASUREMENT_INTERVAL = 1000; // Check once every second
bool isPumpRunning = false;

void setup() {
  Serial.begin(115200);
  dht.begin();
  lcd.init();
  lcd.backlight();
  
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW); // Initially turned off
  
  lcd.setCursor(0, 0);
  lcd.print("System Ready");
  delay(1500);
  lcd.clear();
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - previousTime >= MEASUREMENT_INTERVAL) {
    previousTime = currentTime;

    // Read Sensors
    int soilValue = analogRead(SOIL_PIN);
    float temperature = dht.readTemperature();
    float airHmdt = dht.readHumidity();

    // --- LCD SCREEN PRINTING ---
    lcd.setCursor(0, 0);
    lcd.print("Soil Value: ");
    lcd.print(soilValue);
    lcd.print("    ");

    lcd.setCursor(0, 1);
    lcd.print("Temp: ");
    lcd.print(temperature, 1);
    lcd.print(" C   ");

    lcd.setCursor(0, 2);
    lcd.print("Air: ");
    lcd.print(airHmdt, 1);
    lcd.print(" %   ");

    lcd.setCursor(0, 3);
    if (isPumpRunning) {
      lcd.print("PUMP: ON [WATERING] ");
    } else {
      lcd.print("PUMP: OFF [STANDBY]");
    }

    // --- PUMP CONTROL BASED ONLY ON SOIL MOISTURE ---
    
    if (!isPumpRunning && soilValue > WATERING_START_VALUE) {
      // Soil is dry, turn on the pump
      digitalWrite(PUMP_PIN, HIGH);
      isPumpRunning = true;
      Serial.println("Soil is dry, pump started.");
    } 
    else if (isPumpRunning && soilValue < WATERING_STOP_VALUE) {
      // Soil reached saturation, turn off the pump
      digitalWrite(PUMP_PIN, LOW);
      isPumpRunning = false;
      Serial.println("Soil is wet, pump stopped.");
    }

    // Serial Port Monitoring
    Serial.print("Soil: "); Serial.println(soilValue);
  }
}