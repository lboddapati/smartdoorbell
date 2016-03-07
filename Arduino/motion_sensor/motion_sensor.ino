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


//this initializes a TimedAction object that will check the state of Motion sensor every 5 seconds.
TimedAction sensorStateAction = TimedAction(5000,updateSensorState);

// SSID & PASSWORD of the network ESP will connect to,
// to access internet
// Replace these with your network SSID & PASSWORD
// NOTE: for some reason, it doesnt work with SCU wifi. Works fine with hotspots.
//       use your phone's hotspot to test.
const char* ssid = "your_network_ssid";
const char* password = "yourpassword";

// SSID & PASSWORD ESP shows up as while 
// acting as Access Point.
const char* newssid = "smartdoorbell";
const char* newpassword = "";

const char* host = "www.boddapati.com";
const char* sensorID = "master_bedroom_sensor";
const IPAddress ip (192, 168, 4, 2);
const IPAddress nMask (255, 255, 255, 0);

ESP8266WebServer server(80);

//Power the PIR with 5V and connect ground to ground. Then connect the output to a digital pin. 
//In this use pin 2.
const int ledPin = 13;   // choose the pin for the LED //D7
const int SensorInputPin = 2; // choose the input pin (for PIR sensor) //D4
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
    int val = 0;
    //int motion_detected = 0;
    val = digitalRead(SensorInputPin);  // read input value
    //Serial.println(val);
    if (val == HIGH) {  // check if the input is HIGH    
      if (pirState == LOW) {// we have just turned on
          Serial.println(" Motion detected!" );// We only want to print on the output change, not state
          pirState = HIGH;
          motion_detected = true;
          sendStatusToServer(1);
          delay(5000); //Wait another 5 secs
      }
    } 
    else {           
      if(pirState == HIGH){
         //we have just turned of
         Serial.println("Motion ended");
         //we only want to print on the ouput change,not state
         pirState = LOW;
         motion_detected = false;
         sendStatusToServer(0);
      }
    }
    //sendStatusToServer(motion_detected);
    //return motion_detected;
}


// Function to get status of sensor. Returns a json response.
void getStatus() {
  String response = "{ \"id\": \"";
  response += sensorID;
  response += "\", \"status\": ";
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
  server.begin();
  Serial.println("HTTP server started");

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    yield();
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
