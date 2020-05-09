#include "CompoundHistoUnfolding.hh"
#include "TH1D.h"
#include "TRandom3.h"
#include "TMath.h"
ClassImp(CompoundHistoUnfolding)
void CompoundHistoUnfolding::PullAnalysis()
{
  TH2F *histMdetGenMC = (TH2F*) GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> Clone(CreateName("histMdetGenmc")); 
  histMdetGenMC -> SetDirectory(nullptr);
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
      histMdetGenToy -> SetDirectory(nullptr);
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
      histMgenToy -> SetDirectory(nullptr);
      TH1 * histMunfold = GetLevel(OUT) -> GetHU(SIGNALPROXY, OUT) -> GetTH1(GEN);
      float pull_max = 0.0;
      for (int i = 1; i < histMunfold -> GetNbinsX() + 1; ++i) 
	{
	  const double pull = (histMunfold -> GetBinContent(i) - histMgenToy -> GetBinContent(i)) / histMunfold -> GetBinError(i); 
	  if (TMath::Abs(pull) > pull_max)
	    pull_max = TMath::Abs(pull);
	}
      if (not hPull)
	{
	  hPull = new TH2F(CreateName("pull"), CreateTitle("pull; bin; pull; N_{toys}"), 
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
  hPull -> GetXaxis() -> SetTitle(_XaxisTitle);
  hPull -> GetYaxis() -> SetTitle("pull");
  hPull -> GetZaxis() -> SetTitleOffset(1.25);
  c -> SetRightMargin(0.18);
  c -> SaveAs(TString(_folder) + "/" + "/pull.png");
  TH1F * hstability = (TH1F *) histMdetGenMC -> ProjectionY(CreateName("stability"));
  hstability -> SetDirectory(nullptr);
  hstability -> Reset("ICE");
  TH1F * hpurity = (TH1F *) histMdetGenMC -> ProjectionY(CreateName("purity"));
  hpurity -> SetDirectory(nullptr);
  hpurity -> Reset("ICE");
  TH2F * histsig = GetLevel(IN) -> GetHU(SIGNALPROXY, IN) -> GetTH2();
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
  THStack stack("stabpur", TString("stabpur; ") + _XaxisTitle + "; ratio");
  stack.Add(hstability);
  stack.Add(hpurity);
  stack.SetMinimum(0.0);
  stack.SetMaximum(1.4 * stack.GetMaximum("nostack"));
  TCanvas * cstabpur = new TCanvas(CreateName("stabpur"), CreateTitle("stabpur"));
  hstability -> SetTitle("stability");
  hpurity -> SetTitle("purity");
  stack.Draw("nostack");
  TLegend * legend = new TLegend(0.7, 0.7, 0.85, 0.85);
  legend -> SetLineWidth(0);
  legend -> AddEntry(hstability);
  legend -> AddEntry(hpurity);
  legend -> Draw("SAME");

  cstabpur -> SaveAs(TString(_folder) + "/stabpur.png");
  TH2D* hpurity2D = (TH2D*) histsig -> Clone(CreateName("purity2D"));
  hpurity2D -> SetDirectory(nullptr);
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
  cpur -> SetRightMargin(0.2);
  cpur -> SaveAs(TString(_folder) + "/migrationpurity.png");
  delete histMdetGenMC;

} 

void CompoundHistoUnfolding::CreateListOfExpSysSamples()
{
  for (vector<HistoUnfolding *>::iterator it = GetV(IN, SYSMO, signaltag) -> begin(); it != GetV(IN, SYSMO, signaltag) -> end(); it ++)
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
	  else
	    _nonexpsyssamples.push_back(*it);
	 
	}
      else
	{
	  _nonexpsyssamples.push_back(*it);
	}
    }
}

// void CompoundHistoUnfolding::CreateListOfBckgSamples()
// {
//   for (map<TString, vector<HistoUnfolding *>>::iterator bit = _msyshistos.begin(); bit != _msyshistos.end(); bit ++)
//     {
//       TString sample = bit -> first;
//       if (sample != signaltag)
// 	continue;
//       for (vector<HistoUnfolding *>::iterator it = GetV(IN, SYSMO, sample.Data()) -> begin(); it != GetV(IN, SYSMO, sample.Data()) -> end(); it ++)
// 	{
// 	  TString tag((*it) -> GetTag());
// 	  if (tag.Contains("_up"))
// 	    {
// 	      tag.ReplaceAll("_up", "");
// 	      TString title((*it) -> GetTitle());
// 	      title.ReplaceAll(" up", "");
// 	      SampleDescriptor *sd = new SampleDescriptor(*it);
// 	      sd -> SetTag(tag);
// 	      sd -> SetTitle(title);
// 	      _mbckghistos.at(sample). push_back(sd);
// 	    }
// 	}
//     }
// }

void CompoundHistoUnfolding::MarkSysSample(const char * tag)
{
  HistoUnfolding * sys = GetSys(IN, tag, signaltag);
  if (GetSys(IN, tag, signaltag))
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
  vector<HistoUnfolding *>::iterator it = GetV(resultcode, SYSMO, signaltag) -> begin();
  HistoUnfolding * expsys = nullptr;
  const TString comp = expsyscode == UP ? "_up" : "_down";
  while ( it != GetV(resultcode, SYSMO, signaltag) -> end() or not expsys)
    {
      //(*it) -> ls();
      //      printf("%s\n", (*it) -> GetTag());
      TString tag ((*it) -> GetTag());
      if (TString(expsystag) + comp == tag)
	{
	  expsys = *it;
	}
      it ++;
    }
  return expsys;
}

HistoUnfolding * CompoundHistoUnfolding::GetSys(ResultLevelCode_t resultcode, const char * systag, const char * sample)
{
  
  return GetLevel(resultcode) -> GetSys(systag, sample);
}


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

double CompoundHistoUnfolding::GetChi() 
{
  
  TH1F * hdata = _chunominal -> GetLevel(OUT) -> GetHU(SIGNALPROXY, OUT) -> Project(GEN, "hdata");
  hdata -> SetDirectory(nullptr);
  TH1F * hmodel = GetLevel(IN) -> GetHU(SIGNALPROXY, IN) -> Project(GEN, "hmodel");
  hmodel -> SetDirectory(nullptr);
  hdata -> Scale(1.0/hdata -> Integral());
  hmodel -> Scale(1.0/hmodel -> Integral());
  //NormaliseToBinWidth(hdata);
  //NormaliseToBinWidth(hmodel);
  const char nbins = hmodel -> GetNbinsX() - 1;
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
  TMatrixD inv(*GetLevel(OUT) -> cov);
  inv.Invert();
  
  inv.Print();
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

TH1 * CompoundHistoUnfolding::GetSignalProxy(ResultLevelCode_t resultcode, RecoLevelCode_t recocode, const char * sample, ExpSysType_t sys, const char * tag)
{
  if (resultcode == IN)
    {
      if (TString(sample) == signaltag)
	{
	  return GetLevel(resultcode) -> GetHU(SIGNALPROXY, resultcode) -> Project(recocode, CreateName("sig"));
	}
      else
	{
	  return GetBackgroundH(sample) -> Project(recocode);
	}
    }
  if (resultcode == OUT)
    {
      if (TString(sample) == signaltag)
	{
	  if (sys == EXPSYS)
	    {
	    return GetLevel(resultcode) -> GetHU(SIGNALPROXY, resultcode) -> Project(recocode, CreateName("sig"));
	    }
	  if (sys == THEORSYS)
	    {
	      // if (TString(tag).Contains("non_clos"))
	      // 	printf("%p\n", GetSys(IN, tag, sample));
	      
	      TH1 * hsigprox = GetSys(IN, tag, sample) -> Project(recocode);
	      if (recocode == RECO)
		{
		  ApplyScaleFactor(hsigprox);
		  // 	  TH2 * signalmatrix = GetSys(IN, tag, sample) -> GetTH2();
		  // 	  for (unsigned char bind = 0; bind <= hsigprox -> GetNbinsX() + 1; bind ++)


	      // 	    {
	      // 	      const float fact = signalmatrix -> GetBinContent(bind, 0) / signalmatrix -> Integral(bind, bind, 0, signalmatrix -> GetNbinsY() + 1);
	      // 	      hsigprox -> SetBinContent(bind, hsigprox -> GetBinContent(bind) * fact);
	      // 	    }
	      	}

	      return hsigprox;
	    }
	}
      else
	{
	  return GetLevel(resultcode) -> GetHU(SIGNALPROXY, resultcode) -> Project(recocode, CreateName("sig"));

	}

    }
}
