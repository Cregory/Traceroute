package cn.traceroute;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Handler;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.netease.LDNetDiagnoService.LDNetTraceRoute;

public class MainActivity extends AppCompatActivity {

    private TextView mText;

    private Button mBtn;

    private EditText mEditText;

    private Handler mHandler;

    private StringBuilder builer = new StringBuilder();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mText = (TextView) findViewById(R.id.text);
        mEditText = (EditText) findViewById(R.id.edit);
        mEditText.setText("www.baidu.com");
        mBtn = (Button) findViewById(R.id.btn);
        mHandler = new Handler();

        mBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String domin = mEditText.getText().toString();
                builer = new StringBuilder();
                if (!TextUtils.isEmpty(domin)) {
                    mBtn.setEnabled(false);
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            new LDNetTraceRoute(new LDNetTraceRoute.Callback() {
                                @Override
                                public void onUpdate(final String update) {
                                    Log.d("traceroutelog", update) ;
                                    mHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            builer.append(update);
                                            mText.setText(builer.toString());
                                        }
                                    });
                                }

                                @Override
                                public void onEnd() {
                                    mHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            mBtn.setEnabled(true);
                                            Toast.makeText(getApplicationContext(), "end", 1000).show();
                                        }
                                    });
                                }
                            }).beginTrace("www.baidu.com");
                        }
                    }).start();
                }
            }
        });
    }
}
