#include "DelaySum.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include <math.h>

DelaySum::DelaySum(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
    GetParam(kAngle)->InitDouble("Angle", 0., -90., 90., 0.01, "º");
    GetParam(kMicDist)->InitDouble("Distance", 0.2, 0., 1., 0.01, "m");
    GetParam(kOpt)->InitInt("Mode", 0, 0, 1);
    GetParam(kFreq)->InitDouble("Frequency", 3000., 1000., 20000., 0.1, "Hz");

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_WHITE);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
    
    // Add title
    pGraphics->AttachControl(new ITextControl(b.GetMidVPadded(50).GetVShifted(-70), "Delay Sum Beamformer", IText(40)));

    int idx = 0;
    // Add knobs
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(20).GetHShifted(-150), kAngle));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(20).GetHShifted(-50), kMicDist));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(20).GetHShifted(50), kFreq));
    
    // Add SVG Graphic
    const ISVG svg = pGraphics->LoadSVG(GRAPHIC_FN);
    const IRECT bnds = pGraphics->GetBounds();
    pGraphics->AttachControl(new ISVGControl(bnds.GetScaled(0.6).GetVShifted(50).GetHShifted(400), svg));
    
    // Add ITD and ILD options
    pGraphics->AttachControl(new IVRadioButtonControl(IRECT(30.,70.,80.,170.), [pGraphics](IControl* pCaller) {
      SplashClickActionFunc(pCaller);
      int idx = pCaller->As<IVRadioButtonControl>()->GetSelectedIdx();
      switch (idx) {
        case 0:
              pGraphics->ForStandardControlsFunc([pCaller](IControl& control) {
                  if(&control != pCaller && control.GetParamIdx() == kFreq) {
                      control.SetDisabled(true);
                  }
              });
          break;
        case 1:
              pGraphics->ForStandardControlsFunc([pCaller](IControl& control) {
                  if(&control != pCaller && control.GetParamIdx() == kFreq) {
                      control.SetDisabled(false);
                  }
              });
          break;
      }
    }, {"ITD", "ILD"}, "Mode", DEFAULT_STYLE, EVShape::Ellipse, EDirection::Vertical, 10.f), kOpt);
  };
#endif
}

#if IPLUG_DSP
DelaySum::~DelaySum() {
  if(mpBuffer1) {
    delete [] mpBuffer1;
  }
  if(mpBuffer2) {
    delete [] mpBuffer2;
  }
}

void DelaySum::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  sample* in1 = inputs[0];
  sample* in2 = inputs[1];
  sample* out1 = outputs[0];
  sample* out2 = outputs[1];
    
  for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2)
  {
    // Read Delayed output
    sample yn1 = mpBuffer1[mReadIndex1];
    sample yn2 = mpBuffer1[mReadIndex2];
        
    // Feed input if delay is zero
    if (mDelaySam1 == 0) {
      yn1 = *in1;
    }
    if (mDelaySam2 == 0) {
      yn2 = *in2;
    }
        
    // Write to our delay buffer
    mpBuffer1[mWriteIndex1] = *in1;
    mpBuffer1[mWriteIndex2] = *in2;
        
    // Only need delayed output
    *out1 = (yn1+yn2)*0.5;
    *out2 = *out1;
        
    //increment the write index, wrapping if it goes out of bounds.
    ++mWriteIndex1;
    if(mWriteIndex1 >= mBufferSize1) {
      mWriteIndex1 = 0;
    }
    ++mWriteIndex2;
    if(mWriteIndex2 >= mBufferSize2) {
      mWriteIndex2 = 0;
    }
        
    //same with the read index
    ++mReadIndex1;
    if(mReadIndex1 >= mBufferSize1) {
      mReadIndex1 = 0;
    }
    ++mReadIndex2;
    if(mReadIndex2 >= mBufferSize2) {
      mReadIndex2 = 0;
    }
  }
}

void DelaySum::initBuffer() {
  // Initialize buffers with zeros
    
  if(mpBuffer1) {
    memset(mpBuffer1, 0, mBufferSize1*sizeof(sample));
  }
  mWriteIndex1 = 0;
  mReadIndex1 = 0;
    
  if(mpBuffer2) {
    memset(mpBuffer2, 0, mBufferSize2*sizeof(sample));
  }
  mWriteIndex2 = 0;
  mReadIndex2 = 0;
}

void DelaySum::OnReset() {
  TRACE;
    
  // Set Maximum Delay time
  mBufferSize1 = 2 * GetSampleRate();
  mBufferSize2 = 2 * GetSampleRate();
    
  // Delete buffer if already there
  if(mpBuffer1) {
    delete [] mpBuffer1;
  }
  if(mpBuffer2) {
    delete [] mpBuffer2;
  }
    
  // Create new buffer in heap
  mpBuffer1 = new double[mBufferSize1];
  mpBuffer2 = new double[mBufferSize2];
    
  initBuffer();
  getDelay();
}

void DelaySum::getDelay() {
  // Get value from GUI
  mAngle = GetParam(kAngle)->Value() * M_PI / 180.;
  mMicDist = GetParam(kMicDist)->Value();
  mOpt = GetParam(kOpt)->Int();
  mFreq = GetParam(kFreq)->Value();

  // Set delays according to interaural time delay (ITD)
  if (mAngle == 0) {
    mDelaySam1 = 0.;
    mDelaySam1 = 0.;
  }
  else if(mAngle > 0) {
    if (mOpt == 0) {
      mDelaySam1 = (mMicDist/sound_vel)*(mAngle+sin(mAngle))*GetSampleRate();
    }
    else if (mOpt == 1) {
      mDelaySam1 = 0.18 * sqrt(mFreq*sin(mAngle)) * GetSampleRate();
    }
      
    mDelaySam2 = 0.;
  }
  else if(mAngle < 0) {
    mDelaySam1 = 0.;
      
    if (mOpt == 0) {
      mDelaySam2 = (mMicDist/sound_vel)*(abs(mAngle)+sin(abs(mAngle)))*GetSampleRate();
    }
    else if (mOpt == 1) {
      mDelaySam2 = 0.18 * sqrt(mFreq*sin(abs(mAngle))) * GetSampleRate();
    }
  }
    
  // Set ReadIndex (Wrap if less than 0)
  mReadIndex1 = mWriteIndex1 - floor(mDelaySam1);
  if(mReadIndex1 < 0) {
    mReadIndex1 += mBufferSize1;
  }
    
  mReadIndex2 = mWriteIndex2 - floor(mDelaySam2);
  if(mReadIndex2 < 0) {
    mReadIndex2 += mBufferSize2;
  }
}

void DelaySum::OnParamChange(int paramIdx) {
  getDelay();
}
#endif
