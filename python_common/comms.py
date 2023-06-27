# =======================================================*
# Simple Arduino communication protocol w/ discovery
# 
#
# 
# Copyright 2023, Group Tantor (CreaTe M8 2023)
# You may use the code under the terms of the MIT license
# =======================================================*/

print("[LIB] Comm serial protocol")

import serial
import serial.tools.list_ports
from python_common import motor
import sys
import time
import readline


# util function for interactive port selection 
# it allows us to prefill the input with the first port name to make interactions quicker
def input_with_prefill(prompt, text):
    def hook():
        readline.insert_text(text)
        readline.redisplay()
    readline.set_pre_input_hook(hook)
    result = input(prompt)
    readline.set_pre_input_hook()
    return result


# The serial class which is a wrapper around the pyserial Serial class 
class Serial:
    # Set up connection 
    def __init__(self, path=None):
        # set up connection
        if path is not None:
            try:
                self.serialInst = serial.Serial()
                self.serialInst.port = path
                self.serialInst.timeout = 1
                self.serialInst.baudrate = 9600
                self.serialInst.open()
            except: 
                self.serialInst = None
        else: 
            self.serialInst = self.connectionInteractive()

        if self.serialInst is None:
            raise Exception("[Serial] Could not find valid serial port")

             
        

    # Get a connection interactively   
    def connectionInteractive(self):
        ports = serial.tools.list_ports.comports()
        # Handle no devices being connected
        if len(ports) < 1:
            print("[Error] Could not find any serial devices... \nReconnect the arduino and try again.")
            return None

        print("== Detected ports: ==")
        for port in ports: 
            print("-->\t", str(port.device))
        
        path = input_with_prefill("== Give device path/comport: ==\n> ", str(ports[0].device))
        # Configure serial  
        ser = serial.Serial()
        ser.port = path
        ser.timeout = 1
        ser.open()
        return ser

    @classmethod
    def scan_all(cls):
        ports = serial.tools.list_ports.comports()
        
        print("== Detected ports: ==")
        for port in ports: 
            print("-->\t", str(port.device))
        
        # Handle no devices being connected
        if len(ports) < 1:
            print("[Error] Could not find any serial devices... \nReconnect the arduino and try again.")
            return None

        # Look trough all ports to find the one 
        ids = {}
        for port in ports:
            # Skip the RPI bluetooth/uart port
            if port.device == "/dev/ttyAMA0": 
                continue

            try:
                # Create a serial socket  
                s = serial.Serial()
                s.port=port.device
                s.baudrate=9600
                s.timeout=1
                s.open()
                
                # Send hanshake
                # Read until a handshake is found  
                while True: 
                    print(f"[SCAN_SEND] {port.device}: l=A, v=MST 0")
                    cls.send_message(None,'A',"MST 0", ser=s)

                    l,v = cls.wait_for_message(None,ser=s, timeout=100000)
                    print(f"[SCAN_MESSAGE] {port.device}: {l=}, {v=} ")
                    if l == 'I':
                        print(f"[SCAN] Port {port.device} is an arduino device: {v}")
                        ids[v] = port.device
                        break

                    elif l == 'B': # Motor detection 
                        print(f"[SCAN] Port {port.device} is a motor")
                        ids["_MOTOR"] = port.device
                        print(ids)
                        break

                s.close()
            except Exception as e: 
                print(f"[SCAN] Port {port.device} could not be connected to:\n{e}")
                s.close()
        
        print(f"[SCAN] Scan complete: {ids=}")
        return ids
    
    @classmethod 
    def connect_to_all(cls,ids):
        conns = {}
        for id, port in ids.items():
            if id == "_MOTOR":
                print("[CONN] Found motor")
                conns[id] = motor.Motor.from_path(port,9600)
            else:
                conns[id] = cls(path=port) 

        print(f"[CONN] Created connections:\n{conns=}")
        return conns


        

    def send_message(self, label, value, ser=None):
        # Optional serial override, used for discovery 
        if ser is None: 
            ser = self.serialInst

        # Write message 
        ser.write(f"{label}{value}\r".encode(encoding='ascii', errors='replace'))
        print(f"[WRITE] {label}{value}\\r")
        

    # Reads the message, byte by byte 
    def wait_for_message(self, ser=None, timeout=-1):
        # Optional serial override, used for discovery
        if ser is None: 
            ser = self.serialInst

        # Composes the line  
        line = []
        end_time = time.time_ns() + (timeout * 1000) 
        while True:
             char = str(ser.read(1), 'utf-8')
             if timeout > 0 and time.time_ns() > end_time:
                 return 'y', -1

             if char == '':
                continue

             # Line break and carriage return are the breaking characters in this protocol
             if '\n' in char or '\r' in char:
                 break
            
             line.append(char)
             
             print(f"[SERIAL]: '{''.join(line)}'", end='\r')

        # Return z-message for stray breaking characters 
        if len(line) < 2:
            return 'z', -1

        try: 
            # If we capture too little characters by accident, this will fail
            val = ''.join(line[1:])
            label = line[0]
        except:
            # In that case return a z-message 
            return 'z', -1
        
        # Debug
        print(f"[READ]: {label=} , {val=}")
        return label,val 
    
    # Wrapper to close the socket 
    def close(self):
        self.serialInst.close()
