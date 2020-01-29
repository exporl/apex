package be.kuleuven.med.exporl.apex;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

public class StartApexActivityWhenBootCompletedReceiver
    extends BroadcastReceiver {

    @Override
    public void onReceive(Context context, Intent intent) {
        if (Intent.ACTION_BOOT_COMPLETED.equals(intent.getAction())) {
            Intent newIntent = new Intent(context, ApexActivity.class);
            newIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            context.startActivity(newIntent);
        }
    }
}