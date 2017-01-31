using System;
using System.Collections.Generic;
using System.Globalization;
using System.Threading;
using BK.BasicEnv.Application;

class BKSoundLevelMeter
{
    private RemoteAPI api;
    private string instrumentName;
    private TypeMeasurement type;
    private FrequencyWeighting freqWeight;
    private TimeWeighting timeWeight;
    private bool measuring;
    private bool retrievePercentile;
    private int requestResult;

    private enum TypeMeasurement {
        SPLMeasurement,
        pkSPLMeasurement,
    }

    private enum FrequencyWeighting {
        AWeighting,
        CWeighting,
        ZWeighting,
    }

    private enum TimeWeighting {
        FastWeighting,
        SlowWeighting,
        ImpulseWeighting,
    }

    public BKSoundLevelMeter()
    {
        measuring = false;
    }

    ~BKSoundLevelMeter()
    {
        if (api.IsConnected(instrumentName))
            api.SetParameterEnum(instrumentName, "QueryStatusIsOn", 1);
        api.Close();
    }

    private void checkConnected()
    {
        if (!api.IsConnected(instrumentName))
            throw new System.InvalidOperationException("Device isn't connected");
    }

    // TODO: use somewhere
    public string getMeasurementState()
    {
        checkConnected();
        return api.ReadOutputEnum(instrumentName, "MeasState").ToString();
    }

    public void initialize()
    {
        try {
            api = new RemoteAPI();
        } catch (Exception) {
            throw new System.InvalidOperationException("Couldn't initialize the remote API. Check if all the files in the B&K directory (most likely C:/Program Files/Bruel and Kjaer/ENV/BZ5503/) " +
                                                       "have been copied to the Apex/bin/ directory and ensure the B&K measurement partner suite and usb drivers have been installed. " +
                                                       "While copying the directory, keep the newer files when asked which to overwrite.");
        }
    }

    public void connect()
    {
        // retrieve all connected instruments (in our case one), and take the first.
        instrumentName = api.GetInstrumentNames()[0];
        api.Connect(instrumentName);
        Thread.Sleep(100);
        // Disable pop-ups in the instrument
        api.SetParameterEnum(instrumentName, "QueryStatusIsOn", 0);
    }

    public void disconnect()
    {
        api.SetParameterEnum(instrumentName, "QueryStatusIsOn", 1);
        Thread.Sleep(100);
        api.Disconnect(instrumentName);
    }

    public string transducers()
    {
        return String.Join(",", transducerList());
    }

    public void setTransducer(string name)
    {
        checkConnected();
        // TODO: what if we just set a wrong one and wait what happens?
        if (!transducerList().Contains(name))
            throw new System.ArgumentException("Transducer " + name + " not found or supported.");
        api.SetParameterValue(instrumentName, "TransducerId", name);
    }

    public string instruments()
    {
        return String.Join(",", api.GetInstrumentNames());
    }

    public void setInstrument(string name)
    {
        api.Connect(name);
        Thread.Sleep(100);
        // Disable pop-ups in the instrument
        api.SetParameterEnum(instrumentName, "QueryStatusIsOn", 0);
    }

    public void startMeasurement(string typeMeasure, string frequencyWeight,
            string timeWeightingIn, string percentileS)
    {
        checkConnected();

        setParameters(typeMeasure, frequencyWeight, timeWeightingIn);
        double percentile = Math.Floor(double.Parse(percentileS, CultureInfo.InvariantCulture) * 100);
        if (percentile < 0.1 || percentile > 99.9)
            throw new System.ArgumentOutOfRangeException("Percentile should be from 0.1 to 99.9.");
        api.SetParameterSingle(instrumentName, "StatisticsPercentile1", (float) percentile);
        retrievePercentile = true;
        api.SetParameterEnum(instrumentName, "StopMode", 0);
        api.ExecuteCommand(instrumentName, "Reset");
        switch (freqWeight) {
        case FrequencyWeighting.AWeighting:
            api.SetParameterEnum(instrumentName, "BroadBandFreqWeight", 0); //AZ
            api.SetParameterEnum(instrumentName, "PeakFreqWeight", 0); //A
            break;
        case FrequencyWeighting.CWeighting:
            api.SetParameterEnum(instrumentName, "BroadBandFreqWeight", 1); //AC
            api.SetParameterEnum(instrumentName, "PeakFreqWeight", 1); //C
            break;
        case FrequencyWeighting.ZWeighting:
            api.SetParameterEnum(instrumentName, "BroadBandFreqWeight", 0); //AZ
            api.SetParameterEnum(instrumentName, "PeakFreqWeight", 2); //Z
            break;
        }
        api.ExecuteCommand(instrumentName, "PauseContinue");
        measuring = true;
    }

    public void stopMeasurement()
    {
        checkConnected();

        if (!measuring)
            return;

        api.ExecuteCommand(instrumentName, "PauseContinue");
        measuring = false;
        measure0();
    }

    public void measure0()
    {
        measure1(measurementName());
    }

    public void measure3(string typeMeasure, string frequencyWeight, string timeWeightingIn)
    {
        setParameters(typeMeasure, frequencyWeight, timeWeightingIn);
        measure1(measurementName());
    }

    public void measure1(string measurement)
    {
        checkConnected();
        requestResult = api.ReadOutputInt16(instrumentName, measurement);
    }

    public string result()
    {
        return (requestResult / 100.0).ToString(CultureInfo.InvariantCulture);
    }

    private List<string> transducerList()
    {
        // TODO what happens if we use a null out variable?
        int index = 0;
        return new List<string>(api.GetParameterValues(instrumentName, "TransducerId", out index));
    }

    private void setParameters(string typeMeasure, string frequencyWeight, string timeWeightingIn)
    {
        type = (TypeMeasurement) Int32.Parse(typeMeasure, CultureInfo.InvariantCulture);
        freqWeight = (FrequencyWeighting) Int32.Parse(frequencyWeight, CultureInfo.InvariantCulture);
        timeWeight = (TimeWeighting) Int32.Parse(timeWeightingIn, CultureInfo.InvariantCulture);
    }

    private string frequencyName()
    {
        return freqWeight == FrequencyWeighting.ZWeighting ? "CZ" : "AC";
    }

    private string timeName()
    {
        return timeWeight == TimeWeighting.FastWeighting ? "F" :
            timeWeight == TimeWeighting.SlowWeighting ? "S" : "I";
    }

    private string measurementName()
    {
        if (!measuring)
            return retrievePercentile ? "LAN1" : "CPBLACZeq";
        if (type == TypeMeasurement.SPLMeasurement)
            return "L" + frequencyName() + timeName() + "(SPL)";
        throw new System.ArgumentException("Unknown measurement type");
    }
}
