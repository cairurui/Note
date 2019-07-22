package com.xc.note.media;

import android.text.TextUtils;

/**
 * Created by xiaocai on 2019/7/22.
 */

public class XPlayer {

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

    private native void nPlay(String url);
}
