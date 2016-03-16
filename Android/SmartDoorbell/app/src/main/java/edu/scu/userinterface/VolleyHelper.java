package edu.scu.userinterface;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.LruCache;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.toolbox.ImageLoader;
import com.android.volley.toolbox.Volley;

/**
 * Created by lasyaboddapati on 12/18/15.
 *
 * Helper class for easy utilization of Volley for Network communication
 */
public class VolleyHelper {
    private static VolleyHelper volleyHelper;
    private RequestQueue requestQueue;
    private ImageLoader imageLoader;
    private static Context context;

    private VolleyHelper(Context context) {
        this.context = context;
        this.requestQueue = getRequestQueue();

        this.imageLoader = new ImageLoader(requestQueue,
                new ImageLoader.ImageCache() {
                    private final LruCache<String, Bitmap>
                            cache = new LruCache<String, Bitmap>(20);

                    @Override
                    public Bitmap getBitmap(String url) {
                        return cache.get(url);
                    }

                    @Override
                    public void putBitmap(String url, Bitmap bitmap) {
                        cache.put(url, bitmap);
                    }
                });
    }

    public static synchronized VolleyHelper getInstance(Context context) {
        if (volleyHelper == null) { // Ensure that only ONE instance of the Helper is
                                    // used throughout the application
            volleyHelper = new VolleyHelper(context);
        }
        return volleyHelper;
    }

    public RequestQueue getRequestQueue() {
        if (requestQueue == null) { // Ensure only a single RequestQueue is used throughout
                                    // the application
            requestQueue = Volley.newRequestQueue(context.getApplicationContext());
        }
        return requestQueue;
    }

    public <T> void addToRequestQueue(Request<T> req) {
        getRequestQueue().add(req);
    }

    public ImageLoader getImageLoader() {
        return imageLoader;
    }
}
