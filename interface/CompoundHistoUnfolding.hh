#ifndef _CompoundHistoUnfolding_hh_
#define _CompoundHistoUnfolding_hh_
#include "HistoUnfolding.hh"
#include "CompoundHisto.hh"
#include "TH2F.h"
#include <vector>
#include <map>
#include "SampleDescriptor.hh"
#include "Definitions.hh"
#include "TString.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"

#include "TPad.h"
#include "TNamed.h"
using namespace std;


class CompoundHistoUnfolding: public TNamed
{
  void CreateDataGraph();
  map<TString, HistoUnfolding *> _mbackgroundMC;
public:
  CompoundHistoUnfolding();
  CompoundHistoUnfolding(const char* name, const char* title, Int_t nbinsx, const Float_t* xbins, Int_t nbinsy, const Float_t* ybins);
  CompoundHistoUnfolding(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup);
  char      _title[64]; 
  char      _folder[128];
  void SetFolder(const char *);
  const bool      _isRegular;
  const Int_t     _nbinsx;
  const Double_t  _xlow;
  const Double_t  _xup;
  const Int_t     _nbinsy;
  const Double_t  _ylow;
  const Double_t  _yup;
  const int       _nentriesx;
  const Float_t * _xbins; ///< [_nentriesx]
  const int       _nentriesy;
  const Float_t * _ybins; ///< [_nentriesy]
  float     _orig_bin_width;
  vector<HistoUnfolding *> _vdatahistos;
  
  vector<HistoUnfolding *> _vsignalhistos;
  vector<HistoUnfolding *> _vbackgroundhistos;
  vector<HistoUnfolding *> _vsyshistos;
  int size_IN;
  int size_RECO_IN;
  int size_RECO_OUT;

  int size_GEN_IN;
  int size_GEN_OUT;
  int _size_sf;
  float * _sf; ///< [size_sf]
  TGraphErrors * _datagr; ///< [size_RECO_IN]
  TH2F * _datath2; ///< [size_IN]
  HistoUnfolding * _signalh; ///< [size_IN]
  TH2F * _backgroundth2; ///< [size_IN]
  TH2F * _datambackgroundth2; ///< [size_IN]
  Histo * _signalhunfolded; ///< [size]
  Histo * _signalhfoldedback; ///< [size]
  TH2F * _totalMCth2; ///< [size_IN]
  TH1F * _totalMCRECO_OUT; ///< [size_OUT]
  TH1F * _totalMCGEN_OUT; //< [size_OUT]
  TH1F * _totalMCUncth1RECO_IN; ///< [size_RECO_IN]
  TH1F * _totalMCUncShapeth1RECO_IN; ///< [size_RECO_IN]
  TH1F * _totalMCUncth1RECO_OUT; ///< [size_RECO_OUT]
  TH1F * _totalMCUncShapeth1RECO_OUT; ///< [size_RECO_OUT]


  TH1F * _totalMCUncth1GEN_IN; ///< [size_GEN_IN]
  TH1F * _totalMCUncShapeth1GEN_IN; ///< [size_GEN_IN]
  TH1F * _totalMCUncth1GEN_OUT; ///< [size_GEN_OUT]
  TH1F * _totalMCUncShapeth1GEN_OUT; ///< [size_GEN_OUT]


  TGraphAsymmErrors * _ratiogr; ///< [size_RECO_IN]
  TH1F * _ratioframe; ///< [size_RECO_IN]
  TH1F * _ratioframeshape; ///< [size_RECO_IN]
  void FormatHistograms();
  void AggregateBackgroundMCs();

  TH1F * GetDataHistogram();
  TH1F * GetSignalHistogram();
  TH1F * GetBackgroundHistogram();
  void ScaleFactor();
  void ApplyScaleFactor(TH1 *);
  void LoadHistos(const char *, SysTypeCode_t = NOMINAL);
  void FillHistos(const char *, SysTypeCode_t = NOMINAL);
  void AddHisto(HistoUnfolding *);
  //  void LoadSys(const char *);
  void CreateMCTotal();
  void CreateBackgroundTotal();
  void CreateDataMinusBackground();
  void NormaliseToBinWidth(TH1 *h);

  void CreateTotalMCUnc(RecoLevelCode_t = RECO, ResultLevelCode_t = IN);
  void CreateTotalMCUncShape(RecoLevelCode_t = RECO, ResultLevelCode_t = IN);
  THStack * CreateTHStack(RecoLevelCode_t = RECO, ResultLevelCode_t = IN);
  void PlotDataMC();
  void Do();
  void Process();
  void unfold();
  TPad * CreateRatioGraph(RecoLevelCode_t = RECO, ResultLevelCode_t = IN);
  TPad * CreateMainPlot(RecoLevelCode_t = RECO, ResultLevelCode_t = IN);
  TCanvas * CreateCombinedPlot(RecoLevelCode_t = RECO, ResultLevelCode_t = IN);
  float * splitForMinSigmaM(unsigned long & size, float factor);
  static float * split(float * input, unsigned long size);
  TString CreateName(const char *);
  TString CreateTitle(const char *);

  ClassDef(CompoundHistoUnfolding, 1);
};

#endif

