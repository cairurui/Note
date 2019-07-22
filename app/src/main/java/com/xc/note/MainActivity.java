package com.xc.note;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;

import com.xc.note.media.XPlayer;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    File mMusicFile = new File(Environment.getExternalStorageDirectory(), "input.mp3");
    // Used to load the 'native-lib' library on application startup.
    private XPlayer mPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Log.e("TAG", "file is exist: " + mMusicFile.exists());

        mPlayer = new XPlayer();
        mPlayer.setDataSource(mMusicFile.getAbsolutePath());
        mPlayer.play();
    }

}
