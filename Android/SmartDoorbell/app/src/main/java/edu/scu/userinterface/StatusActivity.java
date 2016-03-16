package edu.scu.userinterface;

import android.app.Activity;
import android.content.Intent;
import android.media.Image;
import android.net.Network;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Toast;

import com.android.volley.Cache;
import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.BasicNetwork;
import com.android.volley.toolbox.DiskBasedCache;
import com.android.volley.toolbox.HurlStack;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;

/**
 * Created by apadha on 2/14/16.
 */

public class StatusActivity extends AppCompatActivity {


   String JSON_URL = "http://www.boddapati.com/motion_sensors";
   //String JSON_URL = "http://simplifiedcoding.16mb.com/UserRegistration/json.php";
    RequestQueue queue;
    ImageView bedRoomStatus, livingRoomStatus, kitchenStatus;
    RequestQueue requestQueue;
    String data = "";
   private JSONArray jsonArray = null;
    //public static final String JSON_ARRAY = "sensors";


    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        sendRequest();
        setContentView(R.layout.activity_status);
        if(savedInstanceState!=null){
            Log.d("STATE",savedInstanceState.toString());
        }


         bedRoomStatus = (ImageView)findViewById(R.id.bedroom_status);
         livingRoomStatus = (ImageView)findViewById(R.id.livingroom_status);
         kitchenStatus = (ImageView)findViewById(R.id.kitchen_status);

         queue = Volley.newRequestQueue(this);


    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_to_pic, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        switch (id) {
            case R.id.menu_to_pic:
            Intent menu = new Intent(this, guest_picture.class);
            startActivity(menu);
            return true;
        }
        return true;
    }


    private void sendRequest(){

        StringRequest stringRequest = new StringRequest(Request.Method.GET,JSON_URL,
                new Response.Listener<String>() {
                    @Override
                    public void onResponse(String response) {
                        showJSON(response);
                    }
                },
                new Response.ErrorListener() {
                    @Override
                    public void onErrorResponse(VolleyError error) {
                        Toast.makeText(StatusActivity.this,error.getMessage(),Toast.LENGTH_LONG).show();
                    }
                });

         requestQueue = Volley.newRequestQueue(this);
        requestQueue.add(stringRequest);
    }

    private void showJSON(String json){
        Log.d("JSONSTRING", json);
        try {
            JSONObject jsonObject = new JSONObject(json);
            jsonArray = jsonObject.getJSONArray("sensors");
            for(int i =0; i < jsonArray.length(); i++) {
                JSONObject jsonSensor = jsonArray.getJSONObject(i);
               String id = jsonSensor.getString("id");
               String status = jsonSensor.getString("status");

                if(id.equalsIgnoreCase("kitchen_sensor") && status.equalsIgnoreCase("1")){
                    Log.d("Status", status);
                    Log.d("ID", id);
                    kitchenStatus.setImageResource(R.drawable.button_active);

                }
                if(id.equalsIgnoreCase("master_bedroom_sensor") && status.equalsIgnoreCase("1")){
                    Log.d("Status", status);
                    Log.d("ID", id);
                    bedRoomStatus.setImageResource(R.drawable.button_active);

                }
                if(id.equalsIgnoreCase("family_room_sensor") && status.equalsIgnoreCase("1")){
                    Log.d("Status", status);
                    Log.d("ID", id);
                    livingRoomStatus.setImageResource(R.drawable.button_active);

                }

            }

        }catch(JSONException e) {
            e.printStackTrace();

        }


    }


}