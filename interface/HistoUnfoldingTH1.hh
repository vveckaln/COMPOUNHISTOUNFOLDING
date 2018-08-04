#ifndef _HISTOUNFOLDINGTH1_HH_
#define _HISTOUNFOLDINGTH1_HH_
#include "HistoUnfolding.hh"
class HistoUnfoldingTH1: public HistoUnfolding
{
  TH1 * _th1RECO; 
  TH1 * _th1GEN; 
public:
  HistoUnfoldingTH1();
  HistoUnfoldingTH1(SampleDescriptor *);
  HistoUnfoldingTH1(const char* name, const char* title, Int_t nbinsx, const Float_t* xbins, SampleDescriptor *);
  HistoUnfoldingTH1(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, SampleDescriptor *);
  void FillFromTree();
  TH1 *& GetTH1Ref(RecoLevelCode_t);
  TH1 * GetTH1(RecoLevelCode_t);
  TH2F *& GetTH2Ref();
  TH2F * GetTH2();
  TH1F * Project(RecoLevelCode_t, const char * name = "" );
  const char * WhoAmI();
  void lsbins(RecoLevelCode_t);
  ~HistoUnfoldingTH1();
  ClassDef(HistoUnfoldingTH1, 1)
};

#endif
