package com.xc.dance.parse;

import android.util.Log;

import com.xc.dance.bean.DanceBean;
import com.xc.dance.event.ArticleEvent;

import org.greenrobot.eventbus.EventBus;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Created by crr on 2019-08-16.
 */
public enum ArticleParse {
    INSTANCE;

    public List<DanceBean> datas = new ArrayList<>();
    private static final String TITLE_PREFIX = "### ";
    private static final String TAG = "ArticleParse";


    public void parseContent(InputStream inputStream,String dir) throws IOException {

        /**
         * ### 标题
         * 文字
         * xxx.png
         * xxx.mp3
         */
        BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream));


        DanceBean bean = null;
        while (reader.ready()) {
            String line = reader.readLine();
            if (line == null) continue;
            line = line.trim();

//          if (Pattern.matches( "^(#+)(.*)", line)) {
//
//          }

            // title --> ### 死锁发生条件
            if (line.startsWith(TITLE_PREFIX)) {
                line = line.replace(TITLE_PREFIX, "");
                bean = new DanceBean();
                datas.add(bean);
                bean.setTitle(line);
                bean.setDir(dir);
                continue;
            }

            // img --> ![image-20190815213007736](assets/image-20190815213007736.png)
            String pattern = "!\\[[^\\]]+\\]\\([^\\)]+\\)";
            if (Pattern.matches(pattern, line)) {
                Log.e(TAG, "accept: line:" + line);

                Pattern p = Pattern.compile("(?<=\\()[^\\)]+");
                Matcher matcher = p.matcher(line);
                while (matcher.find()) {
                    String group = matcher.group();
                    Log.d(TAG, "accept: group:" + group);
                    bean.addImg(group);
                }
                continue;
            }

            if (line.length() == 0) {
                bean.addNewContentPrepare();
                continue;
            }

            if (line.length() > 0) {
                bean.addLine(line);
            }


        }


        for (int i = 0; i < datas.size(); i++) {
            Log.d(TAG, "accept: " + datas);
        }
        EventBus.getDefault().post(new ArticleEvent());

        inputStream.close();
    }


}
