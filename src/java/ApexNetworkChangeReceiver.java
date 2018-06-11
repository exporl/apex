package be.kuleuven.med.exporl.apex;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;

public class ApexNetworkChangeReceiver extends BroadcastReceiver {
    @Override
    public void onReceive(final Context context, final Intent intent) {
        ConnectivityManager connectivityManager =
            (ConnectivityManager)context.getSystemService(
                Context.CONNECTIVITY_SERVICE);
        NetworkInfo activeNetwork = connectivityManager.getActiveNetworkInfo();
        ApexNativeMethods.networkChanged(
            activeNetwork != null && activeNetwork.isConnectedOrConnecting() &&
            activeNetwork.getType() == ConnectivityManager.TYPE_WIFI);
    }
}
