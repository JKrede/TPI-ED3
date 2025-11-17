import sys, serial.tools.list_ports
from ui import Ui_MainWindow
from PyQt5.QtWidgets import  QMainWindow, QApplication  # Clases para widgets y diseños en PyQt5
from scope_window import ScopeHandlerClass
from uart_connection import UARTConnector

class MainApp(QMainWindow, Ui_MainWindow):
    def __init__(self, baudrate):
        self.baudrate = baudrate
        self.UART_flag = False
        self.Ethernet_flag = False

        super().__init__()
        self.setupUi(self) 

        self.scope = ScopeHandlerClass(
        widget_name=self.ScopeWidget,
        label_frequency=self.label_frequency,
        label_scale=self.label_scale,
        label_fft=self.label_fft
        )
        self.scope.init_oscilloscope()
        self.get_available_ports()

    def connectUARTpushButton_clicked(self):
        self.UART_flag = True
        COM = self.portsList.currentText().split(" - ")[0]

        self.uart = UARTConnector(baudrate=self.baudrate, COM=COM, scope=self.scope)
        self.uart.connectUARTpushButton_clicked()

        self.scope.start_frequency_calc()
        self.scope.start_scale_calc()
        self.scope.start_fft_calc()
    
    def connectEthernetPushButton_clicked(self):
        self.Ethernet_flag = True
        pass
        
    def disconnectPushButton_clicked(self):
        if self.UART_flag == True:
            self.uart.disconnectUART()
        elif self.Ethernet_flag == True:
            pass


    def get_available_ports(self):
        self.portsList.clear() 
        ports = serial.tools.list_ports.comports()
        for port in ports:
            self.portsList.addItem(f"{port.device} - {port.description}")
        if not ports:
            self.portsList.addItem("No se encontraron puertos disponibles")




    



if __name__ == "__main__":
    app = QApplication(sys.argv)
    main_window = MainApp(baudrate=115200)
    main_window.show()
    sys.exit(app.exec_())
