package com.example.smedia;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Toast;

import com.example.demo.BeautyActivity;
import com.example.demo.CameraAdjustActivity;
import com.example.demo.CameraDemoActivity;
import com.example.demo.ProcessImageActivity;
import com.example.egldemo.EGLMainActivity;
import com.example.smedia.databinding.ActivityMainBinding;
import com.permissionx.guolindev.PermissionX;
import com.permissionx.guolindev.callback.ExplainReasonCallback;
import com.permissionx.guolindev.callback.ExplainReasonCallbackWithBeforeParam;
import com.permissionx.guolindev.callback.ForwardToSettingsCallback;
import com.permissionx.guolindev.callback.RequestCallback;
import com.permissionx.guolindev.request.ExplainScope;
import com.permissionx.guolindev.request.ForwardScope;

import java.util.HashMap;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    private ActivityMainBinding binding;
    private HashMap<String,Class> map;

    private void initMap() {
        map = new HashMap<>();
        map.put("相机功能", CameraDemoActivity.class);
        map.put("相机美颜", BeautyActivity.class);
        map.put("图片处理", ProcessImageActivity.class);
        map.put("相机预览基本效果", CameraAdjustActivity.class);

    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        PermissionX.init(this)
                .permissions(Manifest.permission.CAMERA,
                        Manifest.permission.WRITE_EXTERNAL_STORAGE,
                        Manifest.permission.READ_EXTERNAL_STORAGE)
                .onExplainRequestReason((scope, deniedList) -> {
                    scope.showRequestReasonDialog(deniedList,"请授予必要的权限","好的","坚决不给");
                })
                .explainReasonBeforeRequest()
                .request((allGranted, grantedList, deniedList) -> {
                    if (!allGranted) {
                        Toast.makeText(MainActivity.this, "请授权权限", Toast.LENGTH_SHORT).show();
                        MainActivity.this.finish();
                    }
                });

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