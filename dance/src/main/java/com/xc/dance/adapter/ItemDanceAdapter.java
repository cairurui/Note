package com.xc.dance.adapter;

import android.content.Context;

import com.chad.library.adapter.base.BaseQuickAdapter;
import com.chad.library.adapter.base.BaseViewHolder;
import com.xc.dance.bean.DanceBean;
import com.xc.dance.R;

import java.util.List;

/**
 * Created by crr on 2019-08-13.
 */
public class ItemDanceAdapter extends BaseQuickAdapter<DanceBean, BaseViewHolder> {

    private Context mContext;

    public ItemDanceAdapter(Context context, List<DanceBean> danceBeans) {
        super(R.layout.item_item_dance, danceBeans);
        this.mContext = context;
    }

    @Override
    protected void convert(BaseViewHolder helper, DanceBean item) {


    }




}