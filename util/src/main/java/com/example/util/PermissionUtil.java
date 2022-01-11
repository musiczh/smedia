package com.example.util;

import android.Manifest;
import android.content.Context;

import androidx.core.content.ContextCompat;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * 暂时使用guolin的permissionX,此工具类先搁置
 */
public class PermissionUtil {

    private static Logger mLogger = Logger.create("PermissionUtil");

    public enum Permission {
        CAMERA(new String[]{Manifest.permission.CAMERA}),
        EXTERNAL_STORAGE(new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE,
                Manifest.permission.READ_EXTERNAL_STORAGE});

        public String[] values;
        private Permission(String[] value) {
            this.values = value;
        }
    }

    public interface PermissionCallback {
        public void onPermissionRequestFinish();
    }

    public static boolean checkAndRequirePermission (Context context, Permission[] permissions,
                                                     PermissionCallback callback) {
        List<String> requirePermissions = new ArrayList<>();
        for (Permission p : permissions) {
            requirePermissions.addAll(Arrays.asList(p.values));
        }
        String[] res = requirePermissions.toArray(new String[0]);
        //if (ContextCompat.checkSelfPermission(context,))
        return false;
    }


}

