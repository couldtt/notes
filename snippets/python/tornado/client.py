#!/usr/bin/env python
# coding=utf-8
import socket
import time

HOST = '127.0.0.1'
PORT = 8000

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST,PORT))

s.sendall('Hello, \nw')
time.sleep(5)
s.sendall('ord! \n')

data = s.recv(1024)

print 'Received', repr(data)

time.sleep(60)
s.close()
