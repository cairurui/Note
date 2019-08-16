package com.xc.dance;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.PagerSnapHelper;
import android.support.v7.widget.RecyclerView;
import android.util.Log;

import com.xc.dance.adapter.ListDanceAdapter;
import com.xc.dance.bean.DanceBean;
import com.xc.dance.bean.FileModel;
import com.xc.dance.event.ArticleEvent;
import com.xc.dance.http.RepoService;
import com.xc.dance.http.core.AppRetrofit;
import com.xc.dance.parse.ArticleParse;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import io.reactivex.functions.Consumer;
import io.reactivex.schedulers.Schedulers;
import okhttp3.ResponseBody;
import retrofit2.Response;

public class DanceActivity extends AppCompatActivity {


    private static final String TAG = "DanceActivity";
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

        mDatas = ArticleParse.INSTANCE.datas;
//        mDatas = new ArrayList<>();

        mAdapter = new ListDanceAdapter(this, mDatas);
        layoutManager = new LinearLayoutManager(DanceActivity.this, LinearLayoutManager.VERTICAL, false);
        rvPage.setLayoutManager(layoutManager);
        rvPage.setAdapter(mAdapter);


        getCategory();


    }

    @Override
    public void onStart() {
        super.onStart();
        EventBus.getDefault().register(this);
    }

    @Override
    public void onStop() {
        super.onStop();
        EventBus.getDefault().unregister(this);
    }

    protected <T> T getServices(Class<T> serviceClass, String baseUrl) {
        return AppRetrofit.INSTANCE
                .getRetrofit(baseUrl)
                .create(serviceClass);
    }

    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onArticleEvent(ArticleEvent event) {

        mAdapter.notifyDataSetChanged();


    }

    @SuppressWarnings("all")
    private void getCategory() {

        // https://api.github.com/repos/cairurui/Note/contents/doc/dance?ref=master
        String owner = "cairurui";
        String repo = "Note";
        String path = "doc/dance";
        String ref = "master";

        getServices(RepoService.class, RepoService.GITHUB_API_BASE_URL)
                .getRepoFiles(owner, repo, path, ref)
                .subscribeOn(Schedulers.io())
                .subscribe(new Consumer<ArrayList<FileModel>>() {
                    @Override
                    public void accept(ArrayList<FileModel> fileModels) throws Exception {
                        Log.d(TAG, "accept() called with: arrayListResponse = [" + fileModels + "]");

                        if (fileModels == null || fileModels.size() == 0) {
                            return;
                        }

                        // 解析第二层

                        for (int i = 0; i < fileModels.size(); i++) {
                            FileModel fileModel = fileModels.get(i);
                            if (!fileModel.isDir()) {
                                continue;
                            }
                            getFiles(fileModel.getName());

                        }

                    }
                });


    }

    @SuppressWarnings("all")
    private void getFiles(final String dirName) {
        // https://api.github.com/repos/cairurui/Note/contents/doc/dance/%E5%A4%9A%E7%BA%BF%E7%A8%8B?ref=master&uniqueLoginId=cairurui
        String owner = "cairurui";
        String repo = "Note";
        String path = "doc/dance/" + dirName;
        String ref = "master";

        getServices(RepoService.class, RepoService.GITHUB_API_BASE_URL)
                .getRepoFiles(owner, repo, path, ref)
                .subscribeOn(Schedulers.io())
                .subscribe(new Consumer<ArrayList<FileModel>>() {
                    @Override
                    public void accept(ArrayList<FileModel> fileModels) throws Exception {
                        Log.d(TAG, "accept() called with: arrayListResponse = [" + fileModels + "]");
                        // 对文件进行解析
                        for (int i = 0; i < fileModels.size(); i++) {
                            FileModel fileModel = fileModels.get(i);
                            if (fileModel.isDir()) {
                                continue;
                            }
                            decodeFile(fileModel.getPath(),dirName);
                        }

                    }
                });
    }

    @SuppressWarnings("all")
    private void decodeFile(String path,final String dir) {
        // https://api.github.com/repos/cairurui/Note/contents/doc/dance/%E5%A4%9A%E7%BA%BF%E7%A8%8B?ref=master&uniqueLoginId=cairurui

//        "path":"doc/dance/多线程/01-死锁.md"
        String url = "https://api.github.com/repos/cairurui/Note/contents/" + path;
        getServices(RepoService.class, RepoService.GITHUB_API_BASE_URL)
                .getFileAsStream(true, url)
                .subscribeOn(Schedulers.io())
                .subscribe(new Consumer<Response<ResponseBody>>() {
                    @Override
                    public void accept(Response<ResponseBody> responseBodyResponse) throws Exception {
                        Log.e(TAG, "accept() called with: responseBodyResponse = [" + responseBodyResponse + "]");
                        ResponseBody body = responseBodyResponse.body();
                        InputStream inputStream = body.byteStream();
                        ArticleParse.INSTANCE.parseContent(inputStream,dir);
                    }
                });
    }

}
