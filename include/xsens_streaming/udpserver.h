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

#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <atomic>
#include <mutex>
#include <thread>
#include <xsens/xssocket.h>
#include <xsens/xsthread.h>
#include <xsens_streaming/angularsegmentkinematicsdatagram.h>
#include <xsens_streaming/centerofmassdatagram.h>
#include <xsens_streaming/eulerdatagram.h>
#include <xsens_streaming/jointanglesdatagram.h>
#include <xsens_streaming/linearsegmentkinematicsdatagram.h>
#include <xsens_streaming/parsermanager.h>
#include <xsens_streaming/positiondatagram.h>
#include <xsens_streaming/quaterniondatagram.h>
#include <xsens_streaming/scaledatagram.h>
#include <xsens_streaming/streamer.h>
#include <xsens_streaming/timecodedatagram.h>
#include <xsens_streaming/trackerkinematicsdatagram.h>

struct Datagram;

class UdpServer
{
public:
  UdpServer(XsString address = "localhost", uint16_t port = 9763);
  ~UdpServer();

  void readMessages();
  void startThread();
  void stopThread();

  std::vector<QuaternionDatagram::Kinematics> quaternions() const
  {
    std::lock_guard<std::mutex> lock(quaternionMutex_);
    return quaternions_;
  }

  std::vector<JointAnglesDatagram::Joint> jointAngles() const
  {
    std::lock_guard<std::mutex> lock(jointAnglesMutex_);
    return jointAngles_;
  }

  std::vector<PositionDatagram::VirtualMarkerSet> virtualMarkerPositions() const
  {
    std::lock_guard<std::mutex> lock(virtualMarkerPositionMutex_);
    return virtualMarkerPositions_;
  }

  std::vector<LinearSegmentKinematicsDatagram::Kinematics> linearSegmentKinematics() const
  {
    std::lock_guard<std::mutex> lock(linearSegmentKinematicsMutex_);
    return linearSegmentKinematics_;
  }

  std::vector<EulerDatagram::Kinematics> euler() const
  {
    std::lock_guard<std::mutex> lock(eulerMutex_);
    return euler_;
  }

  std::vector<AngularSegmentKinematicsDatagram::Kinematics> angularSegmentKinematics() const
  {
    std::lock_guard<std::mutex> lock(angularSegmentKinematicsMutex_);
    return angularSegmentKinematics_;
  }

  std::vector<ScaleDatagram::PointDefinition> pointDefinition() const
  {
    std::lock_guard<std::mutex> lock(pointDefinitionMutex_);
    return pointDefinition_;
  }

  std::vector<ScaleDatagram::NullPoseDefinition> nullPoseDefinition() const
  {
    std::lock_guard<std::mutex> lock(nullPoseDefinitionMutex_);
    return nullPoseDefinition_;
  }

  std::vector<TrackerKinematicsDatagram::Kinematics> trackerData() const
  {
    std::lock_guard<std::mutex> lock(trackerDataMutex_);
    return trackerData_;
  }

  TimeCodeDatagram::TimeCode timeCode() const
  {
    std::lock_guard<std::mutex> lock(timeCodeMutex_);
    return timeCode_;
  }

  CenterOfMassDatagram::Kinematics comData() const
  {
    std::lock_guard<std::mutex> lock(comDataMutex_);
    return comData_;
  }

  void printDatagrams(bool print)
  {
    printDatagrams_ = print;
  }

private:
  std::thread m_th;

  std::vector<QuaternionDatagram::Kinematics> quaternions_;
  mutable std::mutex quaternionMutex_;

  std::vector<JointAnglesDatagram::Joint> jointAngles_;
  mutable std::mutex jointAnglesMutex_;

  std::vector<PositionDatagram::VirtualMarkerSet> virtualMarkerPositions_;
  mutable std::mutex virtualMarkerPositionMutex_;

  std::vector<LinearSegmentKinematicsDatagram::Kinematics> linearSegmentKinematics_;
  mutable std::mutex linearSegmentKinematicsMutex_;

  std::vector<EulerDatagram::Kinematics> euler_;
  mutable std::mutex eulerMutex_;

  std::vector<AngularSegmentKinematicsDatagram::Kinematics> angularSegmentKinematics_;
  mutable std::mutex angularSegmentKinematicsMutex_;

  std::vector<ScaleDatagram::PointDefinition> pointDefinition_;
  mutable std::mutex pointDefinitionMutex_;

  std::vector<ScaleDatagram::NullPoseDefinition> nullPoseDefinition_;
  mutable std::mutex nullPoseDefinitionMutex_;

  std::vector<TrackerKinematicsDatagram::Kinematics> trackerData_;
  mutable std::mutex trackerDataMutex_;

  TimeCodeDatagram::TimeCode timeCode_;
  mutable std::mutex timeCodeMutex_;

  CenterOfMassDatagram::Kinematics comData_;
  mutable std::mutex comDataMutex_;

  std::unique_ptr<XsSocket> m_socket;
  uint16_t m_port;
  XsString m_hostName;

  std::unique_ptr<ParserManager> m_parserManager;
  bool printDatagrams_ = false;

  volatile std::atomic_bool m_started, m_stopping;
};

#endif
