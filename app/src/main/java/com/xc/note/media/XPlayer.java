package com.xc.note.media;

import android.text.TextUtils;
import android.util.Log;

/**
 * Created by xiaocai on 2019/7/22.
 */

public class XPlayer {


    private static final String TAG = "XPlayer";

    static {
        System.loadLibrary("native-lib");
    }

    private String url;

    public void setDataSource(String url) {
        this.url = url;
    }

    public void play() {
        if (TextUtils.isEmpty(url)) {
            throw new NullPointerException("url is null, please call method setDataSource");
        }

        nPlay(url);
    }

    // called from jni
    private void onError(int code, String msg) {
        Log.d(TAG, "onError() called with: code = [" + code + "], msg = [" + msg + "]");
    }

    private native void nPlay(String url);
}
