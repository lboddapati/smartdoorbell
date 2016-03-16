package edu.scu.userinterface;

import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.ImageView;
import android.widget.TextView;

import com.android.volley.toolbox.ImageLoader;
import com.android.volley.toolbox.NetworkImageView;


public class VisitorNotification extends AppCompatActivity {

    private static final String TAG = "VISITOR NOTIFICATION";

    private TextView message;
    private NetworkImageView image;
    private VolleyHelper volleyHelper;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        setTitle("You have a Guest");
        setContentView(R.layout.activity_visitor_notification);

        volleyHelper = VolleyHelper.getInstance(this);

        Bundle notification = getIntent().getBundleExtra("notification");
        for(String key : notification.keySet()) {
            Log.d(TAG, key + ": " + notification.getString(key));
        }

        message = (TextView) findViewById(R.id.message);
        image = (NetworkImageView) findViewById(R.id.visitor_image);

        message.setText(notification.getString("body"));
        ImageLoader imageLoader = volleyHelper.getImageLoader();
        image.setImageUrl(notification.getString("visitor_image"), imageLoader);
        image.setDefaultImageResId(R.drawable.ic_launcher);
        image.setErrorImageResId(android.R.drawable.stat_sys_warning);

    }
}
