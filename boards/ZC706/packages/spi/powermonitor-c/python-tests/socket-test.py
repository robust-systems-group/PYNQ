import socket
import os
import struct
from PowerMonitor_pb2 import ControlData
import select
import time

sock_addr = "/tmp/powermonitor.sock"

sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)


while True:
    print("Waiting for connection")
    sock.connect(sock_addr)
    app_name = ControlData()
    app_name.app_name = "socket-test.py"
    msg_len = app_name.ByteSize()

    msg_len = struct.pack('!I', msg_len)
    try:
        print("Connected from " + client_addr)
        sock.sendall(msg_len)
        sock.sendall(app_name.SerializeToString())
        print("sent message.")
        time.sleep(3)

        # send stop signal
        stop = ControlData()
        stop.stop_monitoring = True
        msg_len = stop.ByteSize()
        msg_len = struct.pack('!I', msg_len)
        print("Sending stop signal.")
        sock.sendall(msg_len)
        sock.sendall(stop.SerializeToString())

        break
    finally:
        sock.close()


