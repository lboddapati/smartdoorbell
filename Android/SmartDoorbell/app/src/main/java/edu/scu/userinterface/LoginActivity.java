package edu.scu.userinterface;

import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.os.Bundle;

import android.preference.PreferenceManager;
import android.support.v7.app.AppCompatActivity;

import android.view.View;



import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;


public class LoginActivity extends AppCompatActivity {
    Button loginBtn, clearBtn;
    EditText email, password;

    TextView attempts;
    int counter = 3;

    private SharedPreferences sharedPreferences;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);

        loginBtn =(Button)findViewById(R.id.loginBtn);
        email =(EditText)findViewById(R.id.login_email);
        password =(EditText)findViewById(R.id.login_password);

        clearBtn =(Button)findViewById(R.id.clearBtn);
        attempts =(TextView)findViewById(R.id.attempts_count);

        loginBtn.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                if (email.getText().toString().equals("admin") && password.getText().toString().equals("admin")) {
                    sharedPreferences.edit().putBoolean(AppPreferences.LOGGED_IN, true).apply();
                    Intent myIntent = new Intent(LoginActivity.this, StatusActivity.class);
                    startActivity(myIntent);
                    finish();
                } else {
                    Toast.makeText(getApplicationContext(), "Invalid Credentials", Toast.LENGTH_SHORT).show();

                    attempts.setVisibility(View.VISIBLE);
                    counter--;
                    attempts.setText(Integer.toString(counter));

                    if (counter == 0) {
                        loginBtn.setEnabled(false);
                    }
                }

            }


        });

        clearBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
            }
        });

    }
}