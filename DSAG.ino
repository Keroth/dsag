

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



WiFiClientSecure wclient;
PubSubClient client(host, port, wclient);


int16_t ax, ay, az;

unsigned long milli;

String mac;

char c_mac[18];

char *id;

String s_topic;

char data[20];

char topic[26];

void setup() {

    Wire.begin(5, 4);
        

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(115200);

    Serial.println("test2");
    
    // join I2C bus (I2Cdev library doesn't do this automatically)
//    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE

//    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
//        Fastwire::setup(400, true);
//        Serial.println("test3");
//    #endif

    

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    Serial.println("test1");

    // verify connection
    Serial.println("Testing device connections...");
    delay(100);
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    // use the code below to change accel/gyro offset values
//    Serial.println("Updating internal sensor offsets...");
    delay(100);
    // -76	-2359	1688	0	0	0
//    Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t"); // -1147
//    delay(100);
//    Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t"); // 349
//    delay(100);
//    Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t"); // 1305
//    delay(100);
//    Serial.print("\n");
//    delay(500);
    accelgyro.setXAccelOffset(-1515);
    accelgyro.setYAccelOffset(2068);
    accelgyro.setZAccelOffset(1151);
//    Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t"); // -1147
//    delay(100);
//    Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t"); // 349
//    delay(100);
//    Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t"); // 1305
//    delay(100);
//    Serial.print("\n");
//    delay(100);

    accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_8);
    delay(100);

    mac = WiFi.macAddress();

    mac.toCharArray(c_mac, 18);
    Serial.println(c_mac);

    id = c_mac;

    Serial.println(id);
    
    s_topic = "test/jj/" + String(id);

    s_topic.toCharArray(topic, (s_topic.length() + 1));
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
    delay(10000);
    }


    // Connect to MQTT
    if (WiFi.status() == WL_CONNECTED) {
      if (!client.connected()) {
        Serial.println("Connecting to MQTT server");
        // .set_auth("test", "test")
        if (client.connect(id, "test", "test")) {
          Serial.println("Connected to MQTT server");
        } else {
          Serial.println("Could not connect to MQTT server");
          delay(5000);
        }
      }
    }

    //Send data to the MQTT
    if (client.connected()){

      milli = millis();
//    Serial.print(float(milli/1000.0000), 3); Serial.print(";");
      float fmill = float(milli/1000.0000);
      // these methods (and a few others) are also available
      accelgyro.getAcceleration(&ax, &ay, &az);
      float fx = float(ax/4096.00);
      float fy = float(ay/4096.00);
      float fz = float(az/4096.00);
    
      String pay = String(fmill) + ";" + String(fx) + ";" + String(fy) + ";" + String(fz); 
      Serial.println(pay);
      pay.toCharArray(data, (pay.length() + 1));
      
      client.publish(topic, data, true);

      delay(200);

      client.loop();
    }

    

    // display tab-separated accel x/y/z values
//    Serial.print(float(ax/4096.00)); Serial.print(";");
//    Serial.print(float(ay/4096.00)); Serial.print(";");
//    Serial.print(float(az/4096.00)); Serial.print("\n");

    delay(200);

}
