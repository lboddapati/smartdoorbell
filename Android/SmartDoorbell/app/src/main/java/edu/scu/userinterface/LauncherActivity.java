package edu.scu.userinterface;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Handler;
import android.preference.PreferenceManager;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;


public class LauncherActivity extends ActionBarActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //setContentView(R.layout.activity_launcher);
        Intent intent;
        SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        if(!sharedPreferences.getBoolean(AppPreferences.REGISTRATION_COMPLETE, false)) {
            intent = new Intent(this, RegistrationActivity.class);
        } else if(!sharedPreferences.getBoolean(AppPreferences.LOGGED_IN, false)){
            intent = new Intent(this, LoginActivity.class);
        } else {
            intent = new Intent(this, StatusActivity.class);
        }

        /*final Handler handler = new Handler();
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                //Wait for 1 sec
            }
        }, 5000);*/
        startActivity(intent);
        finish();
    }

}
