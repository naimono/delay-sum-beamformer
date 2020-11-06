#pragma once

#include "IPlug_include_in_plug_hdr.h"

const int kNumPresets = 1;

enum EParams
{
    
  kMicDist,
  kAngle,
  kNumParams

};

using namespace iplug;
using namespace igraphics;

class DelaySum final : public Plugin
{
public:
  DelaySum(const InstanceInfo& info);

#if IPLUG_DSP // http://bit.ly/2S64BDd
public:
  ~DelaySum();
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnReset() override;
  void OnParamChange(int paramIdx) override;
  void getDelay();
  void initBuffer();
    
private:
  double mGain;
  double mAngle = 0.;
  double mMicDist = 0.2;
  double sound_vel = 343.;
    
  double mDelaySam1 = 0.;
  double mDelaySam2 = 0.;

  sample* mpBuffer1 = NULL;
  sample* mpBuffer2 = NULL;
    
  int mReadIndex1 = 0;
  int mReadIndex2 = 0;
    
  int mWriteIndex1 = 0;
  int mWriteIndex2 = 0;
    
  int mBufferSize1 = 0;
  int mBufferSize2 = 0;
#endif
};
