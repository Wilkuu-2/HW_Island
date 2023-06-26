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
import motor
import sys
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
    def __init__(self, auto_id=None, path=None):
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
        elif auto_id is not None:
            self.serialInst = self.connectionAuto(auto_id)
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

    def connectionAuto(self, id):
        ports = serial.tools.list_ports.comports()
        
        print("== Detected ports: ==")
        for port in ports: 
            print("-->\t", str(port.device))
        
        # Handle no devices being connected
        if len(ports) < 1:
            print("[Error] Could not find any serial devices... \nReconnect the arduino and try again.")
            return None

        # Look trough all ports to find the one 
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
                    self.send_message('A',"MST 0", ser=s)
                    print(f"[AUTO_SEND] {port.device}: l=A, v=MST 0")
                    l,v = self.wait_for_message(ser=s)
                    print(f"[AUTO_MESSAGE] {port.device}: {l=}, {v=} ")
                    if l == 'I':
                        if v == id: # Arduino detection  
                            print(f"[AUTO] Port {port.device} is '{id}', target found. ")
                            return s
                        else:
                            print(f"[AUTO] Port {port.device} is not '{id}' but {v}, continuing. ")
                            break

                    elif l == 'A': # Motor detection 
                        print(f"[AUTO] Port {port.device} is a motor")
                        if id == "_MOTOR":
                            return s

                s.close()
            except Exception as e: 
                print(f"[AUTO] Port {port.device} could not be connected to:\n{e}")
                s.close()
        
        print(f"[AUTO] No working ports with id of '{id}'")
        return None
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
        for port in ports:
            ids = {}
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
                    cls.send_message(None,'A',"MST 0", ser=s)
                    print(f"[SCAN_SEND] {port.device}: l=A, v=MST 0")
                    l,v = cls.wait_for_message(None,ser=s)
                    print(f"[SCAN_MESSAGE] {port.device}: {l=}, {v=} ")
                    if l == 'I':
                        print(f"[SCAN] Port {port.device} is an arduino device: {v}")
                        ids[v] = port.device
                        break

                    elif l == 'A': # Motor detection 
                        print(f"[SCAN] Port {port.device} is a motor")
                        ids["_MOTOR"] = port.device
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
        ser.write(f"{label}{value}\r\n".encode(encoding='ascii', errors='replace'))
        

    # Reads the message, byte by byte 
    def wait_for_message(self, ser=None):
        # Optional serial override, used for discovery
        if ser is None: 
            ser = self.serialInst

        # Composes the line  
        line = []
        while True:
             char = str(ser.read(1), 'utf-8')
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
