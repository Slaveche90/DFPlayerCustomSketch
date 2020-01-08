# DFPlayer custom sketch
Example sketch for sending commands to the DFPlayer Mini usning Uno without using external libraries. 

# Connection diagram  

![alt](https://github.com/Slaveche90/DFPlayer_Custom_Sketch/blob/master/ConnectionDiagram.png?raw=true)

# How to use the sketch

The sketch is tested with the DFPlayer mini module 
https://www.az-delivery.de/products/mp3-player-modul?_pos=2&_sid=30d4586b1&_ss=r

In the loop() function we are waiting for a letter to be sent from the Serial Monitor. If you send one of the letters from the loop() function, a specific function will be executed. You can see in the sketch which letters are used for which function.

When you send several commands to the DFPlayer Mini module, the output in the Serial Monitor should look like the output on the following image:

![alt](https://github.com/Slaveche90/DFPlayer_Custom_Sketch/blob/master/SerialOutput.png?raw=true)

We implemented the most usefull functions for DFPlayer Mini module, but not all of them. You can read about module and exmplanation for parts of the sketch in the eBook:
