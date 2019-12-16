#ifndef _HISTOUNFOLDINGTH2_HH_
#define _HISTOUNFOLDINGTH2_HH_
#include "HistoUnfolding.hh"
class HistoUnfoldingTH2: public HistoUnfolding
{
  TH2F * _th2; 
public:
  HistoUnfoldingTH2();
  HistoUnfoldingTH2(HistoUnfoldingTH2 *, const char * );
  HistoUnfoldingTH2(const char* name, const char* title, Int_t nbinsx, const Float_t* xbins, Int_t nbinsy, const Float_t* ybins, SampleDescriptor *);
  HistoUnfoldingTH2(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup, SampleDescriptor *);
  TH2F * GetTH2();
  TH2F *& GetTH2Ref();
  TH1 * GetTH1(RecoLevelCode_t);
  TH1 *& GetTH1Ref(RecoLevelCode_t);
  TH1F * Project(RecoLevelCode_t, const char * name = "", const char * XaxisTitle = "", const char * YaxisTitle = "");
  void FillFromTree(const char * sampletag, const char * jettag, const char *chargetag, const char * observable);
  const char * WhoAmI();
  void lsbins(RecoLevelCode_t);
  ~HistoUnfoldingTH2();
  ClassDef(HistoUnfoldingTH2, 1)
};

#endif
