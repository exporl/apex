package be.kuleuven.med.exporl.apex;

public class ApexNativeMethods {
    public static native void fileOpen(String path);
    public static native void loadRunner(String runner);
    public static native void logDebugItem(String item);
    public static native void networkChanged(boolean available);
}
