from libcpp.string cimport string
from libcpp cimport bool as cppbool
from libcpp.vector cimport vector


cdef extern from "<xsens_streaming/jointanglesdatagram.h>" namespace "JointAnglesDatagram": 
	cdef struct Joint: 
		unsigned int parent
		unsigned int child 
		float[3] rotation

cdef extern from "<xsens_streaming/quaterniondatagram.h>" namespace "QuaternionDatagram":
	cdef struct Kinematics:
		int segmentId
		float[3] position
		float[4] orientation

cdef extern from "<xsens_streaming/udpserver.h>":
	cdef cppclass UdpServer:
		UdpServer(const string&, unsigned int port)
		UdpServer(const string&)
		UdpServer(unsigned int port)
		UdpServer()

		void startThread()
		void stopThread()
		void readMessages() 
		void printDatagrams(cppbool enable_print)

		vector[Joint] jointAngles()
		vector[Kinematics] quaternions()
		