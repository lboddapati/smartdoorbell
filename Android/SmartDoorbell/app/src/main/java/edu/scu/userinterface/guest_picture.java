package edu.scu.userinterface;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.GridView;
import android.widget.TextView;
import android.widget.Toast;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.ImageLoader;
import com.android.volley.toolbox.NetworkImageView;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class guest_picture extends Activity {

    GridView gridview;
    TextView txtSoloMsg;
    NetworkImageView imgSoloPhoto;
    ImageLoader mImageloader;
    VolleyHelper volleyHelper;
    String JSON_URL = "http://www.boddapati.com/api.php/visitors/2016-03-18";
    RequestQueue queue;
    RequestQueue requestQueue;
    private JSONArray jsonArray = null;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        sendRequest();
        setContentView(R.layout.guest_picture);
        setContentView(R.layout.guest_picture);
        if (savedInstanceState != null) {
            Log.d("STATE", savedInstanceState.toString());
        }

        queue = Volley.newRequestQueue(this);
        volleyHelper = VolleyHelper.getInstance(this);
        mImageloader = volleyHelper.getImageLoader();
    }

    private void sendRequest() {

        StringRequest stringRequest = new StringRequest(Request.Method.GET, JSON_URL,
                new Response.Listener<String>() {
                    @Override
                    public void onResponse(String response) {
                        showJSON(response);
                    }
                },
                new Response.ErrorListener() {
                    @Override
                    public void onErrorResponse(VolleyError error) {
                        Toast.makeText(guest_picture.this, error.getMessage(), Toast.LENGTH_LONG).show();
                    }
                });

        requestQueue = Volley.newRequestQueue(this);
        requestQueue.add(stringRequest);
    }

    private void showJSON(String json) {
        Log.d("JSONSTRING", json);
        try {
            JSONObject jsonObject = new JSONObject(json);
            jsonArray = jsonObject.getJSONObject("visitors").getJSONArray("2016-03-18");
            gridview = (GridView) findViewById(R.id.gridview);
            gridview.setAdapter(new guest_picture_adapter(this, jsonArray));
            gridview.setOnItemClickListener(new OnItemClickListener() {
                @Override
                public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                    showBigScreen(position);
                }
            });

        } catch (JSONException e) {
            e.printStackTrace();

        }
    }



    private void showBigScreen(int position) {
        // show the selected picture as a single frame
        setContentView(R.layout.guest_picture_solo);
        txtSoloMsg = (TextView) findViewById(R.id.txtSoloMsg);
        imgSoloPhoto = (NetworkImageView) findViewById(R.id.networkImageView);

        try {
            String time = jsonArray.getJSONObject(position).getString("time");
            txtSoloMsg.setText("Visited on "+time);

            String url = "http://www.boddapati.com/" + jsonArray.getJSONObject(position).getString("image_url");
            imgSoloPhoto.setImageUrl(url, mImageloader);
        } catch (JSONException e) {
            e.printStackTrace();
        }

    }
}

