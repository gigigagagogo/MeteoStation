/*

$GPGSV,<total_msgs>,<msg_number>,<sat_in_view>,
<sat1_prn>,<elv1>,<az1>,<snr1>,
<sat2_prn>,<elv2>,<az2>,<snr2>,...

.*/

#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>

const char *ssid = "Redmi";
const char *pass = "prova123";
unsigned long ultimoInvio = 0;
const unsigned long intervallo = 30000;
unsigned long startTime = millis();
Adafruit_BME680 bme;
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);  // UART1 su pin 16 (RX) e 17 (TX)

void setup() {
  Wire.begin(21, 22);  // SDA, SCL
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17); // RX = 16, TX = 17

  while (Serial.available() > 0) Serial.read();
  while (gpsSerial.available() > 0) gpsSerial.read();

  Serial.println();

  setupWifi();
  setupSensor();
  //setupSD();
  setupLed();
  //readFile();
  
  Serial.println("Avvio lettura GPS...");
}

void loop() {
  // Leggo i caratteri in arrivo dal GPS

  while (gpsSerial.available() > 0) {
    char c = gpsSerial.read();
    gps.encode(c);
  }

  Serial.print("Satelliti visibili: ");
  Serial.println(gps.satellites.value());

  static double ultimaTemperatura = 0; // valore persistente

  // Leggi sempre la temperatura per LED, anche se non invii
  if (bme.performReading()) {
    ultimaTemperatura = bme.temperature;
    checkLed(ultimaTemperatura); // CHIAMATA CONTINUA
  }

  if(millis() - ultimoInvio >= intervallo) {
    ultimoInvio = millis();

    // Se ci sono nuove coordinate valide
    if (gps.location.isUpdated() && bme.performReading()) {
      double lat = gps.location.lat();
      double lon = gps.location.lng();
      double temp = bme.temperature;
      double pres = bme.pressure / 100.0;
      double humi = bme.humidity;
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin("http://192.168.167.244:8000/api/esp-data");
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        http.setTimeout(3000);
        String dati = "lat=" + String(lat, 6) + "&lon=" + String(lon, 6) + "&temp=" + String(temp) + "&pres=" + String(pres) + "&humi=" + String(humi);
        int code = http.POST(dati);
        // 200 se è corretto
        if (code > 0 && code == HTTP_CODE_OK) {
            String risposta = http.getString();
            Serial.println("Risposta server: " + risposta);
        } else {
            Serial.println("Errore POST o assenza di Wifi");
            salvaSD(lat, lon, temp, pres, humi);
        }
        http.end();
      } else {
        salvaSD(lat, lon, temp, pres, humi);
      }
    }else{
      Serial.println("Fix non ancora pronto o dati BME non validi");
    }
  }
}

void salvaSD(double lat, double lon, double temp, double pres, double humi){
  Serial.println("Wifi non presente/Errore POST, procedo con la scrittura dei dati sull'SD!");
  File fl = SD.open("gps_data.txt", FILE_WRITE);
  if(fl) {
    fl.println("Latitudine: " + String(lat)
              + ", Longitudine: " + String(lon)
              + ", Temperatura: " + String(temp)
              + ", Pressione: " + String(pres)
              + ", Umidità: " + String(humi)
              );
    fl.flush();
    fl.close();
    Serial.println("Scrittura su SD avvenuta con successo!");
  } else {
    Serial.println("Errore apertura file");
  }
}

void setupWifi() {
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000 ) {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED){
      Serial.println();
      Serial.println("WiFi connesso!");
    } else {
      Serial.println();
      Serial.println("WiFi non disponibile. Proseguo in modalità offline.");
    }
}

void setupSensor() {
  if(!bme.begin(0x76, &Wire)) {
      Serial.println("Sensore BME688 non trovata");
      while(1);
  }
    
  Serial.println("Sensore BME688 collegato");

  bme.setTemperatureOversampling(BME680_OS_16X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setHumidityOversampling(BME680_OS_4X);
}

void setupSD() {
  if (!SD.begin(5)) {
    Serial.println("Scheda SD non trovata");
    while (1);
  }
  Serial.println("Scheda SD collegata");
}

void readFile() {
  File fl = SD.open("gps_data.txt");
  if(fl) {
    Serial.println("File aperto");
    while(fl.available()) {
      Serial.println(fl.read());
    }
    fl.close();
  }
}
void setupLed() {
  pinMode(15, OUTPUT); // ROSSO
  pinMode(2, OUTPUT); // GIALLO
  pinMode(0, OUTPUT); // VERDE
  digitalWrite(15, LOW);
  digitalWrite(2, LOW);
  digitalWrite(0, LOW);
}

void checkLed(double temp) {
  static unsigned long lastBlink = 0;
  static bool ledState = false;
  const unsigned long blinkInterval = 500; // lampeggia ogni 500ms

  unsigned long now = millis();
  if (now - lastBlink >= blinkInterval) {
    lastBlink = now;
    ledState = !ledState;    

    if (temp >= 34 || temp < -2) {
      digitalWrite(15, ledState);
    } else if((temp >= -2 && temp <= 10) || (temp >= 28 && temp < 34)){
      digitalWrite(2, ledState);
    } else {
      digitalWrite(0, ledState);
    }
  }
}