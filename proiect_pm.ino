#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <DHT.h>

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

void setup() {
  Serial.begin(115200);
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
  tft.println("Statie Meteo");
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

  Serial.println("--- Date locale ---");
  Serial.print("Temp: "); Serial.println(tempDHT);
  Serial.print("Umid: "); Serial.println(humDHT);
  Serial.print("Presiune: "); Serial.println(pressureBMP);
  Serial.print("UV Index: "); Serial.println(uvIndex);

  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(10, 10);
  tft.setTextSize(2);
  tft.println("Date locale:");

  tft.setCursor(10, 40);
  tft.print("Temp: ");
  tft.print(tempDHT);
  tft.println(" C");

  tft.setCursor(10, 70);
  tft.print("Umid: ");
  tft.print(humDHT);
  tft.println(" %");

  tft.setCursor(10, 100);
  tft.print("Pres: ");
  tft.print(pressureBMP);
  tft.println(" hPa");

  tft.setCursor(10, 130);
  tft.print("UV: ");
  tft.println(uvIndex);

  delay(20000);
}
