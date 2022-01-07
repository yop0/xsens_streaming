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

#include <sstream>
#include <xsens_streaming/udpserver.h>

UdpServer::UdpServer(XsString address, uint16_t port)
{
  port_ = port;
  hostName_ = address;

  parserManager_.reset(new ParserManager());
  socket_.reset(new XsSocket(IpProtocol::IP_UDP, NetworkLayerProtocol::NLP_IPV4));

  XsResultValue res = socket_->bind(hostName_, port_);

  if(res == XRV_OK)
  {
    startThread();
  }
  else
  {
    std::stringstream ss;
    ss << "Failed to bind UDP socket (host: " << hostName_ << ", port: " << port_ << ")";
    throw std::runtime_error(ss.str());
  }
}

UdpServer::~UdpServer()
{
  stopThread();
}

void UdpServer::readMessages()
{
  XsByteArray buffer;

  std::cout << "Waiting to receive packets from the client \"" << hostName_ << "\" on port \"" << port_ << "\" ..."
            << std::endl
            << std::endl;

  while(!stopping_)
  {
    socket_->read(buffer);
    if(buffer.size() > 0)
    {
      auto datagram = parserManager_->readDatagram(buffer, printDatagrams_);
      switch(datagram->messageType())
      {
        case StreamingProtocol::SPPoseQuaternion:
        {
          auto & quaternionDatagram = dynamic_cast<QuaternionDatagram &>(*datagram);
          std::lock_guard<std::mutex> lock(quaternionMutex_);
          quaternions_ = quaternionDatagram.data();
          isQuaternionAvail_ = true;
          quaternionCV_.notify_all();
          break;
        }
        case StreamingProtocol::SPJointAngles:
        {
          auto & jointAnglesDatagram = dynamic_cast<JointAnglesDatagram &>(*datagram);
          std::lock_guard<std::mutex> lock(jointAnglesMutex_);
          jointAngles_ = jointAnglesDatagram.data();
          isJointAnglesAvail_ = true;
          jointAnglesCV_.notify_all();
          break;
        }
        case StreamingProtocol::SPPoseEuler:
        {
          auto & eulerDatagram = dynamic_cast<EulerDatagram &>(*datagram);
          std::lock_guard<std::mutex> lock(eulerMutex_);
          euler_ = eulerDatagram.data();
          isEulerAvail_ = true;
          eulerCV_.notify_all();
          break;
        }
        case StreamingProtocol::SPPosePositions:
        {
          auto & positionDatagram = dynamic_cast<PositionDatagram &>(*datagram);
          std::lock_guard<std::mutex> lock(virtualMarkerPositionMutex_);
          virtualMarkerPositions_ = positionDatagram.data();
          isVirtualMarkerPositionAvail_ = true;
          eulerCV_.notify_all();
          break;
        }
        case StreamingProtocol::SPLinearSegmentKinematics:
        {
          auto & linearSegmentKinematicsDatagram = dynamic_cast<LinearSegmentKinematicsDatagram &>(*datagram);
          std::lock_guard<std::mutex> lock(linearSegmentKinematicsMutex_);
          linearSegmentKinematics_ = linearSegmentKinematicsDatagram.data();
          isLinearSegmentKinematicAvail_ = true;
          linearSegmentKinematicsCV_.notify_all();
          break;
        }
        case StreamingProtocol::SPAngularSegmentKinematics:
        {
          auto & angularSegmentKinematicsDatagram = dynamic_cast<AngularSegmentKinematicsDatagram &>(*datagram);
          std::lock_guard<std::mutex> lock(angularSegmentKinematicsMutex_);
          angularSegmentKinematics_ = angularSegmentKinematicsDatagram.data();
          isAngularSegmentKinematicsAvail_ = true;
          angularSegmentKinematicsCV_.notify_all();
          break;
        }
        case StreamingProtocol::SPTrackerKinematics:
        {
          auto & trackerKinematicsDatagram = dynamic_cast<TrackerKinematicsDatagram &>(*datagram);
          std::lock_guard<std::mutex> lock(trackerDataMutex_);
          trackerData_ = trackerKinematicsDatagram.data();
          isTrackerDataAvail_ = true;
          trackerDataCV_.notify_all();
          break;
        }
        case StreamingProtocol::SPMetaScaling:
        {
          auto & scaleDatagram = dynamic_cast<ScaleDatagram &>(*datagram);
          std::lock_guard<std::mutex> lock(dataDefinitionMutex_);
          pointDefinition_ = scaleDatagram.pointDefinition();
          nullPoseDefinition_ = scaleDatagram.nullPoseDefinition();
          isPointDefinitionAvail_ = true;
          isNullPoseDefinitionAvail_ = true;
          dataDefinitionCV_.notify_all();
          break;
        }
        case StreamingProtocol::SPTimeCode:
        {
          auto & timeCodeDatagram = dynamic_cast<TimeCodeDatagram &>(*datagram);
          std::lock_guard<std::mutex> lock(timeCodeMutex_);
          timeCode_ = timeCodeDatagram.data();
          isTimeCodeAvail_ = true;
          timeCodeCV_.notify_all();
          break;
        }
        case StreamingProtocol::SPCenterOfMass:
        {
          auto & centerOfMassDatagram = dynamic_cast<CenterOfMassDatagram &>(*datagram);
          std::lock_guard<std::mutex> lock(comDataMutex_);
          comData_ = centerOfMassDatagram.data();
          isComDataAvail_ = true;
          comDataCV_.notify_all();
          break;
        }
      }
    }

    buffer.clear();
    XsTime::msleep(1);
  }

  std::cout << "Stopping receiving packets..." << std::endl << std::endl;

  stopping_ = false;
  started_ = false;
}

void UdpServer::startThread()
{
  if(started_) return;

  started_ = true;
  stopping_ = false;
  th_ = std::thread([this]() { this->readMessages(); });
}

void UdpServer::stopThread()
{
  if(!started_) return;
  stopping_ = true;
  th_.join();
}
