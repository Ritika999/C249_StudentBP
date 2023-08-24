#include <WiFi.h>
#include <Adafruit_BMP085.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "" //wifi name
#define WLAN_PASS       "" //password

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME  ""              
#define AIO_KEY       ""




WiFiClient client;



Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'temperature' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>

Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Room Temperature");

Adafruit_MQTT_Publish level = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/airpressure");

// Setup a feed called 'sw1' for subscribing to changes.
Adafruit_MQTT_Subscribe sw1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Room Light");

Adafruit_MQTT_Subscribe sw2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Room AC");

/*************************** Sketch Code ************************************/



void MQTT_connect();



const int led1 = 18;
const int led2 = 19;

float p;
float q;

String stringOne, stringTwo;

Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(9600);
  delay(10);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for sw1 feed.
  mqtt.subscribe(&sw1);

  // Setup MQTT subscription for sw2 feed.
  mqtt.subscribe(&sw2);

  if (!bmp.begin()) {
    Serial.println("BMP180 Sensor not found ! ! !");
    while (1) {}
  }
}

uint32_t x = 0;

void loop() {


  p = bmp.readPressure();
  q = bmp.readTemperature();
  Serial.println(p);
  Serial.println(q);
  delay(100);

  MQTT_connect();



  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &sw1) {
     
    }

    if (subscription == &sw2) {
     
    }
  }


  if (! temperature.publish(q)) {
    //Serial.println(F("Temp Failed"));
  } else {
    //Serial.println(F("Temp OK!"));
  }

  if (! level.publish(p)) {
    //Serial.println(F("pressure Level Failed"));
  } else {
    //Serial.println(F("pressure Level OK!"));
  }


}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  //Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    //Serial.println(mqtt.connectErrorString(ret));
    //Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }

}
