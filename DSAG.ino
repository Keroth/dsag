

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "settings.h"


MPU6050 accelgyro;

WiFiClient client;

HTTPClient http;  //Declare an object of class HTTPClient

const char * headerkeys[] = {"User-Agent","Set-Cookie","Cookie","Date","Content-Type","Connection"};
size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);

int16_t ax, ay, az;

long sumx, sumy, sumz;

unsigned long milli;

char *id;

String test123;

int count = 0, times = 3, flag = 0;

String cookie = "";

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

    accelgyro.setXAccelOffset(-1617);
    accelgyro.setYAccelOffset(-57);
    accelgyro.setZAccelOffset(4913);

    accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_8);
    delay(100);

    
    String mac;
    char c_mac[18];
    mac = WiFi.macAddress();
    mac.toCharArray(c_mac, 18);
    id = c_mac;
    test123 = String(id);
    Serial.println(test123);

    delay(50);

    http.setReuse(true);
    http.collectHeaders(headerkeys,headerkeyssize);
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

    Serial.println(count);
    // these methods (and a few others) are also available
    accelgyro.getAcceleration(&ax, &ay, &az);
    sumx = sumx + ax;
    sumy = sumy + ay;
    sumz = sumz + az;
      

    if (count == times){  
      int avgx = sumx / times; Serial.print(avgx); sumx = 0; Serial.print("\t");
      int avgy = sumy / times; Serial.print(avgy); sumy = 0; Serial.print("\t");
      int avgz = sumz / times; Serial.println(avgz); sumz = 0;
      float fx = float(avgx/4096.00);
      float fy = float(avgy/4096.00);
      float fz = float(avgz/4096.00);
      milli = millis();
      // We now create a URI for the request
      String host = String(mii_host) + ":" + String(mii_Port);
      
      String payload = "/XMII/Illuminator?QueryTemplate=dsag/datahandler/sensorDataHandler&Param.1=" + test123 + "&Param.2=" + String(milli) + "&Param.3=" + String(fx) + "&Param.4=" + String(fy) + "&Param.5=" + String(fz);
      String login = "&IllumLoginName=" + String(mii_user) +"&IllumLoginPassword=" + String(mii_pw);
      String url = "";
      if (flag == 0){
        url = payload + login;
        flag = 1;
      } else {
        url = payload;
      }

      
//      Serial.print("Requesting URL: "); Serial.println(url);

//      Serial.print("connecting to "); Serial.println(mii_host);
    
      if (!client.connect(mii_host, mii_Port)) {
        Serial.println(client.connect(mii_host, mii_Port));
        Serial.println("connection failed");
        return;
      }
  
      // This will send the request to the server
      if (cookie == ""){
        http.begin(mii_host, mii_Port, url);  //Specify request destination
        int httpCode = http.GET();                                                                  //Send the request
        Serial.println(httpCode);
        Serial.println(url);
   

   
          String pay = http.getString();   //Get the request response payload
  //        Serial.println(pay);                     //Print the response payload
  
          Serial.printf("Header count: %d\r\n", http.headers());
          for (int i=0; i < http.headers(); i++) {
            Serial.printf("%s = %s\r\n", http.headerName(i).c_str(), http.header(i).c_str());
          }
          Serial.printf("Cookie: %s\r\n", http.header("Cookie").c_str());
          Serial.printf("Set-Cookie: %s\r\n", http.header("Set-Cookie").c_str());
          cookie = http.header("Set-Cookie").c_str();
  
 
        http.end();   //Close connection
      } else {
        client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + mii_host + "\r\n" + 
               "Cookie: " + cookie + "\r\n" +
               "Connection: close\r\n\r\n");

         Serial.println("Respond:");
         while(client.available()){
            String line = client.readStringUntil('\r');
            Serial.print(line);
         }
      }
       
      count = 0;
    }
    
    count = count + 1;
    delay(50);

}
