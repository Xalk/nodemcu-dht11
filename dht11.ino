#include <DHT.h>  // Including library for dht
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>


const char *ssid = ""; // your WIFI ssid
const char *pass = ""; // your WIFI password
const char *server_url = ""; //application endpoint

String apiKey = ""; // (optional)api for personal use

#define DHTPIN 0  // pin where the dht11 is connected
DHT dht(DHTPIN, DHT11);

void setup() {
  Serial.begin(115200);
  delay(10);
  dht.begin();

  Serial.println("Connecting to ");
  Serial.println(ssid);


  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" degrees Celcius, Humidity: ");
  Serial.print(h);


  // Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    // Your Domain name with URL path or IP address with path
    http.begin(client, server_url);


    // HTTP request with a content type: application/json
    http.addHeader("Content-Type", "application/json");

    String tem = String(t);
    String hum = String(h);

    // Send HTTP POST request
    int httpResponseCode = http.POST("{\"humidity\":\"" + hum + "\",\"temperature\":\"" + tem + "\", \"api_key\":\"" + apiKey + "\"}");


    Serial.print(" -> HTTP Response code: ");
    Serial.println(httpResponseCode);

    // Free resources
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }

  delay(5000);
}