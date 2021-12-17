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

#ifndef JOINTANGLESDATAGRAM_H
#define JOINTANGLESDATAGRAM_H

#include <xsens_streaming/datagram.h>

class JointAnglesDatagram : public Datagram
{
public: 
  struct Joint
  {
    int32_t parent;
    int32_t child;
    float rotation[3];
  };

public:
  JointAnglesDatagram();
  virtual ~JointAnglesDatagram();
  virtual void printData() const override;

  inline const std::vector<Joint> & data() const
  {
    return m_data;
  }

protected:
  virtual void deserializeData(Streamer & inputStreamer) override;

private:

  std::vector<Joint> m_data;

  const std::vector<std::string> joint_names = {
    "jL5S1", 
    "jL4L3", 
    "jL1T12",
    "jT9T8", 
    "jT1C7",
    "jC1Head", 
    "jT4RightShoulder",
    "jRightShoulder",
    "jRightElbow",
    "jRightWrist",
    "jT4LeftShoulder",
    "jLeftShoulder",
    "jLeftElbow",
    "jLeftWrist",
    "jRightHip",
    "jRightKnee",
    "jRightAnkle",
    "jRightBallFoot",
    "jLeftHip",
    "jLeftKnee",
    "jLeftAnkle",
    "jLeftBallFoot",
    "T8_Head", 
    "T8_LeftUpperArm",
    "T8_RightUpperArm", 
    "Pelvis_T8",
    "Vertical_Pelvis",
    "Vertical_T8",
  };
};

#endif
