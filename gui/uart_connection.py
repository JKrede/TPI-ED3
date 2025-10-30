import serial, threading
from ctypes import c_double


class UARTConnector:
    def __init__(self, baudrate, COM, scope):
        self.baudrate = baudrate
        self.COM = COM
        self.scope = scope
        self.running = False

    def connectUARTpushButton_clicked(self):

        try:
            self.port  = serial.Serial(
                self.COM,
                baudrate=self.baudrate,          
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                timeout=0,                       
                write_timeout=0
            )
            try:
                self.running = True
                self.serial_thread = threading.Thread(target=self.read_port, daemon=True)
                self.serial_thread.start()

                print(f"Puerto {self.COM} conectado correctamente")
            except Exception as e:
                print(f"Error al inciar el threading de lectura del puerto {self.COM}: {e}")

        except Exception as e:
            print(f"Error al abrir el puerto {self.COM}: {e}")

    def read_port(self):
        while self.running:
            try:
                if self.port and self.port.in_waiting:
                    data = self.port.read(self.port.in_waiting)
                    self.scope.byte_count += len(data)

                    for byte in data:
                        dato = int(byte)
                        self.data = (c_double * 3)(*[0, 0, dato])
                        self.scope.update_oscilloscope(self.data)
            except Exception as e:
                print(f"Error en read_port(): {e}")

    def disconnectUART(self):    
        self.running = False    
        if self.port and self.port.is_open:
            try:
                if self.running and self.serial_thread and self.serial_thread.is_alive():
                    self.serial_thread.join()  # Esperar a que el hilo de lectura termine

                self.port.close()
                print(f"Puerto {self.COM} cerrado correctamente.")
            except Exception as e:
                print(f"Error al cerrar el puerto {self.COM}: {e}")
        else:
            print("No hay puerto COM abierto.")

