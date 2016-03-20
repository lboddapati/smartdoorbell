#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <TimedAction.h>

// Function declarations
void handleRoot();
void sendStatusToServer(int state);
void updateSensorState();
void readStatusFromServer();
void buttonSignalReceived();
void signalCameraCapture();

// NODE MCU PIN MAPPINGS
static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t D9   = 3;
static const uint8_t D10  = 1;


//this initializes a TimedAction object that will check the state of Motion sensor every 5 seconds.
TimedAction sensorStateAction = TimedAction(5000,updateSensorState);

// SSID & PASSWORD of the network ESP will connect to,
// to access internet
// Replace these with your network SSID & PASSWORD
// NOTE: for some reason, it doesnt work with SCU wifi. Works fine with hotspots.
//       use your phone's hotspot to test.
const char* ssid = "Lasya's iPhone";
const char* password = "pottermore";
//const char* ssid = "HOME-A9B5-5";
//const char* password = "dairy8628artist";

// SSID & PASSWORD ESP shows up as while 
// acting as Access Point.
const char* newssid = "smartsensor";
const char* newpassword = "iamsmart";

const char* host = "www.boddapati.com";
const char* sensorID = "master_bedroom_sensor";
const IPAddress ip (192, 168, 4, 2);
const IPAddress nMask (255, 255, 255, 0);

ESP8266WebServer server(80);

//Power the PIR with 5V and connect ground to ground. Then connect the output to a digital pin. 
//In this use pin D4.
const int ledPin = D7;   // choose the pin for the LED
const int SensorInputPin = D4; // choose the input pin (for PIR sensor) 
int pirState = LOW; // we start, assuming no motion detected
boolean motion_detected = false; // to keep track of motion state. Initially set to false.

void handleRoot() {
  Serial.print("handleRoot: ");
  server.send(200, "text/html", "<h1>You are connected!</h1>");
}


// Send sensor status to web server
void sendStatusToServer(int state) {
  String requestUri = "";
  //requestUri += host;
  requestUri += "/motion_sensors/";
  requestUri += sensorID;
  requestUri += "/";
  requestUri += state;
  
  Serial.print("proxy request to ");
  Serial.println(host);

  WiFiClient client;
  while (!!!client.connect(host, 80)) {
    Serial.println("connection failed, retrying...");
  }

  Serial.print("Requesting uri: ");
  Serial.println(requestUri);

  // Set HTTP request headers
  client.print(String("PUT ") + requestUri);
  client.print(String(" HTTP/1.1\r\n") +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
               
  while(!!!client.available()){
    //Serial.println("client not available yet");
    yield();
  }

  String response = "";
  boolean msg_body_start = false;
  while(client.available()){
    String line = client.readStringUntil('\n');
    response += line;
    response += "\r\n";
  }
  Serial.println(response);
  //server.send(200, "application/json", response); // Serve response to user.
  client.stop();
}

// Test code: Read sensor status from server
// NOTE: this code is just for testing.
void readStatusFromServer() {
  String requestUri = "";
  requestUri += "/motion_sensors/";
  requestUri += sensorID;
  
  Serial.print("proxy request to ");
  Serial.println(host);

  WiFiClient client;
  while (!!!client.connect(host, 80)) {
    Serial.println("connection failed, retrying...");
  }

  Serial.print("Requesting uri: ");
  Serial.println(requestUri);

  // Set HTTP request headers
  client.print(String("GET ") + requestUri);
  
  client.print(String(" HTTP/1.1\r\n") +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
               
  while(!!!client.available()){
    yield();
  }

  String response = "";
  while(client.available()){
    response += client.readStringUntil('\n');
    response += "\r\n";
  }
  Serial.println(response);
  server.send(200, "application/json", response); // Serve response to user.
  client.stop();
}

// Function to check for motion detection
void updateSensorState() {
    int val = LOW;
    val = digitalRead(SensorInputPin);  // read input value
    Serial.print("val = ");
    Serial.println(val);
    Serial.print("pirState = ");
    Serial.println(pirState);
    
    if ((val == HIGH) && (pirState == LOW)) {
        //we have just turned on 
        Serial.println(" Motion detected!" );// We only want to print on the output change, not state
        pirState = HIGH;
        motion_detected = true;
        digitalWrite(ledPin, HIGH);
        sendStatusToServer(1);
        delay(5000); //Wait another 5 secs
    } 
    else if((val == LOW) && (pirState == HIGH)){  
       //we have just turned off
       Serial.println("Motion ended");
       pirState = LOW;
       digitalWrite(ledPin, LOW);
       motion_detected = false;
       sendStatusToServer(0);
    }
}


// Test code: Function to get status of sensor. Returns a json response.
// NOTE: this code is just for testing.
void getStatus() {
  String response = "{ \"id\": \"";
  response += sensorID;
  response += "\", \"status\": ";
  response += motion_detected;
  response += "}";
  server.send(200, "application/json", response);
}


void buttonSignalReceived() {
  Serial.println("button pressed signal received");
  String response = "{ \"motion_detected\": ";
  response += motion_detected;
  response += "}";
  server.send(200, "application/json", response);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT); // declare LED as output
  digitalWrite(ledPin, LOW);//new Set LED as LOW as we are assuming that no motion is detected intitially
  pinMode(SensorInputPin, INPUT); // declare sensor as input
  digitalWrite(SensorInputPin, LOW); //new Set sensor LOW as we are assuming that no motion is detected intitially

  Serial.begin(115200);
  Serial.println("");

  // set both access point and station
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(ip, ip, nMask); 

  // set ssid & password for access point
  WiFi.softAP(newssid, newpassword);

  server.on("/", handleRoot);  
  server.on("/status",getStatus);
  server.on("/button_signal", buttonSignalReceived);
  server.begin();
  Serial.println("HTTP server started");

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

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
}

void loop() {
    sensorStateAction.check();
    server.handleClient();
}
