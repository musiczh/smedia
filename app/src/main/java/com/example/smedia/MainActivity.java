package com.example.smedia;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import com.example.demo.BeautyActivity;
import com.example.demo.CameraDemoActivity;
import com.example.demo.ProcessImageActivity;
import com.example.egldemo.EGLMainActivity;
import com.example.smedia.databinding.ActivityMainBinding;

import java.util.HashMap;

public class MainActivity extends AppCompatActivity {

    private ActivityMainBinding binding;
    private HashMap<String,Class> map;

    private void initMap() {
        map = new HashMap<>();
        map.put("eglDemo", EGLMainActivity.class);
        map.put("相机功能", CameraDemoActivity.class);
        map.put("相机美颜", BeautyActivity.class);
        map.put("图片处理", ProcessImageActivity.class);

    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        initMap();
        ListView listView = binding.listView;
        String[] data = new String[map.size()];
        map.keySet().toArray(data);

        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, data);
        listView.setAdapter(adapter);
        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                String key = (String) parent.getAdapter().getItem(position);
                startActivity(new Intent(MainActivity.this, map.get(key)));
            }
        });

    }


}