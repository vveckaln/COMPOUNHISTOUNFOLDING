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
extern const unsigned char nbckg; 
extern const char * bckgtitles[6];
float calculatearea(TH1 *);
class CompoundHistoUnfolding: public TNamed
{
  vector<TObject *>                       _garbage;
  map<TString, HistoUnfolding *>          aggrbackgroundMC;
  vector<HistoUnfolding *>                _vbackgroundhistos;
  vector<SampleDescriptor *>              _expsyssamples;
  vector<SampleDescriptor *>              _nonexpsyssamples;
  vector<SampleDescriptor *>              _markedsyssamples;
  void CreateListOfExpSysSamples();
  HistoUnfoldingTH2 *            totalbackground;
  char                           _signal_title[64];
  char                           _title[64]; 
  char                           _folder[128];
  char                           _XaxisTitle[64];
  char                           _YaxisTitle[64];
  float                          _luminosity;
  char                           _com[64];
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
  float *                        _sf; ///< [_size_sf]
  CompoundHistoUnfolding *       _chunominal;

public:
  ChargeCode_t                   chargecode;
  unsigned char                  jetcode;
  OptLevelCode_t                 optcode;
  char                           observable[128];
  char                           signaltag[128];
  char                           method[128];
  bool                                    calculate_bins;
  CompoundHistoUnfolding();
  CompoundHistoUnfolding(const char* name, const char* title, Int_t nbinsx, const Float_t* xbins, Int_t nbinsy, const Float_t* ybins);
  CompoundHistoUnfolding(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup);
  ~CompoundHistoUnfolding();
  HistoUnfoldingTH2 * GetTotalBackground();
  void SetFolder(const char *);
  struct Level: public TObject
  {
    Level();
    vector<HistoUnfolding *>                    _vdatahistos;
    vector<HistoUnfolding *>                    _vsignalhistos;
    map<TString, vector<HistoUnfolding *>>      _msyshistos;
    map<TString, vector<array<HistoUnfolding *, 2>>>   _m2dirsyshistos;
    map<TString, vector<HistoUnfolding *>>       _m1dirsyshistos;

    map<TString, vector<SampleDescriptor *>>    _mbckghistos;

    //HistoUnfolding * signalnominal;
    HistoUnfolding                              * signal;
    HistoUnfolding                              * data;
    HistoUnfolding                              * totalMC;
    HistoUnfolding                              * totalMCUnc;
    HistoUnfolding                              * totalMCUncShape;
    HistoUnfolding                              * datambackground;
    
    HistoUnfolding                      *& GetHURef(MOCode_t, ResultLevelCode_t resultcode = 3); 
    HistoUnfolding                      * GetHU(MOCode_t, ResultLevelCode_t resultcode = 3); 
    vector<HistoUnfolding *>            * GetV(MOCode_t, const char * sample = nullptr);
    HistoUnfolding                      * GetInputHU(MOCode_t, const char * name, const char * sample = nullptr);
    HistoUnfolding                      * GetExpSys(const char *, ExpSysType_t );
    HistoUnfolding                      * GetSys(const char * , const char *sample);
    void lsInputHU(MOCode_t, const char * sample = nullptr);
    void Format();
    TMatrixD * cov;
    void SeparateSys();
    struct ProjectionDeco: public TObject
    {
      ProjectionDeco();
      TGraphAsymmErrors * _ratiogr; 
      TGraphAsymmErrors * _grtotalMC;
      TGraphErrors      * _datagr;
      TH1               * _ratioframe; 
      TH1               * _ratioframeshape; 
      TLegend           * legend;
      void Format();
      ~ProjectionDeco();
      ClassDef(ProjectionDeco, 1);
    } RECODECO, GENDECO;
    ProjectionDeco * GetProjectionDeco(RecoLevelCode_t);
    ~Level();
    ClassDef(Level, 1);
  } INLEVEL, OUTLEVEL;
  void PrintMigrationMatrix();
  HistoUnfolding * GetUnfoldingHU(const char *, const char * sample, SysTypeCode_t );     
  void AddXsecSystematics();
  HistoUnfolding * GetBackgroundH(const char *);
  void PrintScaleFactors();
  TH1 * input;
  TH2 * ematrixtotal;
  double GetChi();
  void CreateDataGraph(ResultLevelCode_t resultcode, RecoLevelCode_t recocode);
  void ApproximateTheorSys();
  Level * GetLevel(ResultLevelCode_t);
  vector<HistoUnfolding *> * GetV(ResultLevelCode_t, MOCode_t, const char * sampletag = nullptr);
  void AggregateBackgroundMCs();
  void CreateTotalMCUnc(ResultLevelCode_t = IN, RecoLevelCode_t = RECO, bool shape = false, const char * binning = "", const char * unfm = "", bool includecflip = false, const char * env = "lx");
  void CreateTotalMCUncOLD(ResultLevelCode_t = IN, RecoLevelCode_t = RECO, bool shape = false);
  void Compareunc(ResultLevelCode_t = IN, RecoLevelCode_t = RECO, bool shape = false);
  void CreateMCTotal(ResultLevelCode_t = IN);
  void CreateBackgroundTotal();
  void CreateDataMinusBackground(ResultLevelCode_t);
  void ScaleFactor();
  void ApplyScaleFactor(TH1 *h);
  void ApplyScaleFactor(TH2 *h);
  void Format();
  void createCov();
  void createCov_new();
  
  HistoUnfolding                   * GetExpSys(ResultLevelCode_t, const char *, ExpSysType_t );
  HistoUnfolding                   * GetSys(ResultLevelCode_t, const char * , const char * sample);
  TH1                   * GetSignalProxy(ResultLevelCode_t, RecoLevelCode_t recocode, const char *, ExpSysType_t sys = 0, const char * = nullptr, bool pruned = false);
  void MarkSysSample(const char *);
  void LoadHistos(const char * json, SysTypeCode_t = NOMINAL, const char * sample = nullptr);
  void LoadHistos_cflip(const char * json, SysTypeCode_t = NOMINAL, const char * sample = nullptr);

  void TransferHistos(const char * json, SysTypeCode_t = NOMINAL, const char * sample = nullptr);
  void FillHistos(const char *, SysTypeCode_t = NOMINAL, const char * sample = nullptr);
  void AddHisto(HistoUnfoldingTH2 *, const char * noscale = "");
  //  void LoadSys(const char *);
  void NormaliseToBinWidth(TH1 *h);
  void RejectNonReco(TH1 *h);						
  void PrintNonRecoCoeff();
  THStack * CreateTHStack(RecoLevelCode_t = RECO, ResultLevelCode_t = IN);
  void SetOrigBinWidth(float orig);
  float GetOrigBinWidth() const;
  void PlotDataMC();
  void Do();
  void Process(bool reg = false);
  void unfold(bool reg = false, bool includecflip = false);
  void unfoldBayesianOLD(unsigned char = 1);
  void unfoldBayesian(unsigned char = 1, bool includecflip = false);
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
  const char * GetSignalTag();
  void SetMethod(const char *);
  void SetObservable(const char *);
  void SetCHUnominal(CompoundHistoUnfolding * );
  void SetXaxisTitle(const char *);
  void SetYaxisTitle(const char *);
  void SetLuminosity(float);
  void SetCOM(const char  *);
  void Prune(TH1 * h, const char * tag, const char * sample, SysTypeCode_t code, RecoLevelCode_t recolevel);
  HistoUnfoldingTH2 * GetBackgroundTotal();
  void WriteHistograms();
  bool IsRegular() const;
  void CreateChiTable();
  void CreateChiTableFull();
  void Fix();
  void Check();
  TCanvas * stabpur();
  ClassDef(CompoundHistoUnfolding, 1);
  
};

#endif

