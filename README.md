# socket-programming
Socket Programming in UNIX

# multithreaded-server.c
The executable should be named "myserver". Like all your other projects, it needs to be implemented in C for the Linux OS.

The server will accept incoming connections on 3 ports: 8000, 8001, 8002. Each listening socket must have an independent thread.

It is very important to perform error checks e.g. if the bind to any port fails, you need to display an error message and terminate the server.

=> For port 8000, the server will return the current server time in human readable format (e.g. "Wed Jun 30 21:49:08 1993") and close the connection.

=> For port 8001, the server will echo back whatever is sent to it (e.g. if the client sends the string "Hello", the server will also send back "Hello"). It will keep doing this until the client terminates the connection.
=> For port 8002, the server will display the status of the time and echo services and allow the user to toggle them on/off and also allow the user to shutdown the server (see example below).

* If any service is made inactive, it should terminate any new connections received without performing its specified functionality. 

         ____________________________

          Welcome to myserver.

          The Time service is currently active.

          The Echo service is currently active.

          Input 1 to toggle the time service on/off, 
          2 to toggle the echo service on/off, 
          q to shutdown the server, 
          r to refresh, 
          any other key to terminate connection

          ____________________________
