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
void CompoundHistoUnfolding::Do()
{
  printf("CompoundHistoUnfolding doing\n");
  
  for(vector<HistoUnfolding *> :: iterator it = _vbackgroundhistos.begin(); it != _vbackgroundhistos.end(); it ++)
    {
      //  printf("background histos\n");
      //      (*it) -> ls();
      // TFile *f = TFile::Open(TString("$EOS/analysis/") + (*it) -> _tag + ".root");
      // TH1F * hreco = f ? (TH1F*) f -> Get("L_pull_angle_allconst_reco_leading_jet_2nd_leading_jet_DeltaRTotal") : nullptr; 
      // TH1F * hgen = f ? (TH1F*) f -> Get("L_pull_angle_allconst_gen_leading_jet_2nd_leading_jet_DeltaRTotal") : nullptr; 
      // TH1F * px = (TH1F*)(*it) -> _th2 -> ProjectionX();
      // TH1F * py = (TH1F*)(*it) -> _th2 -> ProjectionY();

      // printf("integral %f projx %f projy %f hgen %p hreco %p\n", (*it) -> _th2 ->  Integral(), px -> Integral(), py -> Integral(), hgen, hreco);
      // if (hreco)
      // 	{
      // 	  hreco -> Scale(luminosity * (*it) -> _xsec);
      // 	  printf("hreco Integral %f\n", hreco -> Integral());
      // 	  if (TMath::Abs(hreco -> Integral() - px -> Integral() > 5.0))
      // 	    {
      // 	      printf("attention reco\n");
      // 	      getchar();
      // 	    }

      // 	}
      // if (hgen)
      // 	{
      // 	  hgen -> Scale(luminosity * (*it) -> _xsec);
      // 	  printf("hgen Integral %f \n", hgen -> Integral());
      // 	  if (TMath::Abs(hgen -> Integral() - py -> Integral() > 5.0))
      // 	    {
      // 	      printf("attention gen\n");
      // 	      getchar();
      // 	    }

      // 	}
      // delete px;
      // delete py;

      // if (f) f -> Close();
    }
  //getchar();
  for(vector<HistoUnfolding *> :: iterator it = GetLevel(IN) -> _vdatahistos.begin(); it != GetLevel(IN) -> _vdatahistos.end(); it ++)
    {
      //printf("data histos\n");
      //(*it) -> ls();
      // TFile *f = TFile::Open(TString("$EOS/analysis/") + (*it) -> _tag + ".root");
      // TH1F * hreco = f ? (TH1F*) f -> Get("L_pull_angle_allconst_reco_leading_jet_2nd_leading_jet_DeltaRTotal") : nullptr; 
      // TH1F * hgen = f ? (TH1F*) f -> Get("L_pull_angle_allconst_gen_leading_jet_2nd_leading_jet_DeltaRTotal") : nullptr; 
      // TH1F * px = (TH1F*)(*it) -> _th2 -> ProjectionX();
      // TH1F * py = (TH1F*)(*it) -> _th2 -> ProjectionY();

      // printf("integral %f projx %f projy %f hgen %p hreco %p\n", (*it) -> _th2 ->  Integral(), px -> Integral(), py -> Integral(), hgen, hreco);
      // if (hreco)
      // 	{
      // 	  printf("hreco Integral %f\n", hreco -> Integral());
      // 	  if (TMath::Abs(hreco -> Integral() - px -> Integral() > 5.0))
      // 	    {
      // 	      printf("attention reco\n");
      // 	      getchar();
      // 	    }

      // 	}
      // if (hgen)
      // 	{
      // 	  printf("hgen Integral %f \n", hgen -> Integral());
      // 	  if (TMath::Abs(hgen -> Integral() - py -> Integral() > 5.0))
      // 	    {
      // 	      printf("attention gen\n");
      // 	      getchar();
      // 	    }

      // 	}
      // delete px;
      // delete py;

      // if (f) f -> Close();
    }
  
  for(vector<HistoUnfolding *> :: iterator it = GetLevel(IN) -> _vsignalhistos.begin(); it != GetLevel(IN) -> _vsignalhistos.end(); it ++)
    {
      //printf("signal histos\n");
      //      (*it) -> ls();
      // TFile *f = TFile::Open(TString("$EOS/analysis/") + (*it) -> _tag + ".root");
      // TH1F * hreco = f ? (TH1F*) f -> Get("L_pull_angle_allconst_reco_leading_jet_2nd_leading_jet_DeltaRTotal") : nullptr; 
      // TH1F * hgen = f ? (TH1F*) f -> Get("L_pull_angle_allconst_gen_leading_jet_2nd_leading_jet_DeltaRTotal") : nullptr; 
      // TH1F * px = (TH1F*)(*it) -> _th2 -> ProjectionX();
      // TH1F * py = (TH1F*)(*it) -> _th2 -> ProjectionY();

      // printf("integral %f projx %f projy %f hgen %p hreco %p\n", (*it) -> _th2 ->  Integral(), px -> Integral(), py -> Integral(), hgen, hreco);
      // if (hreco)
      // 	{
      // 	  hreco -> Scale(luminosity * (*it) -> _xsec);
      // 	  printf("hreco Integral %f\n", hreco -> Integral());
      // 	  if (TMath::Abs(hreco -> Integral() - px -> Integral() > 5.0))
      // 	    {
      // 	      printf("attention reco\n");
      // 	      getchar();
      // 	    }

      // 	}
      // if (hgen)
      // 	{
      // 	  hgen -> Scale(luminosity * (*it) -> _xsec);
      // 	  printf("hgen Integral %f \n", hgen -> Integral());
      // 	  if (TMath::Abs(hgen -> Integral() - py -> Integral() > 5.0))
      // 	    {
      // 	      printf("attention gen\n");
      // 	      getchar();
      // 	    }

      // 	}
      // delete px;
      // delete py;

      // if (f)  f -> Close();
    }

  //getchar();
  for (map<TString, vector<HistoUnfolding *>>::iterator mit = GetLevel(IN) -> _msyshistos.begin(); mit != GetLevel(IN) -> _msyshistos.end(); mit ++)
    {
      TString sample = mit -> first;
      //printf("sample [%s]\n", sample.Data()); 
      if (sample != "MC13TeV_SingleT_tW")
	continue;
      for(vector<HistoUnfolding *> :: iterator it = GetLevel(IN) -> _msyshistos[sample].begin(); it != GetLevel(IN) -> _msyshistos[sample].end(); it ++)
	{
	  printf("sys histos\n");
	  //	  (*it) -> ls();
	  // TFile *f = TFile::Open(TString("$EOS/analysis/") + (*it) -> _tag + ".root");
	  // TH1F * hreco = f ? (TH1F*) f -> Get("L_pull_angle_allconst_reco_leading_jet_2nd_leading_jet_DeltaRTotal") : nullptr; 
	  // TH1F * hgen = f ? (TH1F*) f -> Get("L_pull_angle_allconst_gen_leading_jet_2nd_leading_jet_DeltaRTotal") : nullptr; 
	  // TH1F * px = (TH1F*)(*it) -> _th2 -> ProjectionX();
	  // TH1F * py = (TH1F*)(*it) -> _th2 -> ProjectionY();

	  // printf("integral %f projx %f projy %f hgen %p hreco %p\n", (*it) -> _th2 ->  Integral(), px -> Integral(), py -> Integral(), hgen, hreco);
	  // if (hreco)
	  // 	{
	  // 	  hreco -> Scale(luminosity * (*it) -> _xsec);
	  // 	  printf("hreco Integral %f\n", hreco -> Integral());
	  // 	  if (TMath::Abs(hreco -> Integral() - px -> Integral() > 5.0))
	  // 	    {
	  // 	      printf("attention reco\n");
	  // 	      getchar();
	  // 	    }

	  // 	}
	  // if (hgen)
	  // 	{
	  // 	  hgen -> Scale(luminosity * (*it) -> _xsec);
	  // 	  printf("hgen Integral %f \n", hgen -> Integral());
	  // 	  if (TMath::Abs(hgen -> Integral() - py -> Integral() > 5.0))
	  // 	    {
	  // 	      printf("attention gen\n");
	  // 	      getchar();
	  // 	    }

	  // 	}
	  // delete px;
	  // delete py;

	  // if (f)f -> Close();
	}
    }
}
