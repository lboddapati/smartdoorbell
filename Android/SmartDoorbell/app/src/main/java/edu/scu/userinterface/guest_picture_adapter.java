package edu.scu.userinterface;

import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.TextView;

import com.android.volley.toolbox.ImageLoader;
import com.android.volley.toolbox.NetworkImageView;

import org.json.JSONArray;
import org.json.JSONException;

public class guest_picture_adapter extends BaseAdapter{


    private Context context;
    JSONArray jsonArray;   // calling activity context
VolleyHelper volleyHelper;
    ImageLoader imageLoader;

    public guest_picture_adapter(Context callingActivityContext,
                                JSONArray jsonArray) {
        context = callingActivityContext;
        this.jsonArray = jsonArray;
        volleyHelper = VolleyHelper.getInstance(context);
         imageLoader = volleyHelper.getImageLoader();
    }
    public guest_picture_adapter(){
    }

    // how many entries are there in the data set
    public int getCount() {
        return jsonArray.length();
    }

    // what is in a given 'position' in the data set
    public Object getItem(int position) {
        return position;
    }

    // what is the ID of data item in given 'position'
    public long getItemId(int position) {
        return position;
    }

    // create a view for each thumbnail in the data set
    public View getView(int position, View convertView, ViewGroup parent) {

        LayoutInflater inflater  =  (LayoutInflater)context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View view = inflater.inflate(R.layout.custom_grid_image, null);
        NetworkImageView imageView;
        TextView dateTime;
        // if possible, reuse (convertView) image already held in cache

            // new image in GridView formatted to:
            // match parent dimension suggestions
            // using SquareImageView to force highet matching width
            // center-cropped, and 0dp padding/margin around
           imageView =(NetworkImageView)view.findViewById(R.id.visitor_grid_image);
            dateTime = (TextView)view.findViewById(R.id.dateTime);
//            imageView.setLayoutParams(new GridView.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
//            imageView.setScaleType(ImageView.ScaleType.CENTER_CROP);
            try {
                String url = "http://www.boddapati.com/" + jsonArray.getJSONObject(position).getString("image_url");
                Log.d("IMAGE URL", url);
                imageView.setImageUrl(url , imageLoader);
                dateTime.setText(jsonArray.getJSONObject(position).getString("time"));
            } catch (JSONException e) {
                e.printStackTrace();
            }
            imageView.setPadding(0, 0, 0, 0);
        return view;

    }


}//MyImageAdapter
