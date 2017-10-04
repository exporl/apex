package be.kuleuven.med.exporl.apex;

import be.kuleuven.med.exporl.apex.R;
import org.qtproject.qt5.android.bindings.QtActivity;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.media.AudioManager;
import android.net.Uri;
import android.os.Bundle;
import android.os.Debug;
import android.util.Base64;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Window;
import android.view.WindowManager;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.Runtime;
import java.security.cert.X509Certificate;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.util.Enumeration;

import javax.net.ssl.TrustManager;
import javax.net.ssl.TrustManagerFactory;
import javax.net.ssl.X509TrustManager;

public class ApexActivity extends QtActivity {
    private static final String TAG = "APEX";
    private boolean apexInitialized;
    private Intent lastIntent;

    public ApexActivity() {
        super();
        apexInitialized = false;
        lastIntent = null;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        lastIntent = getIntent();
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        lastIntent = intent;
        processLastIntent();
    }

    @Override
    protected void onResume() {
        super.onResume();
        ApexNativeMethods.logDebugItem("onResume");
    }

    @Override
    protected void onPause() {
        super.onPause();
        ApexNativeMethods.logDebugItem("onPause");
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        /* Disable the volume keys */
        if (keyCode == KeyEvent.KEYCODE_VOLUME_DOWN ||
            keyCode == KeyEvent.KEYCODE_VOLUME_UP)
            return true;
        return super.onKeyDown(keyCode, event);
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
                           new File(path).getName());
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
        maximizeVolume();
    }

    public void signalExperimentStarted() {
        runOnUiThread(new Runnable() {
            public void run() {
                getWindow().addFlags(
                    WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
            }
        });
    }

    public void signalExperimentFinished() {
        runOnUiThread(new Runnable() {
            public void run() {
                getWindow().clearFlags(
                    WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
            }
        });
    }

    public void signalTrialStarted() {
        maximizeVolume();
    }

    public String getSSLCertificateDirectory() {
        String certsDirPath = getValueFromSharedPreferences("certsDir");
        if (!certsDirPath.isEmpty()) {
            return certsDirPath;
        }

        File certsDir = getDir("certs", Context.MODE_PRIVATE);
        try {
            TrustManagerFactory factory = TrustManagerFactory.getInstance(
                TrustManagerFactory.getDefaultAlgorithm());
            factory.init((KeyStore)null);

            for (TrustManager manager : factory.getTrustManagers()) {
                if (manager instanceof X509TrustManager) {
                    X509TrustManager trustManager = (X509TrustManager)manager;

                    for (X509Certificate certificate :
                         trustManager.getAcceptedIssuers()) {
                        FileOutputStream out = null;
                        try {
                            File certFile =
                                File.createTempFile("cert", "", certsDir);
                            certFile.createNewFile();
                            out = new FileOutputStream(certFile);
                            out.write(
                                "-----BEGIN CERTIFICATE-----\n".getBytes());
                            out.write(Base64.encode(certificate.getEncoded(),
                                                    Base64.DEFAULT));
                            out.write("-----END CERTIFICATE-----".getBytes());
                        } catch (Exception e) {
                            Log.e(TAG, "Failed to write certificate", e);
                        } finally {
                            if (out != null) {
                                try {
                                    out.close();
                                } catch (Exception e) {
                                }
                            }
                        }
                    }
                }
            }
        } catch (Exception e) {
            Log.e(TAG, "Failed to get certificates", e);
        }

        try {
            certsDirPath = certsDir.getCanonicalPath();
            setValueInSharedPreferences("certsDir", certsDirPath);
            ApexNativeMethods.logDebugItem("CERTSDIRPATH: " + certsDirPath +
                                           " " + certsDir.getPath());
            return certsDirPath;
        } catch (Exception e) {
            Log.e(TAG, "Failed to get certs directory canonical path", e);
            return "";
        }
    }

    public void startGdbServer() {
        try {
            int id = android.os.Process.myPid();
            Runtime.getRuntime().exec(
                getApplicationContext().getFilesDir().getParent() +
                "/lib/libgdbserver.so :5039 --attach " + id);
        } catch (Exception e) {
            Log.e(TAG, "Failed to start gdbserver", e);
        }
    }

    private void processLastIntent() {
        if (lastIntent == null || !apexInitialized)
            return;

        final String action = lastIntent.getAction();
        Log.d(TAG, "Processing Intent with action: " + action);
        if (Intent.ACTION_VIEW.equals(action))
            ApexNativeMethods.fileOpen(lastIntent.getData().getPath());

        lastIntent = null;
    }

    private void maximizeVolume() {
        AudioManager audioManager =
            (AudioManager)getApplicationContext().getSystemService(
                Context.AUDIO_SERVICE);
        audioManager.setStreamVolume(
            AudioManager.STREAM_MUSIC,
            audioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC), 0);
    }

    private String getValueFromSharedPreferences(String key) {
        SharedPreferences sharedPref = getPreferences(Context.MODE_PRIVATE);
        return sharedPref.getString(key, "");
    }

    private void setValueInSharedPreferences(String key, String value) {
        SharedPreferences sharedPref = getPreferences(Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPref.edit();
        editor.putString(key, value);
        editor.commit();
    }
}
