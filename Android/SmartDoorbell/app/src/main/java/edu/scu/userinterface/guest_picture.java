package edu.scu.userinterface;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.Button;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.TextView;

public class guest_picture extends Activity implements OnItemClickListener {

    GridView gridview;
    TextView txtSoloMsg;
    ImageView imgSoloPhoto;
    Button btnBack;

    // initialize array of smallImages (100x75 thumbnails)
    Integer[] smallImages = { R.drawable.zoo,
            R.drawable.zoo, R.drawable.zoo,
            R.drawable.zoo, R.drawable.zoo,
            R.drawable.zoo, R.drawable.zoo,
            R.drawable.zoo, R.drawable.zoo,
            R.drawable.zoo, R.drawable.zoo,
            R.drawable.zoo, R.drawable.zoo,
            R.drawable.zoo, R.drawable.zoo};

    //initialize array of high-resolution images (1024x768)
    Integer[] largeImages = { R.drawable.zoo,
            R.drawable.zoo, R.drawable.zoo,
            R.drawable.zoo, R.drawable.zoo,
            R.drawable.zoo, R.drawable.zoo,
            R.drawable.zoo, R.drawable.zoo,
            R.drawable.zoo, R.drawable.zoo,
            R.drawable.zoo, R.drawable.zoo,
            R.drawable.zoo, R.drawable.zoo};

    //in case you want to use-save state values
    Bundle myOriginalMemoryBundle;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.guest_picture);
        //myOriginalMemoryBundle = savedInstanceState;
        gridview = (GridView) findViewById(R.id.gridview);
        gridview.setAdapter(new guest_picture_adapter(this, smallImages));
        gridview.setOnItemClickListener(this);

    }


    @Override
    public void onItemClick(AdapterView<?> parent, View v, int position, long id) {
        showBigScreen(position);
    }//onItemClick

    private void showBigScreen(int position) {
        // show the selected picture as a single frame
        setContentView(R.layout.guest_picture_solo);
        txtSoloMsg = (TextView) findViewById(R.id.txtSoloMsg);
        imgSoloPhoto = (ImageView) findViewById(R.id.imgSoloPhoto);
        txtSoloMsg.setText("image " + position );

        imgSoloPhoto.setImageResource( largeImages[position] );

        btnBack = (Button) findViewById(R.id.btnBack);
        btnBack.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                // redraw the main screen showing the GridView
                onCreate(myOriginalMemoryBundle);
            }
        });

    }// showBigScreen

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_to_monitor, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        switch (id) {
            case R.id.menu_to_monitor:
                Intent menu = new Intent(this, StatusActivity.class);
                startActivity(menu);
                return true;
        }
        return true;
    }

}