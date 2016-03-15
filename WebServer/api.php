<?php

/**
 * Step 1: Require the Slim Framework using Composer's autoloader
 */
require 'vendor/autoload.php';

date_default_timezone_set('America/Los_Angeles');


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

/**
 * Step 2: Instantiate a Slim application
 *
 * This example instantiates a Slim application using
 * its default settings. However, you will usually configure
 * your Slim application now by passing an associative array
 * of setting names and values into the application constructor.
 */
$app = new Slim\App();

/**
 * Step 3: Define the Slim application routes
 */
$app->get('/', function ($request, $response, $args) {
    $response->write("Welcome to Slim World!");
    return $response;
});


/**
 * POST request to upload image
 */
$app->post('/visitors', function ($request, $response, $args) {
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
                               ."STR_TO_DATE('".$time."', '%H-%i-%s'), '".$file."')";
            error_log($query);
            $sth = $db->prepare($query);
            $sth->execute();
 
            $resp_body["error"] = false;
            $resp_body["message"] = "image uploaded successfully";
            $response->withStatus(201);

            push_visitor_notification($file);
 
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


function push_visitor_notification($file) {
    ServerRequestInterface $request = new ServerRequestInterface();
    request.
}


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

