package com.xc.note.media;

import android.text.TextUtils;
import android.util.Log;

import com.xc.note.media.listener.MediaErrorListener;
import com.xc.note.media.listener.MediaPreparedListener;

/**
 * Created by xiaocai on 2019/7/22.
 */

public class XPlayer {


    private static final String TAG = "XPlayer";

    static {
        System.loadLibrary("native-lib");
    }

    private String url;
    private MediaErrorListener mErrorListener;
    private MediaPreparedListener mPreparedListener;

    public void setOnErrorListener(MediaErrorListener mErrorListener) {
        this.mErrorListener = mErrorListener;
    }

    public void setOnPreparedListener(MediaPreparedListener preparedListener) {
        this.mPreparedListener = preparedListener;
    }

    public void setDataSource(String url) {
        this.url = url;
    }

    public void play() {
        if (TextUtils.isEmpty(url)) {
            throw new NullPointerException("url is null, please call method setDataSource");
        }

        nPlay(url);
    }

    public void prepare() {
        if (TextUtils.isEmpty(url)) {
            throw new NullPointerException("url is null, please call method setDataSource");
        }
        nPrepare(url);
    }

    /**
     * 异步准备
     */
    public void prepareAsync() {
        if (TextUtils.isEmpty(url)) {
            throw new NullPointerException("url is null, please call method setDataSource");
        }
        nPrepareAsync(url);
    }

    // called from jni
    private void onError(int code, String msg) {
        Log.d(TAG, "onError() called with: code = [" + code + "], msg = [" + msg + "]");
        if (mErrorListener != null) {
            mErrorListener.onError(code, msg);
        }
    }

    // called from jni
    private void onPrepared() {
        if (mPreparedListener != null) {
            mPreparedListener.onPrepared();
        }
    }

    private native void nPlay(String url);

    private native void nPrepareAsync(String url);

    private native void nPrepare(String url);
}
