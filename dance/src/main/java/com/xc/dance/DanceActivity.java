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

import java.io.BufferedReader;
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

        mDatas = new ArrayList<>();
        for (int i = 0; i < 10; i++) {
            mDatas.add(new DanceBean());
        }

        mAdapter = new ListDanceAdapter(this, mDatas);
        layoutManager = new LinearLayoutManager(DanceActivity.this, LinearLayoutManager.VERTICAL, false);
        rvPage.setLayoutManager(layoutManager);
        rvPage.setAdapter(mAdapter);


        getCategory();


    }

    protected <T> T getServices(Class<T> serviceClass, String baseUrl) {
        return AppRetrofit.INSTANCE
                .getRetrofit(baseUrl)
                .create(serviceClass);
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
    private void getFiles(String dirName) {
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
                            decodeFile(fileModel.getPath());
                        }

                    }
                });
    }

    public static final String TITLE_PREFIX = "### ";

    @SuppressWarnings("all")
    private void decodeFile(String path) {
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
//                        String s = new String(body.bytes());
//                        Log.e(TAG, s);
                        /**
                         * ### 标题
                         * 文字
                         * xxx.png
                         * xxx.mp3
                         */

                        InputStream inputStream = body.byteStream();
                        BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream));

                        List<DanceBean> datas = new ArrayList<>();
                        DanceBean bean = null;
                        while (reader.ready()) {
                            String line = reader.readLine();
                            if (line == null) continue;
                            line = line.trim();
//                            if (line.length() == 0) continue;


//                            if (Pattern.matches( "^(#+)(.*)", line)) {
//
//                            }


                            // title --> ### 死锁发生条件
                            if (line.startsWith(TITLE_PREFIX)) {
                                line = line.replace(TITLE_PREFIX, "");
                                bean = new DanceBean();
                                datas.add(bean);
                                bean.setTitle(line);
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
                                bean.addNewContent();
                                continue;
                            }

                            if (line.length() > 0) {
                                bean.addLine(line);
                            }


                        }


                        for (int i = 0; i < datas.size(); i++) {
                            Log.d(TAG, "accept: " + datas);
                        }


                    }
                });
    }

}
