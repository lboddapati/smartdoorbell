#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "smartdoorbell";
const char* password = "iamsmart";


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

const char* motion_sensor_ip = "192.168.4.2";
const int http_port = 80;
const int led = D2;

void setup(void){
  pinMode(D5, INPUT);  // D5 pin mode
  pinMode(led, OUTPUT); // D2 pin mode
  digitalWrite(led, HIGH);
  
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Soft AP IP address: ");
  Serial.println(WiFi.softAPIP());

}

bool signalButtonPressed() {

  Serial.print("connecting to ");
  Serial.println(motion_sensor_ip);
  Serial.print("port ");
  Serial.println(http_port);
  // Attempt to make a connection to the motion sensor server
  WiFiClient client;
  int count = 0;
  while (!!!client.connect(motion_sensor_ip, http_port) && count < 10) {
    Serial.println("connection failed, retrying...");
    count++;
  }

  String url = "/button_signal";

  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // Make an HTTP GET request
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + motion_sensor_ip + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);

  Serial.println("-------------------------- Response ---------------------------");
  // If there are incoming bytes, print them
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  client.stop();
  Serial.println("----------------------------- End -----------------------------");  
  return true;
}

int last = HIGH;
void loop(void)
{
  int curr = digitalRead(D5);
  

  if (curr == last) return;

  else if (curr == LOW && !signalButtonPressed() ) {
      Serial.println("GET request failed");
  }
  
  int val = (curr == LOW ? 1 : 0);
  
  Serial.println("val");
  Serial.println(val);

  last = curr;
}
