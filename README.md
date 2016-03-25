# smartdoorbell
ESP8266 + Arduino based WoT Project.
The project is arranged according to the individual components.
Arduino contains all the Arduino sketches and necessary Libraries.
Android contains the Android Studio project.
Webserver contains the API implementation code.

##Objective
Recent advances in technology and the trend towards adopting IoT and WoT have opened new facets and applications in the filed of Home Security, making it easy to bring more innovative and attractive features to the consumer. In this project, we concentrate on building a home security system which integrates hardware and software with modern web technologies to alert users of visitors to their house when they are not at home. We propose a very simple architecture and present a basic prototype implementation that demonstrates all the features.

##Goals
The main goal of this project is to simulate a smart doorbell for the following use cases:<br />
1. Remotely monitor visitors to your house.<br />
2. Keep track of any missed house calls or package deliveries.<br />
3. Keep a digital record of visitors.<br />

##Approach
SOLUTION<br />
● A push button to detect when bell is pressed. ESP8266 connected to button (ESP1) informs ESP8266 connected to Motion Sensors (ESP2) that there is a visitor at the door.<br />
● ESP8266 (ESP2) connected motion sensors placed around the house check if anyone is at home or not. (For the scope of this project, we will use only 1 motion detector). ESP2 will forward the status of motion sensors to the ESP1 as well as the Web Server.<br />
● ESP1 will in turn instruct the camera controlled by another ESP8266 (ESP3) to take visitor’s picture. ESP3 sends a message to the Web Server which serves this information to the mobile app installed on home owner’s phone.<br />
● The 4 components (ESP8266 controlled press button, ESP8266 controlled motion detector, ESP8266 controlled camera and the Web Server) communicate with each other through HTTP Requests.

DESIGN<br />
See images/high_level_organization.png for the high level architecture of the system.

##Setup
PUSH BUTTON MODULE (see images/push_button_module.png)<br />
CAMERA MODULE (see images/camera_module.png)<br />
MOTION SENSOR MODULE (see images/motion_sensor_module.png)<br />



