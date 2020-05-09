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

void CompoundHistoUnfolding::LoadHistos(const char * json, SysTypeCode_t sys, const char * syssampletag)
{
  const char * sampletag = sys != EXPSYS ? "MC13TeV_TTJets" : signaltag;
  char * temp = nullptr;
  if (TString(method) == "dire2002" or TString(method) == "sherpa" or TString(method) == "herwig7")
    {
      temp = new char[128];
      sprintf(temp, "%s",  (TString(sampletag) + "_" + method).Data());
      sampletag = temp;
    }
  JsonParser parser;
  parser.SetSignalTag(signaltag);
  parser.Load(json, sys, syssampletag);
  for (vector<SampleDescriptor *>::iterator it = parser.GetSamplesV() -> begin(); it != parser.GetSamplesV() -> end(); it ++)
    {
      if (TString(signaltag) != "MC13TeV_TTJets2l2nu_amcatnlo" and TString((*it) -> GetTag()) == "MC13TeV_TTJets2l2nu_amcatnlo") 
	{
	  parser.GetSamplesV() -> erase(it);;
	}
      if (TString(method) == "nominal" and TString((*it) -> GetTag()) == "MC13TeV_TTJets_cflip")
	{
	  //parser.GetSamplesV() -> erase(it);
	}
      if (TString((*it) -> GetTag()) == signaltag)
	{
	  (*it) -> SetSysType(NOMINAL);
	  (*it) -> SetCategory((*it) -> GetTitle());
	}
      else if (calculate_bins)
	{
	  continue;
	}
      if (TString((*it) -> GetTag()) == "MC13TeV_TTJets" and TString(signaltag) != "MC13TeV_TTJets")
	{
	  (*it) -> SetSysType(THEORSYS);
	  (*it) -> SetSample(signaltag);
	  (*it) -> SetSampleType(SIGNAL);
	}
      if (sys == EXPSYS)
	{
	  (*it) -> SetTag(TString((*it) -> GetTag()).ReplaceAll("MC13TeV_TTJets", signaltag));
	}

    }
  // parser.ls();
  //        getchar();
  const unsigned long ssize = parser.GetSize();
  for (unsigned long sind = 0; sind < ssize; sind ++)
    {
      
      HistoUnfoldingTH2 *h = nullptr;
      if (IsRegular())
	{
	  h = new HistoUnfoldingTH2(GetName(), _XaxisTitle, _nbinsx, _xlow, _xup, _nbinsy, _ylow, _yup, parser.GetSample(sind)); 
	  h -> GetTH2() -> GetZaxis() -> SetTitle("N");
	  h -> GetTH2() -> RebinY(2);
	}
      else
	{
	  h = new HistoUnfoldingTH2(GetName(), _XaxisTitle, _nentriesx - 1, _xbins, _nentriesy - 1, _ybins, parser.GetSample(sind));
	  // 	  h -> GetTH2() -> ProjectionY() -> Print("all");

	  // h -> GetTH2() -> ProjectionX() -> Print("all");

	  
	  // getchar();
	}

      TChain chain("migration");
      printf("tag %s \n", h-> GetTag());
      chain.Add(TString("root://eosuser.cern.ch//$EOS/analysis_") + sampletag + ("/HADDmigration/migration_") + h -> GetTag() + ".root/E_" + tag_charge_types_[chargecode] + "_" + tag_jet_types_[jetcode] + "_" + "migration");
      chain.Add(TString("root://eosuser.cern.ch//$EOS/analysis_") + sampletag + ("/HADDmigration/migration_") + h -> GetTag() + ".root/M_" + tag_charge_types_[chargecode] + "_" + tag_jet_types_[jetcode] + "_" + "migration");
      Float_t reco;
      Float_t gen;
      vector<double> * weights = nullptr;
      chain.SetBranchAddress(TString(observable) + "_reco", & reco);
      chain.SetBranchAddress(TString(observable) + "_gen",  & gen);
      chain.SetBranchAddress("weight",          & weights);
      const unsigned int dbcut = 1;
      //      h -> FillFromTree(, tag_jet_types_[jetcode], tag_charge_types_[chargecode], observable);
      for (unsigned long event_ind = 0; event_ind < chain.GetEntries()/dbcut; event_ind ++)
	{
	  if (event_ind % 10000 == 0)
	    printf("%u\r", event_ind);
	  chain.GetEntry(event_ind);
	  if (string(observable).compare("pull_angle") == 0)
	    h -> GetTH2() -> Fill(reco == -10 ? reco : TMath::Abs(reco)/TMath::Pi(), gen == -10.0 ? gen : TMath::Abs(gen)/TMath::Pi(), (*weights)[0]);
	  
	  // if (string(h -> GetTag()).compare("MC13TeV_TTJets_tracking_up") == 0)
	  //   {
	  //     printf("%lu %f %f\n", event_ind, reco, gen);
	  //     //     if (reco != -10 and (reco < - ;
	  //   }
	  if (string(observable).compare("pvmag") == 0)
	    h -> GetTH2() -> Fill(reco, gen, (*weights)[0]);
	  //      _th2 -> Fill(pull_angle_reco == -10 ? pull_angle_reco : pull_angle_reco, pull_angle_gen == -10.0 ? pull_angle_gen : pull_angle_gen, weight);
	  
	}
      printf("Filled %lu events\n", chain.GetEntries());

      AddHisto(h);


      // if (TString(h -> GetTag()) == "MC13TeV_TTJets_tracking_up")
      // 	{
      // 	  printf("printing nongen of MC13TeV_TTJets_tracking_up\n");
      // 	  TH1 *htest = h -> GetTH2() -> ProjectionX("proj", 0, 0);
      // 	  htest -> Print("all");
      // 	  delete htest;
      // 	  getchar();
      // 	}

      // if (TString(parser.GetSample(sind) -> GetTag()) == "MC13TeV_TTJets_herwig")
      // 	{
      // 	  printf("~~~~~~~~~~~~ after loading MC13TeV_TTJets_herwig\n");
      // 	  h -> GetTH2() -> ProjectionX("test") -> Print ("all");
      // 	  getchar();
      // 	}
      bool nondedicated_end =  false;
	
      if (string(parser.GetSample(sind)->GetTag()).compare(signaltag) == 0 and not calculate_bins)
	{
	  //	  	  continue;
	  vector<HistoUnfoldingTH2*> nondedicatedth2d;
	  static const unsigned char Nnondedicated = 20;
	  static const char * nondedicated_titles[Nnondedicated] = {"pileup up", "pileup down",
								    "trig efficiency correction up", "trig efficiency correction down",
								    "sel efficiency correction up", "sel efficiency correction down",
								    "b fragmentation up", "b fragmentation down",
								    "Peterson Frag", 
								    "semilep BR up", "semilep BR down", 
								    "id1002muR1muF2hdampmt272.7225", "id1003muR1muF0.5hdampmt272.7225", "id1004muR2muF1hdampmt272.7225", "id1005muR2muF2hdampmt272.7225", "id1007muR0.5muF1hdampmt272.7225", "id1009muR0.5muF0.5hdampmt272.7225", "id1010", "", ""};
	  static const char * nondedicated_names[Nnondedicated] = {"pileup_up", "pileup_down",
								   "trig_efficiency_correction_up", "trig_efficiency_correction_down",
								   "sel_efficiency_correction_up", "sel_efficiency_correction_down",
								   "b_fragmentation_up", "b_fragmentation_down",
								   "Peterson_frag", 
								   "semilep_BR_up", "semilep_BR_down", 
								   "id1002muR1muF2hdampmt272.7225", "id1003muR1muF0.5hdampmt272.7225", "id1004muR2muF1hdampmt272.7225", "id1005muR2muF2hdampmt272.7225", "id1007muR0.5muF1hdampmt272.7225", "id1009muR0.5muF0.5hdampmt272.7225", "id1010", "", ""};
	  static const char * nondedicated_categories[Nnondedicated] = {"Pile-up", "Pile-up",
								   "Trigger efficiency", "Trigger efficiency",
								   "Selection efficiency", "Selection efficiency",
								   "b-fragmentation", "b-fragmentation",
								   "Peterson fragmentation", 
								   "Semilepton branching ratios", "Semilepton branching ratios", 
								   "QCD scales", "QCD scales", "QCD scales", "QCD scales", "QCD scales", "QCD scales", "QCD Scales", "", ""};

	  for (unsigned char nondedicated_ind = 0; nondedicated_ind < Nnondedicated and not nondedicated_end; nondedicated_ind ++)
	    {
	      printf("%s\n", nondedicated_names[nondedicated_ind]);
	      HistoUnfoldingTH2 *h = nullptr;
	      if (IsRegular())
		{
		  h = new HistoUnfoldingTH2(GetName(), _XaxisTitle, _nbinsx, _xlow, _xup, _nbinsy, _ylow, _yup, parser.GetSample(sind)); 
		  h -> GetTH2() -> GetZaxis() -> SetTitle("N");
		  h -> GetTH2() -> RebinY(2);
		}
	      else
		{
		  h = new HistoUnfoldingTH2(GetName(), _XaxisTitle, _nentriesx - 1, _xbins, _nentriesy - 1, _ybins, parser.GetSample(sind));
		}
	      h -> SetTag(TString(h -> GetTag()) + "_" + nondedicated_names[nondedicated_ind]);
	      h -> GetTH2() -> SetName(nondedicated_names[nondedicated_ind]);
	      h -> GetTH2() -> SetTitle(nondedicated_titles[nondedicated_ind]);
	      h -> SetTitle((TString("t#bar{t} ") + nondedicated_titles[nondedicated_ind]).Data());
	      h -> SetCategory(TString(nondedicated_categories[nondedicated_ind]).Data());
	      h -> SetSysType(EXPSYS);
	      h -> SetSample(signaltag);
	      for (unsigned long event_ind = 0; event_ind < chain.GetEntries()/dbcut; event_ind ++)
		{
		  if (event_ind % 10000 == 0)
		    {
		    printf("%u\r", event_ind);
		    }
		  chain.GetEntry(event_ind);
		  if (nondedicated_ind + 1 > weights -> size() - 1)
		    {
		      nondedicated_end = true;
		      delete h;
		      h = nullptr;
		      printf("breaking\n");
		      
		      break;
		    }
		  if (string(observable).compare("pull_angle") == 0)
		    {
		      //		      printf("%f nondedicated_ind %u\n",  (*weights)[nondedicated_ind + 1], nondedicated_ind);
		      h -> GetTH2() -> Fill(reco == -10 ? reco : TMath::Abs(reco)/TMath::Pi(), gen == -10.0 ? gen : TMath::Abs(gen)/TMath::Pi(), (*weights)[0] * (*weights)[nondedicated_ind + 1]);
		    }
		  /*      if (string(GetTag()).compare("MC13TeV_TTJets") == 0)
			  {
			  printf("%lu %f\n", event_ind, reco);
			  getchar();
			  }*/
		  if (string(observable).compare("pvmag") == 0)
		    h -> GetTH2() -> Fill(reco, gen, (*weights)[0] * (*weights)[nondedicated_ind + 1]);
		  //      _th2 -> Fill(pull_angle_reco == -10 ? pull_angle_reco : pull_angle_reco, pull_angle_gen == -10.0 ? pull_angle_gen : pull_angle_gen, weight);

		}
	      if (h)
		{
		  AddHisto(h);
		  // printf("printing nondedicated %s\n", h -> GetTH2() -> GetName());
		  // TH1 * hp = h -> GetTH2() -> ProjectionX();
		  // hp -> Print("all");
		  // delete hp;
		  // getchar();
		}
	      
	    }
	}
      parser.UnsetSample(sind);
    }
  // TH1F * testsignal = GetLevel(IN) -> GetHU(SIGNALPROXY, IN) -> Project(RECO, "testsignal");
  // printf("printing reco signal\n");
  // testsignal -> Print("all");
  // TH1F * testdata = GetLevel(IN) -> GetHU(DATAMO, IN) -> Project(RECO, "testdata");
  // printf("printing data\n");
  // testdata -> Print("all");
  // getchar();
  // delete testsignal;
  // delete testdata;
    if (TString(method) == "dire2002" or TString(method) == "sherpa" or TString(method) == "herwig7")
    {
      delete [] temp;
    }
    {
  }
}

void CompoundHistoUnfolding::AddHisto(HistoUnfoldingTH2 * histo)
{
  if (histo -> GetSampleType() != DATA)
    histo -> GetTH2() -> Scale(luminosity * histo -> GetXsec());
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
    return nullptr;
  //  printf(" returing background histo %s returning %s\n", tag, (*it) -> GetTag());

  return *it;
}

void CompoundHistoUnfolding::AddXsecSystematics()
{
  const static pair<string, float>  xsecsyst[] = 
    {
      make_pair("MC13TeV_SingleTbar_tW",          0.0537),
      make_pair("MC13TeV_SingleT_tW",             0.0537),
      make_pair("MC13TeV_SingleTbar_t",           0.051),
      make_pair("MC13TeV_SingleT_t",              0.0405), 
      make_pair("MC13TeV_W0Jets",                -0.057),
      make_pair("MC13TeV_W1Jets",                 0.101),
      make_pair("MC13TeV_W2Jets",                 0.328),
      make_pair("MC13TeV_QCDMuEnriched30to50",    1.0),
      make_pair("MC13TeV_QCDMuEnriched50to80",    1.0),
      make_pair("MC13TeV_QCDMuEnriched80to120",   1.0),
      make_pair("MC13TeV_QCDMuEnriched120to170",  1.0),
      make_pair("MC13TeV_QCDMuEnriched170to300",  1.0),
      make_pair("MC13TeV_QCDMuEnriched300to470",  1.0),
      make_pair("MC13TeV_QCDMuEnriched470to600",  1.0),
      make_pair("MC13TeV_QCDMuEnriched600to800",  1.0),
      make_pair("MC13TeV_QCDMuEnriched800to1000", 1.0),
      make_pair("MC13TeV_QCDMuEnriched1000toInf", 1.0),
      make_pair("MC13TeV_QCDEMEnriched30to50",    1.0),
      make_pair("MC13TeV_QCDEMEnriched50to80",    1.0),
      make_pair("MC13TeV_QCDEMEnriched80to120",   1.0),
      make_pair("MC13TeV_QCDEMEnriched120to170",  1.0),
      make_pair("MC13TeV_QCDEMEnriched170to300",  1.0),
      make_pair("MC13TeV_QCDEMEnriched300toInf",  1.0),
      make_pair("MC13TeV_DY50toInf_mlm"         , 0.05),
      make_pair("MC13TeV_DY10to50"              , 0.05),
      make_pair("MC13TeV_ZZTo2L2Nu"             , 0.034),
      make_pair("MC13TeV_ZZTo2L2Q"              , 0.034),
      make_pair("MC13TeV_WWToLNuQQ"             , 0.074),
      make_pair("MC13TeV_WWTo2L2Nu"             , 0.074),
      make_pair("MC13TeV_WZTo3LNu"              , 0.047),
      make_pair("MC13TeV_TTWToQQ"               , 0.162),
      make_pair("MC13TeV_TTZToQQ"               , 0.111),
      make_pair("MC13TeV_TTZToLLNuNu"           , 0.111)

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

void CompoundHistoUnfolding::unfold(bool reg)
{
  CreateDataMinusBackground(IN);
  TCanvas cmigration("cmigration", "cmigration");
  cmigration.SetRightMargin(0.18);

  TH2 * hsignal = (TH2 *) GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> Clone("unfolding_signal");
  for (unsigned char bind = 0; bind <= hsignal -> GetNbinsX() + 1; bind ++)
    {
      hsignal -> SetBinContent(bind, 0, 0.0);
    }
  hsignal -> Draw("COLZ");
  cmigration.SaveAs(TString(_folder) + "/migrationmatrix.png");
  TUnfoldDensity unfold(hsignal, TUnfold::kHistMapOutputVert, TUnfold::kRegModeCurvature, TUnfold::kEConstraintArea, TUnfoldDensity::kDensityModeBinWidthAndUser);
  input = /*(TH1F*) _signalh -> _th2 -> ProjectionX(); */GetLevel(IN) -> GetHU(DATAMBCKG) -> Project(RECO, CreateName("input"));
  ApplyScaleFactor(input);
  TH1F * input_gen = GetLevel(IN) -> GetHU(SIGNALMO) -> Project(GEN, CreateName("inputgen"));
  NormaliseToBinWidth(input_gen);
  // printf("input gen\n");
  // input_gen -> Print("all");
  TH1F * input_reco = GetLevel(IN) -> GetHU(SIGNALMO) -> Project(RECO, CreateName("inputreco"));
  NormaliseToBinWidth(input_reco);
  // printf("input reco\n");
  // input_reco -> Print("all");
  //  ApplyScaleFactor(input);
  input -> SetMinimum(0.0);
  input -> SetDirectory(nullptr);
  unfold.SetInput(input);
  float tau(0.0);
  if (reg)
    {
      static const unsigned char nPoint(100);
      static const double tauMin(string(observable).compare("pvmag") == 0 ? 0.0 : 1E-10);
      static const double tauMax(1E-3);
      TGraph * lCurve;
      TSpline * logTauX;
      TSpline * logTauY;
      const int iBest = unfold.ScanLcurve(nPoint, tauMin, tauMax, & lCurve, & logTauX, & logTauY);
      TCanvas * clCurve = new TCanvas(CreateName("lCurve"), CreateTitle("lCurve"));
      lCurve -> Draw("AP*");
      clCurve -> SaveAs(TString(_folder) + "/lCurve.png");
      TCanvas * clogTauX = new TCanvas(CreateName("logTauX"), CreateTitle("logTauX"));
      logTauX -> Draw("");
      clogTauX -> SaveAs(TString(_folder) + "/logTauX.png");
      delete logTauX;
      delete clogTauX;
      TCanvas * clogTauY = new TCanvas(CreateName("logTauY"), CreateTitle("logTauY"));
      logTauY -> Draw("");
      clogTauY -> SaveAs(TString(_folder) + "/logTauY.png");
      delete logTauY;
      delete clogTauY;
      tau = unfold.GetTau();
      printf("tau %.9f\n", tau);
      if (tau < 2*tauMin or tau > 0.5*tauMax)
	{
	  TSpline * tauScanResult;
	  const int iBest = unfold.ScanTau(nPoint, tauMin, tauMax, & tauScanResult, TUnfoldDensity::kEScanTauRhoAvg);
	  TCanvas * ctauScanResult = new TCanvas(CreateName("tauScanResult"), CreateTitle("tauScanResult"));
	  tauScanResult -> Draw("");
	  ctauScanResult -> SaveAs(TString(_folder) + "/tauScanResult.png");
	  delete tauScanResult;
	  delete tauScanResult;
	  tau = unfold.GetTau();
	  printf("tau from tauscan %.9f\n", tau);
	}
      delete lCurve;
      delete clCurve;
      
    }
  printf("tau used %.9f\n", tau);

  unfold.DoUnfold(tau);
  TCanvas *coutput = new TCanvas(CreateName("coutput"), CreateTitle("coutput"));
  THStack *coutput_stack = new THStack(CreateName("output"), CreateTitle("output"));
  TH1 * houtput = unfold.GetOutput(CreateName("output"));
  printf("output\n");
  houtput -> Print("all");
  //getchar();
  houtput -> SetDirectory(nullptr);
  // printf("ematrixtotal %p\n", ematrixtotal);
  ematrixtotal = unfold.GetEmatrixTotal("ematrix");
  // printf("ematrixtotal after %p %s\n", ematrixtotal, ematrixtotal -> ClassName());
   ematrixtotal -> Scale(1.0/TMath::Power(houtput -> Integral(), 2.0));
  // getchar();
   ematrixtotal -> SetDirectory(nullptr);
  TH1F * py = GetLevel(IN) -> GetHU(SIGNALMO) -> Project(GEN, CreateName("py"));
  py -> SetMinimum(0.0);
  py -> Scale(1.0/py -> Integral());
  NormaliseToBinWidth(py);
  py -> Scale(1.0/py -> Integral());
  coutput_stack -> Add(py);
  TH1F * houtput_draw = (TH1F*) houtput -> Clone(CreateName("houtput_draw"));
  houtput_draw -> SetDirectory(nullptr);
  //  houtput_draw -> Scale(1.0/houtput_draw -> Integral());
  NormaliseToBinWidth(houtput_draw);
  // printf("unfolded output\n");
  // houtput_draw -> Print("all");
  
  //houtput_draw -> Scale(1.0/houtput_draw -> Integral());
  
  houtput_draw -> SetMinimum(0.0);
  coutput_stack -> Add(houtput_draw);

  houtput_draw -> SetLineColor(kGreen);
  const TArrayD * h1Array = houtput_draw -> GetXaxis() ->GetXbins();
  const TArrayD * h2Array = py -> GetXaxis() ->GetXbins();
  Int_t fN = h1Array->fN;
  if ( fN != 0 ) {
    if ( h2Array->fN != fN ) {
      printf("DifferentBinLimits %u %u\n", fN, h2Array -> fN);
    }
    else {
      for ( int i = 0; i < fN; ++i ) {
	if ( ! TMath::AreEqualRel( h1Array->GetAt(i), h2Array->GetAt(i), 1E-10 ) ) {
	  printf("i %u DifferentBinLimits\n", i);
	}
      }
    }
  }
  coutput_stack -> Draw("NOSTACK");

  coutput_stack -> SetMaximum(coutput_stack -> GetMaximum() * 1.2);
  TLegend * legend_output = new TLegend(0.6, 0.7, 1.0, 1.0);
  legend_output -> SetTextSize(0.03);
  legend_output -> AddEntry(houtput_draw, "unfolded");
  legend_output -> AddEntry(py, TString(GetLevel(IN) -> GetHU(SIGNALMO) -> GetTitle()) + " gen");
  legend_output -> Draw("SAME");
  //coutput -> SaveAs(TString(_folder) + "/" + "coutput.png");
  if (GetLevel(OUT) -> GetHURef(DATAMBCKG))
    delete GetLevel(OUT) -> GetHURef(DATAMBCKG);
  for(map<TString, vector<HistoUnfolding *>>::iterator bit = GetLevel(OUT) -> _msyshistos .begin(); bit != GetLevel(OUT) -> _msyshistos . end(); bit ++)
    {
      const TString sample = bit -> first;
      for (vector<HistoUnfolding *> :: iterator it = GetLevel(OUT) -> GetV(SYSMO, sample.Data()) -> begin(); it != GetLevel(OUT) -> GetV(SYSMO, sample.Data()) -> end(); it ++)
	delete *it;
      
      GetLevel(OUT) -> GetV(SYSMO, sample.Data()) -> clear();
    }
  GetLevel(OUT) -> GetHURef(DATAMBCKG) = new HistoUnfoldingTH1(GetLevel(IN) -> GetHURef(DATA));
  GetLevel(OUT) -> GetHU(DATAMBCKG) -> GetTH1Ref(GEN) = houtput;
  houtput -> SetTitle(TString("data (") + GetLevel(IN) -> GetHU(SIGNALMO) -> GetTitle() + ") unfolded");

  TCanvas * cfolded_output = new TCanvas(CreateName("folded_output"), CreateTitle("folded_output"));
  THStack * cfolded_output_stack = new THStack(CreateName("cfolded_output_stack"), CreateTitle("cfolded output"));
  TH1 * hfolded_output = unfold.GetFoldedOutput(CreateName("folded_output"));
  hfolded_output -> SetDirectory(nullptr);
  TH1F * px = GetLevel(IN) -> GetHU(SIGNALMO) -> Project(RECO, CreateName("px")); 
  NormaliseToBinWidth(px);
  px -> SetMinimum(0.0);
 
  cfolded_output_stack -> Add(px);
  TH1F * input_draw = (TH1F*) input -> Clone(CreateName("input_draw"));
  input_draw -> SetDirectory(nullptr);
  NormaliseToBinWidth(input_draw);

  input_draw -> SetLineColor(kRed);
  input_draw -> SetMinimum(0.0);
  cfolded_output_stack -> Add(input_draw);
  TH1F * hfolded_output_draw = (TH1F*) hfolded_output -> Clone(CreateName("hfolded_output_draw"));
  hfolded_output_draw -> SetDirectory(nullptr);
  NormaliseToBinWidth(hfolded_output_draw);
  printf("listing folded back output");
  hfolded_output_draw -> Print("all");
  cfolded_output_stack -> Add(hfolded_output_draw);
  hfolded_output_draw -> SetLineColor(kGreen);
  hfolded_output_draw -> SetMinimum(0.0);
  cfolded_output_stack -> Draw("NOSTACK");
  TLegend * legend_input = new TLegend(0.6, 0.7, 0.95, 0.95);
  legend_input -> SetTextSize(0.03);
  legend_input -> AddEntry(input_draw, "dataminusbckg");
  legend_input -> AddEntry(px, TString(GetLevel(IN) -> GetHU(SIGNALMO) -> GetTitle()) + " reco no fakes");
  legend_input -> AddEntry(hfolded_output_draw, "folded_output");
  legend_input -> SetName("legend");
  legend_input -> Draw("SAME");
  cfolded_output -> SaveAs(TString(_folder) + "/" + "cfolded_output.png");
  GetLevel(OUT) -> GetHU(DATAMBCKG) -> GetTH1Ref(RECO) = hfolded_output; 
  hfolded_output -> SetTitle(TString("data (") + GetLevel(IN) -> GetHU(SIGNALMO) -> GetTitle() + ") folded back");
  
  //  GetLevel(OUT) -> GetHU(SIGNALMO) -> GetTH1Ref(GEN) = houtput;
  if (TString(signaltag) == "MC13TeV_TTJets")
    {
      _chunominal = this;
    }
  vector<HistoUnfolding *>::iterator it = GetLevel(IN) -> GetV(SYSMO, signaltag) -> begin();
  while (it != GetLevel(IN) -> GetV(SYSMO, signaltag) -> end() and not TString((*it) -> GetTag()).Contains("non_clos"))
    {
      it ++;
    }

  if (it != GetLevel(IN) -> GetV(SYSMO, signaltag) -> end())
    {
      printf("removing non clos\n");
      delete (*it);
      GetLevel(IN) -> GetV(SYSMO, signaltag) -> erase(it);
      //      getchar();
    }
  printf("signaltag %s\n", signaltag); 
  for(map<TString, vector<HistoUnfolding *>>::iterator bit = GetLevel(IN) -> _msyshistos .begin(); bit != GetLevel(IN) -> _msyshistos . end(); bit ++)
    {
      const TString sample = bit -> first;
      printf("unfolding sample %s\n", sample.Data());
      if (sample == signaltag)
	{
	  for (vector<HistoUnfolding *>::iterator it = GetLevel(IN) -> GetV(SYSMO, sample.Data()) -> begin(); it != GetLevel(IN) -> GetV(SYSMO, sample.Data()) -> end(); it ++)
	    {
	      printf("unfolding systematic %s systype\n", (*it) -> GetTag(), (*it) -> GetSysType());
	      if ((*it) -> GetSysType() == EXPSYS)
		{
		  TH2 * mmatrixsyst = (TH2 *) (*it) -> GetTH2() -> Clone("matrixsyst");
		  //      (*it) -> ls();
		  TH2 * datambckg = (TH2 * ) GetLevel(IN) -> GetHU(DATAMO) -> GetTH2() -> Clone(CreateName("datambackground_unfolding"));
		  datambckg -> SetDirectory(nullptr);
		  datambckg -> Add(totalbackground -> GetTH2(), -1);
		  TH1 * input = (TH1 *) datambckg -> ProjectionX(TString(datambckg -> GetName()) + "_reco");
		  delete datambckg;
		  for (unsigned char bind = 0; bind <= mmatrixsyst -> GetNbinsX(); bind ++)
		    {
		      const float fractionnongen = mmatrixsyst -> Integral(bind, bind, 0, 0)/mmatrixsyst -> Integral(bind, bind, 0, mmatrixsyst -> GetNbinsY() + 1);
		      if (TString((*it) -> GetTag()) == "MC13TeV_TTJetstrig_efficiency_correction_up")
			printf("%f %f \n", mmatrixsyst -> GetBinContent(bind, 0), fractionnongen);
		      input -> SetBinContent(bind, input -> GetBinContent(bind) * ( 1 - fractionnongen));
		      input -> SetBinError(bind, input -> GetBinError(bind) * ( 1 - fractionnongen));
		      mmatrixsyst -> SetBinContent(bind, 0, 0.0);
		    }
		  TUnfoldDensity unfold(mmatrixsyst, TUnfold::kHistMapOutputVert, TUnfold::kRegModeCurvature, TUnfold::kEConstraintArea, TUnfoldDensity::kDensityModeUser);
		  unfold.SetInput(input);
	
		  float tau(0.0);
		  if (reg)
		    {
		      static const unsigned char nPoint(100);
		      static const double tauMin(string(observable).compare("pvmag") == 0 ? 0.0 : 1E-10);
		      static const double tauMax(1E-3);
		      TGraph * lCurve;
		      const int iBest = unfold.ScanLcurve(nPoint, tauMin, tauMax, & lCurve);
		      tau = unfold.GetTau();
		      if (tau < 2*tauMin or tau > 0.5*tauMax)
			{
			  printf("scanning tau\n");
			  TSpline * tauScanResult;
			  const int iBest = unfold.ScanTau(nPoint, tauMin, tauMax, & tauScanResult, TUnfoldDensity::kEScanTauRhoAvg);
			  delete tauScanResult;
			  delete tauScanResult;
			  tau = unfold.GetTau();
			  //	      printf("tau from tauscan %.9f\n", tau);
			}
		      // else
		      //   {

		      //     printf("attention \n");
		      //     getchar();
		      //   }
		      printf("tau %.9f\n", tau);
		      delete lCurve;
      
		    }

		  unfold.DoUnfold(tau);
		  HistoUnfoldingTH1 * out = new HistoUnfoldingTH1((SampleDescriptor * ) * it);
		  GetLevel(OUT) -> GetV(SYSMO, sample.Data()) -> push_back(out);
		  out -> GetTH1Ref(RECO) = (TH1F*) unfold.GetFoldedOutput(CreateName(TString((*it) -> GetTag()) + "_folded_output"));
		  out -> GetTH1Ref(GEN) = (TH1F*) unfold.GetOutput(CreateName(TString((*it) ->GetTag()) + "_output"));
		  delete input;
		  delete mmatrixsyst;
		}
	      if ((*it) -> GetSysType() == THEORSYS)
		{
	  
		  TUnfoldDensity unfold(hsignal, TUnfold::kHistMapOutputVert, TUnfold::kRegModeCurvature, TUnfold::kEConstraintArea, TUnfoldDensity::kDensityModeUser);
		  //      (*it) -> ls();
		  TH2 * inputsysh2 = (TH2*)(*it) -> GetTH2() -> Clone("inputsysh2");
		  for (unsigned char bind = 0; bind <= inputsysh2 -> GetNbinsX() + 2; bind ++)
		    {
		      //	      inputsysh2 -> SetBinContent(bind, 0, 0.0);
		    }
		  TH1* inputsys = inputsysh2 -> ProjectionX("inputsys");
		  ApplyScaleFactor(inputsys);
		  unfold.SetInput(inputsys);
		  float tau(0.0);
		  if (reg)
		    {
		      static const unsigned char nPoint(100);
		      static const double tauMin(string(observable).compare("pvmag") == 0 ? 0.0 : 1E-10);
		      static const double tauMax(1E-3);
		      TGraph * lCurve;
		      const int iBest = unfold.ScanLcurve(nPoint, tauMin, tauMax, & lCurve);
		      tau = unfold.GetTau();
		      if (tau < 2*tauMin or tau > 0.5*tauMax)
			{
			  printf("scanning tau\n");
			  TSpline * tauScanResult;
			  const int iBest = unfold.ScanTau(nPoint, tauMin, tauMax, & tauScanResult, TUnfoldDensity::kEScanTauRhoAvg);
			  delete tauScanResult;
			  delete tauScanResult;
			  tau = unfold.GetTau();
			  //	      printf("tau from tauscan %.9f\n", tau);
			}
		      // else
		      //   {

		      //     printf("attention \n");
		      //     getchar();
		      //   }
		      printf("tau %.9f\n", tau);
		      delete lCurve;
      
		    }

		  unfold.DoUnfold(tau);
		  HistoUnfoldingTH1 * out = new HistoUnfoldingTH1((SampleDescriptor * ) * it);
		  GetLevel(OUT) -> GetV(SYSMO, sample.Data()) -> push_back(out);
		  out -> GetTH1Ref(RECO) = (TH1F*) unfold.GetFoldedOutput(CreateName(TString((*it) -> GetTag()) + "_folded_output"));
		  out -> GetTH1Ref(GEN) = (TH1F*) unfold.GetOutput(CreateName(TString((*it) ->GetTag()) + "_output"));
		  if (TString((*it) -> GetTag()) == "MC13TeV_TTJets_evtgen")
		    {
		      printf("listing MC13TeV_TTJets_evtgen after unfolding\n");
		      printf("input gen\n");
		      TH1 * inputsysgen = (TH1*)(*it) -> Project(GEN) -> Clone("clonesysgen");

		      inputsysgen -> Print("all");
		      printf("output\n");
		      out -> GetTH1Ref(GEN) -> Print("all");
		      //getchar();
		    }
	  

		  delete 	    inputsys;
		  delete inputsysh2;
		}
	    }
	}
      else
	{
	  for (vector<HistoUnfolding *>::iterator it = GetLevel(IN) -> GetV(SYSMO, sample.Data()) -> begin(); it != GetLevel(IN) -> GetV(SYSMO, sample.Data()) -> end(); it ++)
	    {
	      TH2 * datambackground  = (TH2 * ) GetLevel(IN) -> GetHU(DATAMO) -> GetTH2() -> Clone(CreateName("datambackground_bckgsyst"));
	      datambackground -> SetDirectory(nullptr);
	      TH2 * totalbackground = (TH2 *) this -> totalbackground -> GetTH2() -> Clone("totalbackground_bckgsyst");
	      totalbackground -> Add(GetBackgroundH(sample.Data()) -> GetTH2(), -1);
	      totalbackground -> Add((*it) -> GetTH2());
	      datambackground -> Add(totalbackground, -1);
	      TUnfoldDensity unfold(hsignal, TUnfold::kHistMapOutputVert, TUnfold::kRegModeCurvature, TUnfold::kEConstraintArea, TUnfoldDensity::kDensityModeBinWidthAndUser);
	      ApplyScaleFactor(datambackground);
	      unfold.SetInput(datambackground);
	      unfold.DoUnfold(tau);
	      HistoUnfoldingTH1 * out = new HistoUnfoldingTH1((SampleDescriptor * ) * it);
	      GetLevel(OUT) -> GetV(SYSMO, sample.Data()) -> push_back(out);
	      out -> GetTH1Ref(RECO) = (TH1F*) unfold.GetFoldedOutput(CreateName(TString((*it) -> GetTag()) + "_folded_output"));
	      out -> GetTH1Ref(GEN) = (TH1F*) unfold.GetOutput(CreateName(TString((*it) ->GetTag()) + "_output"));
	      delete datambackground;
	      delete totalbackground;
	    }
	}
    }

  HistoUnfoldingTH1 * nonclosout = new HistoUnfoldingTH1((SampleDescriptor*) GetLevel(IN) -> GetHU(SIGNALMO));
  nonclosout -> SetTag(TString(nonclosout -> GetTag()) + "_non_clos");
  nonclosout -> SetCategory("Non closure");
  nonclosout -> SetSysType(THEORSYS);

  HistoUnfoldingTH2 * nonclosin = new HistoUnfoldingTH2((SampleDescriptor*) GetLevel(IN) -> GetHU(SIGNALMO));
  nonclosin -> SetTag(TString(nonclosin -> GetTag()) + "_non_clos");
  nonclosin -> SetCategory("Non closure");
  nonclosin -> SetSysType(THEORSYS);

  const unsigned char nbins = hsignal -> GetNbinsY(); 
  float binsrecord[nbins];
  double binsrecordold[nbins];
  for (unsigned char bind = 0; bind < nbins; bind ++)
    {
      binsrecord[bind] = 1.0;
      binsrecordold[bind] = binsrecord[bind];
    }


  float covrecord = 1E6;
  auto varylambdanest = [ &binsrecord, &binsrecordold, &covrecord, nbins, this](unsigned char start)  -> void
    {
      float bins[nbins];

      auto varylambda =[&bins, &binsrecord, &binsrecordold, &covrecord, nbins, this](unsigned char start, unsigned char k, auto & func) -> void
      {
	const float dev = 0.2;
        const double min = binsrecordold[start] - dev/TMath::Power(2, k);
        const double max = binsrecordold[start] + dev/TMath::Power(2, k);
        // printf("start %u %f %f %u\n", start, min, max, k);                                                                                                                                               
        // getchar();                                                                                                                                                                                       
        const unsigned long M = 1E4;
        double step = (max - min) /  TMath::Power(M * nbins, 1.0/nbins);
	for (float coeff = min; coeff <= max; coeff += step)
	  {
	    bins[start] = coeff;
	    if (start < nbins - 1)
	      func(start + 1, k, func);
	    else
	      {
		for (unsigned char bind = 0; bind < nbins; bind ++)
		  {
		    //  printf("%f ", bins[bind]);
		  }
		//printf("\n");
		TH2 * hin = (TH2 *) this -> GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> Clone("hin"); 
		for (unsigned char bind = 0; bind <= hin -> GetNbinsX() + 1; bind ++)
		  hin -> SetBinContent(bind, 0, 0.0);
		for (unsigned char biny = 1; biny < hin -> GetNbinsY() +1; biny ++)
		  {
		    for(unsigned char binx = 1; binx < hin -> GetNbinsX() + 1; binx ++)
		      {
			hin -> SetBinContent(binx, biny, hin -> GetBinContent(binx, biny) * bins[biny - 1]);
		      }
		  }
		TH1 * hinx = hin -> ProjectionX();
		hin -> Scale(input -> Integral()/hin->Integral());
		float cov = 0.0;
		for (unsigned char bind = 1; bind < hinx -> GetNbinsX() + 1; bind ++)
		  {
		    cov = TMath::Sqrt(TMath::Power(cov, 2) + TMath::Power(hinx -> GetBinContent(bind) - input -> GetBinContent(bind), 2));
		  }
		//		printf("cov %f\n", cov);
		if (cov < covrecord)
		  {
		    covrecord = cov;
		    for (unsigned char bind = 0; bind < nbins; bind ++)
		      {
			binsrecord[bind] = bins[bind];
		      }
		  }
		delete hinx;
		delete hin;
	      }
	  }
      };
      const double tol = 1E-6;
      bool precis = true;
      unsigned char k = 0;
      do
        {
          printf("new iteration %u\n", k);
          // getchar();                                                                                                                                                                                     
          varylambda(0, k, varylambda);
          k ++;
          precis = true;
          for (unsigned char bind = 0; bind < nbins; bind ++)
            {
              const float d = fabs(binsrecordold[bind] - binsrecord[bind])/binsrecordold[bind];
              printf("%u %.9f %.9f %.9f\n", bind, binsrecord[bind], binsrecordold[bind], d);
              if (d > tol)
                {
                  printf(" not precise\n");
                  precis = false;
                }
            }
	  if (not precis)
            {
              for (unsigned char bind = 0; bind < nbins; bind ++)
                {
                  binsrecordold[bind] = binsrecord[bind];
                }
            }
        } while (not precis);

    };
  varylambdanest(0);

  printf("resulting coefficients:\n");
  for (unsigned char bind = 0; bind < nbins; bind ++)
    {
      printf("%f ", binsrecord[bind]); 
    }
  printf("\n");
  TH2 * hinnonclos = (TH2 *) this -> GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> Clone("hin"); 
  for (unsigned char bind = 0; bind <= hinnonclos -> GetNbinsX() + 1; bind ++)
    hinnonclos -> SetBinContent(bind, 0, 0.0);

  for (unsigned char bindy = 1; bindy < hsignal -> GetNbinsY() + 1; bindy ++)
    {
      for (unsigned char bindx = 0; bindx < hsignal -> GetNbinsX() + 1; bindx ++)
	{
	  hinnonclos -> SetBinContent(bindx, bindy, hinnonclos -> GetBinContent(bindx, bindy) * binsrecord[bindy - 1]);
	}
    }
  
  nonclosin -> GetTH2Ref() = (TH2F *) hinnonclos; 
  GetLevel(IN) -> GetV(SYSMO, signaltag) -> push_back(nonclosin);

  TH1 * inputnonclos = hinnonclos -> ProjectionX("inpuntnonclos");
  
  // //inputnonclos -> Print("all");
  TUnfoldDensity unfoldnonclos(hsignal, TUnfold::kHistMapOutputVert, TUnfold::kRegModeCurvature, TUnfold::kEConstraintArea, TUnfoldDensity::kDensityModeBinWidthAndUser);
  // //ApplyScaleFactor(inputnonclos);
  for (unsigned char bindx = 0; bindx <= inputnonclos -> GetNbinsX() + 1; bindx ++)
    {
      printf("%f \n", inputnonclos -> GetBinContent(bindx)/ input -> GetBinContent(bindx));
    }

  unfoldnonclos.SetInput(inputnonclos);
  unfoldnonclos.DoUnfold(tau);
  GetLevel(OUT) -> GetV(SYSMO, signaltag) -> push_back(nonclosout);
  nonclosout -> GetTH1Ref(RECO) = (TH1F*) unfoldnonclos.GetFoldedOutput(CreateName(TString(nonclosout -> GetTag()) + "_folded_output"));
  nonclosout -> GetTH1Ref(GEN) = (TH1F*) unfoldnonclos.GetOutput(CreateName(TString(nonclosout ->GetTag()) + "_output"));
  nonclosout -> SetTitle(TString(nonclosout -> GetTitle()) + " non closure");
  TH1 * nonclosoutgentest = (TH1 *) nonclosout -> GetTH1(GEN) -> Clone();
  TH1 * nonclosingentest  = hinnonclos -> ProjectionY();
  printf("nonclossys unscaled\n");
  nonclosoutgentest -> Print("all");
  
  nonclosoutgentest -> Scale(nonclosingentest -> Integral() /nonclosoutgentest -> Integral());
  printf("nonclossys scaled\n");
  nonclosoutgentest -> Print("all");
  nonclosingentest -> Print("all");
  for (unsigned char bind = 1; bind < nonclosoutgentest -> GetNbinsX() + 1; bind ++)
    {
      printf("%u %f\n", bind, nonclosoutgentest -> GetBinContent(bind) - nonclosingentest -> GetBinContent(bind));
    }

  nonclosoutgentest -> Scale(houtput -> Integral()/nonclosoutgentest -> Integral());
  for (unsigned char bind = 1; bind < nonclosoutgentest -> GetNbinsX() + 1; bind ++)
    {
      printf("%u %f\n", bind, nonclosoutgentest -> GetBinContent(bind) - houtput -> GetBinContent(bind));
    }

  printf("nonclosin %p nonclosout %p\n", nonclosin, nonclosout); 
  //getchar();
  delete inputnonclos;
  GetLevel(OUT) -> GetHURef(SIGNALMO) = GetLevel(IN) -> GetHU(SIGNALMO); 
  delete hsignal;
}

void CompoundHistoUnfolding::CreateTotalMCUnc(ResultLevelCode_t resultcode, RecoLevelCode_t recocode, bool shape)
{
  using namespace TMath;
  printf("void CompoundHistoUnfolding::CreateTotalMCUnc(ResultLevelCode_t resultcode %u, RecoLevelCode_t recocode %u, bool shape %s)\n", resultcode, recocode, shape ? "shape" : "noshape");

  MOCode_t mo = shape ? TOTALMCUNCSHAPE : TOTALMCUNC;
  if (not GetLevel(resultcode) -> GetHURef(mo))
    GetLevel(resultcode) -> GetHURef(mo) = new HistoUnfoldingTH1();
  TH1 *& htotalMCUnc = GetLevel(resultcode) -> GetHU(mo) -> GetTH1Ref(recocode);
  htotalMCUnc = GetLevel(resultcode) -> GetHU(TOTALMCSIGNALPROXY, resultcode) -> Project(recocode, CreateName(shape ? "totalMCUncShape" : "totalMCUnc"), "", _YaxisTitle);
  //  RejectNonReco(htotalMCUnc);
  
  if (recocode == RECO and resultcode == IN)
    {
      ApplyScaleFactor(htotalMCUnc);
    }
  const int nbins = htotalMCUnc -> GetNbinsX();
  map<TString, vector<pair<SampleDescriptor*, float *>>> errors;
  map<TString, map<TString, pair<SampleDescriptor*, vector<TH1 **> >>> errors2D;
  // if (shape)
  //   {
  //     printf("_m2dirsyshistos.size() %lu  _m2dirsyshistos[signaltag].size() %lu _m1dirsyshistos[signaltag].size() %lu\n", 
  // 	     GetLevel(resultcode) -> _m2dirsyshistos.size(), 
  // 	     GetLevel(resultcode) -> _m2dirsyshistos[signaltag].size(),
  // 	     GetLevel(resultcode) -> _m1dirsyshistos[signaltag].size());
  //     getchar();
  //   }
  for (map<TString, vector<array<HistoUnfolding *, 2>>> :: iterator bit = GetLevel(resultcode) -> _m2dirsyshistos.begin(); bit != GetLevel(resultcode) -> _m2dirsyshistos.end(); bit ++)
    {
      TString sample = bit -> first;
      if (sample != signaltag)
	{
	  //continue;
	}
      for (vector<array<HistoUnfolding *, 2>>::iterator it =  bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  TH1 * hsigup   = GetSignalProxy(resultcode, recocode, sample.Data(), (*it)[0] -> GetSysType(), (*it)[0] -> GetTag() );
	  hsigup -> SetName(TString(hsigup -> GetName()) + "_up");
	  TH1 * hsigdown = GetSignalProxy(resultcode, recocode, sample.Data(), (*it)[1] -> GetSysType(), (*it)[1] -> GetTag() );
	  hsigdown -> SetName(TString(hsigdown -> GetName()) + "_down");
	  if (recocode == RECO and resultcode == IN)
	    {
	      ApplyScaleFactor(hsigup);
	      ApplyScaleFactor(hsigdown);
	    }
	  const double integralnominalup   = hsigup   -> Integral();
	  const double integralnominaldown = hsigdown -> Integral();
	  
	  HistoUnfolding * up     = (*it)[0];
	  
	  TH1 *            hup    = up -> Project(recocode, "up");
	  HistoUnfolding * down   = (*it)[1];
	  TH1 *            hdown  = down -> Project(recocode, "down");
	  hup -> SetTitle(up -> GetTitle());
	  hdown -> SetTitle(down -> GetTitle());
	  TString tag = up -> GetTag();
	  tag.ReplaceAll("_up", "");
	  tag.ReplaceAll("m173v5", "topmass");
	  tag.ReplaceAll("m175v5", "topmass");
	  SampleDescriptor * sd = new SampleDescriptor(*(SampleDescriptor *) up);
	  sd -> SetTag(tag.Data());
	  sd -> SetCategory(up -> GetCategory());
	  const TString category = up -> GetCategory(); 
	  if (recocode == RECO and resultcode == IN)
	    {
	      ApplyScaleFactor(hup);
	      ApplyScaleFactor(hdown);
	    }
	  if (shape)
	    {
	      if (hup -> Integral())
		{
		  hup -> Scale(integralnominalup/hup -> Integral());
		}
	      else
		{
		  //	  printf("deleting %s\n", up -> GetTag());
		  delete   hup;
		  delete   hsigup;
		  hup    = nullptr;
		  hsigup = nullptr;
		}
	      if (hdown -> Integral())
		{
		  hdown -> Scale(integralnominaldown/hdown -> Integral());
		}
	      else
		{
		  //printf("deleting %s\n", down -> GetTag());
		  delete     hdown;
		  delete     hsigdown;
		  hdown    = nullptr;
		  hsigdown = nullptr;
		}
	    }
	  // hsig -> Print("all");
	  // hup -> Print("all");
	  // hdown -> Print("all");
	  if (not hsigdown or not hsigup)

	    continue;
	  auto cor = [&nbins] (TH1 * hsigup, TH1 * hsigdown, TH1 * hup, TH1 * hdown)
	    {
	      float cov = 0.0;
	      float sigmaup = 0.0;
	      float sigmadown = 0.0;
	      for (unsigned char bind = 1; bind < nbins + 1; bind ++)
		{
		  cov       += (hup -> GetBinContent(bind) - hsigup -> GetBinContent(bind)) * (hdown -> GetBinContent(bind) - hsigdown -> GetBinContent(bind));
		  sigmaup   += Power(hup ->GetBinContent(bind) - hsigup -> GetBinContent(bind), 2.0);
		  sigmadown += Power(hdown ->GetBinContent(bind) - hsigdown -> GetBinContent(bind), 2.0);
		}	    
	      return cov/Sqrt(sigmaup*sigmadown);
	    };

	  //printf("%s %f\n", (*it)[0] -> GetTag(), cor(hsigup, hsigdown, hup, hdown));
	  errors[sample].push_back(pair<SampleDescriptor*, float*>(sd, new float[nbins]));
	  TH1 ** herror2D = new TH1*[2];
	  herror2D[0] = (TH1* ) hup -> Clone();
	  herror2D[1] = (TH1 *) hdown -> Clone();
	  herror2D[0] -> SetDirectory(nullptr);
	  herror2D[1] -> SetDirectory(nullptr);
	  herror2D[0] -> Reset("ICE");
	  herror2D[1] -> Reset("ICE");
	 
	  if (errors2D[sample].find(category) == errors2D[sample].end())
	    {
	      errors2D[sample][category] = make_pair(sd , vector<TH1**>());
	    }
	  errors2D[sample][category].second.push_back(herror2D);
	  
	  for (unsigned char bind = 1; bind < nbins + 1; bind ++)
	    {
	      const double      sup   = hup -> GetBinContent(bind) - hsigup -> GetBinContent(bind);
	      const double      sdown = hdown -> GetBinContent(bind) - hsigdown -> GetBinContent(bind);
	      
	      const double      diff   = Sqrt((Power(sup, 2) + Power(sdown, 2))/2.0);
	      //	      printf("%u %.9f %.9f\n", bind, sup, sdown); 
	      errors2D[sample][category] . second.back()[0] -> SetBinContent(bind, sup);
	      errors2D[sample][category] . second.back()[1] -> SetBinContent(bind, sdown);

	      errors[sample].back() . second[bind - 1] = diff;

	    }
	  delete hup;
	  delete hdown;
	  delete hsigdown;
	  delete hsigup;
	}
    }
  map<TString, map<TString, pair<SampleDescriptor*,  vector<TH1 *   >>>> errors1D;

  for (map<TString, vector<HistoUnfolding *>> :: iterator bit = GetLevel(resultcode) -> _m1dirsyshistos.begin(); bit != GetLevel(resultcode) -> _m1dirsyshistos.end(); bit ++)
    {
      TString sample = bit -> first;
      if (sample != signaltag)
	{
	  //continue;
	}
      for (vector<HistoUnfolding *>::iterator it =  bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  if (TString((*it) -> GetCategory()) == "Colour flip")
	    continue;
	  // if (TString((*it) -> GetTag()) == "MC13TeV_TTJets2l2nu_amcatnlo")
	  //   continue;
	  TH1 * hsig = GetSignalProxy(resultcode, recocode, sample.Data(), (*it) -> GetSysType(), (*it) -> GetTag());
	  if (recocode == RECO and resultcode == IN)
	    {
	      ApplyScaleFactor(hsig);
	    }
	  const double integralnominal = hsig -> Integral();
	  if (not integralnominal)
	    continue;
	  HistoUnfolding * hu      = *it;
	  //hu -> ls();
	  TH1 *            hsys    = hu -> Project(recocode);


	  SampleDescriptor * sd    = new SampleDescriptor(*hu);
	  // printf("created SD\n");
	  // sd -> ls();
	  if (recocode == RECO and resultcode == IN)
	    {
	      ApplyScaleFactor(hsys);
	    }
	  if (shape and TString(hu -> GetTag()).Contains("non_clos"))
	    {
	      printf("nonclos sig unscaled\n");
	      hsig -> Print("all");
	      printf("nonclos sys unscaled\n");
	      hsys -> Print("all");
	      
	    }

	  if (shape)
	    {
	      hsys -> Scale(integralnominal/hsys -> Integral());
	    }
	  // if (shape and TString(hu -> GetTag()).Contains("non_clos"))
	  //   {
	  //     printf("%p\n", hu);
	  //     printf("nonclos signal\n");
	  //     hsig -> Print("all");
	  //     printf("nonclos sys\n");
	  //     hsys -> Print("all");
	  //     for (unsigned char bind = 1; bind< hsig -> GetNbinsX() + 1; bind ++)
	  // 	printf("%u %f\n", bind, hsys -> GetBinContent(bind) - hsig -> GetBinContent(bind));
	      
	  //   }
	  TH1 * herror1D  = (TH1* ) hsys -> Clone();
	  herror1D -> SetDirectory(nullptr);
	  herror1D -> Reset("ICE");
	  herror1D -> SetTitle((*it) -> GetTitle());
	  const TString category = (*it) -> GetCategory();
	  if (errors1D[sample].find(category) == errors1D[sample].end())
	    {
	      errors1D[sample][category] = make_pair(sd , vector<TH1*>());
	    }
	  errors1D[sample][category].second.push_back(herror1D);
	  errors[sample].push_back(pair<SampleDescriptor*, float*>(sd, new float[nbins]));
	  for (unsigned char bind = 1; bind < nbins + 1; bind ++)
	    {
	      const double      diff   = hsys -> GetBinContent(bind) - hsig -> GetBinContent(bind); 
	      errors[sample].back() . second[bind - 1] = diff;
	      herror1D -> SetBinContent(bind,  diff);
	    }
	  delete hsys;
	  delete hsig;
	}
    }
  for (map<TString, vector<pair<SampleDescriptor*, float *>>> ::iterator bit = errors.begin(); bit != errors.end(); bit ++)
    {
      TString sample = bit -> first;
      for(vector<pair<SampleDescriptor*, float *>>::iterator it = bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  //	  printf("%s %s\n", it -> first -> GetTag(), it -> first -> GetCategory());
	  for (unsigned char bind = 0; bind < nbins; bind ++)
	    {
	      //     printf("[%f] ", it -> second[bind]);
	    }
	  //printf("\n\n");
	}
    }

  // SampleDescriptor * statsd = new SampleDescriptor;
  // statsd -> SetTag("statistics");
  // statsd -> SetCategory("statistics");
  // errors[signaltag].push_back(pair<SampleDescriptor *, float *>(statsd, new float[nbins]));
  float * statistics = new float[nbins];
  for (unsigned char bind = 1; bind < nbins + 1; bind ++)
    {
      statistics[bind - 1] = htotalMCUnc -> GetBinError(bind);
    }
  map<TString, float *> errcat;
  map<TString, map<TString, float *>> errcatmap;
  for (map<TString, vector<pair<SampleDescriptor*, float *>>> ::iterator bit = errors.begin(); bit != errors.end(); bit ++)
    {
      TString sample = bit -> first;
      for(vector<pair<SampleDescriptor*, float *>>::iterator it = bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  const char * category = it -> first -> GetCategory();
	  if (errcat.find(category) == errcat.end())
	    {
	      errcat[category] = new float[nbins];
	      for (unsigned char ind = 0; ind < nbins; ind ++)
		{
		  errcat[category][ind] = 0.0;
		}
	    }
	  
	  if (errcatmap[sample].find(category) == errcatmap[sample].end())
	    {
	      errcatmap[sample][category] = new float[nbins];
	      for (unsigned char ind = 0; ind < nbins; ind ++)
		{
		  errcatmap[sample][category][ind] = 0.0;
		}
	    }
	  
	  for (unsigned char bind = 0; bind < nbins; bind ++)
	    {
	      if (TString(category) != "Colour reconnection" or (TString(category) == "Colour reconnection" and TString(it -> first -> GetTag()).Contains("gluonMove")))
		{
		  errcat[category][bind] = Sqrt(Power(errcat[category][bind], 2) + Power(it -> second[bind], 2));
		}
	      else
		{
		  printf("skipping %s\n", it -> first -> GetTag());
		  getchar();
		}
	      errcatmap[sample][category][bind] = Sqrt(Power(errcatmap[sample][category][bind], 2) + Power(it -> second[bind], 2));
	    }	      
	}
    }
  for (unsigned char bind = 0; bind < nbins; bind ++)
    {
      errcat["Top mass"][bind] *= 1.0/Sqrt(3);
      errcatmap[signaltag]["Top mass"][bind] *= 1.0/Sqrt(3);
    }	      

  // printf("\n Categories \n");
  const unsigned char msize = errcat.size();
  pair<TString, float *> * errcatarr = new pair<TString, float *>[msize];
  unsigned char pind = 0;
  for (map<TString, float *>::iterator it1 = errcat.begin(); it1 != errcat.end(); it1 ++)
    {
      errcatarr[pind] = *it1;
      pind ++;
    }
  auto swap = [](pair<TString, float *> * it1, pair<TString, float *> * it2)
    {
      pair<TString, float *> itswap = * it1;
      *it1 = *it2;
      *it2 = itswap;
    };
  auto sum = [&nbins] (pair<TString, float *> * it) -> float
    {
      float sumf = 0.0;
      for (unsigned char bind = 0; bind < nbins; bind ++)
	{
	  sumf += it -> second[bind];
	}
      return sumf;
    };
  for (unsigned char ind1 = 0; ind1 < msize - 1; ind1 ++)
    {
      
      for (unsigned char ind2 = ind1 + 1; ind2 < msize; ind2 ++)
	{
	  if (sum(errcatarr + ind2) > sum(errcatarr + ind1))
	    swap(errcatarr + ind1 , errcatarr + ind2);
	}

    }
  TH1 * hsigtest =  GetLevel(resultcode) -> GetHU(SIGNALPROXY, resultcode) -> Project(recocode, CreateName("sig"));

  if (shape)
    {
      for(unsigned char ind = 0; ind < msize; ind ++)
	{
	  pair<TString, float*> * it = errcatarr + ind;
	  printf("%s\n", it -> first.Data());
	  for (unsigned char bind = 0; bind < nbins; bind ++)
	    {
	      printf("[%.2f] ", it -> second[bind]/htotalMCUnc -> GetBinContent(bind + 1)*100.0);
	    }
	  printf("\n");
	}
      printf("\n");
    }
  float * errf = new float[nbins];
  for (unsigned char bind = 0; bind < nbins; bind ++)
    {
      errf[bind] = 0.0;
    }

  for(map<TString, float *>::iterator it = errcat.begin(); it != errcat.end(); it ++)
    {
      for (unsigned char bind = 0; bind < nbins; bind ++)
	{
	  errf[bind] = Sqrt(Power(errf[bind], 2) + Power(it -> second[bind],2));
	}
    }
  printf("\n-----------------------------------------\n");
  // printf("signal\n");
  // hsigtest -> Print("all");

  printf("Total syst errror\n");
  for (unsigned char bind = 0; bind < nbins; bind ++)
    {
      printf("[%.2f] ", errf[bind]/htotalMCUnc -> GetBinContent(bind + 1)*100.0);
    }
  printf("\n");
  printf("Stat error\n");
 
  for (unsigned char bind = 0; bind < nbins; bind ++)
    {
      printf("[%.2f] ", hsigtest -> GetBinError(bind + 1)*100.0/htotalMCUnc -> GetBinContent(bind + 1));
    }
  printf("\n------------------------------------------\n");
  printf("Total error\n");
  for (unsigned char bind = 0; bind < nbins; bind ++)
    {
      htotalMCUnc -> SetBinError(bind + 1, Sqrt(Power(htotalMCUnc -> GetBinError(bind + 1), 2) + Power(errf[bind],2)));
      printf("[%.2f] ", htotalMCUnc -> GetBinError(bind + 1)*100.0/htotalMCUnc -> GetBinContent(bind + 1));
    }
  printf("\n");
  if (shape and recocode == GEN and resultcode == OUT)
    {
      FILE * file = fopen((string(_folder) + "/systematics.txt").c_str(), "w");
      fprintf(file, "\\begin{table}[htbp]\n");
      fprintf(file, "\t\\centering\n");
      fprintf(file, "\t\\begin{tabular}{l");
      for (unsigned char bind = 1; bind < htotalMCUnc -> GetNbinsX() + 1; bind ++)
	{
	  fprintf(file, "c");
	}
      fprintf(file, "}\n");
      fprintf(file, "\t\t\\hline\n");
      for (unsigned char bind = 1; bind < htotalMCUnc -> GetNbinsX() + 1; bind ++)
	{
	  fprintf(file, "\t\t & %.2f \\textendash\\ %.2f", htotalMCUnc -> GetBinLowEdge(bind), htotalMCUnc -> GetXaxis() -> GetBinUpEdge(bind));
	}
      fprintf(file, "\\\\\n");
      fprintf(file, "\t\t\\hline\n");
      for(unsigned char ind = 0; ind < msize; ind ++)
	{
	  pair<TString, float*> * it = errcatarr + ind;
	  fprintf(file, "\t\t%s ", it -> first.Data());
	  for (unsigned char bind = 1; bind < nbins + 1; bind ++)
	    {
	      fprintf(file, "\t & %.2f ", it -> second[bind - 1]/htotalMCUnc -> GetBinContent(bind) * 100.0);
	    }
	  fprintf(file, "\\\\\n");
	}
      fprintf(file, "\t\t\\hline\n");
      fprintf(file, "\t\tTotal syst errror");
      for (unsigned char bind = 0; bind < nbins; bind ++)
	{
	  fprintf(file, " & %.2f ", errf[bind]/htotalMCUnc -> GetBinContent(bind + 1)*100.0);
	}
      fprintf(file, "\\\\\n\t\t\\hline\n");
      fprintf(file, "\t\tStatistics");
 
      for (unsigned char bind = 0; bind < nbins; bind ++)
	{
	  fprintf(file, "\t & %.2f ", hsigtest -> GetBinError(bind + 1)*100.0/htotalMCUnc -> GetBinContent(bind + 1));
	}
      fprintf(file, "\t\t\\\\\n\t\t\\hline\n");
      fprintf(file, "\t\tTotal error");
      for (unsigned char bind = 0; bind < nbins; bind ++)
	{
	  fprintf(file, "\t & %.2f ", htotalMCUnc -> GetBinError(bind + 1)*100.0/htotalMCUnc -> GetBinContent(bind + 1));
	}
      fprintf(file, "\t\t\\\\\n\t\t\\hline\n");
      fprintf(file, "\t\\end{tabular}\n");
      fprintf(file, "\t\\caption{Statistical uncertainties}\n");
      fprintf(file, "\t\\label{tab:statunc}\n");
      fprintf(file, "\\end{table}\n");
      fclose(file);
    }
  delete hsigtest;


  map<TString, map<TString, pair<SampleDescriptor*, THStack *>>> stacks;
  map<TString, map<TString, pair<SampleDescriptor*, TLegend *>>> legends;
  for (map<TString, map<TString, pair<SampleDescriptor*, vector<TH1 **>>>> ::iterator bit = errors2D.begin(); bit != errors2D.end(); bit ++)
    {
      TString sample = bit -> first;
      auto swap = [](vector<TH1 **>::iterator it1, vector<TH1 **> :: iterator it2 )
	{
	  TH1 ** itswap = * it1;
	  *it1 = *it2;
	  *it2 = itswap;
	};
      auto sum = [&nbins] (vector<TH1 **> :: iterator it) -> float
	{
	  float sumf = 0.0;
	  for (unsigned char bind = 1; bind < nbins + 1; bind ++)
	    {
	      sumf += TMath::Sqrt(TMath::Power(sumf, 2) + (TMath::Power((*it)[0] -> GetBinContent(bind), 2) + TMath::Power((*it)[1] -> GetBinContent(bind), 2))/2.0);
	    }
	  return sumf;
	};
      for ( map<TString, pair<SampleDescriptor*, vector<TH1 **>>> ::iterator it = bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  for(vector<TH1 **>::iterator it1 = it -> second.second.begin(); it1 != it -> second.second.end(); it1 ++)
	    {
	      for(vector<TH1 **>::iterator it2 = it1 + 1; it2 != it -> second.second.end(); it2 ++)
		{
		  if (sum (it1) < sum(it2))
		    swap(it1, it2);
		}
	    }
	}
    }

  
  for (map<TString, map<TString, pair<SampleDescriptor*, vector<TH1 *>>>> ::iterator bit = errors1D.begin(); bit != errors1D.end(); bit ++)
    {
      TString sample = bit -> first;
      auto swap = [](vector<TH1 *>::iterator it1, vector<TH1 *> :: iterator it2 )
	{
	  TH1 * itswap = * it1;
	  *it1 = *it2;
	  *it2 = itswap;
	};
      auto sum = [&nbins] (vector<TH1 *> :: iterator it) -> float
	{
	  float sumf = 0.0;
	  for (unsigned char bind = 1; bind < nbins + 1; bind ++)
	    {
	      sumf += TMath::Sqrt(TMath::Power(sumf, 2) + TMath::Power((*it) -> GetBinContent(bind), 2));
	    }
	  return sumf;
	};
      for ( map<TString, pair<SampleDescriptor*, vector<TH1 *>>> ::iterator it = bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  for(vector<TH1 *>::iterator it1 = it -> second.second.begin(); it1 != it -> second.second.end(); it1 ++)
	    {
	      for(vector<TH1 *>::iterator it2 = it1 + 1; it2 != it -> second.second.end(); it2 ++)
		{
		  if (sum (it1) < sum(it2))
		    swap(it1, it2);
		}
	    }
	}
    }

  map<TString, map<TString, FILE *>> errfiles2D;


  for (map<TString, map<TString, pair<SampleDescriptor*, vector<TH1 **>>>> ::iterator bit = errors2D.begin(); bit != errors2D.end(); bit ++)
    {
      TString sample = bit -> first;
      for(map<TString, pair<SampleDescriptor*, vector<TH1 **>>>::iterator it = bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  const char * category = it -> second .first -> GetCategory();
	  printf("it -> first %s %s\n", it -> first.Data(), category);
	  if (stacks[sample].find(category) == stacks[sample].end())
	    {
	      stacks[sample][category] = make_pair(it -> second.first, new THStack);//make_pair(it -> second.first, vector<THStack * >());
	      TLegend * legend = new TLegend(0.3, 0.50, 0.94, 0.93);
	      legends[sample][category] = make_pair(it -> second.first, legend);
	      TString filename = TString(_folder) + "/" + sample + "_" + category;
	      if (shape)
		filename += "_shape";
	      filename += "2D.txt";
	      filename.ReplaceAll(" ", "_").ReplaceAll("-", "_");
	      printf("opening %s\n", filename.Data());
	      FILE * file = fopen(filename.Data(), "w");
	      errfiles2D[sample][category] = file;
	      fprintf(file, "\\begin{table}[htbp]\n");
	      fprintf(file, "\t\\centering\n");
	      fprintf(file, "\t\\begin{tabular}{p{0.3\\textwidth}");
	      const unsigned char dn = 2;
	      for (unsigned char bind = 1; bind < htotalMCUnc -> GetNbinsX() + 1 + dn; bind ++)
		{
		  fprintf(file, "r");
		}
	      fprintf(file, "}\n");
	      fprintf(file, "\t\t\\hline\n");
	      fprintf(file, "\t\t \\makecell[c]{\\multirow{2}{*}{Process}} & \\multicolumn{%u}{c}{Bins} & \\makecell[c]{\\multirow{2}{*}{Correlation}} & \\makecell[c]{\\multirow{2}{*}{D}}\\\\\n", nbins);
	      for (unsigned char bind = 1; bind < htotalMCUnc -> GetNbinsX() + 1; bind ++)
		{
		  fprintf(file, "\t\t & %.2f \\textendash\\ %.2f", htotalMCUnc -> GetBinLowEdge(bind), htotalMCUnc -> GetXaxis() -> GetBinUpEdge(bind));
		}
	      fprintf(file, " & & \\\\\n");
	      fprintf(file, "\t\t\\hline\n");
	    }
	  const static Color_t colors[7] = {kSpring, kPink, kViolet, kCyan, kMagenta, kOrange, kGreen + 2};
	  unsigned char ind = 0;
	  printf("sample %s category %s\n", sample.Data(), category);
	  FILE *file = errfiles2D[sample][category];
	  TH1 * htotal = (TH1*) htotalMCUnc -> Clone();
	  htotal -> Reset("ICE");
	  auto RMS = [&nbins] (TH1 * h) -> float
	    {
	      float RMS = 0.0;
	      const float width = h -> GetXaxis() -> GetXmax() - h -> GetXaxis() -> GetXmin();
	      for (unsigned char bind = 1; bind < nbins + 1; bind ++)
		{
		  //		  printf("%u %f %f %f %f\n", bind, h -> GetBinContent(bind), h -> GetBinWidth(bind)/width, h -> GetBinContent(bind) * h -> GetBinWidth(bind)/width, RMS);
		  RMS       = Sqrt(Power(RMS, 2) + Power(h -> GetBinContent(bind) * h -> GetBinWidth(bind)/width, 2));
		}
	      //	      printf("RMS %f\n", RMS);
	      return RMS;
	    };
	  for (vector<TH1 **>::iterator th1it = it -> second.second.begin(); th1it != it -> second.second.end(); th1it ++)
	    {
	      (*th1it)[0] -> SetMarkerStyle(kOpenTriangleUp);
	      (*th1it)[1] -> SetMarkerStyle(kOpenTriangleDown);
	      (*th1it)[0] -> SetMarkerSize(2);
	      (*th1it)[1] -> SetMarkerSize(2);
	      (*th1it)[0] -> SetLineColor(colors[ind]);
	      (*th1it)[1] -> SetLineColor(colors[ind]);
	      (*th1it)[0] -> SetMarkerColor(colors[ind]);
	      (*th1it)[1] -> SetMarkerColor(colors[ind]);
	      (*th1it)[0] -> SetLineWidth(2.0);
	      (*th1it)[1] -> SetLineWidth(2.0);
	      ind ++;
	      stacks[sample][category].second -> Add((*th1it)[0], "HISTO");
	      stacks[sample][category].second -> Add((*th1it)[1], "HISTO");
	      stacks[sample][category].second -> Add((*th1it)[0], "HISTOP");
	      stacks[sample][category].second -> Add((*th1it)[1], "HISTOP");
	      NormaliseToBinWidth((*th1it)[0]);
	      NormaliseToBinWidth((*th1it)[1]);
	      for (unsigned char bind = 1; bind < nbins + 1; bind ++)
		{
		  htotal -> SetBinContent(bind, 
					  TMath::Sqrt(TMath::Power(htotal -> GetBinContent(bind), 2) + 
						      (TMath::Power((*th1it)[0] -> GetBinContent(bind), 2) + TMath::Power((*th1it)[1] -> GetBinContent(bind), 2))/2.0));
		}
	      
	      legends[sample][category].second -> AddEntry((*th1it)[0], (*th1it)[0] -> GetTitle(), "lp");
	      legends[sample][category].second -> AddEntry((*th1it)[1], (*th1it)[1] -> GetTitle(), "lp");
	      auto cor = [&nbins] (TH1 * hup, TH1 * hdown) -> float
		{
		  float cov = 0.0;
		  float sigmaup = 0.0;
		  float sigmadown = 0.0;
		  const float width = hup -> GetXaxis() -> GetXmax() - hup -> GetXaxis() -> GetXmin();
		  for (unsigned char bind = 1; bind < nbins + 1; bind ++)
		    {
		      cov       += hup -> GetBinContent(bind) * TMath::Power(hup -> GetBinWidth(bind)/width, 2) * hdown -> GetBinContent(bind) ;
		      sigmaup   += Power(hup -> GetBinContent(bind) * hup -> GetBinWidth(bind)/width, 2.0);
		      sigmadown += Power(hdown ->GetBinContent(bind) * hup -> GetBinWidth(bind)/width, 2.0);
		    }	    
		  return cov/Sqrt(sigmaup*sigmadown);
		};
	      

	      fprintf(file, "\t\t $\\rm %s$", TString((*th1it)[0] -> GetTitle()).ReplaceAll("#", "\\").ReplaceAll(" ", "\\ ").ReplaceAll("_", "\\_").Data());
	      for (unsigned char bind = 1; bind < nbins + 1; bind ++)
		{
		  fprintf(file, " & %.2f ", (*th1it)[0] -> GetBinContent(bind));
		}	      
	      fprintf(file, " & \\multirow{2}{*}{%.2f}", cor((*th1it)[0], (*th1it)[1]));  
	      fprintf(file, " & %.1f \\\\\n", RMS((*th1it)[0]));  
	      fprintf(file, "\t\t $\\rm %s$", TString((*th1it)[1] -> GetTitle()).ReplaceAll("#","\\").ReplaceAll(" ","\\ ").ReplaceAll("_", "\\_").Data());
	      for (unsigned char bind = 1; bind < nbins + 1; bind ++)
		{
		  fprintf(file, " & %.2f ", (*th1it)[1] -> GetBinContent(bind));
		}	      
	      fprintf(file, " & ");  
	      fprintf(file, " & %.1f \\\\\n\\hline\n", RMS((*th1it)[1]));  
	      fprintf(file, "\t\t Subtotal");
	      TH1 * subtotal = (TH1 *) (*th1it)[0] -> Clone();
	      
	      for (unsigned char bind = 1; bind < nbins + 1; bind ++)
		{
		  const float unc = TMath::Sqrt((TMath::Power((*th1it)[0] -> GetBinContent(bind), 2) + TMath::Power((*th1it)[1] -> GetBinContent(bind), 2))/2.0);
		  fprintf(file, " & %.2f ", unc );
		  subtotal -> SetBinContent(bind, unc);

		}	      

	      fprintf(file, "& & %.2f \\\\\n\t\t\\hline\n", RMS(subtotal));
	      delete subtotal;
	    }
	  
	  fprintf(file, "\t\tTotal");
	  //NormaliseToBinWidth(htotal);
	  if (TString(category) == "Top mass")
	    htotal -> Scale(1.0/Sqrt(3.0));
	  for (unsigned char bind = 1; bind < nbins + 1; bind ++)
	    {
	      fprintf(file, " & %.2f ", htotal -> GetBinContent(bind));
	    }	      
	  fprintf(file, " & & %.2f \\\\\n\\hline\n", RMS(htotal));
	  fprintf(file, "\t\\end{tabular}\n");
	  fprintf(file, "\t\\caption{}\n");
	  fprintf(file, "\t\\label{tab:uncat_%s_%s_%s_%s%s_2D}\n", sample.Data(), TString(category).ReplaceAll(" ", "_").ReplaceAll("-", "_").Data(), tag_recolevel[recocode], tag_resultlevel[resultcode], shape ? "_shape" : "");
	  fprintf(file, "\\end{table}\n");
	  fclose(file);
	  delete htotal;
	      
	}
    }
  map<TString, map<TString, FILE *>> errfiles1D;

  for (map<TString, map<TString, pair<SampleDescriptor*, vector<TH1 *>>>> ::iterator bit = errors1D.begin(); bit != errors1D.end(); bit ++)
    {
      TString sample = bit -> first;
      for(map<TString, pair<SampleDescriptor*, vector<TH1 *>>>::iterator it = bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  const char * category = it -> second .first -> GetCategory();
	  map<TString, pair<SampleDescriptor*, THStack *>>:: iterator stackit = stacks[sample].find(category);
	  SampleDescriptor *sd = it -> second.first;
	  if (stackit == stacks[sample].end())
	    {
	      stacks[sample][category] = make_pair(sd, new THStack);//make_pair(it -> second.first, vector<THStack * >());
	      TLegend * legend = new TLegend(0.3, 0.50, 0.94, 0.93);
	      legends[sample][category] = make_pair(it -> second.first, legend);
	    }
	  if(errfiles1D[sample].find(category) == errfiles1D[sample].end())
	    {
	      TString filename = TString(_folder) + "/" + sample + "_" + category;
	      if (shape)
		filename += "_shape";
	      filename += "1D.txt";
	      filename.ReplaceAll(" ", "_").ReplaceAll("-", "_");
	      FILE * file = fopen(filename.Data(),"w");
	      errfiles1D[sample][category] = file;
	      fprintf(file, "\\begin{table}[htbp]\n");
	      fprintf(file, "\t\\centering\n");
	      fprintf(file, "\t\\begin{tabular}{p{0.3\\textwidth}");
	      const unsigned char dn = 1;
	      for (unsigned char bind = 1; bind < htotalMCUnc -> GetNbinsX() + 1 + dn; bind ++)
		{
		  fprintf(file, "r");
		}
	      fprintf(file, "}\n");
	      fprintf(file, "\t\t\\hline\n");
	      fprintf(file, "\t\t \\makecell[c]{\\multirow{2}{*}{Process}} & \\multicolumn{%u}{c}{Bins} &  \\makecell[c]{\\multirow{2}{*}{D}}\\\\\n", nbins);
	      fprintf(file, " \n\t\t\t");
	      for (unsigned char bind = 1; bind < htotalMCUnc -> GetNbinsX() + 1; bind ++)
		{
		  fprintf(file, " & \\makecell[c]{%.2f \\textendash\\ %.2f} ", htotalMCUnc -> GetBinLowEdge(bind), htotalMCUnc -> GetXaxis() -> GetBinUpEdge(bind));
		}
	      fprintf(file, " & \\\\\n");
	      fprintf(file, "\t\t\\hline\n");
	      
	    }
	  const static Color_t colors[6] = {kBlue, kGreen, kRed, kBlue + 3, kGreen + 3, kRed + 3};
	  unsigned char ind = 0;
	  TH1 * htotal = (TH1*) htotalMCUnc -> Clone();
	  htotal -> Reset("ICE");
	  FILE * file = errfiles1D[sample][category];
	  auto RMS = [&nbins] (TH1 * h) -> float
	    {
	      float RMS = 0.0;
	      const float width = h -> GetXaxis() -> GetXmax() - h -> GetXaxis() -> GetXmin();
	      for (unsigned char bind = 1; bind < nbins + 1; bind ++)
		{
		  RMS       = Sqrt(Power(RMS, 2) + Power(h -> GetBinContent(bind) * h -> GetBinWidth(bind)/width, 2));
		}
	      return RMS;
	    };
	  for (vector<TH1 *>::iterator th1it = it -> second.second.begin(); th1it != it -> second.second.end(); th1it ++)
	    {
	      (*th1it) -> SetLineColor(colors[ind]);
	      (*th1it) -> SetLineWidth(2.0);
	      ind ++;
	      NormaliseToBinWidth(*th1it);
	      stacks[sample][category] . second -> Add(*th1it);
	      legends[sample][category]. second -> AddEntry(*th1it, (*th1it) -> GetTitle(), "lp"); 
	      

	      fprintf(file, "\t\t $\\rm %s$", TString((*th1it) -> GetTitle()).ReplaceAll("#", "\\").ReplaceAll(" ", "\\ ").ReplaceAll("_", "\\_").Data());
	      for (unsigned char bind = 1; bind < nbins + 1; bind ++)
		{
		  fprintf(file, " & %.2f ", (*th1it) -> GetBinContent(bind));
		  htotal -> SetBinContent(bind, TMath::Sqrt(TMath::Power(htotal -> GetBinContent(bind), 2) + TMath::Power((*th1it) -> GetBinContent(bind), 2)));
		}	      
	      fprintf(file, " & %.2f \\\\\n", RMS(*th1it));  
	    }
	  fprintf(file, "\t\t\\hline\n");
	  fprintf(file, "\t\tTotal ");
	  //NormaliseToBinWidth(htotal);
	  for (unsigned char bind = 1; bind < nbins + 1; bind ++)
	    {
	      fprintf(file, " & %.2f ", htotal -> GetBinContent(bind));
	    }
	  fprintf(file, " & %.2f \\\\\n", RMS(htotal));
	  fprintf(file, "\t\t\\hline\n");
	  fprintf(file, "\t\\end{tabular}\n");
	  fprintf(file, "\t\\caption{}\n");
	  fprintf(file, "\t\\label{tab:uncat_%s_%s_%s_%s%s_1D}\n", sample.Data(), TString(category).ReplaceAll(" ", "_").ReplaceAll("-", "_").Data(), tag_recolevel[recocode], tag_resultlevel[resultcode], shape ? "_shape" : "");
	  fprintf(file, "\\end{table}\n");
	  fclose(file);
	  delete htotal;
	
	}
    }
  gStyle -> SetOptStat(0);
  gStyle -> SetOptTitle(0);
  for (map<TString, map<TString, pair<SampleDescriptor*, THStack *>>>::iterator bit = stacks.begin(); bit != stacks.end(); bit ++)
    {
      const TString sample = bit -> first;
      for(map<TString, pair<SampleDescriptor*, THStack *>>::iterator it = bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  const TString category = it -> first;
	  TString name = bit -> first + "_" + it -> first + "_" + tag_recolevel[recocode] + "_" + tag_resultlevel[resultcode];
	  name.ReplaceAll(" ", "_");
	  if (shape)
	    {
	      name += "_shape";
	    }
	  TCanvas c("c", "c", 800, 800);
	  c.SetTopMargin(0.05);
	  c.SetRightMargin(0.02);
	  c.SetLeftMargin(0.2);
	  c.SetBottomMargin(0.15);
	  THStack * stack = it -> second.second;
	  TH1 * hframe = GetLevel(IN)/*level*/ -> GetHU(SIGNAL/*SIGNALPROXY*/, resultcode) -> Project(recocode, CreateName("frame"), _XaxisTitle, _YaxisTitle);
	  hframe -> Reset("ICE");
	  float * errit = errcatmap.at(sample).at(category);
	  
	  for (unsigned char bind = 1; bind < hframe -> GetNbinsX() + 1; bind ++)
	    {
	      hframe -> SetBinContent(bind, errit[bind -1]);
	    }
	  hframe -> SetLineWidth(3.5);
	  hframe -> SetLineColor(kBlack);
	  TLine line(hframe -> GetXaxis() -> GetXmin(), 0.0, hframe -> GetXaxis() -> GetXmax(), 0.0);
	  line.SetLineColor(kBlack);
	  line.SetLineStyle(kDashed);
	  NormaliseToBinWidth(hframe);
	  stack -> Add(hframe);
	  hframe -> SetMinimum(stack -> GetMinimum("nostack") * 1.2);
	  const float maximum = stack -> GetMaximum("nostack") > hframe -> GetMaximum() ? stack -> GetMaximum("nostack") : hframe -> GetMaximum();
	  hframe -> SetMaximum(maximum * 4.0);
	  hframe -> GetYaxis() -> SetLabelSize(0.20*0.2/0.8);
	  hframe -> GetYaxis() -> SetTitleSize(0.21*0.2/0.8);
  
	  hframe -> GetYaxis() -> SetTitleOffset(0.45*0.8/0.2);
	  hframe -> GetXaxis() -> SetLabelSize(0.20*0.2/0.8);
	  hframe -> GetXaxis() -> SetTitleSize(0.21*0.2/0.8);
  
	  hframe -> GetXaxis() -> SetTitleOffset(0.30*0.8/0.2);

	  hframe -> Draw();
	  TLegend * legend = legends[bit -> first][it -> first].second;
	  TLegendEntry * hframeentry = new TLegendEntry(hframe, TString(category) + " (total)" , "lp");
	  // printf("%s// \n", legend -> GetListOfPrimitives() -> At(0) -> ClassName());
	  // getchar();
	  legend -> GetListOfPrimitives() -> AddAt(hframeentry, 0);
	  legend -> SetBorderSize(0);
	  legend -> SetFillStyle(0);
	  legend -> SetTextFont(42);
	  legend -> SetTextSize(0.025); 
	  stack -> Draw("NOSTACKSAME");
	  legend -> Draw("SAME");
	  line.Draw("SAME");
	  c.SaveAs(TString(_folder) + "/" + name.ReplaceAll("-", "_") + ".png");
	  delete hframe;
	}
    }

  for (map<TString, map<TString, pair<SampleDescriptor*, THStack *>>>::iterator bit = stacks.begin(); bit != stacks.end(); bit ++)
    {
      for(map<TString, pair<SampleDescriptor*, THStack *>>::iterator it = bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  delete it -> second.second;
	}
    }
  for (map<TString, map<TString, pair<SampleDescriptor*, TLegend *>>>::iterator bit = legends.begin(); bit != legends.end(); bit ++)
    {
      for(map<TString, pair<SampleDescriptor*, TLegend *>>::iterator it = bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  delete it -> second.second;
	}
    }
  
  for (map<TString, map<TString, pair<SampleDescriptor*, vector<TH1 **>>>> ::iterator bit = errors2D.begin(); bit != errors2D.end(); bit ++)
    {
      for(map<TString, pair<SampleDescriptor*, vector<TH1 **>>>::iterator it = bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  for (vector<TH1 **>::iterator th1it = it -> second.second.begin(); th1it != it -> second.second.end(); th1it ++)
	    {
	      delete (*th1it)[0];
	      delete (*th1it)[1];
	      delete [] *th1it;
	    }
	  
	}
    }


  for (map<TString, map<TString, pair<SampleDescriptor*, vector<TH1 *>>>> ::iterator bit = errors1D.begin(); bit != errors1D.end(); bit ++)
    {
      for(map<TString, pair<SampleDescriptor*, vector<TH1 *>>>::iterator it = bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  for (vector<TH1 *>::iterator th1it = it -> second.second.begin(); th1it != it -> second.second.end(); th1it ++)
	    {
	      delete *th1it;
	    }
	  
	}
    }


  for (map<TString, vector<pair<SampleDescriptor*, float *>>> ::iterator bit = errors.begin(); bit != errors.end(); bit ++)
    {
      TString sample = bit -> first;
      for(vector<pair<SampleDescriptor*, float *>>::iterator it = bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  delete    it -> first;
	  delete [] it -> second;
	}
    }

  for(map<TString, float *>::iterator it = errcat.begin(); it != errcat.end(); it ++)
    {
      delete [] it -> second; 
    }

  for(map<TString, map<TString, float *>>::iterator bit = errcatmap.begin(); bit != errcatmap.end(); bit ++)
    {
      for(map<TString, float *>::iterator it = errcatmap[bit -> first].begin(); it != errcatmap[bit -> first].end(); it ++)
	{
           
	  delete [] it -> second; 
	}
    }
  delete [] errf;
  delete [] errcatarr;

  //  NormaliseToBinWidth(htotalMCUnc);
}

void CompoundHistoUnfolding::ApproximateTheorSys()
{
  if (string(signaltag).compare("MC13TeV_TTJets") == 0)
    return;
  TH2F * hMC13TeV_TTJets = _chunominal -> GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2();
  for (vector<HistoUnfolding *>::iterator it = GetV(IN, SYSMO, "MC13TeV_TTJets") -> begin(); it != GetV(IN, SYSMO, "MC13TeV_TTJets") -> end(); it ++)
    {
      if ((*it) -> GetSysType() != THEORSYS)
	continue;
      if (TString((*it) -> GetTag()) == "MC13TeV_TTJets")
	{
	  continue;
	}
      TH2F * h = (*it) -> GetTH2();
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
    delete totalbackground;
 for (map<TString, HistoUnfolding *>::iterator it = aggrbackgroundMC.begin(); it != aggrbackgroundMC.end(); it ++)
    {
      if (not totalbackground)
	{
	  totalbackground = new HistoUnfoldingTH2();
	  totalbackground -> GetTH2Ref() = (TH2F*) it -> second -> GetTH2() -> Clone(CreateName("totalbackground"));
	  totalbackground -> GetTH2() -> SetDirectory(nullptr);
	}
      else
        totalbackground -> GetTH2() -> Add(it -> second -> GetTH2());
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
      GetLevel(resultcode) -> GetHU(SIGNALPROXY, OUT) -> GetTH1(RECO) -> Print("all");
      totalMC -> GetTH1Ref(RECO) = GetLevel(resultcode) -> GetHU(SIGNALPROXY, OUT) -> Project(RECO, CreateName("MCtotalOUTRECO"));
      TH1F *bckgsfRECO = totalbackground -> Project(RECO);
      //ApplyScaleFactor(bckgsfRECO);
      totalMC -> GetTH1(RECO) -> Add(bckgsfRECO);
      GetLevel(resultcode) -> GetHURef(DATAMO)  = new HistoUnfoldingTH1((HistoUnfoldingTH1*) GetLevel(resultcode) -> GetHU(DATAMBCKG), "data_out");
      GetLevel(resultcode) -> GetHU(DATAMO) -> GetTH1(RECO) -> Add(bckgsfRECO);
      GetLevel(resultcode) -> GetHU(DATAMO) -> GetTH1(RECO) -> SetTitle(TString("data (") + 
									GetLevel(IN) -> GetHU(SIGNALMO) -> GetTitle() + 
									") folded back + bckg");
      delete bckgsfRECO;
      totalMC -> GetTH1Ref(GEN) = GetLevel(resultcode) -> GetHU(SIGNALPROXY, OUT) -> Project(GEN, CreateName("MCtotalOUTGEN"));
      TH1F *bckgGEN = totalbackground -> Project(GEN);
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

THStack * CompoundHistoUnfolding::CreateTHStack(RecoLevelCode_t recocode, ResultLevelCode_t resultcode)
{
  Format();
  // for (vector<HistoUnfolding *>::iterator it = _vbackgroundhistos.begin(); it !=  _vbackgroundhistos.end(); it ++)
  //   {
  //     ApplyScaleFactor((*it) -> GetTH2());
  //   }
  // ApplyScaleFactor(GetLevel(IN) -> GetHU(DATA) -> GetTH2());

  Level * level = GetLevel(resultcode);
  Level::ProjectionDeco * projdeco = level -> GetProjectionDeco(recocode);
  TLegend *& legend = projdeco -> legend;
  if (legend)
    {
      delete legend;
      legend = nullptr;
    }

  legend = new TLegend(0.52, 0.47, 0.94, 0.93);
  if (projdeco -> _datagr)
    {
      legend -> AddEntry(projdeco -> _datagr, resultcode == IN ? "data" : level -> GetHU(DATAMO) -> GetTH1(recocode) -> GetTitle());
    }
  TH1 * hsig = GetLevel(IN)/*level*/ -> GetHU(SIGNAL/*SIGNALPROXY*/, resultcode) -> Project(recocode, CreateName("sigstack"), "", _YaxisTitle);
  TH1 * htotalmc = GetLevel(IN) -> GetHU(TOTALMCSIGNALPROXY, IN/*resultcode*/) -> Project(recocode, CreateName("totalmc"), "", _YaxisTitle);
  if (recocode == RECO and resultcode == IN)
    {
    ApplyScaleFactor(hsig);
    ApplyScaleFactor(htotalmc);
    }
  NormaliseToBinWidth(htotalmc);
  NormaliseToBinWidth(hsig);
  // RejectNonReco(htotalmc);
  // RejectNonReco(hsig);

  const float totalmcarea(calculatearea(htotalmc));
  if (recocode == RECO and resultcode == IN)
    {
      printf("printing signal\n");
      hsig -> Print("all");
      printf("printing total mc\n");
      htotalmc -> Print("all");
      //getchar();
    }
  hsig -> Scale(1.0/totalmcarea);
  char legendtitle[128];
  if (recocode == RECO)
    sprintf(legendtitle, "%s", level -> GetHU(SIGNALMO) -> GetTitle());
  else
    {
      if (string(method).compare("nominal") == 0)
	sprintf(legendtitle, "%s", "POWHEG+PYTHIA8");
      else if (string(method).compare("cflip"))
	sprintf(legendtitle, "%s", "POWHEG+PYTHIA8 cflip");
      else
	sprintf(legendtitle, "%s", level -> GetHU(SIGNALMO) -> GetTitle());
    }
  hsig -> SetLineWidth(2.0);
    legend -> AddEntry(hsig, legendtitle, "f");

  THStack * stack = new THStack("mc", "mc");
  TH1F * hbackgrounds[nbckg];

  for (unsigned char bin_ind = 0; bin_ind < nbckg; bin_ind ++)
    {
      HistoUnfolding * it = aggrbackgroundMC[bckgtitles[bin_ind]];
      TH1F * hbackground = it -> Project(recocode, CreateName(TString(it -> GetTag()) + "background"), "", _YaxisTitle);
      if (recocode == RECO and resultcode == IN)
	ApplyScaleFactor(hbackground);
      hbackgrounds[bin_ind] = hbackground;
      /*if (recocode == RECO)
	ApplyScaleFactor(hbackground);*/
      NormaliseToBinWidth(hbackground);
      hbackground -> Scale(1.0/totalmcarea);
      stack -> Add(hbackground);
      //delete hbackground;
    }
  for (char bin_ind = nbckg - 1; bin_ind > -1; bin_ind --)
    {
      
      TH1F * hbackground = hbackgrounds[bin_ind];
      if (hbackground -> Integral() > 0.0)

	legend -> AddEntry(hbackground, bckgtitles[bin_ind], "f");
    }  
  stack -> Add(hsig);
  delete htotalmc;
  return stack;
}
 

   
TPad * CompoundHistoUnfolding::CreateMainPlot(RecoLevelCode_t recocode, ResultLevelCode_t resultcode)
{ 

  Format();
  Level * level = GetLevel(resultcode);
  level -> Format();
  Level::ProjectionDeco * projdeco = level -> GetProjectionDeco(recocode);
  THStack * stack = CreateTHStack(recocode, resultcode);
  //TH1F * totalMC = level -> GetHU(TOTALMC) -> Project(recocode, CreateName("totalMCmp"));
  //  TH1F * data = nullptr;
  TH1F * htotalmc = level -> GetHU(TOTALMC) -> Project(recocode, CreateName("totalmc"), "", _YaxisTitle);
  NormaliseToBinWidth(htotalmc);
  const double totalmcarea(calculatearea(htotalmc));
  
  TH1 * totalMCUnc = (TH1 *) level -> GetHU(TOTALMCUNC) -> GetTH1(recocode) -> Clone(TString(level -> GetHU(TOTALMCUNC) -> GetTH1(recocode) -> GetName()) + "drawingclone");
  totalMCUnc -> SetDirectory(nullptr);
  _garbage.push_back(totalMCUnc);
  NormaliseToBinWidth(totalMCUnc);
  totalMCUnc -> Scale(1.0/calculatearea(totalMCUnc));
  //totalMCUnc -> Print("all");
 
  TH1 * totalMCUncShape = (TH1 *) level -> GetHU(TOTALMCUNCSHAPE) -> GetTH1(recocode) -> Clone(TString(level -> GetHU(TOTALMCUNCSHAPE) -> GetTH1(recocode) -> GetName()) + "drawingclone");
  totalMCUncShape -> SetDirectory(nullptr);
  _garbage.push_back(totalMCUncShape);
  //printf("$$$$$$$$$$$$$ unc %p uncshape %p\n", totalMCUnc, totalMCUncShape);
  NormaliseToBinWidth(totalMCUncShape);
  totalMCUncShape -> Scale(1.0/calculatearea(totalMCUncShape));
  //TH1F * hoverlay = nullptr;
  TLegend * legend = projdeco -> legend;
  /*if (recocode == RECO and resultcode == IN)
    ApplyScaleFactor(totalMC);*/
  /*if (resultcode == OUT )
    {
      hoverlay = GetLevel(IN) -> GetHU(SIGNALMO) -> Project(recocode, CreateName("overlay"), "", _YaxisTitle);
      hoverlay -> SetLineColor(kRed);
      TH1F *bckgsf = (TH1F*) totalbackground  -> Project(recocode, CreateName("bckgsf"), "", _YaxisTitle);
      //ApplyScaleFactor(bckgsf);
      hoverlay -> Add(bckgsf);
      NormaliseToBinWidth(hoverlay);
      delete bckgsf;
      }*/
  //NormaliseToBinWidth(totalMC);
  /*if (resultcode == OUT)
    {
      projdeco -> _grtotalMC = new TGraphAsymmErrors(totalMC);
    }*/
  double maximum = 0.0;
  if (recocode == RECO)
    {
      const double grmaximum = TMath::MaxElement(projdeco -> _datagr -> GetN(), projdeco -> _datagr -> GetY());
      maximum = stack -> GetMaximum() > grmaximum ? 2.5 * stack -> GetMaximum() : 2.5 * grmaximum;
      
    }
  else
    maximum = 1.5 *stack -> GetMaximum();
  // stack -> SetMaximum(maximum);
  // stack -> SetMinimum(0.8 * stack -> GetMinimum();
  TPad *pad = new TPad(CreateName("pad1"), CreateTitle("pad1"), 0.0, 0.2, 1.0, 1.0);
  pad -> Draw();
  pad -> cd();
  TH1 * hframe = (TH1 *) ((TH1*)stack -> GetHists() -> At(0)) -> Clone(CreateName("hframe"));
  hframe -> Reset("ICE");
  hframe -> SetMaximum(maximum);
  //stack -> SetMaximum(maximum);
  if (TString(observable) == "pull_angle" and recocode == GEN)
    hframe -> SetMinimum(0.85 * totalMCUnc -> GetMinimum());
  // hframe -> GetYaxis() -> SetLabelSize(0.20);
  // hframe -> GetYaxis() -> SetTitleSize(0.21);
  // hframe -> GetYaxis() -> SetTitleOffset(0.35);
  // hframe -> GetXaxis() -> SetLabelSize(0.20);
  // hframe -> GetXaxis() -> SetTitleSize(0.21);
  hframe -> GetXaxis() -> SetLabelSize(0.0);
  hframe -> GetYaxis() -> SetLabelSize(0.20*0.2/0.8);
  hframe -> GetYaxis() -> SetTitleSize(0.21*0.2/0.8);
  
  hframe -> GetYaxis() -> SetTitleOffset(0.30*0.8/0.2);
  hframe -> Draw();
  stack -> Draw("SAMEHISTO");
  /*  stack -> GetXaxis() -> SetTitle("");
      stack -> GetYaxis() -> SetTitle(_YaxisTitle);*/
  TH1 *h = nullptr;
  TIter next(stack -> GetHists());
  /*  if (hoverlay)
    {
      hoverlay -> Draw("SAME");
      legend -> AddEntry(hoverlay, hoverlay -> GetTitle());
      }*/

  totalMCUnc -> Draw("E2SAME");
  totalMCUnc -> Print("all");
  totalMCUncShape -> Draw("E2SAME");
  if (projdeco -> _datagr)
    {
      projdeco -> _datagr -> Print("all");
      const unsigned int n = projdeco -> _datagr -> GetN();
      double ey;
      for (int i = 0; i < n; i ++) 
	{
	  ey = projdeco -> _datagr -> GetErrorY(i);
	  projdeco -> _datagr -> SetPointError(i, 0, ey);
	}  
          projdeco -> _datagr -> Draw("EP");
    }
  legend -> Draw("SAME");
  projdeco -> Format();
  //  delete totalMC;
  TLatex * txt = new TLatex();
  txt -> SetNDC(kTRUE);
  txt -> SetTextFont(42);
  txt -> SetTextSize(0.06);
  txt -> SetTextAlign(12);
  const float inixlumi = 0.7;
  char text[128];
  sprintf(text, "#scale[0.8]{%3.1f fb^{-1} (%s)}", _luminosity/1000.0, _com);
  txt -> DrawLatex(inixlumi, 0.97, text);
  return pad;
}


void CompoundHistoUnfolding::CreateDataGraph(ResultLevelCode_t resultcode, RecoLevelCode_t recocode)
{
  if (recocode == GEN and resultcode == IN)
    return;
  Level * level = GetLevel(resultcode);
  Level::ProjectionDeco * projdeco = level -> GetProjectionDeco(recocode);
  if (projdeco -> _datagr)
    {
      delete projdeco -> _datagr;
      projdeco -> _datagr = nullptr;
    }

  TH1F * data = level -> GetHU(DATAMO) -> Project(recocode, CreateName("datadg"), "", _YaxisTitle);
  //ApplyScaleFactor(data);

  NormaliseToBinWidth(data);			
  //  RejectNonReco(data);
  data -> Scale(1.0/calculatearea(data));
  // if (recocode == GEN and resultcode == OUT)
  //   {
  //     float integral = 0;
  //     for (unsigned char bind = 1; bind <= data -> GetNbinsX(); bind ++)
  // 	{
  // 	  const float w = data -> GetBinWidth(bind);
  // 	  const float c = data -> GetBinContent(bind);
  // 	  printf("%f %f\n", w, c);
  // 	  integral += w*c;
  // 	}
  //     printf("integral check %f\n", integral);
  //   }
  projdeco -> _datagr = new TGraphErrors(data);
  projdeco -> _datagr -> GetXaxis() -> SetTitle("");
  projdeco -> _datagr -> GetYaxis() -> SetTitle(_YaxisTitle);
  const unsigned int n = projdeco -> _datagr -> GetN();
  double ey;
  for (int i = 0; i < n; i ++) 
    {
      ey = projdeco -> _datagr -> GetErrorY(i);
      projdeco -> _datagr -> SetPointError(i, 0, ey);
    }  
// printf("datagr\n");
  // data -> Print("all");
  // getchar();
  delete data;
}

 
TPad * CompoundHistoUnfolding::CreateRatioGraph(RecoLevelCode_t recocode, ResultLevelCode_t resultcode)
{
  TPad * pad = new TPad(CreateName("pad2"), CreateTitle("pad2"), 0.0, 0.0, 1.0, 0.2);
  pad -> Draw();
  pad -> cd();
  
  Level * level = GetLevel(resultcode);
  Level::ProjectionDeco * projdeco = level -> GetProjectionDeco(recocode);
  if (not projdeco -> _ratioframe)
    {
      TH1 * htotalMC = level -> GetHU(TOTALMCSIGNALPROXY, resultcode) -> Project(recocode, CreateName("totalmcrg"), _XaxisTitle, "MC/data");
      TH1 * htotalMCUnc = level -> GetHU(TOTALMCUNC) -> GetTH1(recocode);
      TH1 * htotalMCUncShape = level -> GetHU(TOTALMCUNCSHAPE) -> GetTH1(recocode);
      /*if (recocode == RECO and resultcode == IN)
	ApplyScaleFactor(htotalMC);*/
      //NormaliseToBinWidth(htotalMC);
      projdeco -> _ratioframe = (TH1F*) htotalMC -> Clone(CreateName("ratioframe"));
      projdeco -> _ratioframe -> SetDirectory(nullptr);
      projdeco -> _ratioframe -> Reset("ICE");
      
      projdeco -> _ratioframe -> GetYaxis() -> SetRangeUser(0.7, 1.3);
      projdeco -> _ratioframeshape = (TH1F*) projdeco -> _ratioframe -> Clone(CreateName("ratioframeshape"));
      projdeco -> _ratioframeshape -> SetDirectory(nullptr);
      TH1F * htotalMCnoUnc = (TH1F*) htotalMC -> Clone(CreateName("totalMCnounc"));
      htotalMCnoUnc -> SetDirectory(nullptr);
      for (unsigned char xbin = 0; xbin < htotalMC -> GetNbinsX() + 1; xbin ++)
	{    
	  projdeco -> _ratioframe -> SetBinContent(xbin, 1.0);
	  const float val = htotalMC -> GetBinContent(xbin);
	  htotalMCnoUnc -> SetBinError(xbin, 0.0);
	  if (val == 0)
	    {
	      continue;
	    }
	  const float totalUnc = htotalMCUnc -> GetBinError(xbin)/val; 
	  const float totalUncShape = htotalMCUncShape -> GetBinError(xbin)/val; 
	  projdeco -> _ratioframeshape -> SetBinContent(xbin, htotalMCUncShape -> GetBinContent(xbin)/val);
	  projdeco -> _ratioframeshape -> SetBinError(xbin, totalUncShape);
	  projdeco -> _ratioframe -> SetBinContent(xbin, htotalMCUnc -> GetBinContent(xbin)/val);
	  projdeco -> _ratioframe -> SetBinError(xbin, totalUnc);
	}
      printf("resultcode %u %u\n", resultcode, resultcode == OUT); 
      if (recocode == RECO and resultcode == IN or resultcode == OUT)
	{
	  
	  TH1F * ratio = resultcode == IN ? 
	    level -> GetHU(TOTALMCSIGNALPROXY, resultcode) -> Project(recocode, "datamp"):
	    GetLevel(IN) -> GetHU(TOTALMC, resultcode) -> Project(recocode, "datamp");
	  
	  TH1F * data = level -> GetHU(DATAMO) -> Project(recocode, CreateName("datadg"), "", _YaxisTitle);
	  //ApplyScaleFactor(ratio);
	  //NormaliseToBinWidth(ratio);
	  ratio -> Divide(data);
	  projdeco -> _ratiogr = new TGraphAsymmErrors(ratio);
	  printf("ratio graph\n");
	  projdeco -> _ratiogr -> Print("all");
	  delete data;
	  delete ratio;
	}
      delete htotalMC;
      delete htotalMCnoUnc;

    }
  projdeco -> _ratioframe -> Draw("E2");
  projdeco -> _ratioframeshape -> Draw("e2 same");
  if (projdeco -> _ratiogr)
    projdeco -> _ratiogr -> Draw("PSAME");
  /*  if (resultcode == OUT and recocode == GEN)
    {
      TH1 * hoverlay = GetLevel(IN) -> GetHU(SIGNALMO) -> Project(GEN);
      hoverlay -> Divide(GetLevel(OUT) -> GetHU(TOTALMC) -> GetTH1(GEN));
      hoverlay -> SetLineColor(kRed);
      hoverlay -> Draw("SAME");
      }*/
  projdeco -> Format();
  return pad;
}
 
TCanvas * CompoundHistoUnfolding::CreateCombinedPlot(RecoLevelCode_t recocode, ResultLevelCode_t resultcode)
{
  TCanvas *c = new TCanvas(CreateName(TString("compound") + "_" + tag_recolevel[recocode] + "_" + tag_resultlevel[resultcode]), CreateTitle(TString("compound") + "_" + tag_recolevel[recocode] + "_" + tag_resultlevel[resultcode]), 500, 500);
  TPad * p1 = CreateMainPlot(recocode, resultcode);
  p1 -> SetRightMargin(0.05);
  p1 -> SetLeftMargin(0.16);
  p1 -> SetTopMargin(0.06);
  p1 -> SetBottomMargin(0.01);
  //p1 -> Draw();
  c -> cd();
  //  printf("probe B %f\n", _signalh -> _th2 -> Integral());
  
  TPad * p2 = CreateRatioGraph(recocode, resultcode);
  p2 -> SetBottomMargin(0.5);
  p2 -> SetRightMargin(0.05);
  p2 -> SetLeftMargin(0.16);
  p2 -> SetTopMargin(0.01);
  p2 -> SetGridx(kFALSE);
  p2 -> SetGridy(kTRUE);
  c -> SetBottomMargin(0.0);
  c -> SetLeftMargin(0.0);
  c -> SetTopMargin(0);
  c -> SetRightMargin(0.00);
  //p2 -> Draw();
  //  c -> SaveAs(TString(_folder) + "/" + c -> GetName() +  ".png");
  //  printf("probe C %f\n", _signalh -> _th2 -> Integral());
  return c;
}

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
  stack -> SetMinimum(0.0);
  stack -> SetMaximum(1.4 * stack -> GetMaximum("nostack"));
  TCanvas * cstabpur = new TCanvas(CreateName("stabpur"), CreateTitle("stabpur"));
  hstability -> SetTitle("stability");
  hpurity -> SetTitle("purity");
  stack -> Draw("nostack");
  TLegend * legend = new TLegend(0.7, 0.7, 0.85, 0.85);
  legend -> SetLineWidth(0);
  legend -> AddEntry(hstability);
  legend -> AddEntry(hpurity);
  legend -> Draw("SAME");
  delete histsig;
  return cstabpur;
}

void CompoundHistoUnfolding::Do()
{
  printf("CompoundHistoUnfolding doing\n");
  /*
  for(vector<HistoUnfolding *> :: iterator it = _vbackgroundhistos.begin(); it != _vbackgroundhistos.end(); it ++)
    {
      (*it) -> ls();
      TFile *f = TFile::Open(TString("$EOS/analysis/") + (*it) -> _tag + ".root");
      TH1F * hreco = f ? (TH1F*) f -> Get("L_pull_angle_allconst_reco_leading_jet_2nd_leading_jet_DeltaRTotal") : nullptr; 
      TH1F * hgen = f ? (TH1F*) f -> Get("L_pull_angle_allconst_gen_leading_jet_2nd_leading_jet_DeltaRTotal") : nullptr; 
      TH1F * px = (TH1F*)(*it) -> _th2 -> ProjectionX();
      TH1F * py = (TH1F*)(*it) -> _th2 -> ProjectionY();

      printf("integral %f projx %f projy %f hgen %p hreco %p\n", (*it) -> _th2 ->  Integral(), px -> Integral(), py -> Integral(), hgen, hreco);
      if (hreco)
	{
	  hreco -> Scale(luminosity * (*it) -> _xsec);
	  printf("hreco Integral %f\n", hreco -> Integral());
	  if (TMath::Abs(hreco -> Integral() - px -> Integral() > 5.0))
	    {
	      printf("attention reco\n");
	      getchar();
	    }

	}
      if (hgen)
	{
	  hgen -> Scale(luminosity * (*it) -> _xsec);
	  printf("hgen Integral %f \n", hgen -> Integral());
	  if (TMath::Abs(hgen -> Integral() - py -> Integral() > 5.0))
	    {
	      printf("attention gen\n");
	      getchar();
	    }

	}
      delete px;
      delete py;

      if (f) f -> Close();
    }
  
  for(vector<HistoUnfolding *> :: iterator it = _vdatahistos.begin(); it != _vdatahistos.end(); it ++)
    {
      (*it) -> ls();
      TFile *f = TFile::Open(TString("$EOS/analysis/") + (*it) -> _tag + ".root");
      TH1F * hreco = f ? (TH1F*) f -> Get("L_pull_angle_allconst_reco_leading_jet_2nd_leading_jet_DeltaRTotal") : nullptr; 
      TH1F * hgen = f ? (TH1F*) f -> Get("L_pull_angle_allconst_gen_leading_jet_2nd_leading_jet_DeltaRTotal") : nullptr; 
      TH1F * px = (TH1F*)(*it) -> _th2 -> ProjectionX();
      TH1F * py = (TH1F*)(*it) -> _th2 -> ProjectionY();

      printf("integral %f projx %f projy %f hgen %p hreco %p\n", (*it) -> _th2 ->  Integral(), px -> Integral(), py -> Integral(), hgen, hreco);
      if (hreco)
	{
	  printf("hreco Integral %f\n", hreco -> Integral());
	  if (TMath::Abs(hreco -> Integral() - px -> Integral() > 5.0))
	    {
	      printf("attention reco\n");
	      getchar();
	    }

	}
      if (hgen)
	{
	  printf("hgen Integral %f \n", hgen -> Integral());
	  if (TMath::Abs(hgen -> Integral() - py -> Integral() > 5.0))
	    {
	      printf("attention gen\n");
	      getchar();
	    }

	}
      delete px;
      delete py;

      if (f) f -> Close();
    }*/
  /*
  for(vector<HistoUnfolding *> :: iterator it = _vsignalhistos.begin(); it != _vsignalhistos.end(); it ++)
    {
      //(*it) -> ls();
      TFile *f = TFile::Open(TString("$EOS/analysis/") + (*it) -> _tag + ".root");
      TH1F * hreco = f ? (TH1F*) f -> Get("L_pull_angle_allconst_reco_leading_jet_2nd_leading_jet_DeltaRTotal") : nullptr; 
      TH1F * hgen = f ? (TH1F*) f -> Get("L_pull_angle_allconst_gen_leading_jet_2nd_leading_jet_DeltaRTotal") : nullptr; 
      TH1F * px = (TH1F*)(*it) -> _th2 -> ProjectionX();
      TH1F * py = (TH1F*)(*it) -> _th2 -> ProjectionY();

      printf("integral %f projx %f projy %f hgen %p hreco %p\n", (*it) -> _th2 ->  Integral(), px -> Integral(), py -> Integral(), hgen, hreco);
      if (hreco)
	{
	  hreco -> Scale(luminosity * (*it) -> _xsec);
	  printf("hreco Integral %f\n", hreco -> Integral());
	  if (TMath::Abs(hreco -> Integral() - px -> Integral() > 5.0))
	    {
	      printf("attention reco\n");
	      getchar();
	    }

	}
      if (hgen)
	{
	  hgen -> Scale(luminosity * (*it) -> _xsec);
	  printf("hgen Integral %f \n", hgen -> Integral());
	  if (TMath::Abs(hgen -> Integral() - py -> Integral() > 5.0))
	    {
	      printf("attention gen\n");
	      getchar();
	    }

	}
      delete px;
      delete py;

      if (f)  f -> Close();
    }
  for(vector<HistoUnfolding *> :: iterator it = _vsyshistos.begin(); it != _vsyshistos.end(); it ++)
    {
      (*it) -> ls();
      TFile *f = TFile::Open(TString("$EOS/analysis/") + (*it) -> _tag + ".root");
      TH1F * hreco = f ? (TH1F*) f -> Get("L_pull_angle_allconst_reco_leading_jet_2nd_leading_jet_DeltaRTotal") : nullptr; 
      TH1F * hgen = f ? (TH1F*) f -> Get("L_pull_angle_allconst_gen_leading_jet_2nd_leading_jet_DeltaRTotal") : nullptr; 
      TH1F * px = (TH1F*)(*it) -> _th2 -> ProjectionX();
      TH1F * py = (TH1F*)(*it) -> _th2 -> ProjectionY();

      printf("integral %f projx %f projy %f hgen %p hreco %p\n", (*it) -> _th2 ->  Integral(), px -> Integral(), py -> Integral(), hgen, hreco);
      if (hreco)
	{
	  hreco -> Scale(luminosity * (*it) -> _xsec);
	  printf("hreco Integral %f\n", hreco -> Integral());
	  if (TMath::Abs(hreco -> Integral() - px -> Integral() > 5.0))
	    {
	      printf("attention reco\n");
	      getchar();
	    }

	}
      if (hgen)
	{
	  hgen -> Scale(luminosity * (*it) -> _xsec);
	  printf("hgen Integral %f \n", hgen -> Integral());
	  if (TMath::Abs(hgen -> Integral() - py -> Integral() > 5.0))
	    {
	      printf("attention gen\n");
	      getchar();
	    }

	}
      delete px;
      delete py;

      if (f)f -> Close();
    }
  */
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
