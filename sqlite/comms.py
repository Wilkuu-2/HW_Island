import serial
import serial.tools.list_ports
import sys
import readline

def input_with_prefill(prompt, text):
    def hook():
        readline.insert_text(text)
        readline.redisplay()
    readline.set_pre_input_hook(hook)
    result = input(prompt)
    readline.set_pre_input_hook()
    return result

class Serial:

    def __init__(self, auto_id=None):
        # set up connection
        if auto_id is not None:
            self.serialInst = self.connectionAuto(auto_id)
        elif sys.platform == 'windows' or sys.platform == 'cygwin':
            self.serialInst = self.connectionWin()
        else: 
            self.serialInst = self.connectionLinux()

    def connectionWin(self):
        ports = serial.tools.list_ports.comports()
        
        if len(ports) < 1:
            print("[Error] Could not find any serial devices... \nReconnect the arduino and try again.")
            exit(1)

        print("== Detected ports: ==")
        for port in ports: 
            print("-->\t", str(port.device))

        comport = input_with_prefill("== Select Com port: ==\n> ", str(ports[0].device))

        for x in range(0, len(portList)):
            if portList[x].startswith('COM' + str(val)):
                portVar = 'COM' + str(val)

        ser = serial.serial()
        ser.baudrate = 9600  # same as arduino
        ser.timeout = 1
        ser.port = comport
        ser.open()
        return ser


    def connectionLinux(self):
        ports = serial.tools.list_ports.comports()
        
        if len(ports) < 1:
            print("[Error] Could not find any serial devices... \nReconnect the arduino and try again.")
            exit(1)

        print("== Detected ports: ==")
        for port in ports: 
            print("-->\t", str(port.device))
        
        path = input_with_prefill("== Give device path: ==\n> ", str(ports[0].device))
        return serial.Serial(path,9600)

    def connectionAuto(self, id):
        ports = serial.tools.list_ports.comports()
        
        for port in ports:
            try:
                s = serial.Serial(port.device, 9600)

                self.send_line_val('u',id, ser=s)
                
                while True: 
                    l,v = self.wait_for_line_val(ser=s)
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

    def send_line_val(self, label, value, ser=None):
        if ser is None: 
            ser = self.serialInst

        ser.write(f"{label}{value}\r\n".encode(encoding='ascii', errors='replace'))
        


    def wait_for_line_val(self, ser=None):
        print(ser)
        if ser is None: 
            ser = self.serialInst

        line = []
        while True:
             char = str(ser.read(1), 'utf-8')
             if '\n' in char or '\r' in char:
                 break
             line.append(char)

        if len(line) < 2:
            return 'z', -1

        try:
            val = ''.join(line[1:])
            label = line[0]
        except:
            return 'z', -1
        
        print(f"READ: {label=} , {val=}")
        return label,val 
    
    def close(self):
        self.serialInst.close()
