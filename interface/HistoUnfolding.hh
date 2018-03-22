#ifndef _HISTOUNFOLDING_HH_
#define _HISTOUNFOLDING_HH_
#include "SampleDescriptor.hh"
#include "TH2F.h"
class HistoUnfolding: public SampleDescriptor
{
public:
  HistoUnfolding();
  HistoUnfolding(TH2F *, SampleDescriptor *);
  HistoUnfolding(const char* name, const char* title, Int_t nbinsx, const Float_t* xbins, Int_t nbinsy, const Float_t* ybins, SampleDescriptor *);
  HistoUnfolding(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup, SampleDescriptor *);
  void FillFromTree();
  int size;
  TH2F * _th2; ///< [size];
  ~HistoUnfolding();
  ClassDef(HistoUnfolding, 1)
};

#endif
