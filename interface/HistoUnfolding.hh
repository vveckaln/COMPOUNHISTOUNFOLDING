#ifndef _HISTOUNFOLDING_HH_
#define _HISTOUNFOLDING_HH_
#include "SampleDescriptor.hh"
#include "TH1F.h"
#include "TH2F.h"
class HistoUnfolding: public SampleDescriptor
{
public:
  HistoUnfolding();
  HistoUnfolding(SampleDescriptor *);
  virtual TH1F * Project(RecoLevelCode_t, const char * = "") = 0;
  virtual TH2F * GetTH2() = 0;
  virtual TH1 * GetTH1(RecoLevelCode_t) = 0;
  virtual TH2F *& GetTH2Ref() = 0;
  virtual TH1 *& GetTH1Ref(RecoLevelCode_t) = 0;
  virtual void lsbins(RecoLevelCode_t) = 0;
  virtual ~HistoUnfolding();
  virtual const char * WhoAmI() = 0;
  ClassDef(HistoUnfolding, 1)
};

#endif
