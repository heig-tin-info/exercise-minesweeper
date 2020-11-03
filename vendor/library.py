from subprocess import Popen, PIPE
from ctypes import CDLL
from ctypes import cdll, c_bool

class Library:
    def __init__(self, filename):
        self.filename = filename
        self.library = cdll.LoadLibrary(filename)

        for func in self._list_functions():
            if (func.startswith('_')): 
                continue
            setattr(self, func, getattr(self.library, func))

    def _list_functions(self):
        out = Popen(
            args=f"nm {self.filename}", 
            shell=True, 
            stdout=PIPE
        ).communicate()[0].decode("utf-8")

        attrs = [
            i.split(" ")[-1].replace("\r", "") 
            for i in out.split("\n") if " T " in i
        ]

        return [i for i in attrs if hasattr(CDLL(self.filename), i)]    

