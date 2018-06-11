package be.kuleuven.med.exporl.apex;

import be.kuleuven.med.exporl.apex.R;
import org.qtproject.qt5.android.bindings.QtActivity;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.media.AudioManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.os.Bundle;
import android.os.Debug;
import android.util.Base64;
import android.util.Log;
import android.util.Patterns;
import android.view.KeyEvent;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import java.io.File;
import java.io.FileFilter;
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
    private static final String ACTION_LOADRUNNER =
        "be.kuleuven.med.exporl.apex.RUNNER";
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
        deleteTemporaryFiles();
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

    public void createShortcutToFile(String path) {
        // Intent to start apex with a certain experiment
        Intent shortcutIntent =
            new Intent(getApplicationContext(), ApexActivity.class);
        shortcutIntent.setData(Uri.parse(path));
        shortcutIntent.setAction(Intent.ACTION_VIEW);
        createShortcut(shortcutIntent, new File(path).getName());
    }

    public void createShortcutToRunner(String runner) {
        Intent shortcutIntent =
            new Intent(getApplicationContext(), ApexActivity.class);
        shortcutIntent.setAction(ACTION_LOADRUNNER);
        shortcutIntent.putExtra(ACTION_LOADRUNNER, runner);
        createShortcut(shortcutIntent, runner);
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

    public void shareText(String text) {
        Intent shareIntent = new Intent();
        if (Patterns.WEB_URL.matcher(text).matches()) {
            shareIntent.setAction(Intent.ACTION_VIEW);
            shareIntent.setData(Uri.parse(text));
        } else {
            shareIntent.setAction(Intent.ACTION_SEND);
            shareIntent.putExtra(Intent.EXTRA_TEXT, text);
            shareIntent.setType("text/plain");
        }
        startActivity(Intent.createChooser(shareIntent, "Share via"));
    }

    public boolean hasNetworkConnection() {
        ConnectivityManager connectivityManager =
            (ConnectivityManager)getApplicationContext().getSystemService(
                Context.CONNECTIVITY_SERVICE);
        NetworkInfo activeNetwork = connectivityManager.getActiveNetworkInfo();
        return activeNetwork != null &&
            activeNetwork.isConnectedOrConnecting() &&
            activeNetwork.getType() == ConnectivityManager.TYPE_WIFI;
    }

    public void maximizeVolume() {
        AudioManager audioManager =
            (AudioManager)getApplicationContext().getSystemService(
                Context.AUDIO_SERVICE);
        audioManager.setStreamVolume(
            AudioManager.STREAM_MUSIC,
            audioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC), 0);
    }

    public void sendToast(String text) {
        class ToastWorker implements Runnable {
            String message;
            ToastWorker(String msg) {
                message = msg;
            }
            @Override
            public void run() {
                Toast
                    .makeText(getApplicationContext(), message,
                              Toast.LENGTH_LONG)
                    .show();
            }
        }
        runOnUiThread(new ToastWorker(text));
    }

    private void processLastIntent() {
        if (lastIntent == null || !apexInitialized)
            return;

        final String action = lastIntent.getAction();
        Log.d(TAG, "Processing Intent with action: " + action);
        if (Intent.ACTION_VIEW.equals(action))
            ApexNativeMethods.fileOpen(lastIntent.getData().getPath());
        else if (ACTION_LOADRUNNER.equals(action))
            ApexNativeMethods.loadRunner(
                lastIntent.getExtras().getString(ACTION_LOADRUNNER));
        lastIntent = null;
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

    private void createShortcut(Intent intent, String name) {
        Intent addIntent = new Intent();
        addIntent.putExtra(Intent.EXTRA_SHORTCUT_INTENT, intent);
        addIntent.putExtra(Intent.EXTRA_SHORTCUT_NAME, name);
        addIntent.putExtra(
            Intent.EXTRA_SHORTCUT_ICON_RESOURCE,
            Intent.ShortcutIconResource.fromContext(getApplicationContext(),
                                                    R.drawable.ic_launcher));
        addIntent.putExtra("duplicate", false);
        addIntent.setAction("com.android.launcher.action.INSTALL_SHORTCUT");
        getApplicationContext().sendBroadcast(addIntent);
    }

    private void deleteTemporaryFiles() {
        class FileDeleter implements Runnable {
            File[] files;
            FileDeleter(File[] f) {
                files = f;
            }
            @Override
            public void run() {
                for (File file : files) {
                    try {
                        recursiveDelete(file);
                    } catch (SecurityException e) {
                        Log.e(TAG, "Unable to delete " + file.getName(), e);
                    } catch (ApexException e) {
                        Log.e(TAG, "Recursive delete failed", e);
                    }
                }
            }
        }

        File filesDir = getApplicationContext().getFilesDir();
        try {
            File[] files = filesDir.listFiles(new FileFilter() {
                @Override
                public boolean accept(File file) {
                    return file.getName().matches("APEX-[a-zA-Z0-9]{6}");
                }
            });
            new Thread(new FileDeleter(files)).start();
        } catch (SecurityException e) {
            Log.e(TAG, "Unable to get list of temporary files.", e);
        }
    }

    private void recursiveDelete(File file) throws ApexException {
        if (!file.exists())
            return;
        if (file.isDirectory())
            for (File child : file.listFiles())
                recursiveDelete(child);
        if (!file.delete())
            throw new ApexException("Unable to delete file " + file.getName());
    }
}
