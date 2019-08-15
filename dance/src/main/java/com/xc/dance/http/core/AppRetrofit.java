

package com.xc.dance.http.core;

import android.support.annotation.NonNull;

import com.facebook.stetho.okhttp3.StethoInterceptor;

import java.util.HashMap;

import okhttp3.OkHttpClient;
import okhttp3.logging.HttpLoggingInterceptor;
import retrofit2.Retrofit;
import retrofit2.adapter.rxjava2.RxJava2CallAdapterFactory;
import retrofit2.converter.gson.GsonConverterFactory;

/**
 * Retrofit 网络请求
 * Created by ThirtyDegreesRay on 2016/7/15 11:39
 */
public enum AppRetrofit {
    INSTANCE;

    private final String TAG = "AppRetrofit";
    private HashMap<String, Retrofit> retrofitMap = new HashMap<>();

    private void createRetrofit(@NonNull String baseUrl) {

        HttpLoggingInterceptor logging = new HttpLoggingInterceptor();
        logging.setLevel(HttpLoggingInterceptor.Level.BODY);

        OkHttpClient okHttpClient = new OkHttpClient.Builder()
                .addInterceptor(new StethoInterceptor())
                .addInterceptor(logging)
                .build();

        Retrofit.Builder builder = new Retrofit.Builder()
                .baseUrl(baseUrl)
                .addCallAdapterFactory(RxJava2CallAdapterFactory.create())
                .client(okHttpClient);


        builder.addConverterFactory(GsonConverterFactory.create());

        retrofitMap.put(baseUrl, builder.build());
    }

    public Retrofit getRetrofit(@NonNull String baseUrl) {
        if (!retrofitMap.containsKey(baseUrl)) {
            createRetrofit(baseUrl);
        }
        return retrofitMap.get(baseUrl);
    }

}
