# 🌱 Smart Flower Pot with Web Dashboard 🌐

This is a **Smart Flower Pot** project using an **ESP32**, which collects data from environmental sensors and **sends it to a Node.js server via HTTP**. The web dashboard displays real-time temperature, humidity, and light level updates, making it a mini IoT plant monitoring system.

![Smart Flower Pot Screenshot]
![image](https://github.com/user-attachments/assets/5c55b87d-9156-4405-902d-7379f8ca9976)
![image](https://github.com/user-attachments/assets/ed936f3c-33fc-4bb9-9e1d-aa8fbcc40f37)



## 🧠 Features

- 📈 Reads **temperature** and **humidity** from a DHT11 sensor.
- ☀️ Measures ambient **light levels** using a photocell.
- 🔴🟡🟢 Uses 3 LEDs to visually indicate environmental status.
- 🖥️ Shows real-time data on a local **OLED/LCD screen**.
- 🌍 Sends data to a **JavaScript server via HTTP POST**.
- 🌐 Real-time data displayed on a simple **web dashboard**.

## 🧰 Hardware Used

| Component        | Description                      |
|------------------|----------------------------------|
| ESP32            | Wi-Fi-enabled microcontroller    |
| DHT11            | Temperature and humidity sensor  |
| Photocell (LDR)  | Light sensor                     |
| 3 LEDs (RGB)     | Status indicators                |
| Jumper wires     | Connections                      |
| Breadboard       | For prototyping                  |

## 🌐 Web Integration

The ESP32 connects to a Wi-Fi network and sends a **JSON payload** to the JavaScript server every few seconds via an **HTTP POST request**:

```json
{
  "temperature": 25.3,
  "humidity": 60,
  "light": 342
}
