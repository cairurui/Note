package com.xc.note;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import com.xc.note.media.XPlayer;
import com.xc.note.media.listener.MediaErrorListener;
import com.xc.note.media.listener.MediaPreparedListener;
import com.xc.note.media.listener.MediaProgressListener;

import java.io.File;

import io.reactivex.Single;
import io.reactivex.SingleObserver;
import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.Disposable;
import io.reactivex.functions.Function;
import io.reactivex.internal.operators.single.SingleJust;
import io.reactivex.internal.operators.single.SingleObserveOn;
import io.reactivex.internal.operators.single.SingleSubscribeOn;
import io.reactivex.schedulers.Schedulers;

public class MainActivity extends AppCompatActivity {

    File mMusicFile = new File(Environment.getExternalStorageDirectory(), "input.mp3");
    // Used to load the 'native-lib' library on application startup.
    private XPlayer mPlayer;

    TextView tv_time_total;
    TextView tv_time_current;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

//        Log.e("TAG", "file is exist: " + mMusicFile.exists());
//        tv_time_current = findViewById(R.id.tv_time_current);
//        tv_time_total = findViewById(R.id.tv_time_total);
//
//        mPlayer = new XPlayer();
//        mPlayer.setDataSource(mMusicFile.getAbsolutePath());
//
//        mPlayer.setOnErrorListener(new MediaErrorListener() {
//            @Override
//            public void onError(int code, String msg) {
//                Log.e("TAG", "error code: " + code);
//                Log.e("TAG", "error msg: " + msg);
//            }
//        });
//
//        mPlayer.setOnProgressListener(new MediaProgressListener() {
//            @Override
//            public void onProgress(final int current, final int total) {
//
//                runOnUiThread(new Runnable() {
//                    @Override
//                    public void run() {
//                        tv_time_total.setText("" + total);
//                        tv_time_current.setText("" + current);
//                    }
//                });
//            }
//        });

        test();
    }

    public void start(View view) {
        mPlayer.setOnPreparedListener(new MediaPreparedListener() {
            @Override
            public void onPrepared() {
                Log.e("TAG", "准备完毕");
                mPlayer.play();
            }
        });

        mPlayer.prepareAsync();
    }

    public void pause(View view) {
        mPlayer.pause();
    }

    public void resume(View view) {
        mPlayer.resume();
    }

    public void stop(View view) {
        mPlayer.stop();
    }

    private String TAG = "xiaocai";

    private void test() {
        SingleJust.just(1)
                .map(new Function<Integer, String>() {
                    @Override
                    public String apply(Integer integer) throws Exception {
                        return String.valueOf(integer);
                    }
                })
                .subscribeOn(Schedulers.io())
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(new SingleObserver<String>() {
                    @Override
                    public void onSubscribe(Disposable d) {
                        Log.d(TAG, "onSubscribe() called with: d = [" + d + "]");
                    }

                    @Override
                    public void onSuccess(String s) {
                        Log.d(TAG, "onSuccess() called with: s = [" + s + "]");
                    }

                    @Override
                    public void onError(Throwable e) {
                        Log.d(TAG, "onError() called with: e = [" + e + "]");
                    }
                });

        Single<Integer> just = SingleJust.just(1);
        Single<String> map = just.map(new Function<Integer, String>() {
            @Override
            public String apply(Integer integer) throws Exception {
                return String.valueOf(integer);
            }
        });
        Single<String> singleSubscribeOn = map.subscribeOn(Schedulers.io());
        Single<String> singleObserveOn = singleSubscribeOn.observeOn(AndroidSchedulers.mainThread());
        singleObserveOn.subscribe(new SingleObserver<String>() {
            @Override
            public void onSubscribe(Disposable d) {
                Log.d(TAG, "onSubscribe() called with: d = [" + d + "]");
            }

            @Override
            public void onSuccess(String s) {
                Log.d(TAG, "onSuccess() called with: s = [" + s + "]");
            }

            @Override
            public void onError(Throwable e) {
                Log.d(TAG, "onError() called with: e = [" + e + "]");
            }
        });
    }
}
