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

void CompoundHistoUnfolding::TransferHistos(const char * json, SysTypeCode_t sys, const char * syssampletag)
{
  const char * sampletag = sys != EXPSYS ? "MC13TeV_TTJets" : signaltag;
  char * temp = nullptr;
  JsonParser parser;
  parser.SetSignalTag("MC13TeV_TTJets");
  parser.Load(json, sys, syssampletag);
  for (vector<SampleDescriptor *>::iterator it = parser.GetSamplesV() -> begin(); it != parser.GetSamplesV() -> end(); it ++)
    {
      if (TString(signaltag) != "MC13TeV_TTJets2l2nu_amcatnlo" and TString((*it) -> GetTag()) == "MC13TeV_TTJets2l2nu_amcatnlo") 
  	{
  	  parser.GetSamplesV() -> erase(it);;
  	}
    }
  const unsigned long ssize = parser.GetSize();
  for (unsigned long sind = 0; sind < ssize; sind ++)
    {
      SampleDescriptor * sample = parser.GetSample(sind);
    }  
  for (unsigned long sind = 0; sind < ssize; sind ++)
    {
      
      SampleDescriptor * sample = parser.GetSample(sind);
      if (sample -> GetSampleType() == DATA)
	{
	  TH2 * hinput = _chunominal -> GetLevel(IN) -> GetInputHU(DATAMO, sample -> GetTag()) -> GetTH2();
	  HistoUnfoldingTH2 * h = new HistoUnfoldingTH2((HistoUnfoldingTH2*)_chunominal ->  GetLevel(IN) -> GetInputHU(DATA, sample -> GetTag()));
	  TH2 * h2 = h -> GetTH2();
	  AddHisto(h, "noscale");
						       
	}

      if (sample -> IsSys())
	{
	  HistoUnfoldingTH2 * h = new HistoUnfoldingTH2((HistoUnfoldingTH2*)_chunominal ->  GetLevel(IN) -> GetInputHU(SYSMO, sample -> GetTag(), sample -> GetSample()));
	  if (TString(h -> GetTag()) == signaltag)
	    {
	      h -> SetSysType(NOMINAL);
	      h -> SetCategory(h -> GetTitle());
	    }
	  if (h -> GetSysType() == THEORSYS and TString(h -> GetSample()) == "MC13TeV_TTJets" )
	    {
	      h -> SetTag(TString(h -> GetTag()).ReplaceAll("MC13TeV_TTJets", signaltag));
	      h -> SetSample(signaltag);
	    }

	  AddHisto(h, "noscale");
	}

      if (sample -> GetSampleType() == SIGNAL and not sample -> IsSys())
	{
	  TH2 * hinput = _chunominal -> GetLevel(IN) -> GetInputHU(SIGNALMO, sample -> GetTag()) -> GetTH2();
	  HistoUnfoldingTH2 * h = new HistoUnfoldingTH2((HistoUnfoldingTH2*)_chunominal ->  GetLevel(IN) -> GetInputHU(SIGNALMO, sample -> GetTag()));
	  h -> SetSysType(THEORSYS);
  	  h -> SetSample(signaltag);
  	  h -> SetSampleType(SIGNAL);
	  TH2 * h2 = h -> GetTH2();

	  AddHisto(h, "noscale");
						       
	}

      if (sample -> GetSampleType() == BACKGROUND and not sample -> IsSys())
      	{
	  //	  sample -> ls();
      	  HistoUnfoldingTH2 * h = new HistoUnfoldingTH2((HistoUnfoldingTH2*)_chunominal ->  GetBackgroundH(sample -> GetTag()));

      	  AddHisto(h, "noscale");
						       
      	}
      parser.UnsetSample(sind);
    }
}
