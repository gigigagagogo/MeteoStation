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

  const char *ssid = "Redmi";
  const char *pass = "prova123";
  unsigned long ultimoInvio = 0;
  const unsigned long intervallo = 30000;
  
  Adafruit_BME680 bme;
  TinyGPSPlus gps;
  HardwareSerial gpsSerial(1);  // UART1 su pin 16 (RX) e 17 (TX)


  void setup() {
    Wire.begin(21, 22);  // SDA, SCL
    Serial.begin(115200);
    gpsSerial.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17 (ESP32)
    WiFi.begin(ssid, pass);

    delay(1000);
    Serial.println("Scanner I2C...");

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi connesso!");

    if(!bme.begin(0x76, &Wire)) {
      Serial.println("Sensore bme688 non connesso");
      while(1);
    }

    while (Serial.available() > 0) {
      Serial.read();
    }
    while (gpsSerial.available() > 0) {
      gpsSerial.read();
    }
    
    bme.setTemperatureOversampling(BME680_OS_16X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setHumidityOversampling(BME680_OS_4X);

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
          String dati = "lat=" + String(lat, 6) + "&lon=" + String(lon, 6) + "&temp=" + String(temp) + "&pres=" + String(pres) + "&humi=" + String(humi);
          int code = http.POST(dati);
          // 200 se è corretto
          if (code > 0) {
              String risposta = http.getString();
              Serial.println("Risposta server: " + risposta);
          } else {
              Serial.println("Errore POST");
          }

          http.end();
        }
      }else{
        Serial.println("Fix non ancora pronto o dati BME non validi");
      }

      

    }
      delay(10000);
  }
