package com.xc.dance.adapter;

import android.content.Context;
import android.support.annotation.NonNull;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.PagerSnapHelper;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.xc.dance.R;
import com.xc.dance.bean.DanceBean;

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
    public void onBindViewHolder(@NonNull final ListDanceHolder holder, int position) {
        DanceBean danceBean = mDatas.get(position);
        Log.d("xiaocai", "onBindViewHolder() called with: danceBean = [" + danceBean + "], position = [" + position + "]");

        holder.adapter = new ItemDanceAdapter(mContext, danceBean);
        holder.rvPage.setAdapter(holder.adapter);

        holder.tv_title.setText(danceBean.getTitle());


        final int total = danceBean.getTotalSize();
        int curIndex = 1;

        holder.tv_indicator.setText(curIndex + "/" + total);

        holder.rvPage.addOnScrollListener(new RecyclerView.OnScrollListener() {
            @Override
            public void onScrollStateChanged(@NonNull RecyclerView recyclerView, int newState) {
                super.onScrollStateChanged(recyclerView, newState);
                int curIndex = holder.layoutManager.findFirstCompletelyVisibleItemPosition()+1;
                holder.tv_indicator.setText(curIndex + "/" + total);
            }

            @Override
            public void onScrolled(@NonNull RecyclerView recyclerView, int dx, int dy) {
                super.onScrolled(recyclerView, dx, dy);
            }
        });

    }

    @Override
    public int getItemCount() {
        return mDatas == null ? 0 : mDatas.size();
    }


    class ListDanceHolder extends RecyclerView.ViewHolder {

        RecyclerView rvPage;
        ItemDanceAdapter adapter;
        TextView tv_title;
        TextView tv_indicator;
        LinearLayoutManager layoutManager;

        public ListDanceHolder(@NonNull View itemView) {
            super(itemView);
            rvPage = itemView.findViewById(R.id.rv_page);
            tv_title = itemView.findViewById(R.id.tv_title);
            tv_indicator = itemView.findViewById(R.id.tv_indicator);

            PagerSnapHelper snapHelper = new PagerSnapHelper();
            snapHelper.attachToRecyclerView(rvPage);
            layoutManager = new LinearLayoutManager(mContext, LinearLayoutManager.HORIZONTAL, false);
            rvPage.setLayoutManager(layoutManager);

        }
    }

}