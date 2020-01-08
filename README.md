# DFPlayer custom sketch
Example sketch for sending commands to the DFPlayer Mini usning Uno without using external libraries. 

# Sending commands to the DFPlayer Mini

To send a command to the module, follow specific format:  
**$SB VB LB CMD ACK DATA1 DATA2 CHKS1 CHKS2 $EB**    

Mark - Byte - Byte description  
$SB - 0x7E - Start byte  
VB - 0xFF - Version byte  
LB - 0xxx - The number of bytes of the command without start and end   bytes (In our case 0x06)  
CMD - 0xxx - Such as PLAY and PAUSE and so on  
ACK - 0xxx - Acknowledge byte 0x00 = not ack, 0x01 = ack  
DATA1 - 0xxx - Data high byte  
DATA2 - 0xxx - Data low byte  
CHKS1 - 0xxx - Checksum high byte  
CHKS2 - 0xxx - Checksum low byte  
$EB - 0xEF - End byte  

We used acknowledge byte equal to 0x01 to get the reponse from the module   

# Connection diagram  

![alt](https://github.com/Slaveche90/DFPlayer_Custom_Sketch/blob/master/ConnectionDiagram.png?raw=true)

# How to use the sketch

The sketch is tested with the DFPlayer mini module 
https://www.az-delivery.de/products/mp3-player-modul?_pos=2&_sid=30d4586b1&_ss=r

In the loop() function we are waiting for a letter to be sent from the Serial Monitor. If you send one of the letters from the loop() function, a specific function will be executed. 

When you send several commands to the DFPlayer Mini module, the output in the Serial Monitor should look like the output on the following image:

![alt](https://github.com/Slaveche90/DFPlayer_Custom_Sketch/blob/master/SerialOutput.png?raw=true)

We implemented the most usefull functions for DFPlayer Mini module, but not all of them. You can read about module and exmplanation for parts of the sketch in the eBook:
