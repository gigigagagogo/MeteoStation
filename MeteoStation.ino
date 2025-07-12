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

// WiFi
const char *ssid = "Redmi";
const char *pass = "prova123";

// Timer
unsigned long ultimoInvio = 0;
const unsigned long intervallo = 30000; // invia dati ogni 30 secondi
unsigned long startTime = millis(); // tempo iniziale

// Sensori
Adafruit_BME680 bme;
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);  // UART1 su pin 16 (RX) e 17 (TX)

void setup() {
  Wire.begin(21, 22);  // SDA, SCL
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17); // RX = 16, TX = 17
  pinMode(4, INPUT_PULLUP); // Pulsante con pull-up interno
  setupLed();

  // Pulizia buffer seriali 
  while (Serial.available() > 0) Serial.read();
  while (gpsSerial.available() > 0) gpsSerial.read();

  Serial.println();

  setupWifi(); 
  setupSensor();
  setupSD();
  readFile();
  
  Serial.println("Avvio lettura GPS...");
}

void loop() {

  // Leggo i caratteri in arrivo dal GPS

  while (gpsSerial.available() > 0) {
    char c = gpsSerial.read();
    gps.encode(c);
  }

  // Reset dati quando premi il pulsante
  if (digitalRead(4) == LOW) {  // pulsante premuto (connesso a GND)
    Serial.println("Pulsante premuto, resetto dati SD...");
    resetSD();
    delay(1000); 
  } 
  
  Serial.print("Satelliti visibili: ");
  Serial.println(gps.satellites.value());

  static double ultimaTemperatura = 0; // valore persistente

  // Leggi sempre la temperatura per LED
  if (bme.performReading()) {
    ultimaTemperatura = bme.temperature;
    Serial.println(ultimaTemperatura);
    checkLed(ultimaTemperatura); // Aggiorna il LED in base alla temperatura
  }

  // Invio dati ogni 30 secondi
  if(millis() - ultimoInvio >= intervallo) {
    ultimoInvio = millis();

    // Se ci sono nuove coordinate valide
    if (bme.performReading()) {
      double lat = 0.0;
      double lon = 0.0;
      if (gps.location.isValid()) {
        lat = gps.location.lat();
        lon = gps.location.lng();
      } else {
        Serial.println("Non ci sono abbastanza satelliti disponibili, imposto latitudine e longitudine di default");
      }
      
      double temp = bme.temperature;
      double pres = bme.pressure / 100.0;
      double humi = bme.humidity;

      // Invio dati al server o salvo su SD
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin("http://10.74.159.244:8000/api/esp-data");
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

// salva dati su SD 
void salvaSD(double lat, double lon, double temp, double pres, double humi){
  Serial.println("Wifi non presente/Errore POST, procedo con la scrittura dei dati sull'SD!");
  
  File fl = SD.open("/gps_data.txt", FILE_APPEND);
  if(!fl) {
    Serial.println("Errore apertura file gps_data.txt");
    return;
  }
  Serial.println("File aperto, scrittura in corso...");
  fl.println("Latitudine: " + String(lat)
            + ", Longitudine: " + String(lon)
            + ", Temperatura: " + String(temp)
            + ", Pressione: " + String(pres)
            + ", Umidità: " + String(humi)
            );
  fl.flush();
  fl.close();
  Serial.println("Scrittura su SD avvenuta con successo!");
}

// resetta dati presenti su SD
void resetSD() {
  if (SD.exists("/gps_data.txt")) {
    SD.remove("/gps_data.txt");
    Serial.println("File gps_data.txt eliminato dalla SD!");
  } else {
    Serial.println("Nessun file da eliminare.");
  }

  File fl = SD.open("/gps_data.txt", FILE_WRITE);
  if (fl) {
    fl.close();
    Serial.println("File vuoto ricreato dopo il reset.");
  } else {
    Serial.println("Errore nella creazione del file vuoto.");
  }
}

// setup per connessione al wifi/passaggio in modalita salvataggio su SD
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

// setup BME688 sensor
void setupSensor() {
  if(!bme.begin(0x76, &Wire)) {
      Serial.println("Sensore BME688 non trovata");
      while(1);
  }
    
  Serial.println("Sensore BME688 collegato");

  // parametri per la precisione dei dati
  bme.setTemperatureOversampling(BME680_OS_16X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setHumidityOversampling(BME680_OS_4X);
}

// setup per SD
void setupSD() {
  if (!SD.begin(5)) {
    Serial.println("Scheda SD non trovata");
    while (1);
  }
  Serial.println("Scheda SD collegata");
}

// lettura da file
void readFile() {
  Serial.println("Avvio lettura dati vecchi su Scheda SD!");
  File fl = SD.open("/gps_data.txt", FILE_READ);
  
  if (!fl) {
    Serial.println("Errore apertura file gps_data.txt");
    return;
  }

  Serial.println("File aperto, contenuto:");
  while (fl.available()) {
    String riga = fl.readStringUntil('\n');
    riga.trim();  // Rimuove eventuali \r\n residui
    Serial.println(riga);
  }
  fl.close();
}


// setup iniziale dei LED
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

    // ROSSO: allarme
    if (temp >= 34 || temp < -2) {
      digitalWrite(15, ledState);
    } 
    // GIALLO: attenzione
    else if((temp >= -2 && temp <= 10) || (temp >= 28 && temp < 34)){
      digitalWrite(2, ledState);
    }
    // VERDE: tutto ok
    else {
      digitalWrite(0, ledState);
    }
  }
}