# Audio File Transfer using TCP & UDP Sockets

Sender requirements:
Following are sender requirements:
* Able to open and read contents of a audio file
* Create segments and send them separately over TCP and UDP
* Indicate the end of file to receiver
* Close the socket once complete file is sent

Receiver requirements:
Following are receiver requirements:
* Able to open a new audio file
* Receive segments and write them in new file
* Able to identify eof to terminate
* Close the file and socket as well
