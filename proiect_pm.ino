#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <DHT.h>

const char* ssid = "Dinamo1948 2.4G";
const char* password = "YxsAQZGZPPCP9A7kFQ";

//const char* ssid = "Android";
//const char* password = "qwertyuiop1";

const char* server = "api.thingspeak.com";
String apiKey = "LIG11O1UJ1LVP6ZZ";

const char* openWeatherServer = "api.openweathermap.org";
const char* city = "Bucharest";
const char* apiKeyOW = "3c234086eab2066147ee9b450524f7d3";

const char* wuServer = "weatherstation.wunderground.com";
const char* stationID = "IBUCHA406";
const char* wuPassword = "yruZoWLT";

#define DHTPIN D6
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

Adafruit_BMP280 bmp;

#define TFT_CS D2
#define TFT_RST D3
#define TFT_DC D4
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

#define UV_PIN A0

float readUV() {
  int uvAnalogValue = analogRead(UV_PIN);
  float voltage = (uvAnalogValue / 1024.0) * 3.3;
  float uvIndex = voltage * 10.0;
  return uvIndex;
}

float tempOW = 0.0, pressureOW = 0.0, humidityOW = 0.0, uvIndexOW = -1.0;

void getWeatherData() {
    WiFiClient client;
    if (client.connect(openWeatherServer, 80)) {
        String url = String("GET /data/2.5/weather?q=") + city + "&appid=" + apiKeyOW + "&units=metric";
        
        client.print(url + " HTTP/1.1\r\n" +
                    "Host: api.openweathermap.org\r\n" +
                    "Connection: close\r\n\r\n");

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

        Serial.println("JSON primit:");
        Serial.println(payload);

        DynamicJsonDocument doc(2048);
        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
            Serial.print("Eroare la parsarea JSON: ");
            Serial.println(error.c_str());
            return;
        }

        tempOW = doc["main"]["temp"];
        pressureOW = doc["main"]["pressure"];
        humidityOW = doc["main"]["humidity"];
        
        Serial.println("Date extrase:");
        Serial.print("Temperatura: "); Serial.println(tempOW);
        Serial.print("Presiune: "); Serial.println(pressureOW);
        Serial.print("Umiditate: "); Serial.println(humidityOW);
    }
}

void getUVIndexFromOpenWeather() {
    WiFiClient client;
    const char* openWeatherUVServer = "api.openweathermap.org";
    String lat = "44.4268";
    String lon = "26.1025";

    if (client.connect(openWeatherUVServer, 80)) {
        String url = "/data/2.5/onecall?lat=" + lat + "&lon=" + lon + 
                    "&exclude=minutely,hourly,daily,alerts&appid=" + apiKeyOW +
                    "&units=metric";
                    
        client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                     "Host: " + openWeatherUVServer + "\r\n" +
                     "Connection: close\r\n\r\n");

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

        if (error) {
            Serial.print("Eroare la parsarea JSON UV: ");
            Serial.println(error.c_str());
            uvIndexOW = -1.0;
            return;
        }

        uvIndexOW = doc["current"]["uvi"];
        Serial.print("UV Index primit: ");
        Serial.println(uvIndexOW);
    }
}

void sendToWeatherUnderground(float tempDHT, float humDHT, float pressureBMP, float uvIndex) {
  if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      if (client.connect(wuServer, 80)) {
          String url = "/weatherstation/updateweatherstation.php?";
          url += "ID=" + String(stationID);
          url += "&PASSWORD=" + String(wuPassword);
          url += "&dateutc=now";
          url += "&tempf=" + String(tempDHT * 1.8 + 32);
          url += "&humidity=" + String(humDHT);
          url += "&baromin=" + String(pressureBMP * 0.02953);
          url += "&uv=" + String(uvIndex);
          url += "&softwaretype=ESP8266";
          url += "&action=updateraw";

          client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                       "Host: " + wuServer + "\r\n" +
                       "Connection: close\r\n\r\n");

          Serial.println("Trimit date către Weather Underground:");
          Serial.println("URL-ul trimis către Weather Underground: " + url);
          while (client.connected() || client.available()) {
              if (client.available()) {
                  String line = client.readStringUntil('\n');
                  Serial.println("Răspuns server: " + line);
              }
          }
          client.stop();
      } else {
          Serial.println("Eroare: Nu s-a conectat la Weather Underground!");
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

  Serial.println("Initializare ecran TFT...");
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
  getUVIndexFromOpenWeather();

  Serial.println("----------- Date locale -----------");
  Serial.print("Temperatura DHT22 (°C): ");
  Serial.println(tempDHT);
  Serial.print("Umiditate DHT22 (%): ");
  Serial.println(humDHT);
  Serial.print("Presiunea BMP280 (hPa): ");
  Serial.println(pressureBMP);
  Serial.print("Index UV: ");
  Serial.println(uvIndex);

  Serial.println("----------- Date OpenWeather -----------");
  Serial.print("Temperatura OpenWeather (°C): ");
  Serial.println(tempOW);
  Serial.print("Presiunea OpenWeather (hPa): ");
  Serial.println(pressureOW);
  Serial.print("Umiditatea OpenWeather (%): ");
  Serial.println(humidityOW);
  Serial.print("Indice UV (OpenWeather): ");
  Serial.println(uvIndexOW);

  tft.fillScreen(ILI9341_BLACK);

  tft.setCursor(10, 10);
  tft.setTextSize(2);
  tft.println("Date locale:");

  tft.setCursor(10, 40);
  tft.print("Temp DHT22: ");
  tft.print(tempDHT);
  tft.println(" C");

  tft.setCursor(10, 70);
  tft.print("Umid DHT22: ");
  tft.print(humDHT);
  tft.println(" %");

  tft.setCursor(10, 100);
  tft.print("Pres BMP280: ");
  tft.print(pressureBMP);
  tft.println(" hPa");

  tft.setCursor(10, 130);
  tft.print("Index UV: ");
  tft.println(uvIndex);

  tft.setCursor(10, 160);
  tft.println("Date OpenWeather:");

  tft.setCursor(10, 190);
  tft.print("Temp OW: ");
  tft.print(tempOW);
  tft.println(" C");

  tft.setCursor(10, 220);
  tft.print("Umid OW: ");
  tft.print(humidityOW);
  tft.println(" %");

  tft.setCursor(10, 250);
  tft.print("Pres OW: ");
  tft.print(pressureOW);
  tft.println(" hPa");

  tft.setCursor(10, 280);
  tft.print("UV OW: ");
  tft.print(uvIndexOW);

  sendToWeatherUnderground(tempDHT, humDHT, pressureBMP, uvIndex);

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
          postStr += "&field8=" + String(uvIndexOW);
          postStr += "\r\n\r\n";

          client.print("POST /update HTTP/1.1\n");
          client.print("Host: api.thingspeak.com\n");
          client.print("Connection: close\n");
          client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
          client.print("Content-Type: application/x-www-form-urlencoded\n");
          client.print("Content-Length: " + String(postStr.length()) + "\n\n");
          client.print(postStr);

          Serial.println("Date trimise către ThingSpeak:");
          Serial.println(postStr);
      }
      client.stop();
  }

  delay(20000);
}