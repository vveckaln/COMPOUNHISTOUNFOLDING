#include "CompoundHistoUnfolding.hh"
#include "TH1D.h"
#include "TRandom3.h"
#include "TMath.h"
ClassImp(CompoundHistoUnfolding)

void CompoundHistoUnfolding::createCov()
{
  printf("creating cov\n");
  const ResultLevelCode_t resultlevel = OUT;
  //  TH1F * hdsignal = GetLevel(resultlevel) -> GetHU(SIGNALNOMINALMO) -> Project(GEN, "hsignal");
  
  TH1F * hdsignal = GetLevel(resultlevel) -> GetHU(SIGNALPROXY, resultlevel) -> Project(GEN, "hsignal");
  hdsignal -> SetDirectory(nullptr);
  hdsignal -> Scale(1.0/hdsignal -> Integral());
  //NormaliseToBinWidth(hdsignal);
  const unsigned char nbins = hdsignal -> GetNbinsX() - 1;
  GetLevel(resultlevel) -> cov = new TMatrixD(nbins, nbins);
  GetLevel(resultlevel) -> cov -> Zero();
  //  GetLevel(resultlevel) -> cov -> Print();
  unsigned char ind = 0;
  for (vector<SampleDescriptor *>::iterator it = _expsyssamples.begin(); it != _expsyssamples.end(); it ++)
    {
      //      if (ind > 0)
      //break;
      printf("creating matrix exp %s\n", (*it) -> GetTag());

      ind ++;
      //printf("*************\n");
      TH1F * hsysdown = GetExpSys(resultlevel, (*it) -> GetTag(), DOWN) -> Project(GEN, "down");
      hsysdown -> SetDirectory(nullptr);
      TH1F * hsysup = GetExpSys(resultlevel, (*it) -> GetTag(), UP) -> Project(GEN, "up");
      hsysup -> SetDirectory(nullptr);
      
      hsysdown -> Scale(1.0/hsysdown -> Integral());
      hsysup -> Scale(1.0/hsysup -> Integral());
      //      NormaliseToBinWidth(hsysdown);
      //NormaliseToBinWidth(hsysup);
      const double meany1 = 0.5 * (hsysup -> GetBinContent(1) + hsysdown -> GetBinContent(1));
      const double meany2 = 0.5 * (hsysup -> GetBinContent(2) + hsysdown -> GetBinContent(2));
      const double cov1 = hsysdown -> GetBinContent(1) - meany1;
      const double cov2 = hsysdown -> GetBinContent(2) - meany2;
      //     printf("meany1 %f cov1 %f meany2 %.9f cov2 %.9f ch1 %.9f ch2 %.9f \n", meany1, cov1, meany2, cov2, 0.5 * (hsysdown -> GetBinContent(1) - hsysup -> GetBinContent(1)), 0.5 * (hsysdown -> GetBinContent(2) - hsysup -> GetBinContent(2)));
      for (unsigned char xind = 1; xind < nbins + 1; xind ++)
	{
	  TH1F * hxmax = 
	    TMath::Abs(hdsignal -> GetBinContent(xind) - hsysdown -> GetBinContent(xind)) >=
	    TMath::Abs(hdsignal -> GetBinContent(xind) - hsysup -> GetBinContent(xind)) ? 
	    hsysdown : hsysup;
	  const double xc = hdsignal -> GetBinContent(xind) - hxmax -> GetBinContent(xind);
	  const float meanx = 0.5 * (hsysup -> GetBinContent(xind) + hsysdown -> GetBinContent(xind));
	  for (unsigned char yind = 1; yind < nbins + 1; yind ++)
	    {
	      TH1F * hymax = 
		TMath::Abs(hdsignal -> GetBinContent(yind) - hsysdown -> GetBinContent(yind)) >=
		TMath::Abs(hdsignal -> GetBinContent(yind) - hsysup -> GetBinContent(yind)) ? 
		hsysdown : hsysup;
	      const double yc = hdsignal -> GetBinContent(yind) - hymax -> GetBinContent(yind);
	      const float meany = 0.5 * (hsysup -> GetBinContent(yind) + hsysdown -> GetBinContent(yind));
	      const float cov = (hsysup  -> GetBinContent(xind) - meanx) * (hsysup  -> GetBinContent(yind) - meany) +
		(hsysdown  -> GetBinContent(xind) - meanx) * (hsysdown  -> GetBinContent(yind) - meany);
	      //      printf("xind %u yind %u  meanx %f meany y %f up(xind) %f up(yind) %f down(xind) %f down(yind) %f cov %.12f\n", xind, yind, meanx, meany, hsysup  -> GetBinContent(xind), hsysup  -> GetBinContent(yind), hsysdown  -> GetBinContent(xind), hsysdown  -> GetBinContent(yind), cov);
	      char sign = 0.0;
	      if (cov > 0.0)
		{
		  sign = 1.0;
		}
	      else if (cov < 0.0)
		{
		  sign = -1.0;
		}
	      (*GetLevel(resultlevel) -> cov)(xind - 1, yind - 1) += TMath::Abs(xc)*TMath::Abs(yc)*sign; 
	    }
	}
      delete hsysdown;
      delete hsysup;
    }
  for (vector<SampleDescriptor *>::iterator it = _markedsyssamples.begin(); it != _markedsyssamples.end(); it ++)
    {
      printf("creating matrix \n", (*it) -> GetTag());
      TH1 * hdsignal = (TH1*) GetSys(IN, (*it) -> GetTag(), signaltag) -> Project(GEN);
      hdsignal -> SetDirectory(nullptr);
      hdsignal -> Scale(1.0/hdsignal -> Integral());

      TH1F * hsys = GetSys(resultlevel, (*it) -> GetTag(), signaltag) -> Project(GEN, "markedsys");
      hsys -> SetDirectory(nullptr);
      //      NormaliseToBinWidth(hsys);
      hsys -> Scale(1.0 / hsys -> Integral());
      for (unsigned char xind = 1; xind < nbins + 1; xind ++)
	{
	  const float meanx = 0.5 * (hsys -> GetBinContent(xind) + hdsignal -> GetBinContent(xind));
	  for (unsigned char yind = 1; yind < nbins + 1; yind ++)
	    {
	      const float meany = 0.5 * (hsys -> GetBinContent(yind) + hdsignal -> GetBinContent(yind));
	      const float cov = (hsys  -> GetBinContent(xind) - meanx) * (hsys  -> GetBinContent(yind) - meany) +
		(hdsignal  -> GetBinContent(xind) - meanx) * (hdsignal  -> GetBinContent(yind) - meany);
	      (*GetLevel(resultlevel) -> cov)(xind - 1, yind - 1) += cov; 
	    }
	}
      delete hdsignal;
    }
  for (vector<SampleDescriptor *>::iterator it = _nonexpsyssamples.begin(); it != _nonexpsyssamples.end(); it ++)
    {
      printf("creating matrix %s\n", (*it) -> GetTag());
      TH1 * hdsignalprox = nullptr;
      if ((*it) -> GetSysType() == THEORSYS)
	{
	  hdsignalprox = (TH1*) GetSys(IN, (*it) -> GetTag(), signaltag) -> Project(GEN);
	  hdsignalprox -> SetDirectory(nullptr);
	  hdsignalprox -> Scale(1.0/hdsignalprox -> Integral());
	}
      else
	{
	  hdsignalprox = hdsignal;
	}
      if (not GetSys(resultlevel, (*it) -> GetTag(), signaltag))
	continue;
      TH1F * hsys = GetSys(resultlevel, (*it) -> GetTag(), signaltag) -> Project(GEN, "markedsys");
      hsys -> SetDirectory(nullptr);
      //      NormaliseToBinWidth(hsys);
      hsys -> Scale(1.0 / hsys -> Integral());
      for (unsigned char xind = 1; xind < nbins + 1; xind ++)
	{
	  const float meanx = 0.5 * (hsys -> GetBinContent(xind) + hdsignalprox -> GetBinContent(xind));
	  for (unsigned char yind = 1; yind < nbins + 1; yind ++)
	    {
	      const float meany = 0.5 * (hsys -> GetBinContent(yind) + hdsignalprox -> GetBinContent(yind));
	      const float cov = (hsys  -> GetBinContent(xind) - meanx) * (hsys  -> GetBinContent(yind) - meany) +
		(hdsignalprox  -> GetBinContent(xind) - meanx) * (hdsignalprox  -> GetBinContent(yind) - meany);
	      (*GetLevel(resultlevel) -> cov)(xind - 1, yind - 1) += cov; 
	    }
	}
      if ((*it) -> GetSysType() == THEORSYS)
	{
	  delete hdsignalprox;
	}
    }

  printf("covariance matrix\n");
  GetLevel(resultlevel) -> cov -> Print();
  TCanvas * c = new TCanvas("cov", "cov");
  GetLevel(resultlevel) -> cov -> Draw("COLZ");
  c -> SaveAs(TString(_folder) + "/cov.png");
  delete hdsignal;
  printf("end creating cov\n");
  //  getchar();
} 
