#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include<dht.h>
dht DHT;

// Define NodeMCU pin 4(D2) as the temperature data pin of  DHT11
#define DHT11_PIN 4

// Update these with values suitable for your network.
const char* ssid = "YourSSIDHere";
const char* password = "YourPasswordHere";
const char* mqtt_server = "10.0.0.3";
//const char* mqtt_server = "iot.eclipse.org";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[110];
int value = 0;



void setup_wifi() {
   delay(100);
  // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//Configure a callback should there be any commands pushed to the subscribed topic - for testing purposes I added a LED to GPIO13
// and added the kitchenlight service/accessory to homebridge.
void callback(char* topic, byte* payload, unsigned int length) 
{
  StaticJsonBuffer<300> jsonBufferTopic;
  Serial.print("Payload is : ");

  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
    }
    Serial.println();
    JsonObject& root = jsonBufferTopic.parseObject(payload);
    String service = root["service_name"];
    Serial.println(service);
    String check = "kitchenlight";
    if (service == check) {
      bool set = root["value"];
      if (set) {
        Serial.println("Turn Light On!");
        digitalWrite(13, HIGH);
        } else if (!set){
          Serial.println("Turn Light Off!");
          digitalWrite(13, LOW);
          }
      }

       
  
} //end callback

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if you MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
     //once connected to MQTT broker, subscribe command if any
      client.subscribe("homebridge/from/set");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(6000);
    }
  }
} //end reconnect()

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  int chk = DHT.read11(DHT11_PIN);
  Serial.print(" Starting Humidity: " );
  Serial.print(DHT.humidity, 1);
  Serial.println('%');
  Serial.print(" Starting Temparature: ");
  Serial.print(DHT.temperature, 1);
  Serial.println('C');
  pinMode(13, OUTPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  // read DHT11 sensor every 60 seconds
  if (now - lastMsg > 60000) {
     lastMsg = now;
     int chk = DHT.read11(DHT11_PIN);  
     float temp = DHT.temperature;
     float humid = DHT.humidity;  
     //post temperature data
     StaticJsonBuffer<300> JSONbuffer;
     JsonObject& JSONencoder = JSONbuffer.createObject();
     JSONencoder["name"] = "living_temp";
     JSONencoder["service_name"] = "Temperature";
     JSONencoder["characteristic"] = "CurrentTemperature";
     JSONencoder["value"] = temp;
     char JSONmessageBuffer[110];
     JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
     
     if (client.publish("homebridge/to/set", JSONmessageBuffer) == true) {
        Serial.println("Success sending temperature message");
        Serial.println(temp);
      } else {
          Serial.println("error sending temperature message");
        }
     //post humidity data
     StaticJsonBuffer<300> JSONHbuffer;
     JsonObject& JSONHencoder = JSONHbuffer.createObject();
     JSONHencoder["name"] = "living_humid";
     JSONHencoder["service_name"] = "Humidity";
     JSONHencoder["characteristic"] = "CurrentRelativeHumidity";
     JSONHencoder["value"] = humid;
     char JSONHmessageBuffer[110];
     JSONHencoder.printTo(JSONHmessageBuffer, sizeof(JSONHmessageBuffer));

     if (client.publish("homebridge/to/set", JSONHmessageBuffer) == true) {
        Serial.println("Success sending humidity message");
        Serial.println(humid);
      } else {
          Serial.println("error sending humidity message");
        }
    
  }
}
