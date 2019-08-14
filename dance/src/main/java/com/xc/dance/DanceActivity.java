package com.xc.dance;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.PagerSnapHelper;
import android.support.v7.widget.RecyclerView;

import com.xc.dance.adapter.ListDanceAdapter;
import com.xc.dance.bean.DanceBean;

import java.util.ArrayList;
import java.util.List;

public class DanceActivity extends AppCompatActivity {


    RecyclerView rvPage;
    private PagerSnapHelper snapHelper;
    private List<DanceBean> mDatas;
    private ListDanceAdapter mAdapter;
    private LinearLayoutManager layoutManager;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_dance);

        initView();
    }

    private void initView() {
        rvPage = findViewById(R.id.rv_page);
        snapHelper = new PagerSnapHelper();
        snapHelper.attachToRecyclerView(rvPage);

        mDatas = new ArrayList<>();
        for (int i = 0; i < 10; i++) {
            mDatas.add(new DanceBean());
        }


        mAdapter = new ListDanceAdapter(this, mDatas);
        layoutManager = new LinearLayoutManager(DanceActivity.this, LinearLayoutManager.VERTICAL, false);
        rvPage.setLayoutManager(layoutManager);
        rvPage.setAdapter(mAdapter);
    }
}
