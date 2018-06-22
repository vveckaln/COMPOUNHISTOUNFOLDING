#include "CompoundHistoUnfolding.hh"
#include "TH1D.h"
#include "TRandom3.h"
#include "TMath.h"
ClassImp(CompoundHistoUnfolding)
void CompoundHistoUnfolding::PullAnalysis()
{
  TH2F *histMdetGenMC = (TH2F*) GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> Clone(CreateName("histMdetGenmc")); 
  histMdetGenMC -> RebinX(2);
  const double integral = histMdetGenMC -> Integral(0, -1, 0, -1);
  const double nExpectedEvents = integral;
  vector<double> wheel = { 0.0 };
  for (int i = 0; i < histMdetGenMC -> GetNcells() + 1; ++ i) 
    {
      wheel.push_back(wheel.back() + histMdetGenMC -> GetBinContent(i));
    }
  TRandom3 random(1);
  
  TH2F* hPull = nullptr;   const unsigned int nToys = 100;
  for (unsigned int t = 0; t < nToys; ++t) 
    {
      TH2D* histMdetGenToy = (TH2D*) histMdetGenMC -> Clone(CreateName("histMdetGenToy"));
      histMdetGenToy -> Reset();
    
      unsigned int drawn = 0;
      while (drawn < nExpectedEvents) 
	{
	  const double pick = random.Uniform(integral);
	  vector<double>::iterator low = lower_bound(wheel.begin(), wheel.end(), pick);
	  const int position = low - wheel.begin() - 1;
	  histMdetGenToy -> SetBinContent(position, histMdetGenToy -> GetBinContent(position) + 1);
	  ++drawn;
	}
      TH1D *histMgenToy = histMdetGenToy -> ProjectionY(CreateName("histMgenToy"));
      TH1F * histMunfold = GetLevel(OUT) -> GetHU(SIGNALMO) -> GetTH1(GEN);
      float pull_max = 0.0;
      for (int i = 1; i < histMunfold -> GetNbinsX() + 1; ++i) 
	{
	  const double pull = (histMunfold -> GetBinContent(i) - histMgenToy -> GetBinContent(i)) / histMunfold -> GetBinError(i); 
	  if (TMath::Abs(pull) > pull_max)
	    pull_max = TMath::Abs(pull);
	}
      if (not hPull)
	{
	  hPull = new TH2F(CreateName("pull"), CreateTitle("pull; bin; pull"), 
			 histMdetGenMC -> GetYaxis() -> GetNbins(), 0.5, histMdetGenMC -> GetYaxis() -> GetNbins() + 0.5, 
			 50, - pull_max * 1.4, pull_max * 1.4);
	  hPull -> SetDirectory(nullptr);
	}
      for (int i = 1; i < histMunfold -> GetNbinsX() + 1; ++i) 
	{
	  const double pull = (histMunfold -> GetBinContent(i) - histMgenToy -> GetBinContent(i)) / histMunfold -> GetBinError(i); 
	  hPull -> Fill(i, pull);
	}
      delete histMdetGenToy;
      delete histMgenToy;
    }
  TCanvas * c = new TCanvas(CreateName("pull"), CreateTitle("pull"));
  hPull -> Draw("COLZ");
  c -> SaveAs(TString(_folder) + "/" + "/pull.png");
  TH1F * hstability = (TH1F *) histMdetGenMC -> ProjectionY(CreateName("stability"));
  hstability -> SetDirectory(nullptr);
  hstability -> Reset("ICE");
  TH1F * hpurity = (TH1F *) histMdetGenMC -> ProjectionY(CreateName("purity"));
  hpurity -> SetDirectory(nullptr);
  hpurity -> Reset("ICE");
  TH2F * histsig = GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2();
  for (unsigned char ind = 1; ind < histsig -> GetNbinsY() + 1; ind ++)
    {
      float diag = histsig -> GetBinContent(2*(ind -1) + 1, ind);
      diag += histsig -> GetBinContent(2*(ind - 1) + 2, ind);
      const float stability = diag / histsig -> Integral(1, histsig -> GetNbinsX(), ind, ind);
      hstability -> SetBinContent(ind, stability);
      const float purity = diag / histsig -> Integral(2*(ind - 1) + 1, 2 * (ind - 1) + 2, 1, histsig -> GetNbinsY());
      hpurity -> SetBinContent(ind, purity);
    }
  hpurity -> SetLineColor(kRed);
  hstability -> SetLineColor(kBlue);
  hstability -> SetLineStyle(7);
  const float max = hstability -> GetMaximum() > hpurity -> GetMaximum() ? hstability -> GetMaximum() : hpurity -> GetMaximum();
  hstability -> SetMaximum(1.5*max);
  hstability -> SetMinimum(0.0);
  hpurity -> SetMaximum(1.5*max);
  hpurity -> SetMinimum(0.0);
  TCanvas * cstabpur = new TCanvas(CreateName("stabpur"), CreateTitle("stabpur"));
  hstability -> SetTitle(CreateTitle("stability"));
  hpurity -> SetTitle(CreateTitle("purity"));
  hstability -> Draw();
  hpurity -> Draw("SAME");
  TLegend * legend = new TLegend(0.6, 0.6, 1.0, 1.0);
  legend -> AddEntry(hstability);
  legend -> AddEntry(hpurity);
  legend -> Draw("SAME");

  cstabpur -> SaveAs(TString(_folder) + "/" + "/stabpur.png");
  TH2D* hpurity2D = (TH2D*) histsig -> Clone(CreateName("purity2D"));
  hpurity2D -> RebinX(2);
  hpurity2D -> SetTitle(CreateTitle("purity"));

  for (unsigned int xbin_ind = 1; xbin_ind < hpurity2D -> GetNbinsX() + 1; xbin_ind ++)
    {
      for (unsigned int ybin_ind = 1; ybin_ind < hpurity2D -> GetNbinsY() + 2; ybin_ind ++)
	{
	  hpurity2D -> SetBinContent(xbin_ind, ybin_ind, hpurity2D -> GetBinContent(xbin_ind, ybin_ind)/hpurity2D -> Integral(xbin_ind, ybin_ind, 1, histsig -> GetNbinsY())); 
	}
    }
  TCanvas * cpur = new TCanvas(CreateName("purity"), CreateTitle("purity"));
  hpurity2D -> Draw("COLZ");
  if (hpurity2D -> GetNbinsX() < 6)
    hpurity2D -> Draw("TEXTSAME");
  cpur -> SaveAs(TString(_folder) + "/migationpurity.png");
  delete histMdetGenMC;

} 

void CompoundHistoUnfolding::CreateListOfExpSysSamples()
{
  for (vector<HistoUnfolding *>::iterator it = GetV(IN, SYSMO) -> begin(); it != GetV(IN, SYSMO) -> end(); it ++)
    {
      if ((*it) -> GetSysType() == EXPSYS)
	{
	  TString tag((*it) -> GetTag());
	  if (tag.Contains("_up"))
	    {
	      tag.ReplaceAll("_up", "");
	      TString title((*it) -> GetTitle());
	      title.ReplaceAll(" up", "");
	      SampleDescriptor *sd = new SampleDescriptor();
	      sd -> SetTag(tag);
	      sd -> SetTitle(title);
	      sd -> SetSampleType(EXPSYS);
	      _expsyssamples.push_back(sd);
	    }
	    
	}
    }
  for (vector<SampleDescriptor *>:: iterator it  = _expsyssamples.begin(); it != _expsyssamples.end(); it ++)
    {
      //(*it) -> ls();
    }
}

void CompoundHistoUnfolding::MarkSysSample(const char * tag)
{
  HistoUnfolding * sys = GetSys(IN, tag);
  if (GetSys(IN, tag))
    {
      SampleDescriptor *sd = new SampleDescriptor();
      sd -> SetTag(tag);
      sd -> SetTitle(sys -> GetTitle());
      sd -> SetSampleType(sys -> GetSampleType());
      _markedsyssamples.push_back(sd);
    }
  else
    {
      printf("systematics %s not found!\n", tag);
    }
}


HistoUnfolding * CompoundHistoUnfolding::GetExpSys(ResultLevelCode_t resultcode, const char * expsystag, ExpSysType_t expsyscode)
{
  vector<HistoUnfolding *>::iterator it = GetV(resultcode, SYSMO) -> begin();
  HistoUnfolding * expsys = nullptr;
  const TString comp = expsyscode == UP ? "_up" : "_down";
  while ( it != GetV(resultcode, SYSMO) -> end() or not expsys)
    {
      TString tag ((*it) -> GetTag());
      //      printf("%s\n", tag.Data());
      if (TString(expsystag) + comp == tag)
	{
	  expsys = *it;
	}
      it ++;
    }
  return expsys;
}

HistoUnfolding * CompoundHistoUnfolding::GetSys(ResultLevelCode_t resultcode, const char * systag)
{
  
  vector<HistoUnfolding *>::iterator it = GetV(resultcode, SYSMO) -> begin();
  HistoUnfolding * sys = nullptr;
  while ( it != GetV(resultcode, SYSMO) -> end() or not sys)
    {
      //      (*it) -> ls();
      TString tag ((*it) -> GetTag());
      //      printf("%s\n", tag.Data());
      if (TString(systag) == tag)
	{
	  sys = *it;
	}
      it ++;
    }
  return sys;
}


void CompoundHistoUnfolding::createCov()
{
  const ResultLevelCode_t resultlevel = OUT;
  TH1F * hdsignal = GetLevel(resultlevel) -> GetHU(SIGNALMO) -> Project(GEN, "hsignal");
  //NormaliseToBinWidth(hdsignal);
  const unsigned char nbins = hdsignal -> GetNbinsX();
  GetLevel(resultlevel) -> cov = new TMatrixD(nbins, nbins);
  GetLevel(resultlevel) -> cov -> Zero();
  GetLevel(resultlevel) -> cov -> Print();
  for (vector<SampleDescriptor *>::iterator it = _expsyssamples.begin(); it != _expsyssamples.end(); it ++)
    {
      TH1F * hsysdown = GetExpSys(resultlevel, (*it) -> GetTag(), DOWN) -> Project(GEN, "down");
      TH1F * hsysup = GetExpSys(resultlevel, (*it) -> GetTag(), UP) -> Project(GEN, "up");
      //      NormaliseToBinWidth(hsysdown);
      //NormaliseToBinWidth(hsysup);
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
      TH1F * hsys = GetSys(resultlevel, (*it) -> GetTag()) -> Project(GEN, "markedsys");
      NormaliseToBinWidth(hsys);
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
    }
  printf("covariance matrix\n");
  GetLevel(resultlevel) -> cov -> Print();
  TCanvas * c = new TCanvas("cov", "cov");
  GetLevel(resultlevel) -> cov -> Draw("COLZ");
  c -> SaveAs(TString(_folder) + "/cov.png");
  delete hdsignal;
} 

double CompoundHistoUnfolding::GetChi() 
{
  TH1F * hdata = GetLevel(OUT) -> GetHU(SIGNALMO) -> Project(GEN, "hdata");
  TH1F * hmodel = GetLevel(IN) -> GetHU(SIGNALMO) -> Project(GEN, "hmodel");
  //NormaliseToBinWidth(hdata);
  //NormaliseToBinWidth(hmodel);
  const char nbins = hmodel -> GetNbinsX();
  TMatrixD D(nbins, 1);
  for (unsigned char bin_ind = 1; bin_ind < nbins + 1; bin_ind ++)
    {
      D(bin_ind - 1, 0) = hdata -> GetBinContent(bin_ind) - hmodel -> GetBinContent(bin_ind);
    }
  printf("D-M\n");
  D.Print();
  TMatrixD DTr(1, nbins);
  DTr = DTr.Transpose(D);
  //DTr.Print();
  TMatrixD inv = GetLevel(OUT) -> cov -> Invert();
  //inv.Print();
  TMatrixD  A(1, nbins);
  A.Mult(DTr, inv);
  TMatrixD chi(1, 1);
  printf("chi:\n");
  chi.Mult(A, D);
  chi.Print();
  delete hdata;
  delete hmodel;
  return chi(0, 0);
}
