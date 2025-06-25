  /*

  $GPGSV,<total_msgs>,<msg_number>,<sat_in_view>,
  <sat1_prn>,<elv1>,<az1>,<snr1>,
  <sat2_prn>,<elv2>,<az2>,<snr2>,...

  .*/

  #include <TinyGPS++.h>
  #include <HardwareSerial.h>
  #include <WiFi.h>
  #include <HTTPClient.h>

  const char *ssid = "Redmi";
  const char *pass = "prova123";
  unsigned long ultimoInvio = 0;
  const unsigned long intervallo = 10000;


  TinyGPSPlus gps;
  HardwareSerial gpsSerial(1);  // UART1 su pin 16 (RX) e 17 (TX)

  void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17 (ESP32)
  WiFi.begin(ssid, pass);

  while (Serial.available() > 0) {
    Serial.read();
  }
  while (gpsSerial.available() > 0) {
    gpsSerial.read();
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connesso!");
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


    // Se ci sono nuove coordinate valide

    if(millis() - ultimoInvio >= intervallo) {
      ultimoInvio = millis();

      if (gps.location.isUpdated()) {
        double lat = gps.location.lat();
        double lon = gps.location.lng();
        int sat = gps.satellites.value();

        if (WiFi.status() == WL_CONNECTED) {
          HTTPClient http;
          http.begin("http://192.168.167.244:8000/api/esp-data");
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");
          String dati = "lat=" + String(lat, 6) + "&lon=" + String(lon, 6);
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
        Serial.println("Fix non ancora pronto");
      }

    }
      delay(1000);
  }
