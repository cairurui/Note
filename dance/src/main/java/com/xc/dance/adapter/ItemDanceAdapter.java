package com.xc.dance.adapter;

import android.content.Context;
import android.widget.ImageView;

import com.bumptech.glide.Glide;
import com.chad.library.adapter.base.BaseQuickAdapter;
import com.chad.library.adapter.base.BaseViewHolder;
import com.xc.dance.R;
import com.xc.dance.bean.DanceBean;

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
        String url = "https://raw.githubusercontent.com/cairurui/Note/master/doc/设计/首页设计.png";
        final ImageView imageView = helper.getView(R.id.photoView);

        Glide.with(mContext).load(url)
                .placeholder(R.mipmap.ic_launcher)
                .into(imageView);


    }


}