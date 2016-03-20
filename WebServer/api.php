<?php

/**
 * Step 1: Require the Slim Framework using Composer's autoloader
 */
require 'vendor/autoload.php';

date_default_timezone_set('America/Los_Angeles');
$HOST="http://www.boddapati.com/";
$GCM_SERVER_API_KEY="AIzaSyDPI6FjtFgt48GhrWzY8QKonlyXc-YkhdU";
$GCM_APP_REG_TOKEN = "eTGCIczkDqQ:APA91bFWaGMw9AzkyLrErvAA34yG-raW4ijJ5WvpPOH1xfv6jYb6qWXlxNe1FXOT59GkARnIW93YkXxeGpYBreepAXMSBW74H-nUvn8q_7KTICYTwxKa5uchyL0NcqSYOreX3BFqNKTk";

function getDB()
{
    $dbhost = "localhost";
    $dbuser = "root";
    $dbpass = "coen369password";
    $dbname = "smartdoorbell";
 
    $mysql_conn_string = "mysql:host=$dbhost;dbname=$dbname";
    $dbConnection = new PDO($mysql_conn_string, $dbuser, $dbpass); 
    $dbConnection->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    return $dbConnection;
}

function sendNotification($image_url) {
    global $GCM_APP_REG_TOKEN;
    global $GCM_SERVER_API_KEY;

    $data = array(
      'to'      => $GCM_APP_REG_TOKEN,
      'data' => array(
           'title' => 'You have a Guest',
           'body' => 'A visitor arrived at your house at '.date('h:i:s a').' on '.date('m/d/Y'),
           'badge' => '1',
           'visitor_image' => $image_url
      )
    );

    error_log(json_encode($data));
    $options = array(
      'http' => array(
        'method'  => 'POST',
        'content' => json_encode( $data ),
        'header'=>  "Authorization:key=".$GCM_SERVER_API_KEY."\r\n" .
                    "Content-Type: application/json\r\n" .
                    "Accept: application/json\r\n"
      )
    );
    $url = "https://gcm-http.googleapis.com/gcm/send";
    $context  = stream_context_create( $options );
    $result = file_get_contents( $url, false, $context );
    $response = json_decode( $result );
    error_log($result);
}

/**
 * Step 2: Instantiate a Slim application
 */
$app = new Slim\App();

/**
 * Step 3: Define the Slim application routes
 */


/**
 * POST request to upload image
 */
$app->post('/visitors', function ($request, $response, $args) {
    global $HOST;

    $response->withHeader('Content-type', 'application/json');
    $resp_body = array();

    $dir = 'visitors/';
    if (!is_dir($dir)){
        $resp_body["error"] = true; 
        $resp_body["message"] = "No directory $dir found"; 
        $response->withStatus(404);
        $response->getBody()->write(json_encode($resp_body));
        return $response;
    }
    $date = date('Y-m-d');
    error_log($date);
    $time = date('H-i-s');
    $imageFileType = pathinfo($_FILES["image"]["name"],PATHINFO_EXTENSION);
    $file = $dir.$date.'_'.$time.".".$imageFileType;//end((explode(".", $_FILES["image"]["name"])));

    // Check if image file is a actual image or fake image
    $check = getimagesize($_FILES["image"]["tmp_name"]);
    if($check !== false) {
        $result = move_uploaded_file($_FILES['image']['tmp_name'], $file);
    } else {
        $resp_body["error"] = true; 
        $resp_body["message"] = "Bad Request. Uploaded file ".$_FILES['image']['name']." is not an image."; 
        $response->withStatus(400);
        $response->getBody()->write(json_encode($resp_body));
        return $response;
    }
    
    if ($result) { 
        try {
            $db = getDB();
            $query = "INSERT INTO visitors (date, time, image_url) values (STR_TO_DATE('".$date."', '%Y-%m-%d'), "
                               ."STR_TO_DATE('".$time."', '%h:%i:%s'), '".$file."')";
            error_log($query);
            $sth = $db->prepare($query);
            $sth->execute();
 
            $resp_body["error"] = false;
            $resp_body["message"] = "image uploaded successfully";
            $response->withStatus(201);

            sendNotification($HOST.$file);
 
        } catch(PDOException $e) {
            unlink($file);
            $resp_body["error"] = true; 
            $resp_body["message"] = "Failed to update database.\n".$e->getMessage(); 
            $response->withStatus(500);
        }
        $db = null;
    } else { 
        $resp_body["error"] = true; 
        $resp_body["message"] = "Failed to upload image. Please try again"; 
        $response->withStatus(500);
    }
    
    $response->getBody()->write(json_encode($resp_body));
    return $response;
});


/**
 * GET request to get all visitors on a date
 */
$app->get('/visitors/{date}', function ($request, $response, $args) {
    $response->withHeader('Content-type', 'application/json');;
    $resp_body = array();
    $dir = 'visitors/';
    if (!is_dir($dir)){
        $resp_body["error"] = true; 
        $resp_body["message"] = "No directory $dir found"; 
        $response->withStatus(404);
        $response->getBody()->write(json_encode($resp_body));
        return $response;
    }

     
    try {
        $db = getDB();
        $query = "SELECT time, image_url FROM visitors WHERE date=STR_TO_DATE('".$args['date']."', '%Y-%m-%d')";
        error_log($query);
        $sth = $db->prepare($query);
        $sth->execute();
        $images = $sth->fetchAll(PDO::FETCH_ASSOC);
        if ($images != null && count($images)>0) { 
            $resp_body["error"] = false; 
            $resp_body["visitors"][$args["date"]] = $images;
            $response->withStatus(200);
        } else { 
            $resp_body["error"] = false; 
            $resp_body["message"] = "No visitors on ".$args['date'];
            $response->withStatus(204);
        }
    } catch(PDOException $e) {
        $resp_body["error"] = true; 
        $resp_body["message"] = "Failed to read from database.\n".$e->getMessage(); 
        $response->withStatus(500);
    }
    $db = null;

    $response->getBody()->write(json_encode($resp_body));
    return $response;
});


/**
 * PUT request to update motion sensor status
 */
$app->put('/motion_sensors/{id}/{status}', function($request, $response, $args) {
    $response->withHeader('Content-type', 'application/json');;
    $resp_body = array();

    try {
        $db = getDB();
        $status = (($args['status']==0)||($args['status']==false))?0:1;
        $query = "UPDATE motion_sensors SET status=".$status." WHERE id='".$args['id']."'";
        error_log($query);
        $sth = $db->prepare($query);
        $sth->execute();
        $resp_body["error"] = false; 
        //$resp_body["message"] = $sth->rowCount()." rows updated.";
        $resp_body["id"] = $args['id'];
        $resp_body["status"] = $args['status'];
        $response->withStatus(200);
    } catch(PDOException $e) {
        $resp_body["error"] = true; 
        $resp_body["message"] = "Failed to update database.\n".$e->getMessage(); 
        $response->withStatus(500);
    }

    $response->getBody()->write(json_encode($resp_body));
    return $response;
});

/**
 * GET request to get motion sensor status
 */
$app->get('/motion_sensors/{id}', function($request, $response, $args) {
    $response->withHeader('Content-type', 'application/json');;
    $resp_body = array();

    try {
        $db = getDB();
        $query = "SELECT status FROM motion_sensors WHERE id='".$args['id']."'";
        error_log($query);
        $sth = $db->prepare($query);
        $sth->execute();
        $res = $sth->fetch(PDO::FETCH_ASSOC);
        if($res != null && count($res)>0) {
            $resp_body["error"] = false; 
            $resp_body["id"] = $args['id'];
            $resp_body["status"] = $res['status'];
            $response->withStatus(200);

        } else {
            $resp_body["error"] = true; 
            $resp_body["message"] = "No such sensor with id ".$args['id']." found";
            $response->withStatus(404);
        }
    } catch(PDOException $e) {
        $resp_body["error"] = true; 
        $resp_body["message"] = "Failed to update database.\n".$e->getMessage(); 
        $response->withStatus(500);
    }

    $response->getBody()->write(json_encode($resp_body));
    return $response;
});

/**
 * GET request to get all motion sensors statuses
 */
$app->get('/motion_sensors', function($request, $response, $args) {
    $response->withHeader('Content-type', 'application/json');
    $resp_body = array();

    try {
        $db = getDB();
        $query = "SELECT * FROM motion_sensors";
        error_log($query);
        $sth = $db->prepare($query);
        $sth->execute();
        $sensors = $sth->fetchAll(PDO::FETCH_ASSOC);
        if($sensors != null && count($sensors)>0) {
            $resp_body["error"] = false; 
            $resp_body["sensors"] = $sensors;
            $response->withStatus(200);

        } else {
            $resp_body["error"] = false; 
            $resp_body["message"] = "No motion sensors found";
            $response->withStatus(204);
        }
    } catch(PDOException $e) {
        $resp_body["error"] = true; 
        $resp_body["message"] = "Failed to update database.\n".$e->getMessage(); 
        $response->withStatus(500);
    }

    $response->getBody()->write(json_encode($resp_body));
    return $response;
});



/**
 * Step 4: Run the Slim application
 */
$app->run();

