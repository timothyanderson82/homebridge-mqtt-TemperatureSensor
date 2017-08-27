# homebridge-mqtt-TemperatureSensor
Code for NodeMCU with DHT11 temperature sensor that will integrate with the homebridge-mqtt plugin

### Objective
In this project we use a NodeMCU to get the Temperature and Humidity from a DHT11 sensor.  This data will be published to the homebridge-mqtt topic which will update the living_humid & living_temp accessories configured in homebridge.  

### Environment prerequisites
You will require an existing homebridge-mqtt enviornment, I used the below repository in my setup:

https://github.com/cflurin/homebridge-mqtt

You will also need access to a mqtt broker. You can use one of the many free cloud mqtt brokers out there, i setup the broker on a windows PC that I have always running on the network. I used the below tutorial to get it setup:

https://sivatechworld.wordpress.com/2015/06/11/step-by-step-installing-and-configuring-mosquitto-with-windows-7/

### Arduino IDE required Libraries
ESP8266WiFi.h

ArduinoJson.h

[PubSubClient.h](https://pubsubclient.knolleary.net/)

[DHT.h](http://osoyoo.com/wp-content/uploads/samplecode/DHT.zip)



