#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "TP-Link_CD7E";
const char* pasword = "80157281";

const int lj1 = 26;
const int lj2 = 25;
const int lk1 = 33;
const int lk2 = 32;
const int lm1 = 15;
const int lm2 = 2;

float t;
float h;

AsyncWebServer server(80);

#define DHTPIN 27     
#define DHTTYPE    DHT22     

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

String readDHTTemperature() {
  t = dht.readTemperature();
  if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
    lcd.setCursor(0,0);
    lcd.print("!!!!!Warning!!!!!");
    lcd.setCursor(0,1);
    lcd.print("Fix your sensor");
    return "--";
  }
  else {
    Serial.println(t);
    lcd.setCursor(0,0);
    lcd.print("Temperature ");
    lcd.print(t);
    lcd.print("C");
    return String(t);
  }
}

String readDHTHumidity() {
  h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println(h);
    lcd.setCursor(0,1);
    lcd.print("Humidity ");
    lcd.print(h);
    lcd.print("%");
    return String(h);
  }
}
// HTML web page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Pengontrol PLC</title>
    <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
</head>
<style>
    body {
    text-align: center;
    background-color: #201e1e;
    position: relative;    
    }
    p {
    margin: 20px 0 20px 0;
    padding: 20px 0 20px 0;
    }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
    
</style>
<script>
    setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
    };
    xhttp.open("GET", "/temperature", true);
    xhttp.send();
    }, 10000 ) ;

    setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
    };
    xhttp.open("GET", "/humidity", true);
    xhttp.send();
    }, 10000 ) ;
</script>
<body>
    <h3 style="color: #ffffff;">
        CONTROLLING & MONITORING WITH ESP32 FOR CONTROL FORWARD REVERSE IN MOTOR 
    </h3>
    <h3 style="color: #ffffff;position: relative; background-color: #000000;">Monitoring DHT22</h3>
    <p style="color: #ffffff;">
        <i class="fas fa-temperature-high" style="color:#ff7b00;"></i> 
        <span class="dht-labels">Temperature</span> 
        <span id="temperature">--</span>
        <span class="units">&deg;C</span>
    </p>
    
    <p style="color: #ffffff;">
        <i class="fas fa-tint" style="color:#008cff;"></i> 
        <span class="dht-labels">Humidity</span>
        <span id="humidity">--</span>
        <span class="units">%</span>
    </p>
    <script>
      function toggleCheckbox(x) {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/" + x, true);
        xhr.send();
      }
     </script>
</body>
</html>)rawliteral";

void setup() {
  Serial.begin(115200);
  lcd.begin();
  dht.begin();
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, pasword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  lcd.setCursor(0,0);
  lcd.print("Connecting to");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  
  pinMode(lj1, OUTPUT);
  pinMode(lj2, OUTPUT);
  pinMode(lk1, OUTPUT);
  pinMode(lk2, OUTPUT);
  pinMode(lm1, OUTPUT);
  pinMode(lm2, OUTPUT);
  
  // Send web page to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTTemperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTHumidity().c_str());
  });
  // Receive an HTTP GET request

  server.begin();
}

void loop() {
  Serial.print(t);
  Serial.println(h);
  if (t > 20 || t < 30){
    digitalWrite(lj1,HIGH);
    digitalWrite(lk1,LOW);
    digitalWrite(lm1,LOW);
  }
  else if (t > 15 && t < 19 || t > 31 && t < 35){
    digitalWrite(lj1,LOW);
    digitalWrite(lk1,HIGH);
    digitalWrite(lm1,LOW);
  }
  else if (t < 14 || t > 36){
    digitalWrite(lj1,LOW);
    digitalWrite(lk1,LOW);
    digitalWrite(lm1,HIGH);
  }
  else{
    Serial.println("Data t tidak terbaca");
  }
  
  if (h > 45 || h < 65){
    digitalWrite(lj2,HIGH);
    digitalWrite(lk2,LOW);
    digitalWrite(lm2,LOW);
  }
  else if(h > 66 || h < 89){
    digitalWrite(lj2,LOW);
    digitalWrite(lk2,HIGH);
    digitalWrite(lm2,LOW);
  }
  else if(h < 44 || h > 90){
    digitalWrite(lj2,LOW);
    digitalWrite(lk2,LOW);
    digitalWrite(lm2,HIGH);    
  }
  else{
    Serial.println("Data h tidak terbaca");
  }

  delay(500);
}
