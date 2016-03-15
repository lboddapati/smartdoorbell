// Arducam mini controlled by ESP8266.  Arduino IDE 1.6.5. Photo upload to a php script on a webserver using html POST.
// Libraries from Arducam github placed under arduino/libraries/arduCAM. Power consumption ca 200 mA from 3.3V.
// PIR module controls FET that switches on 3.3 power Lipo+buck regulator used.  ESP connects to WiFi and starts taking and uploading photos.
// http://www.arducam.com/tag/arducam-esp8266/
// ESP12-E module: CS=GPIO16   MOSI=GPIO13   MISO=GPIO12   SCK=GPIO14  GND&GPIO15  VCC&EN=3.3V SDA=GPIO4  SCL=GPIO5  GPIO0=button  GPIO2=BlueLED

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"

WiFiClient client;

const char* ssid = "Lasya's iPhone";
const char* password = "pottermore";
//const char* ssid     = "BroncoWiFi";
//const char* password = "gosantaclara";
//const char* ssid     = "Atul Padha's iPhone";
//const char* password = "anshu1234";


//const char* ssid     = "HOME-A9B5-5";
//const char* password = "dairy86268artist";

//const char* ssid     = "ATT185";
//const char* password = "5463651522";



// SSID & PASSWORD ESP shows up as while 
// acting as Access Point.
const char* newssid = "smartcamera";
const char* newpassword = "iamsmart";

const char* host = "www.boddapati.com";
const IPAddress ip (192, 168, 4, 5);
const IPAddress gateway (192, 168, 4, 1);
const IPAddress nMask (255, 255, 255, 0);


ESP8266WebServer server(80);

#define DEBUGGING  // Enabe debug tracing to Serial port.
#define MAX_FRAME_LENGTH 64   // Here we define a maximum framelength to 64 bytes. Default is 256.
#define CALLBACK_FUNCTIONS 1  // Define how many callback functions you have. Default is 1.
const int CS = 16; // chip select of camera
ArduCAM myCAM(OV2640, CS);

int led=2; //blue led pin on GPIO2 of ESP-12E  switching it on during picture taking.

void handleroot() {
  server.send(200, "text/html", "you are connected to camera");
  Serial.println("you are connected to camera");
}

void Camera(){      //reads out pixels from the Arducam mini module
  pinMode(led, OUTPUT); digitalWrite(led, LOW); // Illuminate with blue LED on GPIO2.
  myCAM.clear_fifo_flag();
  myCAM.start_capture();
  while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));
  Serial.print("Picture captured. ");

  
  size_t len = myCAM.read_fifo_length();
  if (len >= 393216){
    Serial.println("Over size."); 
    return;
    }
  else if (len == 0){
    Serial.println("Size is 0."); 
    return;
    }
  Serial.print("Length in bytes: "); 
  Serial.println(len); 
  Serial.println();
  myCAM.CS_LOW();
  myCAM.set_fifo_burst(); 
  SPI.transfer(0xFF);

  
  //if (client.connect(host, 80)) {
  while (!!!client.connect(host, 80)) {
    Serial.println("connection failed, retrying...");
  }
    Serial.println("connected to host"); 
    
    
  String start_request = ""; String end_request = "";
  start_request = start_request + 
  "\r\n--AaB03x\r\n" + 
  "Content-Disposition: form-data; name=\"image\"; filename=\"CAM.JPG\"\r\n" + 
  "Content-Transfer-Encoding: binary\r\n\r\n";
  
  end_request = end_request + "\r\n--AaB03x--\r\n";  // in file upload POST method need to specify arbitrary boundary code

  uint16_t full_length;
  full_length = start_request.length() + len + end_request.length();

   // Serial.println("POST /api.php/visitors HTTP/1.1");
   Serial.println("Host: www.boddapati.com");
    Serial.println("Content-Type: multipart/form-data; boundary=AaB03x");
    Serial.print("Content-Length: "); 
    Serial.println(full_length);
    Serial.print(start_request); 
    Serial.println("Here are sent picture data"); 
    Serial.println(end_request); 

    client.print("POST /api.php/visitors HTTP/1.1\r\n");
    client.print("Host: www.boddapati.com\r\n");
    client.print("Content-Type: multipart/form-data; boundary=AaB03x\r\n");
    client.print("Content-Length: "); 
    client.print(full_length); 
    client.print("\r\n");
    client.print(start_request);

   
  // Read image data from Arducam mini and send away to internet
  static const size_t bufferSize = 1024; 
  static uint8_t buffer[bufferSize] = {0xFF};
  while (len) {
      size_t will_copy = (len < bufferSize) ? len : bufferSize;
      SPI.transferBytes(&buffer[0], &buffer[0], will_copy);
      if (!client.connected()) break;
      client.write(&buffer[0], will_copy);
      len -= will_copy;
      }
  
     client.println(end_request);
     myCAM.CS_HIGH(); 
     digitalWrite(led, HIGH);
     Serial.println("data sent to server");

  // Read  the reply from server
  delay(500); 
  String response = "";
  while(client.available()){ 
    String line = client.readStringUntil('\r'); 
    Serial.print(line);
    response += line;
    }
     server.send(200, "application/json", response);
  client.stop();
 // server.send(200, "application/json", response);
  //}
  
  
}



void setup() {
  uint8_t vid, pid, temp;
  Wire.begin();
  Serial.begin(115200); 
  Serial.println("ArduCAM Mini ESP8266 uploading photo to server");

  pinMode(CS, OUTPUT); // set the CS as an output:
  SPI.begin(); // initialize SPI
  SPI.setFrequency(4000000); //4MHz

  //Check if the ArduCAM SPI bus is OK
  myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = myCAM.read_reg(ARDUCHIP_TEST1);
  if (temp != 0x55){
    Serial.println("SPI1 interface Error!");
    while(1);
    }
  

  // Check if the camera module type is OV2640
  myCAM.wrSensorReg8_8(0xff, 0x01);
  myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
  myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
  if ((vid != 0x26) || (pid != 0x42)){
    Serial.println("Can't find OV2640 module!");
  while(1);
  }
  else{
    Serial.println("OV2640 detected.");
    }

 
 
  // Change to JPEG capture mode and initialize the OV2640 module
  myCAM.set_format(JPEG);  
  myCAM.InitCAM();   // sadly, jpeg quality can not be changed
  //myCAM.OV2640_set_JPEG_size(OV2640_640x480);
  myCAM.OV2640_set_JPEG_size(6);   //0...8
  myCAM.clear_fifo_flag(); 
  myCAM.write_reg(ARDUCHIP_FRAMES, 0x00);  
  


//  // Connect to a router
//  WiFi.mode(WIFI_STA);
//  Serial.println("Connecting to AP specified during programming");
//  WiFi.begin("LMT-89B0", "331M372E70J");
////  WiFi.begin("LU_ASI", "photoplethysmography");
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500); 
//    Serial.print("."); 
//    }
//  Serial.print("\r\nWiFi connected IP address: ");
//  Serial.println(WiFi.localIP());
//  Serial.println();
 // set both access point and station

  // set both access point and station
 WiFi.mode(WIFI_AP_STA);
 WiFi.softAPConfig(ip,ip,nMask);

 // set ssid & password for access point
 WiFi.softAP(newssid, newpassword);
 server.on("/", handleroot);
 server.on("/capture", Camera);
 server.begin();
 Serial.println("HTTP server started");

 //if (strcmp (WiFi.SSID(),ssid) != 0) {
     Serial.print("Connecting to ");
     Serial.println(ssid);
     WiFi.begin(ssid, password);
 //}

 while (WiFi.status() != WL_CONNECTED) {
   yield();
   Serial.print(".");
 }

 Serial.print("Connected to: ");
 Serial.print(WiFi.SSID());
 Serial.print(", IP address: ");
 Serial.println(WiFi.localIP());
 Serial.print(newssid);
 Serial.print(" server ip: ");
 Serial.println(WiFi.softAPIP());


 // Camera(myCAM);
  }
  

void loop() {
  //Camera(myCAM);
  server.handleClient();
  
  //delay(60000);
}


