package com.xc.dance.http;

import android.support.annotation.NonNull;

import com.xc.dance.bean.FileModel;

import java.util.ArrayList;

import io.reactivex.Observable;
import retrofit2.Response;
import retrofit2.http.GET;
import retrofit2.http.Path;
import retrofit2.http.Query;


public interface RepoService {

    public final static String GITHUB_API_BASE_URL = "https://api.github.com/";

    @NonNull
    @GET("repos/{owner}/{repo}/contents/{path}")
    Observable<Response<ArrayList<FileModel>>> getRepoFiles(
            @Path("owner") String owner,
            @Path("repo") String repo,
            @Path(value = "path", encoded = false) String path,
            @Query("ref") String branch
    );


}
