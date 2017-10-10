# Python 2.7.12
# M. Asad Ali : 134437

import sys
import os
import json
import datetime
from socket import *	# import socket module

if (len(sys.argv) < 2):
	print "Execution help: python client.py <filename>.<ext>"
	exit(0)

serverIP = '127.0.0.1'	# set IP where server is listening
serverPort = 12007	# port where server is listening

clientSocket = socket(AF_INET, SOCK_STREAM) 	# create tcp socket

clientSocket.connect((serverIP,serverPort)) # request for connection to server
#sentence = raw_input('Enter the file name: ') # input from user

sentence = str(sys.argv[1])

clientSocket.send(sentence)	# send data to server
recv_paths = clientSocket.recv(1024) # receive data
if (recv_paths == "File not found"):
	print recv_paths
	clientSocket.close()
	exit(0)
else:
	count = 1
	recv_paths = json.loads(recv_paths)
	print "----------------------------"
	print "Files available on server:- "
	print "[Sr.] [ File Name ] [ File Size ]"
	print "----------------------------------"
	for file in recv_paths:
		print "[",count,"] [",file,"] [",recv_paths[file],"bytes ]"
		count += 1
	print "----------------------------------"
	select_sr = raw_input("Select file number: ")
	# send user file selecction
	clientSocket.send(select_sr)
	# create new file
	newfile = open(sentence,'wb')
	# receiving file data
	recv_data = clientSocket.recv(1024)
	while(recv_data):
		newfile.write(recv_data)
		recv_data = clientSocket.recv(1024)
	
	clog = open("clog.txt", "a")	
	log = str(datetime.datetime.utcnow())+" : Received : "+str(clientSocket.getsockname())+" : "+str(clientSocket.getpeername())+"\n"
	log += "------------------------------------------------------------------------------------\n"
	# updating the clog file
	clog.write(log)
	print "File received."
	newfile.close()
clog.close()
clientSocket.close()
