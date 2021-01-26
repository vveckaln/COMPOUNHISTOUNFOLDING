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

void CompoundHistoUnfolding::LoadHistos_cflip(const char * json, SysTypeCode_t sys, const char * syssampletag)
{
  const char * sampletag = sys != EXPSYS ? "MC13TeV_TTJets" : signaltag; //!!!
  char * temp = nullptr;
  JsonParser parser;
  parser.SetSignalTag(signaltag);
  parser.Load(json, sys, syssampletag);
  float novalue_proxy(0.0);
  if (string(observable).compare("pull_angle") == 0)
    {
      novalue_proxy = -10.0;
    }
  if (string(observable).compare("pvmag") == 0)
    {
      novalue_proxy = - 1.0;
    }
  // printf("parser samples size %lu \n", parser.GetSamplesV() -> size());
  for (vector<SampleDescriptor *>::iterator it = parser.GetSamplesV() -> begin(); it != parser.GetSamplesV() -> end(); it ++)
    {
      // (*it) -> ls();
      // printf("sys == EXPSYS %s\n", sys == EXPSYS ? "True" : "False");
      // getchar();
      if (TString(signaltag) != "MC13TeV_TTJets2l2nu_amcatnlo" and TString((*it) -> GetTag()) == "MC13TeV_TTJets2l2nu_amcatnlo") 
	{
	  parser.GetSamplesV() -> erase(it);;
	}
      // if (TString(method) == "nominal" and TString((*it) -> GetTag()) == "MC13TeV_TTJets_cflip")
      // 	{
      // 	  //parser.GetSamplesV() -> erase(it);
      // 	}
      // else if (calculate_bins)
      // 	{
      // 	  continue;
      // 	}
      if (sys == EXPSYS)
	{
	  //	  printf("Tag before [%s]\n", (*it) -> GetTag());
	  (*it) -> SetTag(TString((*it) -> GetTag()).ReplaceAll("MC13TeV_TTJets", signaltag));
	  //printf("Tag after [%s]\n", (*it) -> GetTag());
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
      // chain.Add(TString("root://eosuser.cern.ch//$EOS/analysis_") + sampletag + ("/HADDmigration/migration_") + h -> GetTag() + ".root/E_" + tag_charge_types_[chargecode] + "_" + tag_jet_types_[jetcode] + "_" + "migration");
      // chain.Add(TString("root://eosuser.cern.ch//$EOS/analysis_") + sampletag + ("/HADDmigration/migration_") + h -> GetTag() + ".root/M_" + tag_charge_types_[chargecode] + "_" + tag_jet_types_[jetcode] + "_" + "migration");
      if (sys == NOMINAL)

           chain.Add(TString("root://eosuser.cern.ch//$EOS/analysis_") + sampletag + ("/HADDmigration/migration_") + h -> GetTag() + ".root/migration");
      if (sys == EXPSYS)
           chain.Add(TString("root://eosuser.cern.ch//$EOS/analysis_") + signaltag + ("/HADDmigration/migration_") + h -> GetTag() + ".root/migration");
      //      chain.Print("all");
      Float_t reco;
      Float_t gen;
      Float_t E_reco, M_reco;
      Float_t E_gen, M_gen;
      vector<double> * weights = nullptr;
      const char * tag_jet_types_temp[4] = {"first_leading_jet", "scnd_leading_jet", "leading_b", "snd_leading_b"};
      chain.SetBranchAddress(TString("E") + "_" + observable + "_" + tag_jet_types_[jetcode] + "_" + tag_charge_types_[chargecode] + "_" + "reco", & E_reco);
      chain.SetBranchAddress(TString("M") + "_" + observable + "_" + tag_jet_types_[jetcode] + "_" + tag_charge_types_[chargecode] + "_" + "reco", & M_reco);
      chain.SetBranchAddress(TString("E") + "_" + observable + "_" + tag_jet_types_[jetcode] + "_" + tag_charge_types_[chargecode] + "_" + "gen",  & E_gen);
      chain.SetBranchAddress(TString("M") + "_" + observable + "_" + tag_jet_types_[jetcode] + "_" + tag_charge_types_[chargecode] + "_" + "gen",  & M_gen);
      chain.SetBranchAddress("weight", & weights);
      const unsigned int dbcut = 1;
      unsigned long nevents = 0;
      if (string(parser.GetSample(sind) -> GetTag()).compare(signaltag) != 0)
	{
	  for (unsigned long event_ind = 0; event_ind < chain.GetEntries()/dbcut; event_ind ++)
	    {
	      if (event_ind % 10000 == 0)
		printf("%u\r", event_ind);
	      chain.GetEntry(event_ind);
	      // printf("observable %s novalue_proxy %f\n", observable, novalue_proxy);
	      // printf("E_reco %f, M_reco %f E_gen %f M_gen %f\n", E_reco, M_reco, E_gen, M_gen);
	      if (E_reco != novalue_proxy and M_reco != novalue_proxy)
		{
		  fprintf(stderr, "Values set in both E and M reco channels\n");
		  throw "Values set in both E and M reco channels\n";
		}
	      if (E_gen != novalue_proxy and M_gen != novalue_proxy)
		{
		  fprintf(stderr, "Values set in both E and M gen channels\n");
		  throw "Values set in both E and M gen channels\n";
		}
	      if (E_reco == novalue_proxy and M_reco == novalue_proxy)
		reco = novalue_proxy;
	      if (E_reco != novalue_proxy)
		reco = E_reco;
	      else
		reco = M_reco;

	      if (E_gen == novalue_proxy and M_gen == novalue_proxy)
		gen = novalue_proxy;
	      if (E_gen != novalue_proxy)
		gen = E_gen;
	      else
		gen = M_gen;
	      if (reco == novalue_proxy and gen == novalue_proxy)
		continue;
	      nevents ++;
	      if (string(observable).compare("pull_angle") == 0)
		{
		  h -> GetTH2() -> Fill(reco == novalue_proxy ? reco : TMath::Abs(reco)/TMath::Pi(), gen == novalue_proxy ? gen : TMath::Abs(gen)/TMath::Pi(), (*weights)[0]);
		}
	  
	      if (string(observable).compare("pvmag") == 0)
		h -> GetTH2() -> Fill(reco, gen, (*weights)[0]);
	  
	    }
	  printf("Filled %lu events\n", nevents);

	  AddHisto(h);

	}
      bool nondedicated_end =  false;
      if (string(parser.GetSample(sind) -> GetTag()).compare(signaltag) == 0 and not calculate_bins)
	{
	  //	  	  continue;
	  vector<HistoUnfoldingTH2*> nondedicatedth2d;
	  static const unsigned char Nnondedicated = 21;
	  static const char * nondedicated_titles[Nnondedicated] = {"pileup up", "pileup down",
								    "trig efficiency correction up", "trig efficiency correction down",
								    "sel efficiency correction up", "sel efficiency correction down",
								    "b fragmentation up", "b fragmentation down",
								    "Peterson Frag", 
								    "semilep BR up", "semilep BR down", 
								    "top pt",
								    "id1002muR1muF2hdampmt272.7225", "id1003muR1muF0.5hdampmt272.7225", "id1004muR2muF1hdampmt272.7225", "id1005muR2muF2hdampmt272.7225", "id1007muR0.5muF1hdampmt272.7225", "id1009muR0.5muF0.5hdampmt272.7225", "id1010", "", ""};
	  static const char * nondedicated_names[Nnondedicated] = {"pileup_up", "pileup_down",
								   "trig_efficiency_correction_up", "trig_efficiency_correction_down",
								   "sel_efficiency_correction_up", "sel_efficiency_correction_down",
								   "b_fragmentation_up", "b_fragmentation_down",
								   "Peterson_frag", 
								   "semilep_BR_up", "semilep_BR_down", 
								   "top_pt",
								   "id1002muR1muF2hdampmt272.7225", "id1003muR1muF0.5hdampmt272.7225", "id1004muR2muF1hdampmt272.7225", "id1005muR2muF2hdampmt272.7225", "id1007muR0.5muF1hdampmt272.7225", "id1009muR0.5muF0.5hdampmt272.7225", "id1010", "", ""};
	  static const char * nondedicated_categories[Nnondedicated] = {"Pile-up", "Pile-up",
								   "Trigger efficiency", "Trigger efficiency",
								   "Selection efficiency", "Selection efficiency",
								   "b-fragmentation", "b-fragmentation",
								   "Peterson fragmentation", 
								   "Semilepton branching ratios", "Semilepton branching ratios", 
								   "top pt",
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
	      h -> SetTitle((TString("t#bar{t} ") + method + " " + nondedicated_titles[nondedicated_ind]).Data());
	      h -> SetCategory(TString(nondedicated_categories[nondedicated_ind]).Data());
	      h -> SetSysType(EXPSYS);
	      h -> SetSample(signaltag);
	      unsigned long nevents = 0;
	      for (unsigned long event_ind = 0; event_ind < chain.GetEntries()/dbcut; event_ind ++)
		{
		  if (event_ind % 10000 == 0)
		    {
		      printf("%u\r", event_ind);
		    }
		  chain.GetEntry(event_ind);
		  if (E_reco != novalue_proxy and M_reco != novalue_proxy)
		    {
		      fprintf(stderr, "Values set in both E and M reco channels\n");
		      throw "Values set in both E and M reco channels\n";
		    }
		  if (E_gen != novalue_proxy and M_gen != novalue_proxy)
		    {
		      fprintf(stderr, "Values set in both E and M gen channels\n");
		      throw "Values set in both E and M gen channels\n";
		    }
		  if (E_reco == novalue_proxy and M_reco == novalue_proxy)
		    reco = novalue_proxy;
		  if (E_reco != novalue_proxy)
		    reco = E_reco;
		  else
		    reco = M_reco;

		  if (E_gen == novalue_proxy and M_gen == novalue_proxy)
		    gen = novalue_proxy;
		  if (E_gen != novalue_proxy)
		    gen = E_gen;
		  else
		    gen = M_gen;
		  if (reco == novalue_proxy and gen == novalue_proxy)
		    {
		      continue;
		    }
		  nevents ++;
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
		      h -> GetTH2() -> Fill(reco == novalue_proxy ? reco : TMath::Abs(reco)/TMath::Pi(), gen == novalue_proxy ? gen : TMath::Abs(gen)/TMath::Pi(), (*weights)[0] * (*weights)[nondedicated_ind + 1]);
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
	      //printf("here\n");
	      if (h)
		{
		  AddHisto(h);
		  // printf("printing nondedicated %s\n", h -> GetTH2() -> GetName());
		  // TH1 * hp = h -> GetTH2() -> ProjectionX();
		  // hp -> Print("all");
		  // delete hp;
		  // getchar();
		}
	      printf("filled %lu events\n", nevents);
	      
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
