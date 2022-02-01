#include "CompoundHistoUnfolding.hh"
#include "TFile.h"
#include "JsonParser.hh"
#include "TCanvas.h"
#include "TMath.h"
#include "TStyle.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TROOT.h"
#include "TF1.h"
#include "TUnfoldDensity.h"
#include "TLegend.h"
#include "TKey.h"
#include "TLatex.h"
#include "TChain.h"
#include "TLine.h"
#include "TLegendEntry.h"

ClassImp(CompoundHistoUnfolding);
const unsigned char nbckg = 6;
const char * bckgtitles[nbckg] = {"QCD", "t#bar{t}+V", "Multiboson", "DY", "W", "Single top"};

float calculatearea(TH1 *h)
{
  float area(0.0);
  for (unsigned char bind = 1; bind <= h -> GetNbinsX(); bind ++)
    {
      area += h -> GetBinContent(bind) * h -> GetBinWidth(bind);
    }
  return area;
}


void CompoundHistoUnfolding::AddHisto(HistoUnfoldingTH2 * histo, const char * noscale)
{
  if (histo -> GetSampleType() != DATA and string(noscale).compare("noscale") != 0 )
    {
      histo -> GetTH2() -> Scale(luminosity * histo -> GetXsec());
    }
  if (histo -> IsSys())
    {
      GetV(IN, SYSMO, histo -> GetSample()) -> push_back(histo);
    }
  else
    {
      GetV(IN, histo -> GetSampleType()) -> push_back(histo);
    
      if (histo -> GetSampleType() != BACKGROUNDMO)
	{
      
	  HistoUnfolding *& h = GetLevel(IN) -> GetHURef(histo -> GetSampleType());
	  if (h)
	    {
	      h -> GetTH2() -> Add(histo -> GetTH2());
	    }
	  else
	    {
	      h = new HistoUnfoldingTH2(histo, TString(histo -> GetTH2() -> GetName()) + "_aggr");
	      h -> GetTH2() -> SetTitle(histo -> GetTitle());
	    }
	}
    }  
}

HistoUnfolding * CompoundHistoUnfolding::GetBackgroundH(const char * tag)
{
  vector<HistoUnfolding *>::iterator it;
  //  printf("tag [%s] end %p\n", tag, _vbackgroundhistos.end());
  for (it = _vbackgroundhistos.begin(); it != _vbackgroundhistos.end() and TString((*it) -> GetTag()) != tag; it ++)
  //   {
  //     printf("%p [%s] %u \n", *it, (*it) -> GetTag(), TString((*it) -> GetTag()) != tag);
  //   };
  // printf("it %p\n", it);
    if (it == _vbackgroundhistos.end())
      {
	return nullptr;
      }
  //  printf(" returing background histo %s returning %s\n", tag, (*it) -> GetTag());

  return *it;
}

void CompoundHistoUnfolding::AddXsecSystematics()
{
  const static pair<string, float>  xsecsyst[] = 
    {
      make_pair("MC13TeV_SingleTbar_tW"         ,  0.0537),
      make_pair("MC13TeV_SingleT_tW"            ,  0.0537),
      make_pair("MC13TeV_SingleTbar_t"          ,  0.051),
      make_pair("MC13TeV_SingleT_t"             ,  0.0405), 
      make_pair("MC13TeV_W0Jets"                , -0.057),
      make_pair("MC13TeV_W1Jets"                ,  0.101),
      make_pair("MC13TeV_W2Jets"                ,  0.328),
      make_pair("MC13TeV_QCDMuEnriched30to50"   ,  1.0),
      make_pair("MC13TeV_QCDMuEnriched50to80"   ,  1.0),
      make_pair("MC13TeV_QCDMuEnriched80to120"  ,  1.0),
      make_pair("MC13TeV_QCDMuEnriched120to170" ,  1.0),
      make_pair("MC13TeV_QCDMuEnriched170to300" ,  1.0),
      make_pair("MC13TeV_QCDMuEnriched300to470" ,  1.0),
      make_pair("MC13TeV_QCDMuEnriched470to600" ,  1.0),
      make_pair("MC13TeV_QCDMuEnriched600to800" ,  1.0),
      make_pair("MC13TeV_QCDMuEnriched800to1000",  1.0),
      make_pair("MC13TeV_QCDMuEnriched1000toInf",  1.0),
      make_pair("MC13TeV_QCDEMEnriched30to50"   ,  1.0),
      make_pair("MC13TeV_QCDEMEnriched50to80"   ,  1.0),
      make_pair("MC13TeV_QCDEMEnriched80to120"  ,  1.0),
      make_pair("MC13TeV_QCDEMEnriched120to170" ,  1.0),
      make_pair("MC13TeV_QCDEMEnriched170to300" ,  1.0),
      make_pair("MC13TeV_QCDEMEnriched300toInf" ,  1.0),
      make_pair("MC13TeV_DY50toInf_mlm"         ,  0.05),
      make_pair("MC13TeV_DY10to50"              ,  0.05),
      make_pair("MC13TeV_DY0Jets"               ,  0.05),
      make_pair("MC13TeV_DY1Jets"               ,  0.05),
      make_pair("MC13TeV_DY2Jets"               ,  0.05),
      make_pair("MC13TeV_ZZTo2L2Nu"             ,  0.034),
      make_pair("MC13TeV_ZZTo2L2Q"              ,  0.034),
      make_pair("MC13TeV_WWToLNuQQ"             ,  0.074),
      make_pair("MC13TeV_WWTo2L2Nu"             ,  0.074),
      make_pair("MC13TeV_WZTo3LNu"              ,  0.047),
      make_pair("MC13TeV_TTWToQQ"               ,  0.162),
      make_pair("MC13TeV_TTZToQQ"               ,  0.111),
      make_pair("MC13TeV_TTZToLLNuNu"           ,  0.111)

    };
  const static vector<pair<string, float>> xsecsystv(xsecsyst, xsecsyst + sizeof(xsecsyst)/sizeof(pair<string, float>));  
  for (unsigned char bind = 0; bind < xsecsystv.size(); bind ++)
    {
      //printf("%s\n", xsecsystv[bind].first.c_str());
      HistoUnfolding * ref = (HistoUnfoldingTH2 *) GetBackgroundH(xsecsystv[bind].first.c_str());
      HistoUnfoldingTH2 * h2_up = (HistoUnfoldingTH2 *) GetBackgroundH(xsecsystv[bind].first.c_str()) -> Clone();
      //printf("check _category %p %s %p %s\n", h2 -> _category, h2 -> _category, GetBackgroundH(xsecsystv[bind].first.c_str()) -> _category, GetBackgroundH(xsecsystv[bind].first.c_str()) -> _category );
      //getchar();
      h2_up -> SetCategory("Background");
      h2_up -> SetSysType(THEORSYS);
      h2_up -> SetTag(TString(h2_up -> GetTag()) + "_xsec_up");
      h2_up -> SetTitle(TString(ref -> GetTitle()) + " xsec up");
      h2_up -> GetTH2() -> Scale (1.0 + xsecsystv[bind].second);
      GetV(IN, SYSMO, h2_up -> GetSample()) -> push_back(h2_up);
      HistoUnfoldingTH2 * h2_down = (HistoUnfoldingTH2 *) GetBackgroundH(xsecsystv[bind].first.c_str()) -> Clone();
      h2_down -> SetCategory("Background");
      h2_down -> SetSysType(THEORSYS);
      h2_down -> SetTag(TString(h2_down -> GetTag()) + "_xsec_down");
      h2_down -> SetTitle(TString(ref -> GetTitle()) + " xsec down");
      h2_down -> GetTH2() -> Scale (1.0 - xsecsystv[bind].second);
      GetV(IN, SYSMO, h2_down -> GetSample()) -> push_back(h2_down);
      
    }
}

void CompoundHistoUnfolding::Process(bool reg)
{
  ScaleFactor();
  AggregateBackgroundMCs();
  CreateBackgroundTotal();
  CreateListOfExpSysSamples();
  CreateMCTotal(IN);
  unfold(reg);
  GetLevel(OUT) -> SeparateSys();
  GetLevel(IN) -> SeparateSys();
  createCov();
  CreateMCTotal(OUT);

  CreateTotalMCUnc(IN, RECO);
  CreateTotalMCUnc(IN, RECO, true);
  CreateTotalMCUnc(IN, GEN);
  CreateTotalMCUnc(IN, GEN, true);
  CreateTotalMCUnc(OUT, RECO);
  CreateTotalMCUnc(OUT, RECO, true);
  CreateTotalMCUnc(OUT, GEN);
  CreateTotalMCUnc(OUT, GEN, true);
  CreateDataGraph(OUT, RECO);
  CreateDataGraph(IN, RECO);
  CreateDataGraph(OUT, GEN);
}


void CompoundHistoUnfolding::ScaleFactor()
{
  _size_sf  = GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> GetNbinsX() + 2;
  _sf = new float[_size_sf];
  TH1F * sigReco = GetLevel(IN) -> GetHU(SIGNALMO) -> Project(RECO, CreateName("sigReco"));
  sigReco -> SetTitle(CreateTitle("signal RECO"));
  TH1F * sigRecoNonGen = (TH1F*) GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> ProjectionX(CreateName("sigRecoNonGen"), 0, 0);
  sigRecoNonGen -> SetDirectory(nullptr);
  TCanvas *c = new TCanvas(CreateName("checkproj"), CreateTitle("checkproj"));
  
  sigReco -> SetLineColor(kRed);
  sigReco -> SetMinimum(0.0);
  sigReco -> Draw();
  for (unsigned char ind = 0; ind < _size_sf; ind ++)
    {
      float sfi = 1.0;
      if (sigReco -> GetBinContent(ind) > 0.0)
	{
	  sfi = sigRecoNonGen -> GetBinContent(ind) / sigReco -> GetBinContent(ind);
	}
      _sf[ind] = sfi;
      //      GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> SetBinContent(ind, 0, 0.0);
    }
  TH1F * after = GetLevel(IN) -> GetHU(SIGNALMO) -> Project(RECO, CreateName("after"));
  after -> SetTitle("after sf");
  after -> Draw("SAME");
  after -> SetMinimum(0.0);
  TLegend * legend = new TLegend(0.6, 0.6, 1.0, 1.0);
  legend -> AddEntry(sigReco);
  legend -> AddEntry(after);
  legend -> Draw("SAME");

 //delete sigReco;
  delete sigRecoNonGen;
}

void CompoundHistoUnfolding::ApplyScaleFactor(TH1 *h)
{
  for (unsigned char ind = 0; ind < _size_sf; ind ++)
    {
      h -> SetBinContent(ind, (1 - _sf[ind]) *    h -> GetBinContent(ind));
      h -> SetBinError(ind, (1 - _sf[ind]) * h -> GetBinError(ind));
    }
}

void CompoundHistoUnfolding::ApplyScaleFactor(TH2 *h)
{
  for (unsigned char ind = 0; ind < _size_sf; ind ++) //GEN empty!!
    {
      h -> SetBinContent(ind, 0, (1 - _sf[ind]) * h -> GetBinContent(ind));
      h -> SetBinError(ind, 0, (1 - _sf[ind]) * h -> GetBinError(ind));
    }
}



void CompoundHistoUnfolding::ApproximateTheorSys()
{
  if (string(signaltag).compare("MC13TeV_TTJets") == 0)
    return;
  TH2 * hMC13TeV_TTJets = _chunominal -> GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2();
  for (vector<HistoUnfolding *>::iterator it = GetV(IN, SYSMO, signaltag) -> begin(); it != GetV(IN, SYSMO, signaltag) -> end(); it ++)
    {
      if ((*it) -> GetSysType() != THEORSYS)
	continue;
      if (TString((*it) -> GetTag()) == "MC13TeV_TTJets")
	{
	  continue;
	}
      TH2 * h = (*it) -> GetTH2();
      for (unsigned char xbin = 0; xbin < h -> GetNbinsX() + 2; xbin ++)
	{
	  for (unsigned char ybin = 0; ybin < h -> GetNbinsY() + 2; ybin ++)
	    {
	      if (hMC13TeV_TTJets -> GetBinContent(xbin, ybin) < 10.0)
		continue;
	      h -> SetBinContent(xbin, ybin, h -> GetBinContent(xbin, ybin) * GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> GetBinContent(xbin, ybin) / hMC13TeV_TTJets -> GetBinContent(xbin, ybin));
	    }
	}
    }
}



 CompoundHistoUnfolding::Level * CompoundHistoUnfolding::GetLevel(ResultLevelCode_t resultcode)
 {
   switch(resultcode)
     {
     case IN:
       return & INLEVEL;
     case OUT:
       return & OUTLEVEL;
     }
 }

vector<HistoUnfolding *> * CompoundHistoUnfolding::GetV(ResultLevelCode_t resultcode, MOCode_t mo, const char *tag)
 {
  Level * level = GetLevel(resultcode);
  switch(mo)
    {
    case BACKGROUNDMO:
      return & _vbackgroundhistos;
    default:
      return level -> GetV(mo, tag);
    }
}

void CompoundHistoUnfolding::AggregateBackgroundMCs()
{
  printf("aggregating backgrounds\n");
  // for(map<TString, HistoUnfolding *>:: iterator mit = aggrbackgroundMC.begin(); mit != aggrbackgroundMC.end(); mit ++)
  //   {
  //     delete it -> second;
  //   }
  
  aggrbackgroundMC.clear();
  for(vector<HistoUnfolding *> :: iterator it = _vbackgroundhistos.begin(); it != _vbackgroundhistos.end(); it ++)
    {
      //      (*it) -> ls();
      const TString category ((*it) -> GetCategory());
      if (aggrbackgroundMC.find(category) == aggrbackgroundMC.end())
	{
	  HistoUnfoldingTH2 *check = new HistoUnfoldingTH2(* (HistoUnfoldingTH2*) * it);
	  aggrbackgroundMC[category] = check;
	  
	}
      else
	{
	  aggrbackgroundMC[category] -> GetTH2() -> Add((*it) -> GetTH2());
	}
    }
}


void CompoundHistoUnfolding::CreateBackgroundTotal()
{
  if (totalbackground)
    {
      delete totalbackground;
    }
  for (map<TString, HistoUnfolding *>::iterator it = aggrbackgroundMC.begin(); it != aggrbackgroundMC.end(); it ++)
    {
      if (not totalbackground)
	{
	  totalbackground = new HistoUnfoldingTH2();
	  totalbackground -> GetTH2Ref() = (TH2F*) it -> second -> GetTH2() -> Clone(CreateName("totalbackground"));
	  totalbackground -> GetTH2() -> SetDirectory(nullptr);
	}
      
      else
	{
	  totalbackground -> GetTH2() -> Add(it -> second -> GetTH2());
	}
    }
}


void CompoundHistoUnfolding::CreateMCTotal(ResultLevelCode_t resultcode)
{
  HistoUnfolding *& totalMC = GetLevel(resultcode) -> GetHURef(TOTALMC);
  if (totalMC)
    {
      delete totalMC;
      totalMC = nullptr;
    }
  if (resultcode == IN)
    {
      totalMC = new HistoUnfoldingTH2();
      totalMC -> SetTitle("MCTotal");
      totalMC -> GetTH2Ref() = (TH2F*) GetLevel(resultcode) -> GetHU(SIGNALMO) -> GetTH2() -> Clone(CreateName("MCtotalIN"));
      totalMC -> GetTH2() -> SetDirectory(nullptr);
      totalMC -> GetTH2() -> Add(totalbackground -> GetTH2());
      // TH1F * totalMCtest = totalMC -> Project(GEN, "test");
      // printf("printing totalMC !!!!!!!!!!!!!!!!!!!!!!!\n");
      // totalMCtest -> Print("all");
      // getchar();
      // delete totalMCtest;
    }
  if (resultcode == OUT)
    {
      totalMC = new HistoUnfoldingTH1();
      totalMC -> SetTitle("MCTotal");
      printf("%p", GetLevel(resultcode) -> GetHU(SIGNALPROXY, OUT));
      GetLevel(resultcode) -> GetHURef(DATAMO)  = new HistoUnfoldingTH1((HistoUnfoldingTH1*) GetLevel(resultcode) -> GetHU(DATAMBCKG), "data_out");
      if (GetLevel(resultcode) -> GetHU(SIGNALPROXY, OUT) -> GetTH1(RECO))
	{
	  GetLevel(resultcode) -> GetHU(SIGNALPROXY, OUT) -> GetTH1(RECO) -> Print("all");
	
	  totalMC -> GetTH1Ref(RECO) = GetLevel(resultcode) -> GetHU(SIGNALPROXY, OUT) -> Project(RECO, CreateName("MCtotalOUTRECO"));
	  TH1F *bckgsfRECO = totalbackground -> Project(RECO);
	  //ApplyScaleFactor(bckgsfRECO);
	  totalMC -> GetTH1(RECO) -> Add(bckgsfRECO);
	  GetLevel(resultcode) -> GetHU(DATAMO) -> GetTH1(RECO) -> Add(bckgsfRECO);
	  GetLevel(resultcode) -> GetHU(DATAMO) -> GetTH1(RECO) -> SetTitle(TString("data (") + 
									    GetLevel(IN) -> GetHU(SIGNALMO) -> GetTitle() + 
									    ") folded back + bckg");
	  delete bckgsfRECO;
	}
      totalMC -> GetTH1Ref(GEN) = GetLevel(resultcode) -> GetHU(SIGNALPROXY, OUT) -> Project(GEN, CreateName("MCtotalOUTGEN"));
      TH1 *bckgGEN = totalbackground -> Project(GEN);
      totalMC -> GetTH1(GEN) -> Add(bckgGEN);
      GetLevel(resultcode) -> GetHU(DATAMO) -> GetTH1(GEN) -> Add(bckgGEN);
      delete bckgGEN;
    }
}



void CompoundHistoUnfolding::CreateDataMinusBackground(ResultLevelCode_t resultcode)
{
  
  HistoUnfolding *& datambackground = GetLevel(resultcode) -> GetHURef(DATAMBCKG);
  if (datambackground)
    {
      delete datambackground;
      datambackground = nullptr;
    }
  if (resultcode == IN)
    {
      datambackground = new HistoUnfoldingTH2();
      datambackground -> SetTitle("data - bckg");
      datambackground -> SetName("data_minus_bckg_in");
      datambackground -> GetTH2Ref() = (TH2F * ) GetLevel(resultcode) -> GetHU(DATAMO) -> GetTH2() -> Clone(CreateName("datambackground"));
      datambackground -> GetTH2() -> SetDirectory(nullptr);
  
      datambackground -> GetTH2() -> Add(totalbackground -> GetTH2(), -1);
    }
  // if (resultcode == OUT)
  //   {
  //     datambackground = new HistoUnfoldingTH1();
  //     datambackground -> SetTitle("data - bckg");
  //     datambackground -> SetName("data_minus_bckg_out");
  //     datambackground -> GetTH1Ref(RECO) = (TH2F * ) GetLevel(resultcode) -> GetHU(DATAMO) -> GetTH1(RECO) -> Clone(CreateName("datambackground_out_reco"));
  //     datambackground -> GetTH2() -> SetDirectory(nullptr);
  
  //     datambackground -> GetTH2() -> Add(totalbackground -> GetTH2(), -1);
  //   }
}


void CompoundHistoUnfolding::NormaliseToBinWidth(TH1 *h)
{
  for (unsigned char ind = 1; ind < h -> GetNbinsX() + 1; ind ++)
    {
      h -> SetBinContent(ind, h -> GetBinContent(ind) * _orig_bin_width / h -> GetBinWidth(ind));
      h -> SetBinError(ind, h -> GetBinError(ind) * _orig_bin_width / h -> GetBinWidth(ind));

    }
}


/*
void CompoundHisto::PlotDataMC()
{
  TCanvas c;
  _datath1 -> SetMinimum(0.0);
  _datath1 -> Draw();
  TH1F *hMCTotal = NewMCTotal();
  hMCTotal -> SetMinimum(0.0);
  hMCTotal -> SetLineColor(kRed);
  hMCTotal-> Draw("HISTOSAME");
  c.SaveAs("test_output/DataMC.png");
  delete hMCTotal;
}

*/


void CompoundHistoUnfolding::WriteHistograms()
{
  /*  for(vector<Histo *>::iterator it = _vsyshistosGEN_OUT.begin(); it != _vsyshistosGEN_OUT.end(); it ++)
    {
      (*it) -> _th1 -> Write();
    }
  _totalMCGEN_OUT -> Write();
  for(vector<HistoUnfolding *>::iterator it = _vdatahistos.begin(); it != _vdatahistos.end(); it ++)
    {
      (*it) -> _th2 -> Write();
    }
  for(vector<HistoUnfolding *>::iterator it = _vbackgroundhistos.begin(); it != _vbackgroundhistos.end(); it ++)
    {
      (*it) -> _th2 -> Write();
    }

  _signalhunfolded -> _th1 -> Write();
  */
}

void CompoundHistoUnfolding::SetFolder(const char * folder)
{
  
  sprintf(_folder, "%s", folder);
  //system(TString("rm -r ") + _folder);
  system(TString("mkdir - p ") + _folder);

}

void CompoundHistoUnfolding::SetSignalTag(const char * tag)
{
  sprintf(signaltag, "%s", tag);
}

const char * CompoundHistoUnfolding::GetSignalTag()
{
  return signaltag;
}

void CompoundHistoUnfolding::SetMethod(const char * method)
{
  sprintf(this -> method, "%s", method);
}

void CompoundHistoUnfolding::SetObservable(const char * observable)
{
  sprintf(this -> observable, "%s", observable);
}


void CompoundHistoUnfolding::SetOrigBinWidth(float orig)
{
  _orig_bin_width = orig;
}

void CompoundHistoUnfolding::SetCHUnominal(CompoundHistoUnfolding * chunominal)
{
  _chunominal = chunominal;
}

void CompoundHistoUnfolding::SetXaxisTitle(const char * title)
{
  sprintf(_XaxisTitle, "%s", title);
}

void CompoundHistoUnfolding::SetYaxisTitle(const char * title)
{
  sprintf(_YaxisTitle, "%s", title);
}

void CompoundHistoUnfolding::SetLuminosity(float l)
{
  _luminosity = l;
}

void CompoundHistoUnfolding::SetCOM(const char * com)
{
  sprintf(_com, "%s", com);
}


float CompoundHistoUnfolding::GetOrigBinWidth() const
{
  return _orig_bin_width;
}


void CompoundHistoUnfolding::Format()
{
  for (map<TString, HistoUnfolding *> :: iterator it = aggrbackgroundMC.begin(); it != aggrbackgroundMC.end(); it ++)
    {
      //it -> second -> ls();
      it -> second -> GetTH2() -> SetFillColor(TColor::GetColor(it -> second -> GetColor()));
    }
  //_signalh -> ls();
}

CompoundHistoUnfolding::CompoundHistoUnfolding(): 
  _isRegular(false),
  _nbinsx(0),
  _xlow(0.0),
  _xup(0.0),
  _nbinsy(0),
  _ylow(0.0),
  _yup(0.0),
  _nentriesx(0),
  _xbins(nullptr),
  _nentriesy(0),
  _ybins(nullptr),
  _size_sf(0),
  _sf(nullptr),
  totalbackground(nullptr),
  input(nullptr),
  ematrixtotal(nullptr),
  _chunominal(nullptr)
{
  //printf("constructor A\n");
  //getchar();
}

CompoundHistoUnfolding::CompoundHistoUnfolding(const char* name, const char* title, Int_t nbinsx, const Float_t* xbins, Int_t nbinsy, const Float_t* ybins): 
  _nbinsx(nbinsx), 
  _nentriesx(nbinsx + 1), 
  _xbins(xbins), 
  _nbinsy(nbinsy), 
  _nentriesy(nbinsy + 1), 
  _ybins(ybins), 
  _isRegular(false),
  _xlow(0.0),
  _xup(0.0),
  _ylow(0.0),
  _yup(0.0),
  _size_sf(0),
  _sf(nullptr),
  totalbackground(nullptr),
  input(nullptr),
  ematrixtotal(nullptr),
  _chunominal(nullptr)
{
  SetName(name);
  SetTitle(title);
  //  printf("constructor B\n");
  //getchar();
}

CompoundHistoUnfolding::CompoundHistoUnfolding(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup): 
  _nbinsx(nbinsx), 
  _nentriesx(nbinsx + 1), 
  _xlow(xlow), 
  _xup(xup), _nbinsy(nbinsy), 
  _nentriesy(nbinsy + 1), 
  _ylow(ylow), 
  _yup(yup), 
  _isRegular(true),
  _xbins(nullptr),
  _ybins(nullptr),
  _size_sf(0),
  _sf(nullptr),
  totalbackground(nullptr),
  input(nullptr),
  ematrixtotal(nullptr),
  _chunominal(nullptr)
{
  SetName(name);
  SetTitle(title);
  _orig_bin_width = (xup - xlow)/nbinsx;
  //printf("constructor C\n");
  //getchar();
}

CompoundHistoUnfolding::~CompoundHistoUnfolding()
{
  if (_sf)
    delete [] _sf;
  if (_xbins)
    delete [] _xbins;
  if (_ybins)
    delete [] _ybins;
  for(vector<TObject *>::iterator it = _garbage.begin(); it != _garbage.end(); it ++)
    {
      //printf(" check %p  %c\n", *it, "hh");//(*it) -> ClassName());
        delete (*it);
    }
}

bool CompoundHistoUnfolding::IsRegular() const
{
  return _isRegular;
}

TString CompoundHistoUnfolding::CreateName(const char * name)
{
  return TString(GetName()) + "_" + name;
}

TString CompoundHistoUnfolding::CreateTitle(const char * title)
{
  return TString(GetName()) + " " + title;
}

HistoUnfoldingTH2 * CompoundHistoUnfolding::GetBackgroundTotal()
{
  return totalbackground;
}


float* CompoundHistoUnfolding::splitForMinSigmaM(unsigned long & size, float factor)
{
  printf("splitForMinSigmaM\n");
  TH2F & h = *GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2();
  TAxis * Xaxis = h.GetXaxis();
  //TAxis * Yaxix = h.GetYaxis();                                                                                                                                                                           

  TF1 f1 = TF1("f1", "gaus", Xaxis -> GetXmin(), Xaxis -> GetXmax());
  f1.SetParameters(100, (Xaxis -> GetXmax() - Xaxis -> GetXmin())/2., 0.01);
  TObjArray slices;
  h.FitSlicesX(& f1, 1, h.GetNbinsX(), 0, "QNRLM", &slices);

  TCanvas c2("c2", "c2");
  ((TH1D*)slices[0]) -> Draw();
  c2.Print((string(_folder) + "/slices_N.eps").c_str());
  ((TH1D*)slices[1]) -> GetYaxis() -> SetRangeUser(Xaxis -> GetXmin(), Xaxis -> GetXmax());
  ((TH1D*)slices[1]) -> Draw();
  c2.Print((string(_folder) + "/slices_mean.eps").c_str());
  ((TH1D*)slices[2]) -> GetYaxis() -> SetRangeUser(0.0, Xaxis -> GetXmax());
  ((TH1D*)slices[2]) -> Draw();
  c2.Print((string(_folder) + "/slices_sigma.eps").c_str());

  vector<float> bins;
  bins.push_back(Xaxis -> GetXmin());
  vector<float> exact;
  exact.push_back(Xaxis -> GetXmin());
  const float integral = h.Integral();
  float binfraction = 0.0;
  for (unsigned char ind = 0; ind < h.GetNbinsX() + 1; ind ++)
    {
      TH1F * proj = (TH1F*) h.ProjectionX("px", ind, ind);
      binfraction += proj -> Integral()/integral;
      delete proj;
      const float mean      = ((TH1D *)slices[1]) -> GetBinContent(ind);
      const float meanError = ((TH1D *)slices[1]) -> GetBinError(ind);
      if (mean == 0 or meanError/mean > 0.25)
        continue;
      const float sigma = ((TH1D *) slices[2]) -> GetBinContent(ind) * factor;
      if (mean - sigma > exact.back())
	{
	  if (mean + sigma < h.GetXaxis() -> GetXmax())
	    {
	      exact.push_back(mean + sigma);
	      const float newbinedge = h.GetXaxis() -> GetBinUpEdge(h.GetXaxis() -> FindBin(mean + sigma));
	      if (newbinedge > bins.back() and binfraction > 0.01)
		{
		  bins.push_back(newbinedge);
		  binfraction = 0.0;
		}
	    }
	}
    }
  if (bins.size() > 1)
    {
      bins.back() = Xaxis -> GetXmax();
    }
  size = bins.size();
  float * fbins = new float[size];
  for (unsigned char ind = 0; ind < size; ind ++)
    {
      fbins[ind] = bins[ind];
    }
  return fbins;
}

float * CompoundHistoUnfolding::split(float * input, unsigned long size)
{
  float * output = new float[2*(size - 1) + 1];
  for (unsigned long ind = 0; ind < size - 1; ind ++)
    {
      const float half = 0.5*(input[ind + 1] - input[ind]);
      output[2 * ind] = input[ind];
      output[2 * ind + 1] = input[ind] + half;

    }
  output[2*(size - 1)] = input[size - 1];
  return output;
}

HistoUnfoldingTH2 * CompoundHistoUnfolding::GetTotalBackground()
{
  return totalbackground;
}

TCanvas * CompoundHistoUnfolding::stabpur()
{
  TH2 * histsig = (TH2 *) GetLevel(IN) -> GetHU(SIGNALPROXY, IN) -> GetTH2() -> Clone("sig");
  TH1F * hstability = (TH1F *) histsig -> ProjectionY(CreateName("stability"));
  hstability -> SetDirectory(nullptr);
  hstability -> Reset("ICE");
  TH1F * hpurity = (TH1F *) histsig -> ProjectionY(CreateName("purity"));
  hpurity -> SetDirectory(nullptr);
  hpurity -> Reset("ICE");
  const unsigned char k = histsig -> GetNbinsX()/histsig -> GetNbinsY();
  for (unsigned char bindy = 1; bindy < histsig -> GetNbinsY() + 1; bindy ++)
    {
      float diag = 0.0;
      for (unsigned char bindx = k * (bindy - 1) + 1; bindx < k * bindy + 1; bindx ++)
        {
          diag += histsig -> GetBinContent(bindx, bindy);
        }
      hpurity -> SetBinContent(bindy, diag/histsig -> Integral(k * (bindy - 1) + 1, k * bindy, 1, histsig -> GetNbinsY()));
      hstability -> SetBinContent(bindy, diag/histsig -> Integral(1, histsig -> GetNbinsX(), bindy, bindy));
    }
  hpurity -> SetLineColor(kRed);
  hstability -> SetLineColor(kBlue);
  hstability -> SetLineStyle(7);
  hpurity -> Print("all");
  hstability -> Print ("all");
  THStack * stack  = new THStack("stabpur", TString("stabpur; ") + _XaxisTitle + "; ratio");
  stack -> Add(hstability);
  stack -> Add(hpurity);
  stack -> SetMinimum(0.3);
  stack -> SetMaximum(1.0);
  TCanvas * cstabpur = new TCanvas(CreateName("stabpur"), CreateTitle("stabpur"));
  hstability -> SetTitle("stability");
  hpurity -> SetTitle("purity");
  stack -> Draw("nostack");
  TLegend * legend = new TLegend(0.7, 0.7, 0.85, 0.85);
  legend -> SetLineWidth(0);
  legend -> AddEntry(hstability);
  legend -> AddEntry(hpurity);
  legend -> Draw("SAME");
  TLine * line = new TLine(hstability -> GetXaxis() -> GetXmin(), 0.5, hstability -> GetXaxis() -> GetXmax(), 0.5);
  line -> SetLineWidth(1.0);
  line -> SetLineStyle(kDashed);
  line -> Draw("SAME");
  delete histsig;
  return cstabpur;
}


void CompoundHistoUnfolding::PrintScaleFactors()
{
  printf("printing scale factors\n");
  for (unsigned char ind = 0; ind < _size_sf; ind ++)
    printf("%u %f\n", ind, _sf[ind]);
  printf("ended printing scale factors\n");
}

void CompoundHistoUnfolding::RejectNonReco(TH1 * h)
{
  const TH2 * signalmatrix = GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2();
  for (unsigned char bind = 0; bind <= signalmatrix -> GetNbinsY() + 1; bind ++)
    {
      const float coeff = signalmatrix -> Integral(1, signalmatrix -> GetNbinsX(), bind, bind)/signalmatrix -> Integral(0, signalmatrix -> GetNbinsX() + 1, bind, bind);
      h -> SetBinContent(bind, h -> GetBinContent(bind) / coeff);
      h -> SetBinError(bind, h -> GetBinError(bind) / coeff);

    }

}

void CompoundHistoUnfolding::PrintNonRecoCoeff()
{
  const TH2 * signalmatrix = GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2();
  printf("printing non reco coefficients\n");
  for (unsigned char bind = 0; bind <= signalmatrix -> GetNbinsY() + 1; bind ++)
    {
      const float coeff = signalmatrix -> Integral(1, signalmatrix -> GetNbinsX(), bind, bind)/signalmatrix -> Integral(0, signalmatrix -> GetNbinsX() + 1, bind, bind);
      printf("%u %f\n", bind, coeff); 
    }
  printf("end printing non reco coefficients\n");
	

}

void CompoundHistoUnfolding::printforheplib()
{
  FILE * file_data = fopen((string(_folder) + "/forheplibdata.txt").c_str(), "w");
  TH1 * h_data = (TH1*)  GetLevel(OUT) -> GetHU(DATAMBCKG) -> GetTH1(GEN) -> Clone();
  printf("preparing file for heplib\n");
  printf("integrals out %f in %f\n", 
	 GetLevel(OUT) -> GetHU(DATAMBCKG) -> GetTH1(GEN) -> Integral(), 
	 GetLevel(IN) -> GetHU(DATAMBCKG) -> GetTH2() -> Integral(1, GetLevel(IN) -> GetHU(DATAMBCKG) -> GetTH2() -> GetNbinsX(), 0, 0));
  h_data -> Scale(1.0/_luminosity);
  for (unsigned char bind = 1; bind <= h_data -> GetNbinsX(); bind ++)
    {

      // printf("%u\t%f\t%f\t%f\t%f\t%f\n", 
      // 	     bind,
      // 	     h -> GetXaxis() -> GetBinLowEdge(bind),  
      // 	     h -> GetXaxis() -> GetBinCenter(bind),
      // 	     h -> GetXaxis() -> GetBinUpEdge(bind),
      // 	     h -> GetBinContent(bind)/h -> GetBinWidth(bind),
      // 	     GetLevel(OUT) -> totalMCUncShape -> GetTH1(GEN) -> GetBinError(bind)/(h -> GetBinWidth(bind) * _luminosity));


      fprintf(file_data, "%f\t%f\t%f\t%f\t%f\n", 
	      h_data -> GetXaxis() -> GetBinLowEdge(bind),  
	      h_data -> GetXaxis() -> GetBinCenter(bind),
	      h_data -> GetXaxis() -> GetBinUpEdge(bind),
	      h_data -> GetBinContent(bind) / h_data -> GetBinWidth(bind),
	      GetLevel(OUT) -> totalMCUncShape -> GetTH1(GEN) -> GetBinError(bind)/(h_data -> GetBinWidth(bind) * _luminosity));
    }

  fclose(file_data);
  delete h_data;
  FILE * file_mc = fopen((string(_folder) + "/forheplibmc.txt").c_str(), "w");
  TH1 * h_mc = (TH1*)  GetLevel(IN) -> GetHU(SIGNALMO) -> Project(GEN);
  printf("integral prediction %f \n", h_mc -> Integral());
  h_mc -> Scale(1.0/_luminosity);
  for (unsigned char bind = 1; bind <= h_mc -> GetNbinsX(); bind ++)
    {

      // printf("%u\t%f\t%f\t%f\t%f\t%f\n", 
      // 	     bind,
      // 	     h -> GetXaxis() -> GetBinLowEdge(bind),  
      // 	     h -> GetXaxis() -> GetBinCenter(bind),
      // 	     h -> GetXaxis() -> GetBinUpEdge(bind),
      // 	     h -> GetBinContent(bind)/h -> GetBinWidth(bind),
      // 	     GetLevel(OUT) -> totalMCUncShape -> GetTH1(GEN) -> GetBinError(bind)/(h -> GetBinWidth(bind) * _luminosity));


      fprintf(file_mc, "%f\t%f\t%f\t%f\t%f\n", 
	      h_mc -> GetXaxis() -> GetBinLowEdge(bind),  
	      h_mc -> GetXaxis() -> GetBinCenter(bind),
	      h_mc -> GetXaxis() -> GetBinUpEdge(bind),
	      h_mc -> GetBinContent(bind) / h_mc -> GetBinWidth(bind),
	      GetLevel(OUT) -> totalMCUncShape -> GetTH1(GEN) -> GetBinError(bind)/(h_mc -> GetBinWidth(bind) * _luminosity));
    }
  fclose(file_mc);
  delete h_mc;

  //  getchar();
}
