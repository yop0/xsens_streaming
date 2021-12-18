# distutils: language = c++

cimport xsens_streaming.c_xsens_streaming as c_xsens_streaming

cdef class UdpServer(object):
    def __cinit__(self, address: str = "localhost", port: int = 9763):
        self.impl = new c_xsens_streaming.UdpServer(address, port)

    def __dealloc__(self): 
        del self.impl

    def startThread(self): 
        self.impl.startThread()

    def stopThread(self):
        self.impl.stopThread() 

    def readMessages(self):
        self.impl.readMessages()

    def printDatagrams(self, enable_print: bool):
        self.impl.printDatagrams(enable_print)

    def quaternions(self):
        return self.impl.quaternions()

    def jointAngles(self): 
        return self.impl.jointAngles()

