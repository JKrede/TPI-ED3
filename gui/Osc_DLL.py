# Module:  Osc_DLL
import ctypes
import os
class OscDLL(object):
    """ Wrappr for Micheal Osc_DLL for usage from Python """
    def __init__(self):

        dll_path = os.path.abspath("Osc_DLL64.dll")  # Path absoluto de la dll.
        x = ctypes.CDLL(dll_path)

        #   int (__cdecl * AtOpenLib) (int Prm);
        x.AtOpenLib.restype =  ctypes.c_int
        x.AtOpenLib.argtypes=[ ctypes.c_int]

        # int (__cdecl * ScopeCreate) (int Prm, char * P_IniName,  char * P_IniSuffix);
        x.ScopeCreate.restype =  ctypes.c_int
        x.ScopeCreate.argtypes=[ ctypes.c_int, ctypes.c_wchar_p, ctypes.c_wchar_p]

        # int (__cdecl * ScopeShow) (int ScopeHandle);
        x.ScopeShow.restype =  ctypes.c_int
        x.ScopeShow.argtypes=[ ctypes.c_int]

        # int (__cdecl * ShowNext) (int ScopeHandle, double * PArrDbl);
        x.ShowNext.restype =  ctypes.c_int
        x.ShowNext.argtypes=[ ctypes.c_int, ctypes.c_void_p]

        # int (__cdecl * ScopeDestroy) (int ScopeHandle);
        x.ScopeDestroy.restype =  ctypes.c_int
        x.ScopeDestroy.argtypes=[ ctypes.c_int]

        #int (__cdecl * ScopeHide) (int ScopeHandle);
        x.ScopeHide.restype =  ctypes.c_int
        x.ScopeHide.argtypes=[ ctypes.c_int]

        self._hllDll = x
        print(self._hllDll)
        x.AtOpenLib(1)

        #return super().__init__(*args, **kwargs)

    def ScopeCreate(self,P_IniName,P_IniSuffix):
        """ Crea la instancia del osciloscopio """
        ScopeHandle = self._hllDll.ScopeCreate(ctypes.c_int(0),ctypes.create_unicode_buffer(P_IniName),ctypes.create_unicode_buffer(P_IniSuffix))
        return ScopeHandle

    def ScopeShow(self,ScopeHandle):
        """ Muestra la instancia del osciloscopia """
        self._hllDll.ScopeShow(ctypes.c_int(ScopeHandle))

    def ShowNext(self,ScopeHandle,rgb):
        """ Actualiza el osciloscopio """
        self._hllDll.ShowNext(ctypes.c_int(ScopeHandle), ctypes.byref((ctypes.c_double*3)(rgb[0],rgb[1],rgb[2])) )

    def ScopeHide(self,ScopeHandle):
        """ Oculta la instancia del osciloscopia """
        self._hllDll.ScopeHide(ctypes.c_int(ScopeHandle))


    def ScopeDestroy(self,ScopeHandle):
        """ Destruye la instancia del osciloscopia """
        self._hllDll.ScopeDestroy(ctypes.c_int(ScopeHandle))
