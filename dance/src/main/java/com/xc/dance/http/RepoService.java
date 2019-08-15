package com.xc.dance.http;

import android.support.annotation.NonNull;

import com.xc.dance.bean.FileModel;

import java.util.ArrayList;

import io.reactivex.Observable;
import okhttp3.ResponseBody;
import retrofit2.Response;
import retrofit2.http.GET;
import retrofit2.http.Header;
import retrofit2.http.Headers;
import retrofit2.http.Path;
import retrofit2.http.Query;
import retrofit2.http.Url;


public interface RepoService {

    public final static String GITHUB_API_BASE_URL = "https://api.github.com/";

    @NonNull
    @GET("repos/{owner}/{repo}/contents/{path}")
    Observable<ArrayList<FileModel>> getRepoFiles(
            @Path("owner") String owner,
            @Path("repo") String repo,
            @Path(value = "path", encoded = true) String path,
            @Query("ref") String branch
    );

    @NonNull @GET @Headers("Accept: application/vnd.github.VERSION.raw")
    Observable<Response<ResponseBody>> getFileAsStream(
            @Header("forceNetWork") boolean forceNetWork,
            @Url String url
    );

}
