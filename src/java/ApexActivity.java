package be.kuleuven.med.exporl.apex;

import be.kuleuven.med.exporl.apex.R;
import org.qtproject.qt5.android.bindings.QtActivity;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Debug;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager;

import java.io.File;

public class ApexActivity extends QtActivity
{
    private static final String TAG = "ApexActivity";
    private boolean apexInitialized;
    private Intent lastIntent;

    public ApexActivity() {
        super();
        apexInitialized = false;
        lastIntent = null;
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        lastIntent = getIntent();
    }

    @Override
    protected void onNewIntent(Intent intent)
    {
        super.onNewIntent(intent);
        lastIntent = intent;
        processLastIntent();
    }

    @Override
    protected void onResume()
    {
        super.onResume();
        ApexNativeMethods.logDebugItem("onResume");
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        ApexNativeMethods.logDebugItem("onPause");
    }

    public void addShortcut(String path) {
        // Intent to start apex with a certain experiment
        Intent shortcutIntent =
            new Intent(getApplicationContext(), ApexActivity.class);
        shortcutIntent.setData(Uri.parse(path));
        shortcutIntent.setAction(Intent.ACTION_VIEW);

        // Intent to create shortcut
        Intent addIntent = new Intent();
        addIntent.putExtra(Intent.EXTRA_SHORTCUT_INTENT, shortcutIntent);
        addIntent.putExtra(Intent.EXTRA_SHORTCUT_NAME,
                           "Apex: " + new File(path).getName());
        addIntent.putExtra(
            Intent.EXTRA_SHORTCUT_ICON_RESOURCE,
            Intent.ShortcutIconResource.fromContext(getApplicationContext(),
                                                    R.drawable.ic_launcher));
        addIntent.putExtra("duplicate", false);
        addIntent.setAction("com.android.launcher.action.INSTALL_SHORTCUT");
        getApplicationContext().sendBroadcast(addIntent);
    }

    public void signalApexInitialized() {
        runOnUiThread(new Runnable() {
                public void run() {
                    Log.d(TAG, "Ready to process intents.");
                    apexInitialized = true;
                    processLastIntent();
                }
            });
    }

    public void signalExperimentStarted() {
        runOnUiThread(new Runnable() {
                public void run() {
                    getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
                }
            });
    }

    public void signalExperimentFinished() {
        runOnUiThread(new Runnable() {
                public void run() {
                    getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
                }
            });
    }

    private void processLastIntent()
    {
        if (lastIntent == null || !apexInitialized)
            return;

        final String action = lastIntent.getAction();
        Log.d(TAG, "Processing Intent with action: " + action);
        if (Intent.ACTION_VIEW.equals(action))
            ApexNativeMethods.fileOpen(lastIntent.getData().getPath());

        lastIntent = null;
    }
}
