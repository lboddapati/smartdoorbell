This RESTful Apache webserver is built on Slim PHP framework.

Download and install Slim PHP V3 by following the instructions here:
http://www.slimframework.com/docs/start/installation.html

Set up and configure the webserver.


APIs
====
NOTE: All the APIs return a json response.

1. POST REQUEST TO UPLOAD IMAGES TO SERVER
   Usage: POST /api.php/visitors HTTP/1.1
          Host: www.boddapati.com


   Request must have enctype: multipart/form-data.
   Post data must include input of 'file' type  with name and id 'image';
   Check the example in test.html
   Response: json response in the format ->
    {
        "error": true/false,
        "message" : "response message here"
    }

2. GET REQUEST TO GET ALL VISITORS ON A DATE
   Usage: GET /visitors/yyyy-mm-dd HTTP/1.1
          Host: www.boddapati.com

   Response: json response in the format ->
    {
        error: false,
        visitors: {
            2016-03-07: [
                {
                    time: "0000-00-00 00:00:00",
                    image_url: "visitors/2016-03-07_14-32-07.jpg"
                }
            ]
        }
    }

    Example: www.boddapati.com/visitors/2016-03-06

3. PUT REQUEST TO UPDATE MOTION SENSOR STATUS
   Usage: PUT /motions_sensors/sensor_id/status HTTP/1.1
          Host: www.boddapati.com

   sensor_id is the unique name of the sensor (eg: masterbedroom_sensor)
   status can be 0/false and 1/true

   Response: if successful json response in the format ->
    {
        "error":false,
        "id":"master_bedroom_sensor",
        "status":"0"
    }

    Check the example in test.html


4. GET REQUEST TO GET MOTION SENSOR STATUS
   Usage: GET /motions_sensors/sensor_id HTTP/1.1
          Host: www.boddapati.com

   sensor_id is the unique name of the sensor (eg: masterbedroom_sensor)

   Response: json response in the format ->
    {
        "error":false,
        "id":"master_bedroom_sensor",
        "status":"0"
    }

   Example: www.boddapati.com/motions_sensors/master_bedroom_sensor

5. GET REQUEST TO GET ALL MOTION SENSORS STATUSES
   Usage: GET /motions_sensors HTTP/1.1
          Host: www.boddapati.com

   Response: json response in the format ->
    {
        error: false,
        sensors: [
            {
                id: "family_room_sensor",
                status: "0"
            },
            {
                id: "guest_bedroom_sensor",
                status: "0"
            },
            {
                id: "kitchen_sensor",
                status: "1"
            },
            {
                id: "master_bedroom_sensor",
                status: "1"
            }
        ]
    }

   Example: www.boddapati.com/motions_sensors
