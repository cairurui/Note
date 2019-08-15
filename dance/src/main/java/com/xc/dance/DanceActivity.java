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
import com.xc.dance.http.RepoService;
import com.xc.dance.http.core.AppRetrofit;

import java.util.ArrayList;
import java.util.List;

import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.functions.Consumer;
import io.reactivex.schedulers.Schedulers;
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

        mDatas = new ArrayList<>();
        for (int i = 0; i < 10; i++) {
            mDatas.add(new DanceBean());
        }

        mAdapter = new ListDanceAdapter(this, mDatas);
        layoutManager = new LinearLayoutManager(DanceActivity.this, LinearLayoutManager.VERTICAL, false);
        rvPage.setLayoutManager(layoutManager);
        rvPage.setAdapter(mAdapter);

        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    test();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }).start();

    }

    protected <T> T getServices(Class<T> serviceClass, String baseUrl) {
        return AppRetrofit.INSTANCE
                .getRetrofit(baseUrl)
                .create(serviceClass);
    }

    @SuppressWarnings("all")
    private void test() throws Exception {

        String owner = "cairurui";
        String repo = "Note";
        String path = "doc/dance";
        String ref = "master";

        getServices(RepoService.class, RepoService.GITHUB_API_BASE_URL)
                .getRepoFiles(owner, repo, path, ref)
                .subscribeOn(Schedulers.io())
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(new Consumer<Response<ArrayList<FileModel>>>() {
                    @Override
                    public void accept(Response<ArrayList<FileModel>> arrayListResponse) throws Exception {
                        Log.d(TAG, "accept() called with: arrayListResponse = [" + arrayListResponse + "]");
                    }
                });


//        OkHttpClient client = new OkHttpClient();
//        Request request = new Request.Builder()
//                .url("https://api.github.com/repos/cairurui/Note/contents/?ref=master&uniqueLoginId=cairurui")
//                .build();
//
//        try (Response response = client.newCall(request).execute()) {
//            if (!response.isSuccessful()) throw new IOException("Unexpected code " + response);
//
//            Headers responseHeaders = response.headers();
//            for (int i = 0; i < responseHeaders.size(); i++) {
//                System.out.println(responseHeaders.name(i) + ": " + responseHeaders.value(i));
//            }
//
//            System.out.println(response.body().string());
//        }
    }
}
