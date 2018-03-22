#ifndef _CompoundHisto_hh_
#define _CompoundHisto_hh_
#include "TH1.h"
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


class Histo: public SampleDescriptor
{
public:
  Histo();
  Histo(TH1F *, SampleDescriptor);
  Histo(TH1F *, SampleDescriptor *);
  int size;
  TH1F * _th1; ///< [size];
  ~Histo();
  ClassDef(Histo, 1)
};


class CompoundHisto: public TNamed
{
  void AggregateBackgroundMCs();
  void FormatHistograms();
  void CreateDataGraph();
  map<TString, Histo *> _mbackgroundMC;
  int size;
public:
  CompoundHisto();
  vector<Histo *> _vdatahistos;
  
  vector<Histo *> _vsignalhistos;
  vector<Histo *> _vbackgroundhistos;
  vector<Histo *> _vsyshistos;

  TGraphErrors * _datagr; ///< [size];
  TH1F * _datath1; ///< [size];
  Histo * _signalh; ///< [size];
  TH1F * _backgroundth1; ///< [size];
  TH1F * _totalMCth1; ///< [size];
  TH1F * _totalMCUncth1; ///< [size];
  TH1F * _totalMCUncShapeth1; ///< [size];
  TGraphAsymmErrors * _ratiogr; ///< [size];
  TH1F * _ratioframe; ///< [size];
  TH1F * _ratioframeshape; ///< [size];
  TH1F * GetDataHistogram();
  TH1F * GetSignalHistogram();
  TH1F * GetBackgroundHistogram();
  void LoadHistos(const char *, SysTypeCode_t = NOMINAL);
  void FillHistos(const char *, SysTypeCode_t = NOMINAL);
  void AddHisto(Histo *);
  //  void LoadSys(const char *);
  TH1F * NewMCTotal(const char * = "MCtotal");
  TH1F * NewDataMinusBackground();
  void CreateTotalMCUnc();
  void CreateTotalMCUncShape();
  THStack * CreateTHStack();
  void PlotDataMC();
  void Do();
  void Process();
  TPad * CreateRatioGraph();
  TPad * CreateMainPlot();
  TCanvas * CreateCombinedPlot();
  ClassDef(CompoundHisto, 1);
};

class MinCompoundHisto: public TNamed
{
  int size;
public:
  MinCompoundHisto();
  TH1F *h;///< [size]
  vector<Histo *> _vbackgroundhistos;
  void Do();
  ~MinCompoundHisto();
  ClassDef(MinCompoundHisto, 1);
};



#endif

