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

void CompoundHistoUnfolding::Fix()
{
  static const unsigned char Nnondedicated = 8;
  static const char * nondedicated_titles[Nnondedicated] = { "top pt",
							    "id1002muR1muF2hdampmt272.7225", "id1003muR1muF0.5hdampmt272.7225", "id1004muR2muF1hdampmt272.7225", "id1005muR2muF2hdampmt272.7225", "id1007muR0.5muF1hdampmt272.7225", "id1009muR0.5muF0.5hdampmt272.7225", "id1010"};
  static const char * nondedicated_names[Nnondedicated] = {"top_pt",
							   "id1002muR1muF2hdampmt272.7225", "id1003muR1muF0.5hdampmt272.7225", "id1004muR2muF1hdampmt272.7225", "id1005muR2muF2hdampmt272.7225", "id1007muR0.5muF1hdampmt272.7225", "id1009muR0.5muF0.5hdampmt272.7225", "id1010"};
  static const char * nondedicated_categories[Nnondedicated] = {"top pt",
								"QCD scales", "QCD scales", "QCD scales", "QCD scales", "QCD scales", "QCD scales", "QCD Scales"};

  TString sample = signaltag;
  bool start = false;
  unsigned char ind = 1;
  for (vector<HistoUnfolding *>::iterator it = GetLevel(IN) -> GetV(SYSMO, sample.Data()) -> begin(); it != GetLevel(IN) -> GetV(SYSMO, sample.Data()) -> end(); it ++)
    {
	  (*it) -> ls();
      for (unsigned char find = 0; find < Nnondedicated; find ++)
	{
	  if (TString((*it) -> GetTag()) == TString(signaltag) + "_" + nondedicated_names[find])
	    {
	      printf("found\n");
	      (*it) -> SetTag(TString(signaltag) + "_" + nondedicated_names[find - 1]);
	      (*it) -> GetTH2() -> SetName(nondedicated_names[find - 1]);
	      (*it) -> GetTH2() -> SetTitle(nondedicated_titles[find - 1]);
	      (*it) -> SetTitle((TString("t#bar{t} ") + nondedicated_titles[find - 1]).Data());
	      (*it) -> SetCategory(TString(nondedicated_categories[find - 1]).Data());
	    }
	  
	  // (*it) -> ls();

	}
      ind ++;
    }
}


void CompoundHistoUnfolding::Check()
{
  TString sample = signaltag;
  bool start = false;
  unsigned char ind = 0;
  for (vector<HistoUnfolding *>::iterator it = GetLevel(IN) -> GetV(SYSMO, sample.Data()) -> begin(); it != GetLevel(IN) -> GetV(SYSMO, sample.Data()) -> end(); it ++)
    {
      (*it) -> ls();
    }
}
