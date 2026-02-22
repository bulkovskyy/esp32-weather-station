# ESP32 Weather Station 
  
Just tinkering with **ESP32** and **FreeRTOS**. This project reads temperature, humidity, and pressure from a **BME280 sensor** and serves the measurements over an **HTTP web server**.

## Features
- Reads temperature, humidity, and pressure from a **BME280 sensor** over I2C protocol.
- Periodically collects measurements using **FreeRTOS software timers**.
- Serves measurements over HTTP via a simple **web server**.
- Connects to WiFi using **ESP-IDF WiFi APIs**.
- Modular design: WiFi, sensors, measurements provider, and web server are separate components.

## Hardware Required
- ESP32 development board
- BME280 sensor** (I2C protocol)
- A couple of jumper wires

## Wiring (I2C)
| ESP32 Pin | BME280 Pin |
|-----------|------------|
| 25 | SDA |
| 26 | SCL |
| GND | GND |
| 3.3V | VCC |

*(This is a default configuration that can be changed via `measurements_provider_config_t`.) All required configuration is placed in `main.c` file.*

## Software Structure
- `sensor_data_provider` — Handles BME280 sensor initialization and readings.
- `measurements_provider` — Periodically collects measurements using a FreeRTOS timer and sends them to a queue.
- `wifi_connector` — Connects ESP32 to WiFi network.
- `web_server` — Serves HTTP requests, providing JSON-formatted measurements.
- `main.c` — Initializes all components and connects them together.

## How It Works
1. The measurements provider reads data from the BME280 sensor every N milliseconds.
2. Measurements are sent to a FreeRTOS queue.
3. The web server reads the latest measurements from the queue and returns them via HTTP.
4. WiFi connection is handled in station mode, reconnecting automatically if disconnected.

## Example HTTP Request
```http
GET http://<ESP32_IP>/measurements
```

## Example JSON Response:
```json
{
    "temperature": "25.12 °C",
    "humidity": "40.50 %",
    "pressure": "1013.25 hPa"
}
```

## How to Build and Flash
1.  Install **ESP-IDF** following the official instructions.
2.  Clone this project into your ESP-IDF workspace.
3. Configure WiFi, measurement, and pin parameters in `main.c`.
4.  Run:
```bash
    idf.py set-target esp32
    idf.py build flash monitor
```
5.  Open the serial monitor to see logs, including sensor readings and HTTP requests.

## License
This project is released under the **MIT License**.