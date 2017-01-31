MatlabFilter

Uses a matlab function as processing callback.

Make sure the matlab libraries are available for apex:
LD_LIBRARY_PATH=/usr/lib:/usr/local/matlab2007a/bin/glnx86:/usr/local/matlab2007a/sys/os/glnx86/ apex3


Implement 2 m-files:
    - a prepare function (setting the path, etc)
    - a process function, accepting and returning a data buffer

    
Specify their name as parameter
    - preparefunction
    - processfunction
    
    