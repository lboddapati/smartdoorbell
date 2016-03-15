package edu.scu.userinterface;

import android.content.Context;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.ImageView;

public class guest_picture_adapter extends BaseAdapter{


    private Context context;	// calling activity context
    Integer[] smallImages;		// thumbnail data set

    public guest_picture_adapter(Context callingActivityContext,
                          Integer[] thumbnails) {
        context = callingActivityContext;
        smallImages = thumbnails;
    }

    // how many entries are there in the data set
    public int getCount() {
        return smallImages.length;
    }

    // what is in a given 'position' in the data set
    public Object getItem(int position) {
        return smallImages[position];
    }

    // what is the ID of data item in given 'position'
    public long getItemId(int position) {
        return position;
    }

    // create a view for each thumbnail in the data set
    public View getView(int position, View convertView, ViewGroup parent) {
        SquareImageView imageView;
        // if possible, reuse (convertView) image already held in cache
        if (convertView == null) {
            // new image in GridView formatted to:
            // match parent dimension suggestions
            // using SquareImageView to force highet matching width
            // center-cropped, and 0dp padding/margin around
            imageView = new SquareImageView(context);
            imageView.setLayoutParams(new GridView.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
            imageView.setScaleType(ImageView.ScaleType.CENTER_CROP);
            imageView.setPadding(0, 0, 0, 0);
        } else {
            imageView = (SquareImageView) convertView;
        }
        imageView.setImageResource(smallImages[position]);
        return imageView;
    }

    public class SquareImageView extends ImageView {
        public SquareImageView(Context context) {
            super(context);
        }
        @Override
        public void onMeasure(int width, int height) {
            super.onMeasure(width, width);
        }
    }
}//MyImageAdapter
