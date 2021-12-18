/*! \file
  \section FileCopyright Copyright Notice
  This is free and unencumbered software released into the public domain.

  Anyone is free to copy, modify, publish, use, compile, sell, or
  distribute this software, either in source code form or as a compiled
  binary, for any purpose, commercial or non-commercial, and by any
  means.

  In jurisdictions that recognize copyright laws, the author or authors
  of this software dedicate any and all copyright interest in the
  software to the public domain. We make this dedication for the benefit
  of the public at large and to the detriment of our heirs and
  successors. We intend this dedication to be an overt act of
  relinquishment in perpetuity of all present and future rights to this
  software under copyright law.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
  OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.
*/

#include <xsens_streaming/udpserver.h>

UdpServer::UdpServer(XsString address, uint16_t port) : m_started(false), m_stopping(false)
{
  m_port = port;
  m_hostName = address;

  m_parserManager.reset(new ParserManager());
  m_socket.reset(new XsSocket(IpProtocol::IP_UDP, NetworkLayerProtocol::NLP_IPV4));

  XsResultValue res = m_socket->bind(m_hostName, m_port);

  if(res == XRV_OK)
    startThread();
  else
    std::cout << "Failed to bind UDP socket (host: " << m_hostName << ", port: " << m_port << ")" << std::endl;
}

UdpServer::~UdpServer()
{
  stopThread();
}

void UdpServer::readMessages()
{
  XsByteArray buffer;

  std::cout << "Waiting to receive packets from the client \"" << m_hostName << "\" on port \"" << m_port << "\" ..."
            << std::endl
            << std::endl;

  while(!m_stopping)
  {
    // std::cout << ".";
    int rv = m_socket->read(buffer);
    if(buffer.size() > 0)
    {
      auto datagram = m_parserManager->readDatagram(buffer, printDatagrams_);
      switch(datagram->messageType())
      {
        case StreamingProtocol::SPPoseQuaternion:
        {
          auto & quaternionDatagram = dynamic_cast<QuaternionDatagram &>(*datagram);
          std::lock_guard<std::mutex> lock(quaternionMutex_);
          quaternions_ = quaternionDatagram.data();
          break;
        }
        case StreamingProtocol::SPJointAngles:
        {
          auto & jointAnglesDatagram = dynamic_cast<JointAnglesDatagram &>(*datagram);
          std::lock_guard<std::mutex> lock(jointAnglesMutex_);
          jointAngles_ = jointAnglesDatagram.data();
          break;
        }
        case StreamingProtocol::SPPoseEuler:
        {
          auto & eulerDatagram = dynamic_cast<EulerDatagram &>(*datagram);
          std::lock_guard<std::mutex> lock(eulerMutex_);
          euler_ = eulerDatagram.data();
          break;
        }
        case StreamingProtocol::SPPosePositions:
        {
          auto & positionDatagram = dynamic_cast<PositionDatagram &>(*datagram);
          std::lock_guard<std::mutex> lock(virtualMarkerPositionMutex_);
          virtualMarkerPositions_ = positionDatagram.data();
          break;
        }
        case StreamingProtocol::SPLinearSegmentKinematics:
        {
          auto & linearSegmentKinematicsDatagram = dynamic_cast<LinearSegmentKinematicsDatagram &>(*datagram);
          std::lock_guard<std::mutex> lock(linearSegmentKinematicsMutex_);
          linearSegmentKinematics_ = linearSegmentKinematicsDatagram.data();
          break;
        }
        case StreamingProtocol::SPAngularSegmentKinematics:
        {
          auto & angularSegmentKinematicsDatagram = dynamic_cast<AngularSegmentKinematicsDatagram &>(*datagram);
          std::lock_guard<std::mutex> lock(angularSegmentKinematicsMutex_);
          angularSegmentKinematics_ = angularSegmentKinematicsDatagram.data();
          break;
        }
        case StreamingProtocol::SPTrackerKinematics:
        {
          auto & trackerKinematicsDatagram = dynamic_cast<TrackerKinematicsDatagram &>(*datagram);
          std::lock_guard<std::mutex> lock(trackerDataMutex_);
          trackerData_ = trackerKinematicsDatagram.data();
          break;
        }
        case StreamingProtocol::SPMetaScaling:
        {
          auto & scaleDatagram = dynamic_cast<ScaleDatagram &>(*datagram);
          std::lock_guard<std::mutex> lock1(pointDefinitionMutex_);
          pointDefinition_ = scaleDatagram.pointDefinition();
          std::lock_guard<std::mutex> lock2(nullPoseDefinitionMutex_);
          nullPoseDefinition_ = scaleDatagram.nullPoseDefinition();
          break;
        }
        case StreamingProtocol::SPTimeCode:
        {
          auto & timeCodeDatagram = dynamic_cast<TimeCodeDatagram &>(*datagram);
          std::lock_guard<std::mutex> lock(timeCodeMutex_);
          timeCode_ = timeCodeDatagram.data();
        }
        case StreamingProtocol::SPCenterOfMass:
        {
          auto & centerOfMassDatagram = dynamic_cast<CenterOfMassDatagram &>(*datagram);
          std::lock_guard<std::mutex> lock(comDataMutex_);
          comData_ = centerOfMassDatagram.data();
        }
      }
    }

    buffer.clear();
    XsTime::msleep(1);
  }

  std::cout << "Stopping receiving packets..." << std::endl << std::endl;

  m_stopping = false;
  m_started = false;
}

void UdpServer::startThread()
{
  if(m_started) return;

  m_started = true;
  m_stopping = false;
  m_th = std::thread([this]() { this->readMessages(); });
}

void UdpServer::stopThread()
{
  if(!m_started) return;
  m_stopping = true;
  m_th.join();
}
