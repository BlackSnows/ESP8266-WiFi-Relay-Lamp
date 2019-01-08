# ESP8266-WiFi-Relay-Lamp
Controls a Lamp via WiFi with a Relay connected to a ESP8266 Board with Capacitive Touch and a RGB LED, using Homebridge.

Project made to control a Lamp via HTTP, with an ESP8266 connected to the WiFi. The objective was to use the Apple HomeKit using Homebridge with the Better-HTTP-RGB plugin on a Raspberry Pi 3 B+

The Lamp is connected to a Relay in a ESP8266 board. There is a Capacitive Touch Sensor to control the lamp manually. There is also a RGB LED connected to show the lamp state, and the colors of each state can be configured via HTTP.

## HTTP URL Call examples:
### Turn Lamp ON
192.168.0.201/bedroomlamp/switch/on

### Turn Lamp OFF
192.168.0.201/bedroomlamp/switch/off

### Get Lamp STATUS
192.168.0.201/bedroomlamp/switch/status

### Get Lamp ON/OFF Color (For the actual state)
192.168.0.201/bedroomlamp/color/status

### Set Lamp ON/OFF Color (For the actual state)
192.168.0.201/bedroomlamp/color/HexColor

## Links
### Homebridge page
https://github.com/nfarina/homebridge

### Better-HTTP-RGB plugin page
https://github.com/jnovack/homebridge-better-http-rgb
