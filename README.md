# Aave Health Factor Indicator
<img src="https://user-images.githubusercontent.com/49351206/168518173-9a4f7ced-1660-4d01-b9d0-e142a39e72ac.gif" width="350px" height="190px"/>

This project shows how healthy is your aave account. It has 16 built in leds as well as buzzer which signals when your hf drops below certain level.
It has a nice logo which shines in AAVE brand colors.

If you want to build this project you need:

* ESP8266 or ESP32
* 3x NEOPIXEL WS2812B modules, in my case I used two pcbs modules + few cm of flexible strips to light up logo
* one 5v buzzer
* few cables
* 3d prints based on attached stl files, one print needs transparent filament
* few insterts
* few screws

Pins needed to connect peripherals(leds, buzzer) are written in code - you can change them if you preffer, doesnt matter.
After programming controller, connect to access point created by ESP8266, configure your SSID and password.


![IMG_20220516_033127](https://user-images.githubusercontent.com/49351206/168518558-dfd8e9a5-55ce-43d1-bdf6-e57125e12950.jpg)
![IMG_20220516_033134](https://user-images.githubusercontent.com/49351206/168518570-74c8da92-ec00-4ae7-bf22-1a4ea571079b.jpg)
