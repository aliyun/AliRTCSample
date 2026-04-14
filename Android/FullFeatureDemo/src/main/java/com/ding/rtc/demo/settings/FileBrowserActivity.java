package com.ding.rtc.demo.settings;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;

import androidx.annotation.Nullable;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import android.widget.ListView;
import android.widget.Toast;

import com.ding.rtc.demo.R;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

public class FileBrowserActivity extends AppCompatActivity {

    // constant
    private static final String ROOT_PATH = Environment.getExternalStorageDirectory().getPath();
    public static final String EXTRA_DATA_PATH = "EXTRA_DATA_PATH";

    // view
    private ListView fileListView;
    private final List<FileBrowserAdapter.FileItem> fileListItems = new ArrayList<>();


    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_file_borwser_layout);

        setupView();
        showFileDir(ROOT_PATH);
    }

    private void setupView() {
        fileListView = findViewById(R.id.file_list);

        Toolbar toolbar = findViewById(R.id.toolbar);
        toolbar.setTitle(R.string.file_browser);
        setSupportActionBar(toolbar);

        ActionBar supportActionBar = getSupportActionBar();
        if (supportActionBar != null) {
            supportActionBar.setDisplayShowTitleEnabled(true);
        }
        toolbar.setNavigationOnClickListener(v -> finish());
    }


    private void showFileDir(String path) {
        ArrayList<String> names = new ArrayList<>();
        ArrayList<String> paths = new ArrayList<>();
        File file = new File(path);

        //如果当前目录不是根目录
        if (!ROOT_PATH.equals(path)) {
            names.add("@1");
            paths.add(ROOT_PATH);

            names.add("@2");
            paths.add(file.getParent());
        }

        List<File> fileArrayList = Arrays.asList(file.listFiles());
        Collections.sort(fileArrayList, (o1, o2) -> {
            if (o1.isDirectory() && o2.isFile()) {
                return -1;
            }
            if (o1.isFile() && o2.isDirectory()) {
                return 1;
            }

            return o2.getName().compareTo(o1.getName());
        });

        //添加所有文件
        for (File f : fileArrayList) {
            names.add(f.getName());
            paths.add(f.getPath());
        }

        fileListItems.clear();
        for (int i = 0; i < names.size(); i++) {
            fileListItems.add(new FileBrowserAdapter.FileItem(names.get(i), paths.get(i)));
        }


        fileListView.setItemsCanFocus(true);
        fileListView.setAdapter(new FileBrowserAdapter(this, fileListItems));
        fileListView.setOnItemClickListener((parent, view, position, id) -> {
            String tempPath = fileListItems.get(position).getPath();
            File tempFile = new File(tempPath);
            // 文件存在并可读
            if (tempFile.exists() && tempFile.canRead()) {
                if (tempFile.isDirectory()) {
                    //显示子目录及文件
                    showFileDir(tempPath);
                } else {
                    //处理文件
                    selectFile(tempPath);
                }
            } else {
                //没有权限
                Toast.makeText(FileBrowserActivity.this, "无读取权限", Toast.LENGTH_SHORT).show();
            }
        });
    }

    private void selectFile(String path) {
        Intent intent = new Intent();
        intent.putExtra(EXTRA_DATA_PATH, path);
        setResult(Activity.RESULT_OK, intent);
        finish();
    }
}
