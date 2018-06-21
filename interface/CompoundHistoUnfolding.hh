#ifndef _CompoundHistoUnfolding_hh_
#define _CompoundHistoUnfolding_hh_
#include "HistoUnfoldingTH1.hh"
#include "HistoUnfoldingTH2.hh"
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
#include "TLegend.h"
#include "TPad.h"
#include "TNamed.h"
#include "TMatrixD.h"
using namespace std;

class CompoundHistoUnfolding: public TNamed
{
  void CreateDataGraph(ResultLevelCode_t resultcode);
  map<TString, HistoUnfolding *> aggrbackgroundMC;
  vector<HistoUnfolding *>       _vbackgroundhistos;
  vector<SampleDescriptor *>     _expsyssamples;
  vector<SampleDescriptor *>     _markedsyssamples;
  void CreateListOfExpSysSamples();
  HistoUnfoldingTH2 *            totalbackground;
  char                           _signal_title[64];
  char                           _title[64]; 
  char                           _folder[128];
  const bool                     _isRegular;
  const Int_t                    _nbinsx;
  const Double_t                 _xlow;
  const Double_t                 _xup;
  const Int_t                    _nbinsy;
  const Double_t                 _ylow;
  const Double_t                 _yup;
  const int                      _nentriesx;
  const Float_t *                _xbins; ///< [_nentriesx]
  const int                      _nentriesy;
  const Float_t *                _ybins; ///< [_nentriesy]
  float                          _orig_bin_width;
  unsigned int                   _size_sf;
  float *                        _sf;
public:
  ChargeCode_t                   chargecode;
  unsigned char                  jetcode;
  OptLevelCode_t                 optcode;
  const char                   * observable;
  const char                   * signaltag;
  const char                   * method;
  CompoundHistoUnfolding();
  CompoundHistoUnfolding(const char* name, const char* title, Int_t nbinsx, const Float_t* xbins, Int_t nbinsy, const Float_t* ybins);
  CompoundHistoUnfolding(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup);
  void SetFolder(const char *);
  struct Level: public TObject
  {
    Level();
    vector<HistoUnfolding *> _vdatahistos;
    vector<HistoUnfolding *> _vsignalhistos;
    vector<HistoUnfolding *> _vsyshistos;
    HistoUnfolding * signal;
    HistoUnfolding * data;
    HistoUnfolding * totalMC;
    HistoUnfolding * totalMCUnc;
    HistoUnfolding * totalMCUncShape;
    HistoUnfolding * datambackground;;
    HistoUnfolding *& GetHURef(MOCode_t); 
    HistoUnfolding * GetHU(MOCode_t); 
    vector<HistoUnfolding *> * GetV(MOCode_t);
    void Format();
    TMatrixD * cov;
    struct ProjectionDeco: public TObject
    {
      ProjectionDeco();
      TGraphAsymmErrors * _ratiogr; 
      TGraphAsymmErrors * _grtotalMC;
      TGraphErrors * _datagr;
      TH1F * _ratioframe; 
      TH1F * _ratioframeshape; 
      TLegend * legend;
      void Format();
      ~ProjectionDeco();
      ClassDef(ProjectionDeco, 1);
    } RECODECO, GENDECO;
    ProjectionDeco * GetProjectionDeco(RecoLevelCode_t);
    ~Level();
    ClassDef(Level, 1);
  } INLEVEL, OUTLEVEL;
  double GetChi();
  void ApproximateTheorSys();
  Level * GetLevel(ResultLevelCode_t);
  vector<HistoUnfolding *> * GetV(ResultLevelCode_t, MOCode_t );
  void AggregateBackgroundMCs();
  void CreateTotalMCUnc(ResultLevelCode_t = IN, RecoLevelCode_t = RECO, bool shape = false);
  void CreateMCTotal(ResultLevelCode_t = IN);
  void CreateBackgroundTotal();
  void CreateDataMinusBackground(ResultLevelCode_t = IN);
  void ScaleFactor();
  void ApplyScaleFactor(TH1 *h);
  void Format();
  void createCov(RecoLevelCode_t);
  HistoUnfolding * GetExpSys(RecoLevelCode_t, const char *, ExpSysType_t );
  HistoUnfolding * GetSys(RecoLevelCode_t, const char * );
  void MarkSysSample(const char *);
  void LoadHistos(const char *, SysTypeCode_t = NOMINAL);
  void FillHistos(const char *, SysTypeCode_t = NOMINAL);
  void AddHisto(HistoUnfoldingTH2 *);
  //  void LoadSys(const char *);
  void NormaliseToBinWidth(TH1 *h);

  THStack * CreateTHStack(RecoLevelCode_t = RECO, ResultLevelCode_t = IN);
  void SetOrigBinWidth(float orig);
  float GetOrigBinWidth() const;
  void PlotDataMC();
  void Do();
  void Process();
  void unfold();
  void PullAnalysis();
  TPad * CreateRatioGraph(RecoLevelCode_t = RECO, ResultLevelCode_t = IN);
  TPad * CreateMainPlot(RecoLevelCode_t = RECO, ResultLevelCode_t = IN);
  TCanvas * CreateCombinedPlot(RecoLevelCode_t = RECO, ResultLevelCode_t = IN);
  float * splitForMinSigmaM(unsigned long & size, float factor);
  static float * split(float * input, unsigned long size);
  TString CreateName(const char *);
  TString CreateTitle(const char *);
  void CreateLegend(RecoLevelCode_t, ResultLevelCode_t);
  void SetSignalTag(const char *);
  void WriteHistograms();
  bool IsRegular() const;
  ClassDef(CompoundHistoUnfolding, 1);
};

#endif

