package be.kuleuven.med.exporl.apex;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;

public class ApexNetworkChangeReceiver extends BroadcastReceiver {

    @Override
    public void onReceive(Context context, Intent intent) {
        if (!isNetworkAvailable(context))
            return;

        try {
            ApexNativeMethods.networkAvailable();
        } catch (UnsatisfiedLinkError e) {
            // If the apex activity isn't running, this results in an
            // UnsatisfiedLinkError. Another solution would be to create a new
            // intent for the apex activity here and announce the
            // network-available event from within the activity. But this starts
            // a new activity on each network change, which might not be
            // desired.
        }
    }

    private boolean isNetworkAvailable(Context context) {
        NetworkInfo networkInfo = getNetworkInfo(context);
        return networkInfo != null && networkInfo.isConnectedOrConnecting() &&
            networkInfo.getType() == ConnectivityManager.TYPE_WIFI;
    }

    private NetworkInfo getNetworkInfo(Context context) {
        return getConnectivityManager(context).getActiveNetworkInfo();
    }

    private ConnectivityManager getConnectivityManager(Context context) {
        return (ConnectivityManager)context.getSystemService(
            Context.CONNECTIVITY_SERVICE);
    }
}
