from libcpp.string cimport string

cdef extern from "<xsens_streaming/udpserver.h>":
	cdef cppclass UdpServer:
		UdpServer(const string&, unsigned int port)
		UdpServer(const string&)
		UdpServer(unsigned int port)
		UdpServer()

		
