class Program
{
    static void Main(string[] args)
    {
        new SlaveClient(new BKSoundLevelMeter(), args[0]).serve();
    }
}
