#include "CompoundHistoUnfolding.hh"
#include "TMath.h"
#include "TROOT.h"
ClassImp(CompoundHistoUnfolding);
void CompoundHistoUnfolding::CreateTotalMCUncOLD(ResultLevelCode_t resultcode, RecoLevelCode_t recocode, bool shape)
{
  printf("void CompoundHistoUnfolding::CreateTotalMCUnc(ResultLevelCode_t resultcode %u, RecoLevelCode_t recocode %u, bool shape %s)\n", resultcode, recocode, shape ? "shape" : "noshape");

  MOCode_t mo = shape ? TOTALMCUNCSHAPE : TOTALMCUNC;
  if (not GetLevel(resultcode) -> GetHURef(mo))
    GetLevel(resultcode) -> GetHURef(mo) = new HistoUnfoldingTH1();
  TH1 *& htotalMCUnc = GetLevel(resultcode) -> GetHU(mo) -> GetTH1Ref(recocode);
  htotalMCUnc = GetLevel(resultcode) -> GetHU(TOTALMCSIGNALPROXY, resultcode) -> Project(recocode, CreateName(shape ? "totalMCUncShape" : "totalMCUnc"), "", _YaxisTitle);
  //  RejectNonReco(htotalMCUnc);
  const unsigned char nbins = htotalMCUnc -> GetNbinsX();

  /*  if (recocode == RECO and resultcode == IN)
      ApplyScaleFactor(htotalMCUnc);*/
  TH1 * hsig = GetLevel(resultcode) -> GetHU(SIGNALPROXY, resultcode) -> Project(recocode, CreateName("sig"));
  
  if (recocode == RECO and resultcode == IN)
    {
      ApplyScaleFactor(htotalMCUnc);
      ApplyScaleFactor(hsig);
    }
  const double integralnominal = hsig -> Integral();
  float sysUp[nbins + 2];
  float sysDown[nbins + 2];
  for (unsigned char ind = 0; ind < nbins + 2; ind ++)
    {
      sysUp[ind] = 0.0;
      sysDown[ind] = 0.0;
    }
  FILE * uncfile(nullptr), * uncfilefull(nullptr);
  if (not shape)
    {
      const TString uncfilenamefull = TString(_folder) + 
	"/unc_" + 
	tag_resultlevel[resultcode] + "_" + 
	tag_recolevel[recocode] + "_full.txt";
      uncfilefull = fopen(uncfilenamefull, "w");
      //      printf("%p %s\n", uncfile, uncfilename.Data());
    }
  unsigned char sys_ind = 0;
  map<TString, float> mcaterr;
  for(map<TString, vector<HistoUnfolding *>>::iterator bit = GetLevel(IN) -> _msyshistos.begin(); bit != GetLevel(IN) -> _msyshistos.end(); bit ++)
    {
      const TString sample = bit -> first;
      for (vector<HistoUnfolding *>::iterator it = GetLevel(resultcode) -> GetV(SYSMO, sample.Data()) -> begin(); it != GetLevel(resultcode) -> GetV(SYSMO, sample.Data()) -> end(); it ++)
	{
	  TH1 * hsigprox = nullptr;
	  TH1 * hsys = (*it) -> Project(recocode);
	  bool delsigprox = false;
	  if (sample == signaltag)
	    {
	      if (resultcode == OUT)
		{
		  if ((*it) -> GetSysType() == EXPSYS)
		    {
		      hsigprox = hsig;
		    }
		  else
		    {
		      hsigprox = GetSys(IN, (*it) -> GetTag(), sample.Data()) -> Project(recocode);
		      if (recocode == RECO)
			{
			  TH2 * signalmatrix = GetSys(IN, (*it) -> GetTag(), sample.Data()) -> GetTH2();
			  for (unsigned char bind = 0; bind <= hsigprox -> GetNbinsX() + 1; bind ++)
			    {
			      const float fact = signalmatrix -> GetBinContent(bind, 0) / signalmatrix -> Integral(bind, bind, 0, signalmatrix -> GetNbinsY() + 1);
			      hsigprox -> SetBinContent(bind, hsigprox -> GetBinContent(bind) * fact);
			    }
			}
		      delsigprox = true;
		    }
		}
	      else
		{
		  hsigprox = hsig;
		  
		  if (recocode == RECO)
		    {
		      ApplyScaleFactor(hsys);
		    }
		}
	    }
	  else
	    {
	      if (resultcode == IN)
		{
		  hsigprox = GetBackgroundH(sample.Data()) -> Project(recocode);
		  delsigprox = true;
		  if (recocode == RECO)
		    {
		      ApplyScaleFactor(hsys);
		      ApplyScaleFactor(hsigprox);
		    }			    ;
		}
	      else
		{
		  hsigprox = hsig;
		  
		}
	    }

	  const double integralnominal = hsigprox -> Integral();

	  if (shape)
	    {
	      const double integral = hsys -> Integral();
	      if (integral)
		hsys -> Scale(integralnominal/integral);
	  
	    }
	  for (unsigned char bin_ind = 0; bin_ind < nbins + 2; bin_ind ++)
	    {
	      const float diff = hsys -> GetBinContent(bin_ind) - hsigprox -> GetBinContent(bin_ind);
	      //	  diff *= signalmatrix -> Integral(0, signalmatrix -> GetNbinsX() + 1, bin_ind, bin_ind) / signalmatrix -> Integral(1, signalmatrix -> GetNbinsX(), bin_ind, bin_ind);
	      if (bin_ind == 1 and not shape)
		{
		  // float diffnom = hsys -> GetBinContent(bin_ind)/hsys -> Integral() - hsigprox -> GetBinContent(bin_ind)/hsigprox -> Integral();
		  // diffnom = fabs(diffnom) * hsigprox -> Integral()/hsigprox -> GetBinContent(bin_ind) * 100;
		  fprintf(uncfilefull, "%s,\t%.9f\t%.9f", (*it) -> GetTitle(), hsys -> GetBinContent(bin_ind)/hsys -> Integral(), hsigprox -> GetBinContent(bin_ind)/hsigprox -> Integral());
	   
		}
	      if (bin_ind > 1 and bin_ind < nbins + 1 and not shape)
		{
		  fprintf(uncfilefull, "\t%.9f\t%.9f", hsys -> GetBinContent(bin_ind)/hsys -> Integral(), hsigprox -> GetBinContent(bin_ind)/hsigprox -> Integral());
		}
	      if (bin_ind == nbins + 1 and not shape)
		{
		  fprintf(uncfilefull, "\n");

		}
	      if (diff > 0)
		{
		  sysUp[bin_ind] = TMath::Sqrt(TMath::Power(sysUp[bin_ind], 2) + TMath::Power(diff, 2));
		}
	      else
		{
		  sysDown[bin_ind] = TMath::Sqrt(TMath::Power(sysDown[bin_ind], 2) + TMath::Power(diff, 2));
		}
	    }
	  //printf("\n");
	  delete hsys;
	  if (delsigprox)//(*it) -> GetSysType() == THEORSYS and resultcode == OUT and TString(sample.Data()) == signaltag)
	    {
	      delete hsigprox;
	    }
	}
    }

  
  if (uncfilefull)
    {
      fclose(uncfilefull);
    }
  printf("listing unc resultcode %u recocode %u %s\n", resultcode, recocode, shape ? "shape": "noshape");
  // printf("initial");
  // htotalMCUnc -> Print("all");
  for (unsigned char bin_ind = 0; bin_ind < nbins + 2; bin_ind ++)
    {
      htotalMCUnc -> SetBinContent(bin_ind, htotalMCUnc -> GetBinContent(bin_ind) + (sysUp[bin_ind] - sysDown[bin_ind])/2.0);
            htotalMCUnc -> SetBinError(bin_ind, TMath::Sqrt(TMath::Power(htotalMCUnc -> GetBinError(bin_ind), 2) + TMath::Power((sysUp[bin_ind] + sysDown[bin_ind])/2.0, 2)));
      //      htotalMCUnc -> SetBinError(bin_ind, TMath::Sqrt(TMath::Power(htotalMCUnc -> GetBinError(bin_ind), 2) + 0.5*(TMath::Power(sysUp[bin_ind], 2) + TMath::Power(sysDown[bin_ind],2))));

    }
  delete hsig;
  //  NormaliseToBinWidth(htotalMCUnc);
  htotalMCUnc -> SetName(CreateName(TString("unc_") + tag_resultlevel[resultcode] + "_" + tag_recolevel[recocode] + (shape ? "_shape" : "")));
  htotalMCUnc -> SetTitle(CreateTitle(TString("unc ") + tag_resultlevel[resultcode] + " " + tag_recolevel[recocode] + (shape ? " shape" : "")));
  // printf("final");
  // htotalMCUnc -> Print("all");
  // printf("normalised\n");
  printf("calculating unc test %p resultcode %u recocode %u shape %u\n", GetLevel(resultcode) -> GetHU(mo) -> GetTH1(recocode), resultcode, recocode, shape);
  //  getchar();
  return;
  TH1F * hnorm = (TH1F*)htotalMCUnc -> Clone();
  NormaliseToBinWidth(hnorm);
  //  hnorm -> Print("all");
  if (resultcode == OUT and recocode == GEN and not shape)
    {
      TH1F * htotalMCUncNorm = GetLevel(resultcode) -> GetHU(TOTALMC) -> Project(recocode, CreateName(TString("unc_norm_") + tag_resultlevel[resultcode] + "_" + tag_recolevel[recocode] + (shape ? "_shape" : "")));
      htotalMCUncNorm -> SetTitle(CreateTitle(TString("unc norm ") + tag_resultlevel[resultcode] + " " + tag_recolevel[recocode] + (shape ? " shape" : "")));
      htotalMCUncNorm -> Scale(1.0/htotalMCUncNorm -> Integral());
      TH1F * hsig_norm = GetLevel(resultcode) -> GetHU(SIGNALMO) -> Project(recocode, CreateName("sig_norm"));
      hsig_norm -> Scale(1.0/hsig_norm -> Integral());
      float sysUp[nbins + 2];
      float sysDown[nbins + 2];
      for (unsigned char ind = 0; ind < nbins + 2; ind ++)
	{
	  sysUp[ind] = 0.0;
	  sysDown[ind] = 0.0;
	}
      for (vector<HistoUnfolding *>::iterator it = GetLevel(resultcode) -> GetV(SYSMO, "MC13TeV_TTJets") -> begin(); it != GetLevel(resultcode) -> GetV(SYSMO, "MC13TeV_TTJets") -> end(); it ++)
	{
	  TH1F* hsys = (*it) -> Project(recocode);
	  hsys -> Scale(1.0/hsys -> Integral());
	  for (unsigned char bin_ind = 0; bin_ind < nbins + 2; bin_ind ++)
	    {
	      const float diff = hsys -> GetBinContent(bin_ind) - hsig_norm -> GetBinContent(bin_ind);
	      if (diff > 0)
		{
		  sysUp[bin_ind] = TMath::Sqrt(TMath::Power(sysUp[bin_ind], 2) + TMath::Power(diff, 2));
		}
	      else
		{
		  sysDown[bin_ind] = TMath::Sqrt(TMath::Power(sysDown[bin_ind], 2) + TMath::Power(diff, 2));
		}
	    }
	  delete hsys;
	}
      for (unsigned char bin_ind = 0; bin_ind < nbins + 2; bin_ind ++)
	{
	  htotalMCUncNorm -> SetBinContent(bin_ind, htotalMCUncNorm -> GetBinContent(bin_ind) + (sysUp[bin_ind] - sysDown[bin_ind])/2.0);
	  htotalMCUncNorm -> SetBinError(bin_ind, TMath::Sqrt(TMath::Power(htotalMCUncNorm -> GetBinError(bin_ind), 2) + TMath::Power((sysUp[bin_ind] + sysDown[bin_ind])/2.0, 2)));
	}
      delete hsig_norm;
      htotalMCUncNorm -> Scale(1.0/htotalMCUncNorm -> Integral());
      NormaliseToBinWidth(htotalMCUncNorm);
      htotalMCUncNorm -> Scale(1.0/htotalMCUncNorm -> Integral());


      TCanvas * coutput = (TCanvas *) gROOT -> GetListOfCanvases() -> FindObject(CreateName("coutput"));
      //      coutput -> GetListOfPrimitives() ->  ls();
      TIter next(coutput -> GetListOfPrimitives());
      bool legend_found = false;
      TObject * obj = nullptr;
      while (not legend_found and (obj = next()))
	{
	  if (obj -> InheritsFrom("TLegend"))
	    {
	      legend_found = true;
	    }
	}
      TLegend * legend = (TLegend *) obj;
      legend -> AddEntry(htotalMCUncNorm);
      coutput -> cd();
      htotalMCUncNorm -> SetLineColor(kRed);
      htotalMCUncNorm -> Draw("SAME");
      coutput -> Modified();
      coutput -> Update();
      coutput -> SaveAs(TString(_folder) + "/" + "coutput.png");
    }
}


void CompoundHistoUnfolding::Compareunc(ResultLevelCode_t resultcode, RecoLevelCode_t recocode, bool shape)
{

}
