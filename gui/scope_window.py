import Osc_DLL, ctypes as c, threading, time, numpy as np
from ctypes import wintypes
from PyQt5.QtCore import QObject, pyqtSignal
from PyQt5.QtGui import QWindow
from PyQt5.QtWidgets import QWidget, QVBoxLayout

class ScopeHandlerClass(QObject):
    freq_text = pyqtSignal(str)
    scale_text = pyqtSignal(str)
    fft_text  = pyqtSignal(str)

    def __init__(self, widget_name, label_frequency, label_scale, label_fft, parent=None):
        super().__init__(parent)
        # dentro de __init__
        self.sampling_frequency = 0     
        self.fft_thread = None          
        self.widget_name = widget_name
        self.label_frequency = label_frequency
        self.label_scale = label_scale
        self.label_fft = label_fft
        self.fft_size = 2048
        self.data_buffer = []


        self.osc = Osc_DLL.OscDLL()
        self.scope_handle = None
        self.osc_hwnd = None
        self.byte_count = 0
        self.running = False
        self.sampling_thread = None

        self.freq_text.connect(self.label_frequency.setText)
        self.scale_text.connect(self.label_scale.setText)
        self.fft_text.connect(self.label_fft.setText)


    def init_oscilloscope(self):
        try:
            self.scope_handle = self.osc.ScopeCreate('scope_1.ini', '1')
            self.osc.ScopeShow(self.scope_handle)

            self.osc_hwnd = self.find_oscilloscope_window("Oscilloscope")
            if not self.osc_hwnd:
                raise RuntimeError("No se pudo obtener el HWND del osciloscopio.")

            container = QWindow.fromWinId(self.osc_hwnd)
            embedded_widget = QWidget.createWindowContainer(container, self.widget_name)
            layout = QVBoxLayout(self.widget_name)
            layout.setContentsMargins(0,0,0,0)
            layout.addWidget(embedded_widget)
            print("DLL cargada correctamente")
        except Exception as e:
            print(f"Error al inicializar el osciloscopio: {e}")


    def find_oscilloscope_window(self, window_name):
        """
        Encuentra el identificador de la ventana (HWND) de una ventana por su nombre.
        """
        hwnds = []  # Lista para almacenar los HWND encontrados

        # Función de callback que será llamada para cada ventana abierta
        def callback(hwnd, _):
            # Obtiene el título de la ventana
            length = c.windll.user32.GetWindowTextLengthW(hwnd)  # Longitud del título
            buff = c.create_unicode_buffer(length + 1)  # Buffer para almacenar el título
            c.windll.user32.GetWindowTextW(hwnd, buff, length + 1)  # Obtiene el título
            if window_name in buff.value:  # Si el título contiene el nombre de la ventana buscada
                hwnds.append(hwnd)  # Agrega el HWND a la lista
            return True

        # Enumera todas las ventanas abiertas
        EnumWindows = c.windll.user32.EnumWindows
        EnumWindowsProc = c.WINFUNCTYPE(c.c_bool, wintypes.HWND, wintypes.LPARAM)
        EnumWindows(EnumWindowsProc(callback), 0)  # Llama a la función `callback` para cada ventana

        # Devuelve el primer HWND encontrado o None si no se encontró
        return hwnds[0] if hwnds else None
    
    def update_oscilloscope(self, data):
        try:
            if self.scope_handle:
                self.osc.ShowNext(self.scope_handle, data)
                try:
                    sample = float(data[2])  # tu c_double * 3 = [0, 0, dato]
                    self.data_buffer.append(sample)
                    # recortar a fft_size
                    if len(self.data_buffer) > self.fft_size:
                        self.data_buffer = self.data_buffer[-self.fft_size:]
                except Exception:
                    pass
        except Exception as e:
            print(f"Error al actualizar el osciloscopio: {e}")


    def start_fft_calc(self):
        if self.fft_thread and self.fft_thread.is_alive():
            return
        self.fft_thread = threading.Thread(target=self.calculate_fft, daemon=True)
        self.fft_thread.start()


    def start_scale_calc(self):
        pass

    def start_frequency_calc(self):
        if self.sampling_thread and self.sampling_thread.is_alive():
            return
        self.running = True
        self.sampling_thread = threading.Thread(target=self.calculate_frequency, daemon=True)
        self.sampling_thread.start()

    def calculate_scale(self, horizontal_scale, sampling_frequency):
        """
        Calcula la escala horizontal y selecciona la unidad apropiada.
        """
        if sampling_frequency > 0:
            scale_in_s = horizontal_scale / sampling_frequency
            if scale_in_s < 0.001:
                return round(scale_in_s * 1_000_000), "useg/div"
            elif scale_in_s < 1:
                return round(scale_in_s * 1_000), "mseg/div"
            else:
                return round(scale_in_s), "seg/div"
        else:
            return 0, "N/A"

    def calculate_frequency(self):
        while self.running:
            start_time = time.time()
            time.sleep(1)
            elapsed_time = time.time() - start_time

            if elapsed_time > 0:
                self.sampling_frequency = int(self.byte_count / elapsed_time)
                self.byte_count = 0

                try:
                    self.osc._hllDll.ScopeGetCellSampleSize.argtypes = [c.c_int]
                    self.osc._hllDll.ScopeGetCellSampleSize.restype = c.c_double
                    horizontal_scale = self.osc._hllDll.ScopeGetCellSampleSize(self.scope_handle)
                except Exception as e:
                    print(f"Error al obtener escala horizontal: {e}")
                    horizontal_scale = 0.0

                scale, unit = self.calculate_scale(horizontal_scale, self.sampling_frequency)

                # Emitir a la UI en el hilo principal
                if self.sampling_frequency == 0:
                    self.freq_text.emit("N/A")
                    self.scale_text.emit("N/A")
                else:
                    self.freq_text.emit(f"{self.sampling_frequency} samples/seg")
                    self.scale_text.emit(f"{scale} {unit}")

    def calculate_fft(self):
        while self.running:
            time.sleep(0.1)

            if len(self.data_buffer) < self.fft_size:
                continue

            Fs = self.sampling_frequency
            if Fs <= 0:
                self.fft_text.emit("N/A")
                continue

            data_block = np.array(self.data_buffer[-self.fft_size:], dtype=float)
            data_block -= np.mean(data_block)
            fft_result = np.fft.fft(data_block)
            magnitude = np.abs(fft_result)[:self.fft_size // 2]
            if magnitude.size == 0:
                continue

            peak_index = int(np.argmax(magnitude))
            peak_freq = int(peak_index * Fs / self.fft_size)
            self.fft_text.emit(f"{peak_freq:.0f} Hz")

