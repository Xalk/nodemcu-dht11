#include <EEPROM.h>
#include <ESP8266WebServer.h>

#include <DHT.h>  // Including library for dht
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

ESP8266WebServer server(80);


const char *server_url = "http://192.168.0.103:4444/api/sensor"; // any endpoint

String apiKey = "123"; //optional

String deviceID = "354AF";


#define DHTPIN 0  //pin where the dht11 is connected
DHT dht(DHTPIN, DHT11);


struct settings {
  char ssid[30];
  char password[30];
} user_wifi = {};



void setup() {

  

  EEPROM.begin(sizeof(struct settings));
  EEPROM.get(0, user_wifi);

  WiFi.mode(WIFI_STA);
  WiFi.begin(user_wifi.ssid, user_wifi.password);


  Serial.begin(115200);
  delay(10);
  dht.begin();


  // with auto connecting
  // byte tries = 0;
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(250);
  //   if (tries++ > 30) {
  //     WiFi.mode(WIFI_AP);
  //     WiFi.softAP("Novabee Portal", "novabe8e8");
  //     break;
  //   }
  // }

  // without auto connecting
  WiFi.mode(WIFI_AP);
  WiFi.softAP("Novabee Portal", "novabe8e8");


  server.on("/", handlePortal);
  server.begin();
}

void loop() {

  server.handleClient();

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


  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    // Your Domain name with URL path or IP address with path
    http.begin(client, server_url);


    // If you need an HTTP request with a content type: application/json, use the following:
    http.addHeader("Content-Type", "application/json");

    String tem = String(t);
    String hum = String(h);

    // Send HTTP POST request
    int httpResponseCode = http.POST("{\"humidity\":\"" + hum + "\",\"temperature\":\"" + tem + "\", \"apiKey\":\"" + apiKey + "\",\"deviceID\":\"" + deviceID + "\"}");



    Serial.print(" -> HTTP Response code: ");
    Serial.println(httpResponseCode);

    // Free resources
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }


  delay(5000);
}



void handlePortal() {

  if (server.method() == HTTP_POST) {

    strncpy(user_wifi.ssid, server.arg("ssid").c_str(), sizeof(user_wifi.ssid));
    strncpy(user_wifi.password, server.arg("password").c_str(), sizeof(user_wifi.password));
    user_wifi.ssid[server.arg("ssid").length()] = user_wifi.password[server.arg("password").length()] = '\0';
    EEPROM.put(0, user_wifi);
    EEPROM.commit();



    const char *ssid = server.arg("ssid").c_str();
    const char *password = server.arg("password").c_str();

    connectToWiFi(password, ssid);


    //Handle Successfull connection.
    if (WiFi.status() == WL_CONNECTED) {
      server.send(200, "text/html", "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Novabee Setup</title><style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1,p{text-align: center}</style> </head> <body><main class='form-signin'> <h1>Nova<span style='color: #fab040'>bee</span> Setup</h1> <br/> <p>Your settings have been saved successfully!<br /></p></main></body></html>");

    } else {

      server.send(200, "text/html", "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Novabee Setup</title><style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1,p{text-align: center}</style> </head> <body><main class='form-signin'> <h1>Nova<span style='color: #fab040'>bee</span> Setup</h1> <br/> <p>Ssid or password is incorrect!<br />Please try again.</p></main></body></html>");

      WiFi.disconnect();
    }

  } else {
    server.send(200, "text/html", "<!DOCTYPE html><html lang='en'> <head> <meta charset='utf-8' /> <meta name='viewport' content='width=device-width, initial-scale=1' /> <title>Novabee Setup</title> <style> *, ::after, ::before { box-sizing: border-box; } body { margin: 0; font-family: 'Segoe UI', Roboto, 'Helvetica Neue', Arial, 'Noto Sans', 'Liberation Sans'; font-size: 1rem; font-weight: 400; line-height: 1.5; color: #424143; background-color: #f5f5f5; } .form-control { display: block; width: 100%; height: calc(1.5em + 0.75rem + 2px); border: 1px solid #ced4da; } .button { position: relative; cursor: pointer; border: 1px solid transparent; color: #fff; background-color: #fab040; border-color: #fab040; padding: 0.5rem 1rem; font-size: 1.25rem; line-height: 1.5; border-radius: 0.3rem; width: 100%; } .button--disable { pointer-events: none; background: #b57f2e; } .button:active { background: #b57f2e; } .button__text { color: #ffffff; transition: all 0.2s; letter-spacing: 1px; } .button--loading .button__text { visibility: hidden; opacity: 0; } .button--loading::after { content: ''; position: absolute; width: 16px; height: 16px; top: 0; left: 0; right: 0; bottom: 0; margin: auto; border: 4px solid transparent; border-top-color: #ffffff; border-radius: 50%; animation: button-loading-spinner 1s ease infinite; } @keyframes button-loading-spinner { from { transform: rotate(0turn); } to { transform: rotate(1turn); } } .form-signin { width: 100%; max-width: 400px; padding: 15px; margin: auto; } h1 { text-align: center; } </style> </head> <body> <main class='form-signin'> <form action='/' method='post'> <h1 class=''>Nova<span style='color: #fab040'>bee</span> Setup</h1> <br /> <div class='form-floating'> <label>SSID</label><input type='text' class='form-control' name='ssid' /> </div> <div class='form-floating'> <br /><label>Password</label ><input type='password' class='form-control' name='password' /> </div> <br /><br /> <button type='submit' class='button' onclick='this.classList.add(`button--loading`, `button--disable`)' > <span class='button__text'>Save</span> </button> <p style='text-align: right'> <a href='https://google.com'>novabee.com</a> </p> </form> </main> </body></html>");
  }
}




void connectToWiFi(const char *password, const char *ssid) {
  WiFi.disconnect();
  //Connect to WiFi Network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to WiFi");
  Serial.println("...");
  WiFi.begin(ssid, password);
  int retries = 0;
  while ((WiFi.status() != WL_CONNECTED) && (retries < 15)) {
    retries++;
    delay(500);
    Serial.print(".");
  }
  if (retries > 14) {
    Serial.println(F("WiFi connection FAILED"));
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("WiFi connected!"));
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
  Serial.println(F("Setup ready"));
}
