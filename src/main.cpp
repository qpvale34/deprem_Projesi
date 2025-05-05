#include <Arduino.h>
#include <HX711.h>
#include <Deneyap_GPSveGLONASSkonumBelirleyici.h>
#include <Callmebot_ESP32.h>
#include <WiFi.h>

// WiFi credentials and Callmebot config
const char* WIFI_SSID = "TurkTelekom_T6C7F";
const char* WIFI_PASS = "jFRh0DgC.!.?09";
const String PHONE_NUMBER = "+905065508830";
const String API_KEY = "2966605";

// Define HX711 pins and instantiate sensor & GPS
const int HX711_DOUT = D4; // adjust as needed
const int HX711_SCK = SCK;  // adjust as needed
HX711 scale;
GPS myGPS;

void setup() {
  Serial.begin(9600); // Changed baud rate to 9600

  // Initialize WiFi
  Serial.println("WiFi Bağlantısı kuruluyor...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi bağlantısı kuruldu.");

  // Setup HX711 sensor
  scale.begin(HX711_DOUT, HX711_SCK);
  scale.set_scale(2280.f); // adjust calibration factor as needed
  scale.tare();
  Serial.println("HX711 modülü başlatıldı.");

  // Initialize GPS module
  if(myGPS.begin()) {
    Serial.println("GPS modülü başlatıldı.");
  } else {
    Serial.println("GPS modülü başlatılamadı.");
  }
}

void loop() {
  float weight = scale.get_units(10);
  Serial.print("Ağırlık: ");
  Serial.print(weight);
  Serial.println(" kg");

  // Request and read GPS data, including location and date/time
  myGPS.readGPS(RMC);
  double latitude = myGPS.readLocationLat();
  double longitude = myGPS.readLocationLng();
  uint8_t day = myGPS.readDay();
  uint8_t month = myGPS.readMonth();
  uint16_t year = myGPS.readYear();
  uint8_t hour = myGPS.readHour();
  uint8_t minute = myGPS.readMinute();
  uint8_t second = myGPS.readSecond();

  // Print GPS data to Serial
  Serial.print("GPS konumu: ");
  Serial.print(latitude, 6);
  Serial.print(", ");
  Serial.println(longitude, 6);
  Serial.print("Tarih: ");
  Serial.print(day);
  Serial.print("/");
  Serial.print(month);
  Serial.print("/");
  Serial.println(year);
  Serial.print("Saat: ");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.println(second);

  if(weight >= 5.0) {
    // Compose a message with location, date, and time
    String message = "Deprem oldu! Konum: " + String(latitude, 6) + ", " + String(longitude, 6) +
                     " Tarih: " + String(day) + "/" + String(month) + "/" + String(year) +
                     " Saat: " + String(hour) + ":" + String(minute) + ":" + String(second);
    Serial.println("Callmebot aracılığı ile whatsApp mesajı gönderiliyor...");
    Callmebot.whatsappMessage(PHONE_NUMBER, API_KEY, message);
    Serial.println("Mesaj gönderildi.");

    delay(60000); // wait 1 minute to prevent repeated messages
  }
  delay(2000); // sensor reading interval
}
