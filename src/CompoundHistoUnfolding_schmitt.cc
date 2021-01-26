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
void CompoundHistoUnfolding::unfold(bool reg, bool includecflip)
{
  printf("includecflip %s\n", includecflip ? "True" : "False");
  getchar();
  CreateDataMinusBackground(IN);
  TCanvas cmigration("cmigration", "cmigration");
  cmigration.SetRightMargin(0.18);

  TH2 * hsignal = (TH2 *) GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> Clone("unfolding_signal");
  for (unsigned char bind = 0; bind <= hsignal -> GetNbinsX() + 1; bind ++)
    {
      hsignal -> SetBinContent(bind, 0, 0.0);
    }
  hsignal -> Draw("COLZ");
  cmigration.SaveAs(TString(_folder) + "/migrationmatrix.png");
  TUnfoldDensity unfold(hsignal, TUnfold::kHistMapOutputVert, TUnfold::kRegModeCurvature, TUnfold::kEConstraintArea, TUnfoldDensity::kDensityModeBinWidthAndUser);
  input = /*(TH1F*) _signalh -> _th2 -> ProjectionX(); */GetLevel(IN) -> GetHU(DATAMBCKG) -> Project(RECO, CreateName("input"));
  ApplyScaleFactor(input);
  TH1F * input_gen = GetLevel(IN) -> GetHU(SIGNALMO) -> Project(GEN, CreateName("inputgen"));
  NormaliseToBinWidth(input_gen);
  // printf("input gen\n");
  // input_gen -> Print("all");
  TH1F * input_reco = GetLevel(IN) -> GetHU(SIGNALMO) -> Project(RECO, CreateName("inputreco"));
  NormaliseToBinWidth(input_reco);
  // printf("input reco\n");
  // input_reco -> Print("all");
  //  ApplyScaleFactor(input);
  input -> SetMinimum(0.0);
  input -> SetDirectory(nullptr);
  unfold.SetInput(input);
  float tau(0.0);
  if (reg)
    {
      static const unsigned char nPoint(100);
      static const double tauMin(string(observable).compare("pvmag") == 0 ? 0.0 : 1E-10);
      static const double tauMax(1E-3);
      TGraph * lCurve;
      TSpline * logTauX;
      TSpline * logTauY;
      const int iBest = unfold.ScanLcurve(nPoint, tauMin, tauMax, & lCurve, & logTauX, & logTauY);
      TCanvas * clCurve = new TCanvas(CreateName("lCurve"), CreateTitle("lCurve"));
      lCurve -> Draw("AP*");
      clCurve -> SaveAs(TString(_folder) + "/lCurve.png");
      TCanvas * clogTauX = new TCanvas(CreateName("logTauX"), CreateTitle("logTauX"));
      logTauX -> Draw("");
      clogTauX -> SaveAs(TString(_folder) + "/logTauX.png");
      delete logTauX;
      delete clogTauX;
      TCanvas * clogTauY = new TCanvas(CreateName("logTauY"), CreateTitle("logTauY"));
      logTauY -> Draw("");
      clogTauY -> SaveAs(TString(_folder) + "/logTauY.png");
      delete logTauY;
      delete clogTauY;
      tau = unfold.GetTau();
      printf("tau %.9f\n", tau);
      if (tau < 2*tauMin or tau > 0.5*tauMax)
	{
	  TSpline * tauScanResult;
	  const int iBest = unfold.ScanTau(nPoint, tauMin, tauMax, & tauScanResult, TUnfoldDensity::kEScanTauRhoAvg);
	  TCanvas * ctauScanResult = new TCanvas(CreateName("tauScanResult"), CreateTitle("tauScanResult"));
	  tauScanResult -> Draw("");
	  ctauScanResult -> SaveAs(TString(_folder) + "/tauScanResult.png");
	  delete tauScanResult;
	  delete tauScanResult;
	  tau = unfold.GetTau();
	  printf("tau from tauscan %.9f\n", tau);
	}
      delete lCurve;
      delete clCurve;
      
    }
  printf("tau used %.9f\n", tau);

  unfold.DoUnfold(tau);
  TCanvas *coutput = new TCanvas(CreateName("coutput"), CreateTitle("coutput"));
  THStack *coutput_stack = new THStack(CreateName("output"), CreateTitle("output"));
  TH1 * houtput = unfold.GetOutput(CreateName("output"));
  printf("output\n");
  houtput -> Print("all");
  //getchar();
  houtput -> SetDirectory(nullptr);
  // printf("ematrixtotal %p\n", ematrixtotal);
  ematrixtotal = unfold.GetEmatrixTotal("ematrix");
  // printf("ematrixtotal after %p %s\n", ematrixtotal, ematrixtotal -> ClassName());
   ematrixtotal -> Scale(1.0/TMath::Power(houtput -> Integral(), 2.0));
  // getchar();
   ematrixtotal -> SetDirectory(nullptr);
  TH1F * py = GetLevel(IN) -> GetHU(SIGNALMO) -> Project(GEN, CreateName("py"));
  py -> SetMinimum(0.0);
  py -> Scale(1.0/py -> Integral());
  NormaliseToBinWidth(py);
  py -> Scale(1.0/py -> Integral());
  coutput_stack -> Add(py);
  TH1F * houtput_draw = (TH1F*) houtput -> Clone(CreateName("houtput_draw"));
  houtput_draw -> SetDirectory(nullptr);
  //  houtput_draw -> Scale(1.0/houtput_draw -> Integral());
  NormaliseToBinWidth(houtput_draw);
  // printf("unfolded output\n");
  // houtput_draw -> Print("all");
  
  //houtput_draw -> Scale(1.0/houtput_draw -> Integral());
  
  houtput_draw -> SetMinimum(0.0);
  coutput_stack -> Add(houtput_draw);

  houtput_draw -> SetLineColor(kGreen);
  const TArrayD * h1Array = houtput_draw -> GetXaxis() ->GetXbins();
  const TArrayD * h2Array = py -> GetXaxis() ->GetXbins();
  Int_t fN = h1Array->fN;
  if ( fN != 0 ) 
    {
      if ( h2Array -> fN != fN ) 
	{
	  printf("DifferentBinLimits %u %u\n", fN, h2Array -> fN);
	}
      else 
	{
	  for ( int i = 0; i < fN; ++i ) 
	    {
	      if ( ! TMath::AreEqualRel( h1Array -> GetAt(i), h2Array -> GetAt(i), 1E-10 ) ) 
		{
		  printf("i %u DifferentBinLimits\n", i);
		}
	    }
	}
    }
  coutput_stack -> Draw("NOSTACK");

  coutput_stack -> SetMaximum(coutput_stack -> GetMaximum() * 1.2);
  TLegend * legend_output = new TLegend(0.6, 0.7, 1.0, 1.0);
  legend_output -> SetTextSize(0.03);
  legend_output -> AddEntry(houtput_draw, "unfolded");
  legend_output -> AddEntry(py, TString(GetLevel(IN) -> GetHU(SIGNALMO) -> GetTitle()) + " gen");
  legend_output -> Draw("SAME");
  //coutput -> SaveAs(TString(_folder) + "/" + "coutput.png");
  if (GetLevel(OUT) -> GetHURef(DATAMBCKG))
    {
      delete GetLevel(OUT) -> GetHURef(DATAMBCKG);
      GetLevel(OUT) -> GetHURef(DATAMBCKG) = nullptr;
    }
  for(map<TString, vector<HistoUnfolding *>>::iterator bit = GetLevel(OUT) -> _msyshistos .begin(); bit != GetLevel(OUT) -> _msyshistos . end(); bit ++)
    {
      const TString sample = bit -> first;
      for (vector<HistoUnfolding *> :: iterator it = GetLevel(OUT) -> GetV(SYSMO, sample.Data()) -> begin(); it != GetLevel(OUT) -> GetV(SYSMO, sample.Data()) -> end(); it ++)
	delete *it;
      
      GetLevel(OUT) -> GetV(SYSMO, sample.Data()) -> clear();
    }
  if (GetLevel(OUT) -> GetHU(DATAMO))
    {
      delete GetLevel(OUT) -> GetHU(DATAMO);
      GetLevel(OUT) -> GetHURef(DATAMO) = nullptr;
    }
  GetLevel(OUT) -> GetHURef(DATAMBCKG) = new HistoUnfoldingTH1(GetLevel(IN) -> GetHURef(DATA));
  GetLevel(OUT) -> GetHU(DATAMBCKG) -> GetTH1Ref(GEN) = houtput;

  houtput -> SetTitle(TString("data (") + GetLevel(IN) -> GetHU(SIGNALMO) -> GetTitle() + ") unfolded");

  TCanvas * cfolded_output = new TCanvas(CreateName("folded_output"), CreateTitle("folded_output"));
  THStack * cfolded_output_stack = new THStack(CreateName("cfolded_output_stack"), CreateTitle("cfolded output"));
  TH1 * hfolded_output = unfold.GetFoldedOutput(CreateName("folded_output"));
  hfolded_output -> SetDirectory(nullptr);
  TH1F * px = GetLevel(IN) -> GetHU(SIGNALMO) -> Project(RECO, CreateName("px")); 
  NormaliseToBinWidth(px);
  px -> SetMinimum(0.0);
 
  cfolded_output_stack -> Add(px);
  TH1F * input_draw = (TH1F*) input -> Clone(CreateName("input_draw"));
  input_draw -> SetDirectory(nullptr);
  NormaliseToBinWidth(input_draw);

  input_draw -> SetLineColor(kRed);
  input_draw -> SetMinimum(0.0);
  cfolded_output_stack -> Add(input_draw);
  TH1F * hfolded_output_draw = (TH1F*) hfolded_output -> Clone(CreateName("hfolded_output_draw"));
  hfolded_output_draw -> SetDirectory(nullptr);
  NormaliseToBinWidth(hfolded_output_draw);
  printf("listing folded back output");
  hfolded_output_draw -> Print("all");
  cfolded_output_stack -> Add(hfolded_output_draw);
  hfolded_output_draw -> SetLineColor(kGreen);
  hfolded_output_draw -> SetMinimum(0.0);
  cfolded_output_stack -> Draw("NOSTACK");
  TLegend * legend_input = new TLegend(0.6, 0.7, 0.95, 0.95);
  legend_input -> SetTextSize(0.03);
  legend_input -> AddEntry(input_draw, "dataminusbckg");
  legend_input -> AddEntry(px, TString(GetLevel(IN) -> GetHU(SIGNALMO) -> GetTitle()) + " reco no fakes");
  legend_input -> AddEntry(hfolded_output_draw, "folded_output");
  legend_input -> SetName("legend");
  legend_input -> Draw("SAME");
  cfolded_output -> SaveAs(TString(_folder) + "/" + "cfolded_output.png");
  GetLevel(OUT) -> GetHU(DATAMBCKG) -> GetTH1Ref(RECO) = hfolded_output; 
  hfolded_output -> SetTitle(TString("data (") + GetLevel(IN) -> GetHU(SIGNALMO) -> GetTitle() + ") folded back");
  
  //  GetLevel(OUT) -> GetHU(SIGNALMO) -> GetTH1Ref(GEN) = houtput;
  if (TString(signaltag) == "MC13TeV_TTJets")
    {
      _chunominal = this;
    }
  vector<HistoUnfolding *>::iterator it = GetLevel(IN) -> GetV(SYSMO, signaltag) -> begin();
  while (it != GetLevel(IN) -> GetV(SYSMO, signaltag) -> end() and not TString((*it) -> GetTag()).Contains("non_clos"))
    {
      it ++;
    }

  if (it != GetLevel(IN) -> GetV(SYSMO, signaltag) -> end())
    {
      printf("removing non clos\n");
      delete (*it);
      GetLevel(IN) -> GetV(SYSMO, signaltag) -> erase(it);
      //      getchar();
    }
  printf("signaltag %s\n", signaltag); 
  for(map<TString, vector<HistoUnfolding *>>::iterator bit = GetLevel(IN) -> _msyshistos .begin(); bit != GetLevel(IN) -> _msyshistos . end(); bit ++)
    {
      const TString sample = bit -> first;
      printf("unfolding sample %s\n", sample.Data());
      if (sample == signaltag)
	{
	  for (vector<HistoUnfolding *>::iterator it = GetLevel(IN) -> GetV(SYSMO, sample.Data()) -> begin(); it != GetLevel(IN) -> GetV(SYSMO, sample.Data()) -> end(); it ++)
	    {
	      printf("unfolding systematic %s systype\n", (*it) -> GetTag(), (*it) -> GetSysType());
	      if ((*it) -> GetSysType() == EXPSYS)
		{
		  TH2 * mmatrixsyst = (TH2 *) (*it) -> GetTH2() -> Clone("matrixsyst");
		  //      (*it) -> ls();
		  TH2 * datambckg = (TH2 * ) GetLevel(IN) -> GetHU(DATAMO) -> GetTH2() -> Clone(CreateName("datambackground_unfolding"));
		  datambckg -> SetDirectory(nullptr);
		  datambckg -> Add(totalbackground -> GetTH2(), -1);
		  TH1 * input = (TH1 *) datambckg -> ProjectionX(TString(datambckg -> GetName()) + "_reco");
		  delete datambckg;
		  for (unsigned char bind = 0; bind <= mmatrixsyst -> GetNbinsX(); bind ++)
		    {
		      const float fractionnongen = mmatrixsyst -> Integral(bind, bind, 0, 0)/mmatrixsyst -> Integral(bind, bind, 0, mmatrixsyst -> GetNbinsY() + 1);
		      if (TString((*it) -> GetTag()) == "MC13TeV_TTJetstrig_efficiency_correction_up")
			printf("%f %f \n", mmatrixsyst -> GetBinContent(bind, 0), fractionnongen);
		      input -> SetBinContent(bind, input -> GetBinContent(bind) * ( 1 - fractionnongen));
		      input -> SetBinError(bind, input -> GetBinError(bind) * ( 1 - fractionnongen));
		      mmatrixsyst -> SetBinContent(bind, 0, 0.0);
		    }
		  TUnfoldDensity unfold(mmatrixsyst, TUnfold::kHistMapOutputVert, TUnfold::kRegModeCurvature, TUnfold::kEConstraintArea, TUnfoldDensity::kDensityModeUser);
		  unfold.SetInput(input);
	
		  float tau(0.0);
		  if (reg)
		    {
		      static const unsigned char nPoint(100);
		      static const double tauMin(string(observable).compare("pvmag") == 0 ? 0.0 : 1E-10);
		      static const double tauMax(1E-3);
		      TGraph * lCurve;
		      const int iBest = unfold.ScanLcurve(nPoint, tauMin, tauMax, & lCurve);
		      tau = unfold.GetTau();
		      if (tau < 2*tauMin or tau > 0.5*tauMax)
			{
			  printf("scanning tau\n");
			  TSpline * tauScanResult;
			  const int iBest = unfold.ScanTau(nPoint, tauMin, tauMax, & tauScanResult, TUnfoldDensity::kEScanTauRhoAvg);
			  delete tauScanResult;
			  delete tauScanResult;
			  tau = unfold.GetTau();
			  //	      printf("tau from tauscan %.9f\n", tau);
			}
		      // else
		      //   {

		      //     printf("attention \n");
		      //     getchar();
		      //   }
		      printf("tau %.9f\n", tau);
		      delete lCurve;
      
		    }

		  unfold.DoUnfold(tau);
		  HistoUnfoldingTH1 * out = new HistoUnfoldingTH1((SampleDescriptor * ) * it);
		  GetLevel(OUT) -> GetV(SYSMO, sample.Data()) -> push_back(out);
		  out -> GetTH1Ref(RECO) = (TH1F*) unfold.GetFoldedOutput(CreateName(TString((*it) -> GetTag()) + "_folded_output"));
		  out -> GetTH1Ref(GEN) = (TH1F*) unfold.GetOutput(CreateName(TString((*it) ->GetTag()) + "_output"));
		  delete input;
		  delete mmatrixsyst;
		}
	      if ((*it) -> GetSysType() == THEORSYS)
		{
		  if (TString(method) == "nominal" and TString((*it) -> GetTag()) == "MC13TeV_TTJets_cflip" and not includecflip)
		    {
		      printf("found cflip: continuing");
		      getchar();
		      continue;
		    }
		  TUnfoldDensity unfold(hsignal, TUnfold::kHistMapOutputVert, TUnfold::kRegModeCurvature, TUnfold::kEConstraintArea, TUnfoldDensity::kDensityModeUser);
		  //      (*it) -> ls();
		  TH2 * inputsysh2 = (TH2*)(*it) -> GetTH2() -> Clone("inputsysh2");
		  for (unsigned char bind = 0; bind <= inputsysh2 -> GetNbinsX() + 2; bind ++)
		    {
		      //	      inputsysh2 -> SetBinContent(bind, 0, 0.0);
		    }
		  TH1* inputsys = inputsysh2 -> ProjectionX("inputsys");
		  ApplyScaleFactor(inputsys);
		  unfold.SetInput(inputsys);
		  float tau(0.0);
		  if (reg)
		    {
		      static const unsigned char nPoint(100);
		      static const double tauMin(string(observable).compare("pvmag") == 0 ? 0.0 : 1E-10);
		      static const double tauMax(1E-3);
		      TGraph * lCurve;
		      const int iBest = unfold.ScanLcurve(nPoint, tauMin, tauMax, & lCurve);
		      tau = unfold.GetTau();
		      if (tau < 2*tauMin or tau > 0.5*tauMax)
			{
			  printf("scanning tau\n");
			  TSpline * tauScanResult;
			  const int iBest = unfold.ScanTau(nPoint, tauMin, tauMax, & tauScanResult, TUnfoldDensity::kEScanTauRhoAvg);
			  delete tauScanResult;
			  delete tauScanResult;
			  tau = unfold.GetTau();
			  //	      printf("tau from tauscan %.9f\n", tau);
			}
		      // else
		      //   {

		      //     printf("attention \n");
		      //     getchar();
		      //   }
		      printf("tau %.9f\n", tau);
		      delete lCurve;
      
		    }

		  unfold.DoUnfold(tau);
		  HistoUnfoldingTH1 * out = new HistoUnfoldingTH1((SampleDescriptor * ) * it);
		  GetLevel(OUT) -> GetV(SYSMO, sample.Data()) -> push_back(out);
		  out -> GetTH1Ref(RECO) = (TH1F*) unfold.GetFoldedOutput(CreateName(TString((*it) -> GetTag()) + "_folded_output"));
		  out -> GetTH1Ref(GEN) = (TH1F*) unfold.GetOutput(CreateName(TString((*it) ->GetTag()) + "_output"));
		  if (TString((*it) -> GetTag()) == "MC13TeV_TTJets_evtgen")
		    {
		      printf("listing MC13TeV_TTJets_evtgen after unfolding\n");
		      printf("input gen\n");
		      TH1 * inputsysgen = (TH1*)(*it) -> Project(GEN) -> Clone("clonesysgen");

		      inputsysgen -> Print("all");
		      printf("output\n");
		      out -> GetTH1Ref(GEN) -> Print("all");
		      //getchar();
		    }
	  

		  delete 	    inputsys;
		  delete inputsysh2;
		}
	    }
	}
      else
	{
	  for (vector<HistoUnfolding *>::iterator it = GetLevel(IN) -> GetV(SYSMO, sample.Data()) -> begin(); it != GetLevel(IN) -> GetV(SYSMO, sample.Data()) -> end(); it ++)
	    {
	      TH2 * datambackground  = (TH2 * ) GetLevel(IN) -> GetHU(DATAMO) -> GetTH2() -> Clone(CreateName("datambackground_bckgsyst"));
	      datambackground -> SetDirectory(nullptr);
	      TH2 * totalbackground = (TH2 *) this -> totalbackground -> GetTH2() -> Clone("totalbackground_bckgsyst");
	      totalbackground -> Add(GetBackgroundH(sample.Data()) -> GetTH2(), -1);
	      totalbackground -> Add((*it) -> GetTH2());
	      datambackground -> Add(totalbackground, -1);
	      TUnfoldDensity unfold(hsignal, TUnfold::kHistMapOutputVert, TUnfold::kRegModeCurvature, TUnfold::kEConstraintArea, TUnfoldDensity::kDensityModeBinWidthAndUser);
	      ApplyScaleFactor(datambackground);
	      unfold.SetInput(datambackground);
	      // (*it) -> ls();
	      // datambackground -> ProjectionX() -> Print("all");
	      // getchar();

	      unfold.DoUnfold(tau);
	      HistoUnfoldingTH1 * out = new HistoUnfoldingTH1((SampleDescriptor * ) * it);
	      GetLevel(OUT) -> GetV(SYSMO, sample.Data()) -> push_back(out);
	      out -> GetTH1Ref(RECO) = (TH1F*) unfold.GetFoldedOutput(CreateName(TString((*it) -> GetTag()) + "_folded_output"));
	      out -> GetTH1Ref(GEN) = (TH1F*) unfold.GetOutput(CreateName(TString((*it) ->GetTag()) + "_output"));
	      delete datambackground;
	      delete totalbackground;
	    }
	}
    }

  HistoUnfoldingTH1 * nonclosout = new HistoUnfoldingTH1((SampleDescriptor*) GetLevel(IN) -> GetHU(SIGNALMO));
  nonclosout -> SetTag(TString(nonclosout -> GetTag()) + "_non_clos");
  nonclosout -> SetCategory("Non closure");
  nonclosout -> SetSysType(THEORSYS);

  HistoUnfoldingTH2 * nonclosin = new HistoUnfoldingTH2((SampleDescriptor*) GetLevel(IN) -> GetHU(SIGNALMO));
  nonclosin -> SetTag(TString(nonclosin -> GetTag()) + "_non_clos");
  nonclosin -> SetCategory("Non closure");
  nonclosin -> SetSysType(THEORSYS);

  const unsigned char nbins = hsignal -> GetNbinsY(); 
  double binsrecord[nbins];
  double binsrecordold[nbins];
  for (unsigned char bind = 0; bind < nbins; bind ++)
    {
      binsrecord[bind] = 1.0;
      binsrecordold[bind] = binsrecord[bind];
    }


  float covrecord = 1E6;
  auto varylambdanest = [ &binsrecord, &binsrecordold, &covrecord, nbins, this](unsigned char start)  -> void
    {
      double bins[nbins];

      auto varylambda =[&bins, &binsrecord, &binsrecordold, &covrecord, nbins, this](unsigned char start, unsigned char k, auto & func) -> void
      {
	const float dev = 0.2;
        const double min = binsrecordold[start] - dev/TMath::Power(2, k);
        const double max = binsrecordold[start] + dev/TMath::Power(2, k);
        // printf("start %u %f %f %u\n", start, min, max, k);                                                                                                                                               
        // getchar();                                                                                                                                                                                       
        const unsigned long M = 1E4;
        double step = (max - min) /  TMath::Power(M * nbins, 1.0/nbins);
	// if (k == 18 and start == 2)
	//   {
	//     printf("step %.9f\n", step);
	//     //getchar();
	//   }
	for (double coeff = min; coeff <= max; coeff += step)
	  {
	    bins[start] = coeff;
	    // if (k == 18 and start == 2)
	    //   {
	    // 	printf("coeff %.12f\n", coeff);
	    // 	for (unsigned char bind = 0; bind < nbins; bind ++)
	    // 	  {
	    // 	    printf("%.12f ", bins[bind]);
	    // 	  }
	    // 	printf("\n");
	    //   }
	    if (start < nbins - 1)
	      func(start + 1, k, func);
	    else
	      {
		// if (k==18 and start == 2)
		//   {
		//     for (unsigned char bind = 0; bind < nbins; bind ++)
		//       {
		// 	printf("%f ", bins[bind]);
		//       }
		//     printf("\n");
		//   }
		TH2 * hin = (TH2 *) this -> GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> Clone("hin"); 
		for (unsigned char bind = 0; bind <= hin -> GetNbinsX() + 1; bind ++)
		  hin -> SetBinContent(bind, 0, 0.0);
		for (unsigned char biny = 1; biny < hin -> GetNbinsY() +1; biny ++)
		  {
		    for(unsigned char binx = 1; binx < hin -> GetNbinsX() + 1; binx ++)
		      {
			hin -> SetBinContent(binx, biny, hin -> GetBinContent(binx, biny) * bins[biny - 1]);
		      }
		  }
		TH1 * hinx = hin -> ProjectionX();
		hin -> Scale(input -> Integral()/hin->Integral());
		float cov = 0.0;
		for (unsigned char bind = 1; bind < hinx -> GetNbinsX() + 1; bind ++)
		  {
		    cov = TMath::Sqrt(TMath::Power(cov, 2) + TMath::Power(hinx -> GetBinContent(bind) - input -> GetBinContent(bind), 2));
		  }
		//		printf("cov %f\n", cov);
		if (cov < covrecord)
		  {
		    covrecord = cov;
		    for (unsigned char bind = 0; bind < nbins; bind ++)
		      {
			binsrecord[bind] = bins[bind];
		      }
		  }
		delete hinx;
		delete hin;
	      }
	  }
      };
      const double tol = 1E-6;
      bool precis = true;
      unsigned char k = 0;
      do
        {
          printf("new iteration %u\n", k);
	  //	  if (k < 16)
	    varylambda(0, k, varylambda);
          k ++;
          precis = true;
          for (unsigned char bind = 0; bind < nbins; bind ++)
            {
              const float d = fabs(binsrecordold[bind] - binsrecord[bind])/binsrecordold[bind];
              printf("%u %.9f %.9f %.9f\n", bind, binsrecord[bind], binsrecordold[bind], d);
              if (d > tol)
                {
                  printf(" not precise\n");
                  precis = false;
                }
            }
	  if (not precis)
            {
              for (unsigned char bind = 0; bind < nbins; bind ++)
                {
                  binsrecordold[bind] = binsrecord[bind];
                }
            }
        } while (not precis);

    };
  varylambdanest(0);

  printf("resulting coefficients:\n");
  for (unsigned char bind = 0; bind < nbins; bind ++)
    {
      printf("%f ", binsrecord[bind]); 
    }
  printf("\n");
  TH2 * hinnonclos = (TH2 *) this -> GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> Clone("hin"); 
  for (unsigned char bind = 0; bind <= hinnonclos -> GetNbinsX() + 1; bind ++)
    hinnonclos -> SetBinContent(bind, 0, 0.0);

  for (unsigned char bindy = 1; bindy < hsignal -> GetNbinsY() + 1; bindy ++)
    {
      for (unsigned char bindx = 0; bindx < hsignal -> GetNbinsX() + 1; bindx ++)
	{
	  hinnonclos -> SetBinContent(bindx, bindy, hinnonclos -> GetBinContent(bindx, bindy) * binsrecord[bindy - 1]);
	}
    }
  
  nonclosin -> GetTH2Ref() = (TH2F *) hinnonclos; 
  GetLevel(IN) -> GetV(SYSMO, signaltag) -> push_back(nonclosin);

  TH1 * inputnonclos = hinnonclos -> ProjectionX("inpuntnonclos");
  
  // //inputnonclos -> Print("all");
  TUnfoldDensity unfoldnonclos(hsignal, TUnfold::kHistMapOutputVert, TUnfold::kRegModeCurvature, TUnfold::kEConstraintArea, TUnfoldDensity::kDensityModeBinWidthAndUser);
  // //ApplyScaleFactor(inputnonclos);
  for (unsigned char bindx = 0; bindx <= inputnonclos -> GetNbinsX() + 1; bindx ++)
    {
      printf("%f \n", inputnonclos -> GetBinContent(bindx)/ input -> GetBinContent(bindx));
    }

  unfoldnonclos.SetInput(inputnonclos);
  unfoldnonclos.DoUnfold(tau);
  GetLevel(OUT) -> GetV(SYSMO, signaltag) -> push_back(nonclosout);
  nonclosout -> GetTH1Ref(RECO) = (TH1F*) unfoldnonclos.GetFoldedOutput(CreateName(TString(nonclosout -> GetTag()) + "_folded_output"));
  nonclosout -> GetTH1Ref(GEN) = (TH1F*) unfoldnonclos.GetOutput(CreateName(TString(nonclosout ->GetTag()) + "_output"));
  nonclosout -> SetTitle(TString(nonclosout -> GetTitle()) + " non closure");
  TH1 * nonclosoutgentest = (TH1 *) nonclosout -> GetTH1(GEN) -> Clone();
  TH1 * nonclosingentest  = hinnonclos -> ProjectionY();
  printf("nonclossys unscaled\n");
  nonclosoutgentest -> Print("all");
  
  nonclosoutgentest -> Scale(nonclosingentest -> Integral() /nonclosoutgentest -> Integral());
  printf("nonclossys scaled\n");
  nonclosoutgentest -> Print("all");
  nonclosingentest -> Print("all");
  for (unsigned char bind = 1; bind < nonclosoutgentest -> GetNbinsX() + 1; bind ++)
    {
      printf("%u %f\n", bind, nonclosoutgentest -> GetBinContent(bind) - nonclosingentest -> GetBinContent(bind));
    }

  nonclosoutgentest -> Scale(houtput -> Integral()/nonclosoutgentest -> Integral());
  for (unsigned char bind = 1; bind < nonclosoutgentest -> GetNbinsX() + 1; bind ++)
    {
      printf("%u %f\n", bind, nonclosoutgentest -> GetBinContent(bind) - houtput -> GetBinContent(bind));
    }

  delete inputnonclos;
  GetLevel(OUT) -> GetHURef(SIGNALMO) = GetLevel(IN) -> GetHU(SIGNALMO); 

  delete hsignal;
}
