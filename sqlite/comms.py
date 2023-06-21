# =======================================================*
# Simple Arduino communication protocol w/ discovery
# 
#
# 
# Copyright 2023, Group Tantor (CreaTe M8 2023)
# You may use the code under the terms of the MIT license
# =======================================================*/

import serial
import serial.tools.list_ports
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
    def __init__(self, auto_id=None):
        # set up connection
        if auto_id is not None:
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
        
        # Handle no devices being connected
        if len(ports) < 1:
            print("[Error] Could not find any serial devices... \nReconnect the arduino and try again.")
            return None

        # Look trough all ports to find the one 
        for port in ports:
            try:
                # Create a serial socket  
                s = serial.Serial()
                s.port=port.device
                s.baudrate=9600
                s.timeout=1
                s.open()
                
                # Send hanshake
                self.send_message('u',id, ser=s)
               
                # Read until a handshake is found  
                while True: 
                    l,v = self.wait_for_message(ser=s)
                    if l == 'i':
                        if v == id: 
                            print(f"[AUTO] Port {port.device} is '{id}', target found. ")
                            return s
                        else:
                            print(f"[AUTO] Port {port.device} is not '{id}' but {v}, continuing. ")
                            break
                s.close()
            except Exception as e: 
                print(f"[AUTO] Port {port.device} could not be connected to:\n{e}")
                s.close()
        
        print(f"[AUTO] No working ports with id of '{id}'")
        return None

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
             # Line break and carriage return are the breaking characters in this protocol
             if '\n' in char or '\r' in char:
                 break
            
             line.append(char)

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
        print(f"READ: {label=} , {val=}")
        return label,val 
    
    # Wrapper to close the socket 
    def close(self):
        self.serialInst.close()
