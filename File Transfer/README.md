# File Finder Server (FFS) & File Finder Client (FFC) 

<b> Specifications: </b>
* The FFC takes the name of the file to search as command line arguments. For example “./FFC file_to_find.txt”
*  The FFC then connects with the FFS running at a known port No on the server.
* FFC sends the file name to search (file_to_find.txt, in this case) to FFS.
* FFS, on receiving the file name to search, crawls the the server database folder.
* FFS populates a Python dictionary with the key (absolute path name) : value (size of the file) pairs for all instances of the file found on the system.
* If the file is not available on the file system, the FFC is informed. Both FFC and FFS exit gracefully.
*  In case the file is found, the FFS sends the detail (absolute path name and file size) of all the instances of the file to the FFC.
* The FFC displays these as options to the user asking for file name to be transferred.
* The user enters the choice through keyboard input.
* The user selection is then transferred by FFC to FFS.
* The FFS reads the requested file and then transfers it to FFC through the open socket.
* The FFC receives the file and saves it with the name file_found.txt in the current directory.
* Both FFC and FFS maintains their separate log files clog.txt and slog.txt respectively. You should log the following:
	Time: Event( Send/Recv): Source IP: Destination IP 
* Program works for both Windows and Linux OS. 

