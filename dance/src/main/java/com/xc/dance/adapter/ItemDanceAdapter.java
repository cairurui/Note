package com.xc.dance.adapter;

import android.content.Context;
import android.support.annotation.NonNull;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.bumptech.glide.Glide;
import com.chad.library.adapter.base.BaseQuickAdapter;
import com.chad.library.adapter.base.BaseViewHolder;
import com.xc.dance.R;
import com.xc.dance.bean.DanceBean;

import java.util.List;

/**
 * Created by crr on 2019-08-13.
 */
public class ItemDanceAdapter extends RecyclerView.Adapter {

    private Context mContext;
    private DanceBean danceBean;
    private LayoutInflater mInflater;

    public static final int TYPE_CONTENT = 100;
    public static final int TYPE_IMAGE = 101;
    private int sizeOfContent = 0;

    public ItemDanceAdapter(Context context, DanceBean danceBean) {
        this.mContext = context;
        this.mInflater = LayoutInflater.from(context);
        this.danceBean = danceBean;

        if (danceBean != null && danceBean.getContents() != null) {
            sizeOfContent = danceBean.getContents().size();
        }
    }

    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup viewGroup, int viewType) {
        if (viewType == TYPE_CONTENT) {
            View view = mInflater.inflate(R.layout.item_item_content, viewGroup, false);
            return new ContentHolder(view);
        } else if (viewType == TYPE_IMAGE) {
            View view = mInflater.inflate(R.layout.item_item_image, viewGroup, false);
            return new ImageHolder(view);
        }
        return null;
    }

    @Override
    public void onBindViewHolder(@NonNull RecyclerView.ViewHolder viewHolder, int position) {
        int itemViewType = getItemViewType(position);
        if (itemViewType == TYPE_CONTENT) {
            ContentHolder holder = (ContentHolder) viewHolder;


            StringBuilder sb = new StringBuilder();

            List<String> content = danceBean.getContent(position);
            for (int i = 0; i < content.size(); i++) {
                String s = content.get(i);
                sb.append(s);
                sb.append("\r\n");
            }

            holder.tv_content.setText(sb.toString());

        } else if (itemViewType == TYPE_IMAGE) {
            ImageHolder holder = (ImageHolder) viewHolder;

            position = position - sizeOfContent;

            String path = danceBean.getImgs().get(position);

            String url = "https://raw.githubusercontent.com/cairurui/Note/master/doc/dance/" + danceBean.getDir() + "/" + path;


            Glide.with(mContext).load(url)
                    .placeholder(R.mipmap.ic_launcher)
                    .into(holder.iv);
        }


    }

    @Override
    public int getItemCount() {
        //    private List<List<String>> contents; // 内容
        //    private List<String> imgs; // 图片
        if (danceBean == null) {
            return 0;
        }

        int size = 0;
        if (danceBean.getImgs() != null) {
            size += danceBean.getImgs().size();
        }
        if (danceBean.getContents() != null) {
            size += danceBean.getContents().size();
        }

        Log.d("xiaocai2", "getItemCount() called" + size);

        return size;
    }

    @Override
    public int getItemViewType(int position) {
        if (danceBean.getContents() != null) {
            int size = danceBean.getContents().size();
            if (size > 0 && position < size) {
                return TYPE_CONTENT;
            }
        }
        return TYPE_IMAGE;
    }

    public static class ImageHolder extends RecyclerView.ViewHolder {

        ImageView iv;

        public ImageHolder(@NonNull View itemView) {
            super(itemView);

            iv = itemView.findViewById(R.id.iv);
        }
    }

    public static class ContentHolder extends RecyclerView.ViewHolder {

        TextView tv_content;

        public ContentHolder(@NonNull View itemView) {
            super(itemView);

            tv_content = itemView.findViewById(R.id.tv_content);
        }
    }


}