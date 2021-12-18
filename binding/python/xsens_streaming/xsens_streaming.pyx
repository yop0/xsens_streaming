# distutils: language = c++

cimport xsens_streaming.c_xsens_streaming as c_xsens_streaming

cdef class UdpServer(object):
    def __cinit__(self, *args):
        self.impl = new c_xsens_streaming.UdpServer(args[0], args[1])

    def __dealloc__(self): 
        del self.impl