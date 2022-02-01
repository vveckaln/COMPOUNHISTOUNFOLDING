#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#include "CompoundHistoUnfolding.hh"
#include "TMath.h"
ClassImp(CompoundHistoUnfolding);
using namespace TMath;
void CompoundHistoUnfolding::unfoldBayesian(unsigned char kiter, bool includecflip)
{
  TH2 * hsignal = (TH2 *) GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> Clone("unfolding_signal");
  TH2 * hsignalorig = (TH2 *) GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> Clone("unfolding_signal_orig");

  //hsignal -> RebinX(2);
  TH2 * datambackground = (TH2*) this -> GetLevel(IN) -> GetHU(DATAMO) -> GetTH2() -> Clone("datambackground");

  datambackground -> Add(this -> GetTotalBackground() -> GetTH2(), -1);
  //datambackground -> RebinX(2);                                                                                                                                                                           
  TH1 * hmeas = (TH1*) datambackground -> ProjectionX("meas");
  for (unsigned char bind = 0; bind <= hsignal -> GetNbinsX() + 1; bind ++)
    {
      // if (hsignal -> Integral(bind, bind, 0, hsignal -> GetNbinsY() + 1) > 0.0)
      //   hmeas -> SetBinContent(bind, hmeas -> GetBinContent(bind) * (1.0 - hsignal -> GetBinContent(bind, 0)/hsignal -> Integral(bind, bind, 0, hsignal -> GetNbinsY() + 1)));
      hsignal -> SetBinContent(bind, 0, 0.0);
      //      hmeas -> SetBinError(bind, 0, 0.0);
    }
  for (unsigned char bind = 0; bind <= hsignal -> GetNbinsY() + 1; bind ++)
    {
      // if (hsignal -> Integral(0, hsignal -> GetNbinsX() + 1, bind, bind) > 0.0)
      // 	{
      // 	  hgen -> SetBinContent(bind, hgen -> GetBinContent(bind) * (1.0 - hsignal -> GetBinContent(0, bind)/hsignal -> Integral(0, hsignal -> GetNbinsX() + 1, bind, bind)));
      // 	  hgen -> SetBinError(bind, hgen -> GetBinError(bind) * (1.0 - hsignal -> GetBinContent(0, bind)/hsignal -> Integral(0, hsignal -> GetNbinsX() + 1, bind, bind)));
      // 	}
      //      hsignal -> SetBinContent(0, bind, 0.0);
    }

  ApplyScaleFactor(hmeas);
  TH1 * hreco = hsignal -> ProjectionX();
  TH1 * hgen = hsignal -> ProjectionY("gen");
  RooUnfoldResponse response(hreco, hgen, hsignal);
  RooUnfoldBayes   unfold (&response, hmeas, kiter);
  TH1* houtput = (TH1D*) unfold.Hreco();
  houtput -> SetTitle(TString("data (") + GetLevel(IN) -> GetHU(SIGNALMO) -> GetTitle() + ") unfolded");
  for (unsigned char bind = 0; bind < houtput -> GetNbinsX() + 1; bind ++)
    {
      const float coeff = hsignalorig -> Integral(1, hsignalorig -> GetNbinsX() + 1, bind, bind)/hsignalorig -> Integral(0, hsignalorig -> GetNbinsX(), bind, bind);
      //houtput -> SetBinContent(bind, houtput -> GetBinContent(bind) / coeff);
    }
  if (GetLevel(OUT) -> GetHURef(DATAMBCKG))
    delete GetLevel(OUT) -> GetHURef(DATAMBCKG);
  GetLevel(OUT) -> GetHURef(DATAMBCKG) = new HistoUnfoldingTH1(GetLevel(IN) -> GetHURef(DATA));
  GetLevel(OUT) -> GetHU(DATAMBCKG) -> GetTH1Ref(GEN) = houtput;

  // printf("unfolded signal\n");
  // houtput -> Print("all");
  // printf("gen\n");
  // hsignalorig -> ProjectionY() -> Print("all");
  // getchar();
  for(map<TString, vector<HistoUnfolding *>>::iterator bit = GetLevel(OUT) -> _msyshistos .begin(); bit != GetLevel(OUT) -> _msyshistos . end(); bit ++)
    {
      const TString sample = bit -> first;
      for (vector<HistoUnfolding *> :: iterator it = GetLevel(OUT) -> GetV(SYSMO, sample.Data()) -> begin(); it != GetLevel(OUT) -> GetV(SYSMO, sample.Data()) -> end(); it ++)
	delete *it;
      
      GetLevel(OUT) -> GetV(SYSMO, sample.Data()) -> clear();
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
  

  for(map<TString, vector<HistoUnfolding *>>::iterator bit = GetLevel(IN) -> _msyshistos .begin(); bit != GetLevel(IN) -> _msyshistos . end(); bit ++)
    {
      const TString sample = bit -> first;
      // if (sample != signaltag)
      // 	continue;

      printf("unfolding sample %s\n", sample.Data());
      if (sample == signaltag)
	{
	  for (vector<HistoUnfolding *>::iterator it = GetLevel(IN) -> GetV(SYSMO, sample.Data()) -> begin(); it != GetLevel(IN) -> GetV(SYSMO, sample.Data()) -> end(); it ++)
	    {
	      printf("unfolding systematic %s systype\n", (*it) -> GetTag(), (*it) -> GetSysType());
	      if ((*it) -> GetSysType() == EXPSYS)
		{
		  //continue;
		  TH2 * mmatrixsyst = (TH2 *) (*it) -> GetTH2() -> Clone("matrixsyst");
		  TH2 * mmatrixsystorig = (TH2 *) (*it) -> GetTH2() -> Clone("matrixsystorig");
		  //      (*it) -> ls();
		  //	  mmatrixsyst -> RebinX(2);
		  TH2 * datambckg = (TH2 * ) GetLevel(IN) -> GetHU(DATAMO) -> GetTH2() -> Clone(CreateName("datambackground_unfolding"));
		  datambckg -> SetDirectory(nullptr);
		  datambckg -> Add(totalbackground -> GetTH2(), -1);
		  //datambckg -> RebinX(2);
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
		  for (unsigned char bind = 0; bind <= hsignal -> GetNbinsY() + 1; bind ++)
		    {
		      // if (mmatrixsyst -> Integral(0, mmatrixsyst -> GetNbinsX() + 1, bind, bind) > 0.0)
		      // 	{
		      // 	  input -> SetBinContent(bind, input -> GetBinContent(bind) * (1.0 - mmatrixsyst -> GetBinContent(bind, 0)/mmatrixsyst -> Integral(0, mmatrixsyst -> GetNbinsX() + 1, bind, bind)));
		      // 	  input -> SetBinError(bind, input -> GetBinError(bind) * (1.0 - mmatrixsyst -> GetBinContent(bind, 0)/mmatrixsyst -> Integral(0, mmatrixsyst -> GetNbinsX() + 1, bind, bind)));
		      	      // 	}
		      //mmatrixsyst -> SetBinContent(0, bind, 0.0);
		    }
		  TH1 * hmatrixsystgen = mmatrixsyst -> ProjectionY("mmatrixsystgen");
		  TH1 * hmatrixsystreco = mmatrixsyst -> ProjectionX("mmatrixsystreco");
		  RooUnfoldResponse response(/*hreco/*input*/hmatrixsystreco, /*hgen*/hmatrixsystgen, mmatrixsyst);
		  RooUnfoldBayes   unfold (&response, input, kiter);
	  
		  HistoUnfoldingTH1 * out = new HistoUnfoldingTH1((SampleDescriptor * ) * it);
		  GetLevel(OUT) -> GetV(SYSMO, sample.Data()) -> push_back(out);
		  //	  out -> GetTH1Ref(RECO) = (TH1F*) unfold.GetFoldedOutput(CreateName(TString((*it) -> GetTag()) + "_folded_output"));
		  TH1 * houtput = unfold.Hreco();
	  
		  out -> GetTH1Ref(GEN) = houtput;
		  houtput -> SetName(CreateName(TString((*it) ->GetTag()) + "_output"));
		  for (unsigned char bind = 0; bind < houtput -> GetNbinsX() + 1; bind ++)
		    {
		      const float coeff = mmatrixsystorig -> Integral(1, mmatrixsystorig -> GetNbinsX() + 1, bind, bind)/mmatrixsystorig -> Integral(0, mmatrixsystorig -> GetNbinsX(), bind, bind);
		      //houtput -> SetBinContent(bind, houtput -> GetBinContent(bind) / coeff);
		    }
		  // printf("unfolded exp systematic %s \n", (*it) -> GetTag());
		  // out -> GetTH1(GEN) -> Print("all");
		  // getchar();
		  delete input;
		  delete mmatrixsyst;
		  delete hmatrixsystgen;
		  delete hmatrixsystreco;
		  delete mmatrixsystorig;
		}
	      if ((*it) -> GetSysType() == THEORSYS or (*it) -> GetSysType() == THEORSYSTEST)
		
		{
		  if (TString(method) == "nominal" and TString((*it) -> GetTag()) == "MC13TeV_TTJets_cflip" and not includecflip)
		    {
		      printf("found cflip: continuing");
		      getchar();
		      continue;
		    }

		  TH2 * inputsysh2 = (TH2*)(*it) -> GetTH2() -> Clone("inputsysh2");

		  //inputsysh2 -> RebinX(2);
		  TH1* inputsys = inputsysh2 -> ProjectionX("inputsys");
		  TH1* inputsysgen = inputsysh2 -> ProjectionY("inputsysgen");
	  
		  ApplyScaleFactor(inputsys);
		  RooUnfoldResponse response(hreco, hgen, hsignal);
		  RooUnfoldBayes   unfold (&response, inputsys, kiter);
		  HistoUnfoldingTH1 * out = new HistoUnfoldingTH1((SampleDescriptor * ) * it);
		  GetLevel(OUT) -> GetV(SYSMO, sample.Data()) -> push_back(out);
		  TH1 *  houtput  = unfold.Hreco();
		  for (unsigned char bind = 0; bind < houtput -> GetNbinsX() + 1; bind ++)
		    {
		      const float coeff = hsignalorig -> Integral(1, hsignalorig -> GetNbinsX() + 1, bind, bind)/hsignalorig -> Integral(0, hsignalorig -> GetNbinsX(), bind, bind);
		      //  houtput -> SetBinContent(bind, houtput -> GetBinContent(bind) / coeff);
		    }

		  out -> GetTH1Ref(GEN) = houtput;
  
		  houtput -> SetName(CreateName(TString((*it) ->GetTag()) + "_output"));
		  delete 	    inputsys;
		  delete inputsysh2;
		  delete inputsysgen;
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

	      RooUnfoldResponse response(hreco, hgen, hsignal);
	      TH1 * datambackgroundh1 = datambackground -> ProjectionX();

	      ApplyScaleFactor(datambackgroundh1);
	      RooUnfoldBayes unfold(&response, datambackgroundh1, kiter);
	      TH1 * houtput = unfold.Hreco();
	      houtput -> SetName(CreateName(TString((*it) ->GetTag()) + "_output"));
	      HistoUnfoldingTH1 * out = new HistoUnfoldingTH1((SampleDescriptor * ) * it);
	      GetLevel(OUT) -> GetV(SYSMO, sample.Data()) -> push_back(out);
	      out -> GetTH1Ref(RECO) = nullptr;
	      out -> GetTH1Ref(GEN) = houtput;
	      delete datambackground;
	      delete datambackgroundh1;
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
	for (double coeff = min; coeff <= max; coeff += step)
	  {
	    bins[start] = coeff;
	    if (start < nbins - 1)
	      func(start + 1, k, func);
	    else
	      {
		for (unsigned char bind = 0; bind < nbins; bind ++)
		  {
		    //  printf("%f ", bins[bind]);
		  }
		//printf("\n");
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
          // getchar();                                                                                                                                                                                     
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
      for (unsigned char bindx = 0; bindx <= hsignal -> GetNbinsX() + 1; bindx ++)
	{
	  hinnonclos -> SetBinContent(bindx, bindy, hinnonclos -> GetBinContent(bindx, bindy) * binsrecord[bindy - 1]);
	}
    }
  
  nonclosin -> GetTH2Ref() = (TH2F *) hinnonclos; 
  GetLevel(IN) -> GetV(SYSMO, signaltag) -> push_back(nonclosin);

  TH1 * inputnonclos = hinnonclos -> ProjectionX("inpuntnonclos");
  TH1 * hinnonclosgen = hinnonclos -> ProjectionY("hinnonclosgen");
  RooUnfoldResponse responsenonclos(inputnonclos, hinnonclosgen, hinnonclos);
  
  RooUnfoldBayes unfoldnonclos(&response, inputnonclos, 20/*kiter*/);
  // //ApplyScaleFactor(inputnonclos);
  for (unsigned char bindx = 0; bindx <= inputnonclos -> GetNbinsX() + 1; bindx ++)
    {
      printf("%f \n", inputnonclos -> GetBinContent(bindx)/ input -> GetBinContent(bindx));
    }
  // printf("before pushing nonclos to out\n");
  // for (vector<HistoUnfolding *> :: iterator it = GetLevel(OUT) -> GetV(SYSMO, signaltag) -> begin(); it != GetLevel(OUT) -> GetV(SYSMO, signaltag) -> end(); it ++)
  //   {
  //     if (TString((*it) -> GetTag()).Contains("non_clos"))
  // 	printf("tag %s\n", (*it) -> GetTag());
      
  //   }
  //  getchar();
  delete hinnonclosgen;
  GetLevel(OUT) -> GetV(SYSMO, signaltag) -> push_back(nonclosout);
  TH1 * houtputnonclos = unfoldnonclos.Hreco();
  houtputnonclos -> SetName(CreateName(TString(nonclosout -> GetTag()) + "_folded_output")); 
  nonclosout -> GetTH1Ref(RECO) = nullptr;
  nonclosout -> GetTH1Ref(GEN) = houtputnonclos;
  nonclosout -> SetTitle(TString(nonclosout -> GetTitle()) + " non closure");

  GetLevel(OUT) -> GetHURef(SIGNALMO) = GetLevel(IN) -> GetHU(SIGNALMO); 

  delete hsignal;

}

void CompoundHistoUnfolding::unfoldBayesianOLD(unsigned char kiter)
{
  TH2 * hsignal = (TH2 *) GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> Clone("unfolding_signal");
  TH2 * hsignalorig = (TH2 *) GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> Clone("unfolding_signal_orig");

  //hsignal -> RebinX(2);
  TH2 * datambackground = (TH2*) this -> GetLevel(IN) -> GetHU(DATAMO) -> GetTH2() -> Clone("datambackground");

  datambackground -> Add(this -> GetTotalBackground() -> GetTH2(), -1);
  //datambackground -> RebinX(2);                                                                                                                                                                           
  TH1 * hmeas = (TH1*) datambackground -> ProjectionX("meas");
  for (unsigned char bind = 0; bind <= hsignal -> GetNbinsX() + 1; bind ++)
    {
      // if (hsignal -> Integral(bind, bind, 0, hsignal -> GetNbinsY() + 1) > 0.0)
      //   hmeas -> SetBinContent(bind, hmeas -> GetBinContent(bind) * (1.0 - hsignal -> GetBinContent(bind, 0)/hsignal -> Integral(bind, bind, 0, hsignal -> GetNbinsY() + 1)));
      hsignal -> SetBinContent(bind, 0, 0.0);
      //      hmeas -> SetBinError(bind, 0, 0.0);
    }
  for (unsigned char bind = 0; bind <= hsignal -> GetNbinsY() + 1; bind ++)
    {
      // if (hsignal -> Integral(bind, bind, 0, hsignal -> GetNbinsY() + 1) > 0.0)
      //   hmeas -> SetBinContent(bind, hmeas -> GetBinContent(bind) * (1.0 - hsignal -> GetBinContent(bind, 0)/hsignal -> Integral(bind, bind, 0, hsignal -> GetNbinsY() + 1)));
      //      hsignal -> SetBinContent(0, bind, 0.0);
    }

  ApplyScaleFactor(hmeas);
  TH1 * hreco = hsignal -> ProjectionX();
  TH1 * hgen = hsignal -> ProjectionY("gen");
  RooUnfoldResponse response(hreco, hgen, hsignal);
  RooUnfoldBayes   unfold (&response, hmeas, kiter);
  TH1* houtput = (TH1D*) unfold.Hreco();
  for (unsigned char bind = 0; bind < houtput -> GetNbinsX() + 1; bind ++)
    {
      const float coeff = hsignalorig -> Integral(1, hsignalorig -> GetNbinsX() + 1, bind, bind)/hsignalorig -> Integral(0, hsignalorig -> GetNbinsX(), bind, bind);
      //houtput -> SetBinContent(bind, houtput -> GetBinContent(bind) / coeff);
    }
  if (GetLevel(OUT) -> GetHURef(DATAMBCKG))
    delete GetLevel(OUT) -> GetHURef(DATAMBCKG);
  GetLevel(OUT) -> GetHURef(DATAMBCKG) = new HistoUnfoldingTH1(GetLevel(IN) -> GetHURef(DATA));
  GetLevel(OUT) -> GetHU(DATAMBCKG) -> GetTH1Ref(GEN) = houtput;
  // printf("unfolded signal\n");
  // houtput -> Print("all");
  // printf("gen\n");
  // hsignalorig -> ProjectionY() -> Print("all");
  // getchar();
  GetLevel(OUT) -> GetV(SYSMO) -> clear();

  for (vector<HistoUnfolding *>::iterator it = GetLevel(IN) -> GetV(SYSMO) -> begin(); it != GetLevel(IN) -> GetV(SYSMO) -> end(); it ++)
    {
      printf("unfolding systematic %s systype\n", (*it) -> GetTag(), (*it) -> GetSysType());
      if ((*it) -> GetSysType() == EXPSYS)
	{
	  //continue;
	  TH2 * mmatrixsyst = (TH2 *) (*it) -> GetTH2() -> Clone("matrixsyst");
	  TH2 * mmatrixsystorig = (TH2 *) (*it) -> GetTH2() -> Clone("matrixsystorig");
	  //      (*it) -> ls();
	  //	  mmatrixsyst -> RebinX(2);
	  TH2 * datambckg = (TH2 * ) GetLevel(IN) -> GetHU(DATAMO) -> GetTH2() -> Clone(CreateName("datambackground_unfolding"));
	  datambckg -> SetDirectory(nullptr);
	  datambckg -> Add(totalbackground -> GetTH2(), -1);
	  //datambckg -> RebinX(2);
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
	  for (unsigned char bind = 0; bind <= hsignal -> GetNbinsY() + 1; bind ++)
	    {
	      // if (hsignal -> Integral(bind, bind, 0, hsignal -> GetNbinsY() + 1) > 0.0)
	      //   hmeas -> SetBinContent(bind, hmeas -> GetBinContent(bind) * (1.0 - hsignal -> GetBinContent(bind, 0)/hsignal -> Integral(bind, bind, 0, hsignal -> GetNbinsY() + 1)));
	      //mmatrixsyst -> SetBinContent(0, bind, 0.0);
	    }
	  TH1 * hmatrixsystgen = mmatrixsyst -> ProjectionY("mmatrixsystgen");
	  TH1 * hmatrixsystreco = mmatrixsyst -> ProjectionX("mmatrixsystreco");
	  RooUnfoldResponse response(/*hreco/*input*/hmatrixsystreco, /*hgen*/hmatrixsystgen, mmatrixsyst);
	  RooUnfoldBayes   unfold (&response, input, kiter);
	  
	  HistoUnfoldingTH1 * out = new HistoUnfoldingTH1((SampleDescriptor * ) * it);
	  GetLevel(OUT) -> GetV(SYSMO) -> push_back(out);
	  //	  out -> GetTH1Ref(RECO) = (TH1F*) unfold.GetFoldedOutput(CreateName(TString((*it) -> GetTag()) + "_folded_output"));
	  TH1 * houtput = unfold.Hreco();
	  
	  out -> GetTH1Ref(GEN) = houtput;
	  houtput -> SetName(CreateName(TString((*it) ->GetTag()) + "_output"));
	  for (unsigned char bind = 0; bind < houtput -> GetNbinsX() + 1; bind ++)
	    {
	      const float coeff = mmatrixsystorig -> Integral(1, mmatrixsystorig -> GetNbinsX() + 1, bind, bind)/mmatrixsystorig -> Integral(0, mmatrixsystorig -> GetNbinsX(), bind, bind);
	      //houtput -> SetBinContent(bind, houtput -> GetBinContent(bind) / coeff);
	    }
	  // printf("unfolded exp systematic %s \n", (*it) -> GetTag());
	  // out -> GetTH1(GEN) -> Print("all");
	  // getchar();
	  delete input;
	  delete mmatrixsyst;
	  delete hmatrixsystgen;
	  delete hmatrixsystreco;
	  delete mmatrixsystorig;
	}
      if ((*it) -> GetSysType() == THEORSYS)
	{
	  printf("unfolding %s\n", (*it) -> GetTag());
	  TH2 * inputsysh2 = (TH2*)(*it) -> GetTH2() -> Clone("inputsysh2");
	  //inputsysh2 -> RebinX(2);
	  TH1* inputsys = inputsysh2 -> ProjectionX("inputsys");
	  TH1* inputsysgen = inputsysh2 -> ProjectionY("inputsysgen");
	  
	  ApplyScaleFactor(inputsys);
	  RooUnfoldResponse response(hreco, hgen, hsignal);
	  RooUnfoldBayes   unfold (&response, inputsys, kiter);
	  HistoUnfoldingTH1 * out = new HistoUnfoldingTH1((SampleDescriptor * ) * it);
	  GetLevel(OUT) -> GetV(SYSMO) -> push_back(out);
	  TH1 *  houtput  = unfold.Hreco();
	  for (unsigned char bind = 0; bind < houtput -> GetNbinsX() + 1; bind ++)
	    {
	      const float coeff = hsignalorig -> Integral(1, hsignalorig -> GetNbinsX() + 1, bind, bind)/hsignalorig -> Integral(0, hsignalorig -> GetNbinsX(), bind, bind);
	      //  houtput -> SetBinContent(bind, houtput -> GetBinContent(bind) / coeff);
	    }

	  out -> GetTH1Ref(GEN) = houtput;
  
	  houtput -> SetName(CreateName(TString((*it) ->GetTag()) + "_output"));
	  delete 	    inputsys;
	  delete inputsysh2;
	  delete inputsysgen;
	}

    }
  GetLevel(OUT) -> GetHURef(SIGNALMO) = GetLevel(IN) -> GetHU(SIGNALMO); 

  delete hsignal;

}
