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
#include "WeightRecord.hh"

ClassImp(CompoundHistoUnfolding);

void CompoundHistoUnfolding::LoadHistos(const char * json, SysTypeCode_t sys, const char * syssampletag)
{
  const char * sampletag = sys != EXPSYS ? "MC13TeV_TTJets" : signaltag; //!!!
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
  float novalue_proxy(0.0);
  if (string(observable) == "pull_angle")
    {
      novalue_proxy = -10.0;
    }
  if (string(observable) == "pvmag" or string(observable) == "pvmag_par")
    {
      novalue_proxy = - 1.0;
    }
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
      printf("tag %s \n", h -> GetTag());
      // chain.Add(TString("root://eosuser.cern.ch//$EOS/analysis_") + sampletag + ("/HADDmigration/migration_") + h -> GetTag() + ".root/E_" + tag_charge_types_[chargecode] + "_" + tag_jet_types_[jetcode] + "_" + "migration");
      // chain.Add(TString("root://eosuser.cern.ch//$EOS/analysis_") + sampletag + ("/HADDmigration/migration_") + h -> GetTag() + ".root/M_" + tag_charge_types_[chargecode] + "_" + tag_jet_types_[jetcode] + "_" + "migration");
      chain.Add(TString("root://eosuser.cern.ch//$EOS/analysis_") + sampletag + ("/HADDmigration/migration_") + h -> GetTag() + ".root/migration");
      Float_t reco;
      Float_t gen;
      Float_t E_reco, M_reco;
      Float_t E_gen, M_gen;
      WeightMap * wmap = nullptr;
      //      const char * tag_jet_types_temp[4] = {"first_leading_jet", "scnd_leading_jet", "leading_b", "snd_leading_b"};
      const TString infix = TString(observable) + "_" + tag_jet_types_[jetcode] + "_" + tag_charge_types_[chargecode];
      printf("reading ... [%s] ...\n", infix.Data());
      chain.SetBranchAddress(TString("E") + "_" + infix + "_" + "reco", & E_reco);
      chain.SetBranchAddress(TString("M") + "_" + infix + "_" + "reco", & M_reco);
      chain.SetBranchAddress(TString("E") + "_" + infix + "_" + "gen",  & E_gen);
      chain.SetBranchAddress(TString("M") + "_" + infix + "_" + "gen",  & M_gen);
      chain.SetBranchAddress("weight", & wmap);
      const unsigned int dbcut = 1;
      //      h -> FillFromTree(, tag_jet_types_[jetcode], tag_charge_types_[chargecode], observable);
      unsigned long nevents = 0;
      printf("nentries %lu\n", chain.GetEntries());
      for (unsigned long event_ind = 0; event_ind < chain.GetEntries()/dbcut; event_ind ++)
	{
	  if (event_ind % 10000 == 0)
	    {
	      printf("%u\r", event_ind);
	    }
	  chain.GetEntry(event_ind);
	  
	  // if ((*wmap) -> size() == 0)
	  //   {
	  //     printf("weights -> size() == 0 !!!!\n");
	  //   }
	  // printf("%p size %lu ", weights, weights -> size() );
	  // printf("%lu %.9f\n", event_ind,  (*weights)[0]);
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
	    {
	      reco = novalue_proxy;
	    }
	  if (E_reco != novalue_proxy)
	    {
	      reco = E_reco;
	    }
	  else
	    {
	      reco = M_reco;
	    }

	  if (E_gen == novalue_proxy and M_gen == novalue_proxy)
	    {
	      gen = novalue_proxy;
	    }
	  if (E_gen != novalue_proxy)
	    {
	      gen = E_gen;
	    }
	  else
	    {
	      gen = M_gen;
	    }
	  if (reco == novalue_proxy and gen == novalue_proxy)
	    {
	      continue;
	    }
	  nevents ++;
	  double weight0 = 0.0;
	  if (reco == novalue_proxy)
	    {
	      weight0 = wmap -> GetWeight0(WorkEnum_t::GEN);
	    }
	  else
	    {
	      weight0 = wmap -> GetWeight0(WorkEnum_t::RECO);
	    }
	  if (fabs(weight0) > 10000.0)
	    {
	      printf("weight > 10 000.0");
	      continue;
	    }
	  if (string(observable).compare("pull_angle") == 0)
	    {
	      h -> GetTH2() -> Fill(reco == novalue_proxy ? reco : TMath::Abs(reco)/TMath::Pi(), gen == novalue_proxy ? gen : TMath::Abs(gen)/TMath::Pi(), weight0);
	    }
	  
	  // if (string(h -> GetTag()).compare("MC13TeV_TTJets_tracking_up") == 0)
	  //   {
	  //     printf("%lu %f %f\n", event_ind, reco, gen);
	  //     //     if (reco != -10 and (reco < - ;
	  //   }
	  if (string(observable) == "pvmag" or string(observable) == "pvmag_par")
	    {
	      h -> GetTH2() -> Fill(reco, gen, weight0);
	    }
	  //      _th2 -> Fill(pull_angle_reco == -10 ? pull_angle_reco : pull_angle_reco, pull_angle_gen == -10.0 ? pull_angle_gen : pull_angle_gen, weight);
	  
	}
      printf("Filled %lu events dbcut %f\n", nevents, dbcut);

      AddHisto(h);
      h -> GetTH2() -> Print("all");

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
      // if (string(parser.GetSample(sind)->GetTag()).compare(signaltag) == 0)
      // 	{
      // 	  TCanvas * cstabpur = stabpur();
      // 	  cstabpur -> SaveAs("output/stabpurtest.png");
      // 	  delete cstabpur;
      // 	  getchar();
      // 	}
      if (string(parser.GetSample(sind) -> GetTag()).compare(signaltag) == 0 and not calculate_bins)
	{
	  //	  	  continue;
	  vector<HistoUnfoldingTH2*> nondedicatedth2d;
	  static const unsigned char Nnondedicated = 21;
	  // static const char * nondedicated_titles[Nnondedicated] = 
	  //   {
	  //     "pileup up", "pileup down",
	  //     "trig. efficiency correction up", "trig. efficiency correction down",
	  //     "sel. efficiency correction up", "sel. efficiency correction down",
	  //     "b fragmentation up", "b fragmentation down",
	  //     "Peterson frag.", 
	  //     "semilep BR up", "semilep BR down", 
	  //     "top $p_{T}$",
	  //     "id1002 $#mu_{R}=1.0$ $#mu_{F}=2.0$ $h_{damp}$ $m_{t}=272.7225$", 
	  //     "id1003 $#mu_{R}=1.0$ $#mu_{F}=0.5$ $h_{damp}$ $m_{t}=272.7225$", 
	  //     "id1004 $#mu_{R}=2.0$ $#mu_{F}=1.0$ $h_{damp}$ $m_{t}=272.7225$", 
	  //     "id1005 $#mu_{R}=2.0$ $#mu_{F}=2.0$ $h_{damp}$ $m_{t}=272.7225$", 
	  //     "id1007 $#mu_{R}=0.5$ $#mu_{F}=1.0$ $h_{damp}$ $m_{t}=272.7225$", 
	  //     "id1009 $#mu_{R}=0.5$ $#mu_{F}=0.5$ $h_{damp}$ $m_{t}=272.7225$", 
	  //     "id3001PDFset13100", "id4001PDFset25200id1010",  ""
	  //   };
	  // static const char * nondedicated_names[Nnondedicated] = 
	  //   {
	  //     "pileup_up", "pileup_down",
	  //     "trig_efficiency_correction_up", "trig_efficiency_correction_down",
	  //     "sel_efficiency_correction_up", "sel_efficiency_correction_down",
	  //     "b_fragmentation_up", "b_fragmentation_down",
	  //     "Peterson_frag", 
	  //     "semilep_BR_up", "semilep_BR_down", 
	  //     "top_pt",
	  //     "id1002muR1muF2hdampmt272.7225", "id1003muR1muF0.5hdampmt272.7225", "id1004muR2muF1hdampmt272.7225", "id1005muR2muF2hdampmt272.7225", "id1007muR0.5muF1hdampmt272.7225", "id1009muR0.5muF0.5hdampmt272.7225", 
	  //     "id3001PDFset13100", "id4001PDFset25200", ""
	  //   };
	  // static const char * nondedicated_categories[Nnondedicated] = 
	  //   {
	  //     "Pile-up", "Pile-up",
	  //     "Trigger efficiency", "Trigger efficiency",
	  //     "Selection efficiency", "Selection efficiency",
	  //     "b-fragmentation", "b-fragmentation",
	  //     "Peterson fragmentation", 
	  //     "Semilepton branching ratios", "Semilepton branching ratios", 
	  //     "top pt",
	  //     "QCD scales", "QCD scales", "QCD scales", "QCD scales", "QCD scales", "QCD scales", 
	  //     "PDF", "PDF", ""
	  //   };

	  for (map<unsigned char, pair<Weights, Vari>>::const_iterator it = windmap.cbegin(); it !=windmap.cend(); ++it )
	    {						
	      unsigned char ind = it -> first;
	      Weights wgt = windmap.at(ind).first;
	      Vari var = windmap.at(ind).second;
	      Categories cat = wcats.at(wgt);
 
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
	      const TString nondedicated_name = weightstitles.at(wgt) + "_" + varititles.at(var);
	      const TString cattitle = cattitles.at(cat);
	      h -> SetTag(TString(h -> GetTag()) + "_" + nondedicated_name);
	      h -> GetTH2() -> SetName(nondedicated_name);
	      h -> GetTH2() -> SetTitle(nondedicated_name);
	      h -> SetTitle(TString("t#bar{t} ") + nondedicated_name);
	      h -> SetCategory(cattitle.Data());
	      h -> SetSysType(EXPSYS);
	      h -> SetSample(signaltag);
	      unsigned long nevents = 0;
	      printf("%s nevents %lu dbcut %f\n", nondedicated_name.Data(), chain.GetEntries(), dbcut);
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
		    {
		      reco = novalue_proxy;
		    }
		  if (E_reco != novalue_proxy)
		    {
		      reco = E_reco;
		    }
		  else
		    {
		      reco = M_reco;
		    }

		  if (E_gen == novalue_proxy and M_gen == novalue_proxy)
		    {
		      gen = novalue_proxy;
		    }
		  if (E_gen != novalue_proxy)
		    {
		      gen = E_gen;
		    }
		  else
		    {
		      gen = M_gen;
		    }
		  if (reco == novalue_proxy and gen == novalue_proxy)
		    {
		      continue;
		    }
		  nevents ++;
		  double weight0;
		  if (reco == novalue_proxy)
		    {
		      weight0 = wmap -> GetWeight0(WorkEnum_t::GEN);
		    }
		  else
		    {
		      weight0 = wmap -> GetWeight0(WorkEnum_t::RECO);
		    }
		  // if (nondedicated_ind + 1 > (*wmap) -> size() - 1)
		  //   {
		  //     nondedicated_end = true;
		  //     delete h;
		  //     h = nullptr;
		  //     printf("breaking\n");
		      
		  //     break;
		  //   }
		  if (string(observable) == "pull_angle")
		    {
		      //		      printf("%f nondedicated_ind %u\n",  (*weights)[nondedicated_ind + 1], nondedicated_ind);
		      h -> GetTH2() -> Fill(reco == novalue_proxy ? reco : TMath::Abs(reco)/TMath::Pi(), 
					    gen == novalue_proxy ? gen : TMath::Abs(gen)/TMath::Pi(), 
					    weight0 * wmap -> GetWeight(windmap.at(ind).first, windmap.at(ind).second, WorkEnum_t::RECO));
		    }
		  /*      if (string(GetTag()).compare("MC13TeV_TTJets") == 0)
			  {
			  printf("%lu %f\n", event_ind, reco);
			  getchar();
			  }*/
		  if (string(observable) == "pvmag" or string(observable) == "pvmag_par")
		    {
		      h -> GetTH2() -> Fill(reco, gen, weight0);
		    }
		  // }
		  //      _th2 -> Fill(pull_angle_reco == -10 ? pull_angle_reco : pull_angle_reco, pull_angle_gen == -10.0 ? pull_angle_gen : pull_angle_gen, weight);

		}
	      if (h)
		{
		  AddHisto(h);
		  printf("printing nondedicated %s\n", h -> GetTH2() -> GetName());
		  TH1 * hp = h -> GetTH2() -> ProjectionX();
		  hp -> Print("all");
		  delete hp;
		  getchar();
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
