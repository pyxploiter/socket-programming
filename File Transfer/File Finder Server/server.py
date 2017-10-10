import sys
import os
import json
import datetime
from socket import *

serverIP = "127.0.0.1"
serverPort = 12007

def find(name):
	paths = {}
	# source: https://www.tutorialspoint.com/python/os_walk.htm
	for root, dirs, files in os.walk("database"):
		if name in files:
			file = os.path.join(root,name)
			# source: https://stackoverflow.com/questions/2104080/how-to-check-file-size-in-python
			file_size = os.stat(file).st_size
			paths.update({file:file_size})
	return paths

def ready_file(file_sr, paths):
	count = 1
	file = ""
	for filepath in paths:
		if (count == int(file_sr)):
			file = filepath
			break
		count+=1
	newfile = open(file, "r")
	return newfile

serverSocket = socket(AF_INET,SOCK_STREAM)
serverSocket.bind((serverIP,serverPort))
serverSocket.listen(1)

while 1:
	print "Waiting for connection"
	# wating for connection
	connectionSocket, addr = serverSocket.accept() 
	# receiving data
	print "connected."
	recv_filename = connectionSocket.recv(1024) 
	# finding file in server databse folder
	send_paths = find(recv_filename)
	# checking for empty paths
	if(any(send_paths) is False):
		connectionSocket.send("File not found")
		connectionSocket.close()
		break	
	# serializing the dictionary for transfering
	# source: https://stackoverflow.com/questions/15190362/sending-a-dictionary-using-sockets-in-python
	serialized_paths = json.dumps(send_paths)
	# sending data
	connectionSocket.send(serialized_paths) 
	# receiving data
	selected_file_sr = connectionSocket.recv(1024)
	
	# ready the required file to be sent
	file = ready_file(selected_file_sr, send_paths)

	#read from file
	read_data = file.read(1024)
	while (read_data):
		connectionSocket.send(read_data)
		read_data = file.read(1024)
	
	slog = open("slog.txt", "a")	
	log = str(datetime.datetime.utcnow())+" : Sent : "+str(connectionSocket.getsockname())+" : "+str(connectionSocket.getpeername())+"\n"
	log += "--------------------------------------------------------------------------------\n"
	# updating the slog file
	slog.write(log)
	print "File Sent."
	file.close()
	slog.close()
	connectionSocket.close()
print "Server is shutting down."
serverSocket.close()
