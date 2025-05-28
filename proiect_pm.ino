#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <DHT.h>

const char* ssid = "Android";
const char* password = "";

const char* server = "api.thingspeak.com";
String apiKey = "";

const char* openWeatherServer = "api.openweathermap.org";
const char* city = "Bucharest";
const char* apiKeyOW = "";

#define DHTPIN D6
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

Adafruit_BMP280 bmp;

#define TFT_CS D2
#define TFT_RST D3
#define TFT_DC D4
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

#define UV_PIN A0

float tempOW = 0.0, pressureOW = 0.0, humidityOW = 0.0;

float readUV() {
  int uvAnalogValue = analogRead(UV_PIN);
  float voltage = (uvAnalogValue / 1024.0) * 3.3;
  float uvIndex = voltage * 10.0;
  return uvIndex;
}

void getWeatherData() {
  WiFiClient client;
  if (client.connect(openWeatherServer, 80)) {
    String url = String("GET /data/2.5/weather?q=") + city + "&appid=" + apiKeyOW + "&units=metric";
    
    client.print(url + " HTTP/1.1\r\nHost: api.openweathermap.org\r\nConnection: close\r\n\r\n");

    bool jsonStarted = false;
    String payload;
    
    while (client.connected() || client.available()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        jsonStarted = true;
        continue;
      }
      if (jsonStarted) {
        payload = line;
        break;
      }
    }

    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      tempOW = doc["main"]["temp"];
      pressureOW = doc["main"]["pressure"];
      humidityOW = doc["main"]["humidity"];
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectat!");

  dht.begin();
  if (!bmp.begin(0x76)) {
    Serial.println("Eroare la detectarea BMP280!");
    while (1);
  }

  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Initializare completa!");
  delay(2000);
}

void loop() {
  float tempDHT = dht.readTemperature();
  float humDHT = dht.readHumidity();
  
  if (isnan(tempDHT) || isnan(humDHT)) {
    Serial.println("Eroare la citirea DHT22!");
    return;
  }

  float pressureBMP = bmp.readPressure() / 100.0F;
  float uvIndex = readUV();

  getWeatherData();

  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(10, 10);
  tft.setTextSize(2);
  tft.println("Date locale:");
  tft.setCursor(10, 40);
  tft.printf("Temp DHT22: %.1f C\n", tempDHT);
  tft.setCursor(10, 70);
  tft.printf("Umiditate: %.1f %%\n", humDHT);
  tft.setCursor(10, 100);
  tft.printf("Presiune: %.1f hPa\n", pressureBMP);
  tft.setCursor(10, 130);
  tft.printf("UV Index: %.1f\n", uvIndex);

  tft.setCursor(10, 170);
  tft.println("Date OpenWeather:");
  tft.setCursor(10, 200);
  tft.printf("Temp: %.1f C\n", tempOW);
  tft.setCursor(10, 230);
  tft.printf("Umid: %.1f %%\n", humidityOW);
  tft.setCursor(10, 260);
  tft.printf("Pres: %.1f hPa\n", pressureOW);

  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    if (client.connect(server, 80)) {
      String postStr = apiKey;
      postStr += "&field1=" + String(tempDHT);
      postStr += "&field2=" + String(tempOW);
      postStr += "&field3=" + String(pressureBMP);
      postStr += "&field4=" + String(pressureOW);
      postStr += "&field5=" + String(humDHT);
      postStr += "&field6=" + String(humidityOW);
      postStr += "&field7=" + String(uvIndex);
      postStr += "\r\n\r\n";

      client.print("POST /update HTTP/1.1\n");
      client.print("Host: api.thingspeak.com\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: " + String(postStr.length()) + "\n\n");
      client.print(postStr);
    }
    client.stop();
  }

  delay(20000);
}
