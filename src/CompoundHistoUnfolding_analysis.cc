#include "CompoundHistoUnfolding.hh"
#include "TH1D.h"
#include "TRandom3.h"
#include "TMath.h"
#include "TUnfoldDensity.h"
#include "TExec.h"
ClassImp(CompoundHistoUnfolding)
void CompoundHistoUnfolding::PullAnalysis()
{
  TH2 *histMdetGenMC = (TH2*) GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> Clone(CreateName("histMdetGenmc")); 
  histMdetGenMC -> SetDirectory(nullptr);
  printf("pull analaysis nbinsx %u nbinsy %u\n", histMdetGenMC -> GetXaxis() -> GetNbins(), histMdetGenMC -> GetYaxis() -> GetNbins());
  //  getchar();
  const double integral = histMdetGenMC -> Integral(0, -1, 0, -1);
  const double nExpectedEvents = integral;
  vector<double> wheel = { 0.0 };
  for (int ind = 0; ind < histMdetGenMC -> GetNcells() + 1; ++ ind) 
    {
      wheel.push_back(wheel.back() + histMdetGenMC -> GetBinContent(ind));
    }
  TRandom3 random(1);
  TH2 * hsignal = (TH2*) GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> Clone(CreateName("hsignal_pullanalysis"));


  for (unsigned char bind = 0; bind <= hsignal -> GetNbinsX() + 1; bind ++)
    {
      hsignal -> SetBinContent(bind, 0, 0.0);
    }

  const unsigned int nToys = 100;
  const unsigned char nbinsgen = histMdetGenMC -> GetYaxis() -> GetNbins();
  vector<float> pullv[nbinsgen]; 
  for (unsigned char bingenind = 0; bingenind < nbinsgen; bingenind ++)
    {
      pullv[bingenind].reserve(nToys);
    }

  for (unsigned int tind = 0; tind < nToys; ++tind) 
    {
      TH2 * histMdetGenToy = (TH2*) histMdetGenMC -> Clone(CreateName("histMdetGenToy"));
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

      for (unsigned char cind = 0; cind < histMdetGenToy -> GetNcells() + 2; ++ cind) 
	{
	  histMdetGenToy -> SetBinError(cind, sqrt(histMdetGenToy -> GetBinContent(cind)));
	}
      TH1 *histMrecoToy = histMdetGenToy -> ProjectionX(CreateName("histMrecoToy"));
      TH1 *histMgenToy = histMdetGenToy -> ProjectionY(CreateName("histMgenToy"));
      
      histMrecoToy -> SetDirectory(nullptr);
      ApplyScaleFactor(histMrecoToy);
      TUnfoldDensity unfold(hsignal, TUnfold::kHistMapOutputVert, TUnfold::kRegModeCurvature, TUnfold::kEConstraintArea, TUnfoldDensity::kDensityModeBinWidthAndUser);
      unfold.SetInput(histMrecoToy);
      const float tau = 0.0;
      unfold.DoUnfold(tau);

      TH1 * histMunfold = unfold.GetOutput("histMunfold");
      for (unsigned char bind = 1; bind < histMunfold -> GetNbinsX() + 1; ++ bind) 
	{
	  const float pull = (histMunfold -> GetBinContent(bind) - histMgenToy -> GetBinContent(bind)) / histMunfold -> GetBinError(bind); 
	  pullv[bind - 1].push_back(pull);

	}
      delete histMdetGenToy;
      delete histMgenToy;
      delete histMunfold;
      delete histMrecoToy;
    }

  float pull_max = 0.0;
  for(unsigned int toyind = 0; toyind < nToys; toyind ++)
    {
      for (unsigned char bind = 0; bind < nbinsgen; bind ++)
	{
	  if (fabs(pullv[bind][toyind]) > pull_max)
	    {
	      pull_max = fabs(pullv[bind][toyind]);
	    }
	}
    }

  TH2 * hPull = new TH2F(CreateName("pull"), CreateTitle("pull; bin; pull; N_{toys}"), 
			 nbinsgen, 0.5, nbinsgen + 0.5, 
			 50, - pull_max * 1.4, pull_max * 1.4);
  hPull -> SetDirectory(nullptr);

  for(unsigned int toyind = 0; toyind < nToys; toyind ++)
    {
      for (unsigned char bind = 0; bind < nbinsgen; bind ++)
	{
	  hPull -> Fill(bind + 1, pullv[bind][toyind]);
	}
    }



  TCanvas * c = new TCanvas(CreateName("pull"), CreateTitle("pull"));
  TExec *ex = new TExec("ex", "palettedefault()");
  hPull -> Draw("COLZ");
  ex -> Draw();
  hPull -> Draw("COLZSAME");
  hPull -> GetXaxis() -> SetTitle(_XaxisTitle);
  hPull -> GetYaxis() -> SetTitle("pull");
  hPull -> GetZaxis() -> SetTitleOffset(1.25);
  c -> SetRightMargin(0.18);
  c -> SaveAs(TString(_folder) + "/" + "pull.png");

  TObjArray aSlices;
  TF1 * fgauss = new TF1("g", "gaus", -pull_max, pull_max);
  fgauss -> SetParameters(1.0, 0.0, 1.0);
  hPull -> FitSlicesY(fgauss, 0, -1, 0, "QNR", &aSlices);
  

  TCanvas * cmean = new TCanvas;
  ((TH1 * ) aSlices[1]) -> GetYaxis() -> SetTitle("mean #mu [rad]");
  aSlices[1] -> Draw();
  cmean -> SaveAs(TString(_folder) + "/" + "pull_mean.png");


  TCanvas * csigma = new TCanvas;
  ((TH1*) aSlices[2]) -> GetYaxis() -> SetTitle("#sigma [rad]");
  aSlices[2] -> Draw();
  csigma -> SaveAs(TString(_folder) + "/" + "pull_sigma.png");

  // TH1F * hstability = (TH1F *) histMdetGenMC -> ProjectionY(CreateName("stability"));
  // hstability -> SetDirectory(nullptr);
  // hstability -> Reset("ICE");
  // TH1F * hpurity = (TH1F *) histMdetGenMC -> ProjectionY(CreateName("purity"));
  // hpurity -> SetDirectory(nullptr);
  // hpurity -> Reset("ICE");
  // TH2F * histsig = GetLevel(IN) -> GetHU(SIGNALPROXY, IN) -> GetTH2();
  // for (unsigned char ind = 1; ind < histsig -> GetNbinsY() + 1; ind ++)
  //   {
  //     float diag = histsig -> GetBinContent(2*(ind -1) + 1, ind);
  //     diag += histsig -> GetBinContent(2*(ind - 1) + 2, ind);
  //     const float stability = diag / histsig -> Integral(1, histsig -> GetNbinsX(), ind, ind);
  //     hstability -> SetBinContent(ind, stability);
  //     const float purity = diag / histsig -> Integral(2*(ind - 1) + 1, 2 * (ind - 1) + 2, 1, histsig -> GetNbinsY());
  //     hpurity -> SetBinContent(ind, purity);
  //   }
  // hpurity -> SetLineColor(kRed);
  // hstability -> SetLineColor(kBlue);
  // hstability -> SetLineStyle(7);
  // THStack stack("stabpur", TString("stabpur; ") + _XaxisTitle + "; ratio");
  // stack.Add(hstability);
  // stack.Add(hpurity);
  // stack.SetMinimum(0.0);
  // stack.SetMaximum(1.4 * stack.GetMaximum("nostack"));
  // TCanvas * cstabpur = new TCanvas(CreateName("stabpur"), CreateTitle("stabpur"));
  // hstability -> SetTitle("stability");
  // hpurity -> SetTitle("purity");
  // stack.Draw("nostack");
  // TLegend * legend = new TLegend(0.7, 0.7, 0.85, 0.85);
  // legend -> SetLineWidth(0);
  // legend -> AddEntry(hstability);
  // legend -> AddEntry(hpurity);
  // legend -> Draw("SAME");

  // TCanvas * cstabpur = stabpur();
  // cstabpur -> SaveAs(TString(_folder) + "/stabpur.png");
  // TH2D* hpurity2D = (TH2D*) histsig -> Clone(CreateName("purity2D"));
  // hpurity2D -> SetDirectory(nullptr);
  // hpurity2D -> RebinX(2);
  // hpurity2D -> SetTitle(CreateTitle("purity"));

  // for (unsigned int xbin_ind = 1; xbin_ind < hpurity2D -> GetNbinsX() + 1; xbin_ind ++)
  //   {
  //     for (unsigned int ybin_ind = 1; ybin_ind < hpurity2D -> GetNbinsY() + 2; ybin_ind ++)
  // 	{
  // 	  hpurity2D -> SetBinContent(xbin_ind, ybin_ind, hpurity2D -> GetBinContent(xbin_ind, ybin_ind)/hpurity2D -> Integral(xbin_ind, ybin_ind, 1, histsig -> GetNbinsY())); 
  // 	}
  //   }
  // TCanvas * cpur = new TCanvas(CreateName("purity"), CreateTitle("purity"));
  // hpurity2D -> Draw("COLZ");
  // if (hpurity2D -> GetNbinsX() < 6)
  //   hpurity2D -> Draw("TEXTSAME");
  // cpur -> SetRightMargin(0.2);
  // cpur -> SaveAs(TString(_folder) + "/migrationpurity.png");
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



void CompoundHistoUnfolding::createCov_new()
{
  printf("void CompoundHistoUnfolding::createCov_new()\n");
  ResultLevelCode_t recolevels[2] = {RECO, GEN};
  for (ResultLevelCode_t resultlevel = 0; resultlevel < 2; resultlevel ++)
    {
      const RecoLevelCode_t recolevel = recolevels[resultlevel];
      TH1 * hsignal = GetLevel(resultlevel) -> GetHU(SIGNALMO) -> Project(recolevel);
      const int nbins = hsignal -> GetNbinsX();
      const unsigned char range = nbins - 1;  
      delete hsignal;
      printf("resultlevel %s nbins %u\n", tag_resultlevel[resultlevel], nbins);
      TMatrixD expunc(nbins, nbins);
      //  float scales[nbins];
      // for (unsigned char bind = 1; bind <= nbins; bind ++)
      //   {
      //     scales[bind - 1] = inputsig -> Integral(1, inputsig -> GetNbinsX(), bind, bind)/inputsig -> Integral(0, inputsig -> GetNbinsX(), bind, bind);
      //     printf("%f\n", scales[bind -1]);
      //   }
      // getchar();
      const bool prune = false;
      for (map<TString, vector<array<HistoUnfolding *, 2>>> :: iterator bit = GetLevel(resultlevel) -> _m2dirsyshistos.begin(); bit != GetLevel(resultlevel) -> _m2dirsyshistos.end(); bit ++)
	{
	  //    continue;
	  TString sample = bit -> first;
	  if (sample != signaltag)
	    {
	    }
	  for (vector<array<HistoUnfolding *, 2>>::iterator it =  bit -> second.begin(); it != bit -> second.end(); it ++)
	    {
	      if ((*it)[0] -> GetSysType() != EXPSYS)
		continue;
	      // (*it)[0] -> ls();
	      TH1 * hsigup   = GetSignalProxy(resultlevel, recolevel, sample.Data(), (*it)[0] -> GetSysType(), (*it)[0] -> GetTag(), prune);
	      // TH2 * hsigup2 = GetLevel(OUT) -> GetHU(SIGNALPROXY, OUT);
	      hsigup -> SetName(TString(hsigup -> GetName()) + "_up");
	      TH1 * hsigdown = GetSignalProxy(resultlevel, recolevel, sample.Data(), (*it)[1] -> GetSysType(), (*it)[1] -> GetTag(), prune);
	      hsigdown -> SetName(TString(hsigdown -> GetName()) + "_down");
	      // for (unsigned char bind = 0; bind < nbins; bind ++)
	      //   {
	      //     const float fact = hsigup2 -> Integral(1, hsigup2 -> GetNbinsX(), 1, nbins)/hsigup2 -> Integral(1, hsigup2 -> GetNbinsX(), 0, nbins);
	      //     printf("%f\n", fact);
	      //   }
	      // getchar();
	      // hsigup -> Print("all");
	      // hsigdown -> Print("all"); 
	      HistoUnfolding * up     = (*it)[0];
	  
	      TH1 *            hup    = up -> Project(recolevel, "up");
	      HistoUnfolding * down   = (*it)[1];
	      TH1 *            hdown  = down -> Project(recolevel, "down");
	      if (prune)
		{
		  Prune(hup, (*it)[0] -> GetTag(), sample.Data(), (*it)[0] -> GetSysType(), GEN);
		  Prune(hdown, (*it)[1] -> GetTag(), sample.Data(), (*it)[1] -> GetSysType(), GEN);
		}

	      // for (unsigned char bind = 1; bind < nbins + 1; bind ++)
	      //   {
	      //     hsigdown -> SetBinContent(bind, hsigdown -> GetBinContent(bind) * (1.0 - scales[bind -1]));
	      //     hsigup   -> SetBinContent(bind, hsigup -> GetBinContent(bind) * (1.0 - scales[bind -1]));
	      //     hup      -> SetBinContent(bind, hup -> GetBinContent(bind) * (1.0 - scales[bind -1]));
	      //     hdown    -> SetBinContent(bind, hdown -> GetBinContent(bind) * (1.0 - scales[bind -1]));

	      //   }
	      const double integralnominalup   = hsigup   -> Integral();
	      const double integralnominaldown = hsigdown -> Integral();

	      if (hup -> Integral())
		{
		  hup -> Scale(1.0/*integralnominalup*//hup -> Integral());
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
		  hdown -> Scale(/*integralnominaldown*/1.0/hdown -> Integral());
		}
	      else
		{
		  //printf("deleting %s\n", down -> GetTag());
		  delete     hdown;
		  delete     hsigdown;
		  hdown    = nullptr;
		  hsigdown = nullptr;
		}
	      if (not hsigdown or not hsigup)

		continue;
	      hsigdown -> Scale(1.0/hsigdown -> Integral());
	      hsigup -> Scale(1.0/hsigup -> Integral());
	  
	      for (unsigned char bind1 = 1; bind1 <= nbins; bind1 ++)
		{
		  const double      sup1   = hup -> GetBinContent(bind1) - hsigup -> GetBinContent(bind1);
		  const double      sdown1 = hdown -> GetBinContent(bind1) - hsigdown -> GetBinContent(bind1);
		  double diff1 = fabs(sup1) > fabs(sdown1) ? fabs(sup1) : fabs(sdown1);
		  const double mean1 = 0.5 * (hup -> GetBinContent(bind1) + hdown -> GetBinContent(bind1));
		  // if (sdown1 < 0.0)
		  // 	diff1 *= - 1.0;
		  for (unsigned char bind2 = 1; bind2 <= nbins; bind2 ++)
		    {
		      const double      sup2   = hup -> GetBinContent(bind2) - hsigup -> GetBinContent(bind2);
		      const double      sdown2 = hdown -> GetBinContent(bind2) - hsigdown -> GetBinContent(bind2);
		      const double mean2 = 0.5 * (hup -> GetBinContent(bind2) + hdown -> GetBinContent(bind2));
		      double diff2 = fabs(sup2) > fabs(sdown2) ? fabs(sup2) : fabs(sdown2);
		      double cov = (hup  -> GetBinContent(bind1) - mean1) * (hup  -> GetBinContent(bind2) - mean2) +
			//double cov = (hup  -> GetBinContent(bind1) - hsigup -> GetBinContent(bind1)) * (hup  -> GetBinContent(bind2) - hsigup -> GetBinContent(bind2)) + 
			//(hdown  -> GetBinContent(bind1) - hsigdown -> GetBinContent(bind1)) * (hdown  -> GetBinContent(bind2) - hsigdown -> GetBinContent(bind2)) +
			(hdown  -> GetBinContent(bind1) - mean1) * (hdown  -> GetBinContent(bind2) - mean2);
		      const char sign = cov > 0.0 ? 1.0 : - 1.0;
		      // if (sup1 * sup2 < 0.0)
		      //   cov *= - 1.0;
		      // printf("sign %d\n", sign);
		      expunc(bind1 - 1, bind2 -1) += diff1 * diff2 * sign;//(sup1*sup2 + sdown1*sdown2)/2.0;
		    }
		}
	      delete hup;
	      delete hdown;
	      delete hsigdown;
	      delete hsigup;
	    }
	}


      for (map<TString, vector<HistoUnfolding *>> :: iterator bit = GetLevel(resultlevel) -> _m1dirsyshistos.begin(); bit != GetLevel(resultlevel) -> _m1dirsyshistos.end(); bit ++)
	{
	  //continue;
	  TString sample = bit -> first;
	  for (vector<HistoUnfolding *>::iterator it =  bit -> second.begin(); it != bit -> second.end(); it ++)
	    {
	      if ((*it) -> GetSysType() != EXPSYS)
		continue;
	      if (TString((*it) -> GetCategory()) == "Colour flip")
		continue;
	      TH1 * hsig = GetSignalProxy(resultlevel, recolevel, sample.Data(), (*it) -> GetSysType(), (*it) -> GetTag(), prune);
	      // (*it) -> ls();
	      // hsig -> Print("all");
	      HistoUnfolding * hu      = *it;
	      //hu -> ls();
	      TH1 *            hsys    = hu -> Project(recolevel);
	      if (prune)
		{
		  Prune(hsys, (*it) -> GetTag(), sample.Data(), (*it) -> GetSysType(), GEN);
		}
	      //  hsig -> Scale(1.0/hsig -> Integral());
	      // for (unsigned char bind = 1; bind < nbins + 1; bind ++)
	      //   {

	      //     hsig -> SetBinContent(bind, hsig -> GetBinContent(bind) * (1.0 - scales[bind - 1]));
	      //     hsys -> SetBinContent(bind, hsys -> GetBinContent(bind) * (1.0 - scales[bind - 1]));
	      //   }
	      const double integralnominal = hsig -> Integral();
	      if (not integralnominal)
		continue;
	      hsig -> Scale(1.0/hsig -> Integral());
	      hsys -> Scale(/*integralnominal*/1.0/hsys -> Integral());
	      for (unsigned char bind1 = 1; bind1 <= nbins; bind1 ++)
		{
		  const double mean1 = 0.5 * (hsys -> GetBinContent(bind1) + hsig -> GetBinContent(bind1));
		  const double      diff1   = hsys -> GetBinContent(bind1) - mean1;//hsig -> GetBinContent(bind1); 
		  //const double      diff1   = hsys -> GetBinContent(bind1) - hsig -> GetBinContent(bind1); 
		  for (unsigned char bind2 = 1; bind2 <= nbins; bind2 ++)
		    {
		      const double mean2 = 0.5 * (hsys -> GetBinContent(bind2) + hsig -> GetBinContent(bind2));
		      const double      diff2   = hsys -> GetBinContent(bind2) - mean2;//hsig -> GetBinContent(bind2); 
		      //const double      diff2   = hsys -> GetBinContent(bind2) - hsig -> GetBinContent(bind2); 
		      expunc(bind1 - 1, bind2 -1) += diff1 * diff2 + (hsig -> GetBinContent(bind1) - mean1) * (hsig -> GetBinContent(bind2) - mean2);
		    }
		}
	  
	      delete hsys;
	      delete hsig;
	    }

	}

      if (TString(method) == "cflip" and TString(signaltag) == "MC13TeV_TTJets")
	{
	  TH1 * hsys = GetLevel(OUT) -> GetSys("MC13TeV_TTJets_cflip", "MC13TeV_TTJets") -> Project(GEN, "cflip_out");
	  TH1 * hsig = GetLevel(IN) -> GetSys("MC13TeV_TTJets_cflip", "MC13TeV_TTJets") -> Project(GEN, "cflip_out");
	  hsys -> Scale(1.0/hsys -> Integral());
	  hsig -> Scale(1.0/hsig -> Integral());
	  for (unsigned char bind1 = 1; bind1 <= nbins; bind1 ++)
	    {
	      const float diff1 = hsys -> GetBinContent(bind1) - hsig -> GetBinContent(bind1);
	      for (unsigned char bind2 = 1; bind2 <= nbins; bind2 ++)
		{
		  const float diff2 = hsys -> GetBinContent(bind2) - hsig -> GetBinContent(bind2);
		  expunc(bind1 - 1, bind2 - 1) += diff1 * diff2;
	      
		}
	  
	    }
	  // hsys -> Print("all");
	  // hsig -> Print("all");
	  //          getchar();
	}
      //  expunc.Print();
      TMatrixD cov(nbins - 1, nbins - 1);
      for (unsigned char bindr = 0; bindr < nbins - 1; bindr ++)
	{
	  for (unsigned char bindc = 0; bindc < nbins -1; bindc ++)
	    {
	  
	      cov(bindr, bindc) = expunc(bindr, bindc);
	      //	  printf("%u %u\n", bindr, bindc);
	    }

	}
      // cov.Print();
      //getchar();
      if (GetLevel(resultlevel) -> GetCov())
	{
	  delete GetLevel(resultlevel) -> GetCov();
	  GetLevel(resultlevel) -> SetCov(nullptr);
	}
      GetLevel(resultlevel) -> SetCov( new TMatrixD(cov));
    }
  
} 

void CompoundHistoUnfolding::CreateChiTable()
{
  RecoLevelCode_t recolevels[2] = { RECO, GEN};
  
  for (ResultLevelCode_t resultcode = 0; resultcode < 2; resultcode ++)
    {
      const RecoLevelCode_t recolevel = recolevels[resultcode];
      const unsigned char nbins = recolevel == RECO ? _nbinsx : _nbinsy;

      struct TableEntry
      {
	TString title;
	float D;
	float p;
      };
      vector<TableEntry *> table;
      for (vector<HistoUnfolding *> :: iterator it = GetLevel(resultcode) -> GetV(SYSMO, signaltag) -> begin(); it != GetLevel(resultcode) -> GetV(SYSMO, signaltag) -> end(); it ++)
	{
	  //      (*it) -> ls();
	  if ((*it) -> GetSysType() != THEORSYS)
	    continue;
	  if (TString(method) == "cflip" and TString((*it) -> GetTag()) == "MC13TeV_TTJets_cflip")
	    {
	      continue;
	    }
	  const float D = GetChi((*it) -> GetTag(), resultcode, recolevel, (*it) -> GetSysType());
	  TableEntry * entry = new TableEntry;
	  entry -> title = (*it) -> GetTitle();
	  entry -> D = D;
	  entry -> p = TMath::Prob(D, nbins -1);
	  table.push_back(entry);
	}

      for (vector<TableEntry *> ::iterator it1 = table.begin(); it1 != table.end(); it1 ++)
	{
	  for (vector<TableEntry *> ::iterator it2 = it1 + 1; it2 != table.end(); it2 ++)
	    {
	      if ((*it1) -> D < (*it2) -> D)
		{
		  TableEntry *swap = *it1;
		  (*it1) = (*it2);
		  (*it2) = swap;
		}
	    }

	}
  

      {
	const float D = GetChi(signaltag, resultcode, recolevel, NOMINAL); 
	TableEntry * entry = new TableEntry;
	entry -> title = /*"data-bckg";*/(GetLevel(IN) -> GetHU(SIGNALMO)) -> GetTitle();
	entry -> D = D;
	entry -> p = TMath::Prob(D, nbins -1);
	table.insert(table.begin(), entry);

      }


      FILE *chifile = fopen((TString(_folder) + "/chi2_" + tag_resultlevel[resultcode] + ".txt").Data(), "w");

      for (vector<TableEntry *> ::iterator it = table.begin(); it != table.end(); it ++)
	{
	  printf("%s %f %f\n", ((*it) -> title).Data(), (*it) -> D, (*it) -> p);
	  fprintf(chifile, "%s, %f %f\n", ((*it) -> title).Data(), (*it) -> D, (*it) -> p);
      
	}
      fclose(chifile);
      for (vector<TableEntry *> ::iterator it = table.begin(); it != table.end(); it ++)
	{
	  delete *it;
      
	}
    }
}


double CompoundHistoUnfolding::GetChi(const char * tag, ResultLevelCode_t resultlevel, RecoLevelCode_t recolevel, SysTypeCode_t systype) 
{
  if (systype == EXPSYS)
    {
      printf("double CompoundHistoUnfolding::GetChi(const char * tag, ResultLevelCode_t resultlevel, RecoLevelCode_t recolevel, SysTypeCode_t systype) \n no routines for EXPSYS");
      throw "double CompoundHistoUnfolding::GetChi(const char * tag, ResultLevelCode_t resultlevel, RecoLevelCode_t recolevel, SysTypeCode_t systype)  \n no routines for EXPSYS";
    }
  if (not GetLevel(resultlevel) -> GetCov())
    {
      printf("double CompoundHistoUnfolding::GetChi(const char * tag, ResultLevelCode_t resultlevel, RecoLevelCode_t recolevel, SysTypeCode_t systype) \n no covariance matrix");
      throw "double CompoundHistoUnfolding::GetChi(const char * tag, ResultLevelCode_t resultlevel, RecoLevelCode_t recolevel, SysTypeCode_t systype)  \n no covariance matrix";
      
    }
  TMatrixD covinv(*GetLevel(resultlevel) -> GetCov());
  covinv.Invert();
  
  TH1 * hdatambckg = GetLevel(resultlevel) -> GetHU(DATAMBCKG) -> Project(recolevel, "hsdatambckg"); //the data minus background
  TH1 * hMCpred = nullptr;
  if (systype == NOMINAL)
    hMCpred = /*h2 -> ProjectionY(); */GetSignalProxy(resultlevel, recolevel, signaltag, NOMINAL, ""); //the MC prediction
  if (systype == THEORSYS)
    hMCpred  =  GetLevel(IN) -> GetSys(tag, signaltag) -> Project(recolevel);
  
  hMCpred -> Scale(1.0/hMCpred -> Integral());
  hdatambckg -> Scale(/*integralnominal*/1.0/hdatambckg -> Integral());
  const unsigned char nbins = hdatambckg -> GetNbinsX();
  TMatrixD Diff(1, nbins - 1);
  for (unsigned char bind = 0; bind < nbins - 1; bind ++)
    {
      Diff(0, bind) = hdatambckg -> GetBinContent(bind + 1) - hMCpred -> GetBinContent(bind + 1);
    }
  // Diff.Print();
  TMatrixD DiffT(Diff);
  DiffT.T();
  return (Diff * covinv * DiffT)(0, 0);
}


void CompoundHistoUnfolding::Prune(TH1 * h, const char * tag, const char * sample, SysTypeCode_t code, RecoLevelCode_t recolevel)
{
  
  if (recolevel == GEN)
    {
      TH2  * matrix = GetUnfoldingHU(tag, sample, code) -> GetTH2();
      for (unsigned char bind = 1; bind <= h -> GetNbinsX(); bind ++)
	{
	  const float fact = matrix -> Integral(1, matrix -> GetNbinsX(), bind, bind)/matrix -> Integral(0, matrix -> GetNbinsX(), bind, bind);
	  //printf("pruning %f %f", fact, h -> GetBinContent(bind));
	  h -> SetBinContent(bind, h -> GetBinContent(bind) * fact);
	  //printf(" after %f\n", h -> GetBinContent(bind));
	}
    }
  
}

