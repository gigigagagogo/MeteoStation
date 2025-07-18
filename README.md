# 🌦️ WeatherStation

## Introduzione

**Weather Station** è un progetto di monitoraggio ambientale basato su **ESP32**, progettato per raccogliere e trasmettere dati meteorologici (temperatura, umidità, pressione atmosferica) e geolocalizzazione (GPS). I dati vengono inviati via Wi-Fi a un server, dove possono essere visualizzati in tempo reale attraverso un’interfaccia web.

Il progetto è stato sviluppato per il corso di Elettronica Generale, con l'obiettivo di integrare competenze di elettronica, programmazione embedded, comunicazione wireless e visualizzazione dati.

---

## 🚀 Funzionalità principali

- Rilevamento **temperatura**, **umidità relativa**, **pressione atmosferica** (BME688)
- Acquisizione **posizione GPS** (NEO-6M)
- Trasmissione dati via **Wi-Fi** mediante richieste HTTP POST
- Backup locale dei dati su **scheda microSD**
- Interfaccia web per la visualizzazione dei dati in tempo reale
- Alimentazione da USB o power bank per operatività continua

---

## ⚙️ Componenti utilizzati

| Componente               | Descrizione                            |
|--------------------------|----------------------------------------|
| ESP32 DevKit             | Microcontrollore principale            |
| Adafruit BME688          | Sensore ambientale (T, P, RH, gas)     |
| Modulo GPS NEO-6M        | Modulo per tracciamento posizione      |
| Adattatore microSD       | Per salvataggio dati su memoria locale |
| LED + resistenze         | Indicatori di stato                    |
| Pulsante                 | Reset dati MicroSD                     |
| Breadboard e cavetti     | Cablaggio prototipale                  |

---

## 🧠 Architettura del sistema

1. **ESP32** raccoglie dati dai sensori (BME688, GPS).
2. I dati vengono formattati e inviati via HTTP a un **server web**.
3. I dati vengono salvati in un database MySQL.
4. Una dashboard PHP mostra i dati in tempo reale su **pagina web responsiva**.

---

## 🛠️ Tecnologie

- **Sensoristica**: BME688 (I2C), GPS NEO-6M (UART)
- **Reti**: Wi-Fi 2.4 GHz
- **Web**: Laravel, HTML, CSS (Bootstrap), MySQL
- **Hardware prototipale**: Breadboard, LED, pulsanti
- **Storage locale**: microSD (SPI)

---

*Aggiungi qui qualche foto del circuito montato e della dashboard web in azione, per dare credibilità visiva al progetto.*
