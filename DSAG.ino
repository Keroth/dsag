

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "settings.h"


// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high



//WiFiClientSecure wclient;
WiFiClient client;
//PubSubClient mqtt_client(mqtt_host, mqtt_port, client);

int16_t ax, ay, az;

unsigned long milli;

char *id;


char data[20];

char topic[26];

//-------------------------------------------
//              SETUP
//-------------------------------------------

void setup() {

    Wire.begin(5, 4);
        
    Serial.begin(115200);

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    delay(100);
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    accelgyro.setXAccelOffset(-1515);
    accelgyro.setYAccelOffset(2068);
    accelgyro.setZAccelOffset(1151);

    accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_8);
    delay(100);

    
    String mac;
    char c_mac[18];
    mac = WiFi.macAddress();
    mac.toCharArray(c_mac, 18);
    id = c_mac;
//    Serial.println(id);

//    String s_topic;
//    s_topic = "/test/jj/" + String(id);
//    s_topic.toCharArray(topic, (s_topic.length() + 1));
//    Serial.println(topic);

    delay(50);
}

void loop() {
    // Connect to Wifi
    if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    WiFi.begin(ssid, pass);
    if (WiFi.waitForConnectResult() != WL_CONNECTED)
      return;
    Serial.println("WiFi connected");
    delay(10);
    }


//    // Connect to MQTT
//    if (WiFi.status() == WL_CONNECTED) {
//      if (!mqtt_client.connected()) {
//        Serial.println("Connecting to MQTT server");
//        if (mqtt_client.connect(id, mqtt_user, mqtt_pw)) {
//          Serial.println("Connected to MQTT server");
//        } else {
//          Serial.println("Could not connect to MQTT server");
//        }
//      }
//    }


      milli = millis();
      // these methods (and a few others) are also available
      accelgyro.getAcceleration(&ax, &ay, &az);
      float fx = float(ax/4096.00);
      float fy = float(ay/4096.00);
      float fz = float(az/4096.00);
    
//      String pay = String(milli) + ";" + String(fx) + ";" + String(fy) + ";" + String(fz); 
//      Serial.println(pay);
//      pay.toCharArray(data, (pay.length() + 1));

    Serial.print("connecting to "); Serial.println(mii_host);
    
    if (!client.connect(mii_host, mii_Port)) {
      Serial.println(client.connect(mii_host, mii_Port));
      Serial.println("connection failed");
      return;
    }
    
  // We now create a URI for the request
  String url = "/XMII/Illuminator?QueryTemplate=dsag/datahandler/sensorDataHandler&Param.1=" + String(id) + "&Param.2=" + String(milli) + "&Param.3=" + String(fx) + "&Param.4=" + String(fy) + "&Param.5=" + String(fz) + "&IllumLoginName=" + mii_user +"&IllumLoginPassword=" + mii_pw;
//  Serial.print("Requesting URL: "); Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + mii_host + "\r\n" + 
               "Connection: close\r\n\r\n");

//  //Send data to the MQTT
//  if (mqtt_client.connected()){
//      
//    mqtt_client.publish(topic, data, true);
//
//    mqtt_client.loop();
//  }

  delay(200);

}
