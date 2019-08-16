package com.xc.dance.bean;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by crr on 2019-08-13.
 */
public class DanceBean {

    private String title; // ### 死锁发生条件
    private List<List<String>> contents; // 内容
    private List<String> imgs; // 图片
    private List<String> sounds; // 配音

    private List<String> temp;

    public void setTitle(String title) {
        this.title = title;
    }

    public void addNewContent() {
        if (temp != null && temp.size() == 0) return;

        temp = new ArrayList<>();
        contents.add(temp);
    }

    public void addLine(String content) {
        if (contents == null) {
            contents = new ArrayList();
            addNewContent();
        }
        temp.add(content);
    }

    public void addImg(String img) {
        if (imgs == null) {
            imgs = new ArrayList();
        }
        imgs.add(img);
    }

    public void setSounds(List<String> sounds) {
        this.sounds = sounds;
    }

    public String getTitle() {
        return title;
    }

    public List<List<String>> getContents() {
        return contents;
    }

    public List<String> getImgs() {
        return imgs;
    }

    public List<String> getSounds() {
        return sounds;
    }

    @Override
    public String toString() {
        return "DanceBean{" +
                "title='" + title + '\'' +
                ", contents=" + contents +
                ", imgs=" + imgs +
                ", sounds=" + sounds +
                '}';
    }
}
