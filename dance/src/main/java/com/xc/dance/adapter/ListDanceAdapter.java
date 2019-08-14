package com.xc.dance.adapter;

import android.content.Context;
import android.support.annotation.NonNull;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.PagerSnapHelper;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.xc.dance.bean.DanceBean;
import com.xc.dance.R;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by crr on 2019-08-13.
 */
public class ListDanceAdapter extends RecyclerView.Adapter<ListDanceAdapter.ListDanceHolder> {

    private Context mContext;
    private List<DanceBean> mDatas;
    private LayoutInflater mInflater;

    public ListDanceAdapter(Context context, List<DanceBean> danceBeans) {
        this.mDatas = danceBeans;
        this.mContext = context;
        this.mInflater = LayoutInflater.from(context);
    }

    @NonNull
    @Override
    public ListDanceHolder onCreateViewHolder(@NonNull ViewGroup viewGroup, int i) {
        View view = mInflater.inflate(R.layout.item_dance, viewGroup, false);
        return new ListDanceHolder(view);
    }

    @Override
    public void onBindViewHolder(@NonNull ListDanceHolder listDanceHolder, int position) {
        List<DanceBean> data = new ArrayList<>();
        for (int i = 0; i < 10; i++) {
            data.add(new DanceBean());
        }

        listDanceHolder.adapter.setNewData(data);
    }

    @Override
    public int getItemCount() {
        return mDatas == null ? 0 : mDatas.size();
    }


    class ListDanceHolder extends RecyclerView.ViewHolder {

        RecyclerView rvPage;
        ItemDanceAdapter adapter;

        public ListDanceHolder(@NonNull View itemView) {
            super(itemView);
            rvPage = itemView.findViewById(R.id.rv_page);
            PagerSnapHelper snapHelper = new PagerSnapHelper();
            snapHelper.attachToRecyclerView(rvPage);

            adapter = new ItemDanceAdapter(mContext, mDatas);
            LinearLayoutManager layoutManager = new LinearLayoutManager(mContext, LinearLayoutManager.HORIZONTAL, false);
            rvPage.setLayoutManager(layoutManager);
            rvPage.setAdapter(adapter);
        }
    }

}