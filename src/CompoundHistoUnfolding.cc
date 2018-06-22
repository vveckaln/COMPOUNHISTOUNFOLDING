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
ClassImp(CompoundHistoUnfolding);
void CompoundHistoUnfolding::Process()
{
  AggregateBackgroundMCs();
  CreateBackgroundTotal();
  CreateListOfExpSysSamples();
  CreateMCTotal(IN);
  unfold();
  createCov();
  CreateMCTotal(OUT);
  CreateTotalMCUnc(IN, RECO);
  CreateTotalMCUnc(IN, RECO, true);
  CreateTotalMCUnc(IN, GEN);
  CreateTotalMCUnc(IN, GEN, true);
  CreateTotalMCUnc(OUT, RECO);
  CreateTotalMCUnc(OUT, RECO, true);
  CreateTotalMCUnc(OUT, GEN);
  CreateTotalMCUnc(OUT, GEN, true);
  CreateDataGraph(OUT);
  CreateDataGraph(IN);

}


float* CompoundHistoUnfolding::splitForMinSigmaM(unsigned long & size, float factor)
{
  printf("splitForMinSigmaM\n");
  TH2F & h = *GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2();
  TAxis * Xaxis = h.GetXaxis();
  //TAxis * Yaxix = h.GetYaxis();                                                                                                                                                                           

  TF1 f1 = TF1("f1", "gaus", Xaxis -> GetXmin(), Xaxis -> GetXmax());
  f1.SetParameters(100, (Xaxis -> GetXmax() - Xaxis -> GetXmin())/2., 0.01);
  TObjArray slices;
  h.FitSlicesX(& f1, 1, h.GetNbinsX(), 0, "QNRLM", &slices);

  TCanvas c2("c2", "c2");
  ((TH1D*)slices[0]) -> Draw();
  c2.Print((string(_folder) + "/slices_N.eps").c_str());
  ((TH1D*)slices[1]) -> GetYaxis() -> SetRangeUser(Xaxis -> GetXmin(), Xaxis -> GetXmax());
  ((TH1D*)slices[1]) -> Draw();
  c2.Print((string(_folder) + "/slices_mean.eps").c_str());
  ((TH1D*)slices[2]) -> GetYaxis() -> SetRangeUser(0.0, Xaxis -> GetXmax());
  ((TH1D*)slices[2]) -> Draw();
  c2.Print((string(_folder) + "/slices_sigma.eps").c_str());

  vector<float> bins;
  bins.push_back(Xaxis -> GetXmin());
  vector<float> exact;
  exact.push_back(Xaxis -> GetXmin());
  const float integral = h.Integral();
  float binfraction = 0.0;
  for (unsigned char ind = 0; ind < h.GetNbinsX() + 1; ind ++)
    {
      TH1F * proj = (TH1F*) h.ProjectionX("px", ind, ind);
      binfraction += proj -> Integral()/integral;
      delete proj;
      const float mean      = ((TH1D *)slices[1]) -> GetBinContent(ind);
      const float meanError = ((TH1D *)slices[1]) -> GetBinError(ind);
      if (mean == 0 or meanError/mean > 0.25)
        continue;
      const float sigma = ((TH1D *) slices[2]) -> GetBinContent(ind) * factor;
      if (mean - sigma > exact.back())
	{
	  if (mean + sigma < h.GetXaxis() -> GetXmax())
	    {
	      exact.push_back(mean + sigma);
	      const float newbinedge = h.GetXaxis() -> GetBinUpEdge(h.GetXaxis() -> FindBin(mean + sigma));
	      if (newbinedge > bins.back() and binfraction > 0.01)
		{
		  bins.push_back(newbinedge);
		  binfraction = 0.0;
		}
	    }
	}
    }
  if (bins.size() > 1)
    {
      bins.back() = Xaxis -> GetXmax();
    }
  size = bins.size();
  float * fbins = new float[size];
  for (unsigned char ind = 0; ind < size; ind ++)
    {
      fbins[ind] = bins[ind];
    }
  return fbins;
}

float * CompoundHistoUnfolding::split(float * input, unsigned long size)
{
  float * output = new float[2*(size - 1) + 1];
  for (unsigned long ind = 0; ind < size - 1; ind ++)
    {
      const float half = 0.5*(input[ind + 1] - input[ind]);
      output[2 * ind] = input[ind];
      output[2 * ind + 1] = input[ind] + half;

    }
  output[2*(size - 1)] = input[size - 1];
  return output;
}
void CompoundHistoUnfolding::unfold()
{
  ScaleFactor();
  CreateDataMinusBackground();
  TCanvas cmigration("cmigration", "cmigration");
  TH2F * hsignal = GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2();
  hsignal -> Draw("COLZ");
  cmigration.SaveAs(TString(_folder) + "/migrationmatrix.png");
  TUnfoldDensity unfold(hsignal, TUnfold::kHistMapOutputVert, TUnfold::kRegModeCurvature, TUnfold::kEConstraintArea, TUnfoldDensity::kDensityModeBinWidthAndUser);
  TH1F * input = /*(TH1F*) _signalh -> _th2 -> ProjectionX(); */GetLevel(IN) -> GetHU(DATAMBCKG) -> Project(RECO, CreateName("input"));

  ApplyScaleFactor(input);
  input -> SetMinimum(0.0);
  input -> SetDirectory(nullptr);
  unfold.SetInput(input);
  unfold.DoUnfold(0.0);
  TCanvas *coutput = new TCanvas(CreateName("coutput"), CreateTitle("coutput"));
  THStack *coutput_stack = new THStack(CreateName("output"), CreateTitle("output"));
  TH1F * houtput = (TH1F*) unfold.GetOutput(CreateName("output"));
  TH1F * py = GetLevel(IN) -> GetHU(SIGNALMO) -> Project(GEN, CreateName("py"));
  py -> SetMinimum(0.0);
  NormaliseToBinWidth(py);
  coutput_stack -> Add(py);
  TH1F * houtput_draw = (TH1F*) houtput -> Clone(CreateName("houtput_draw"));
  houtput_draw -> SetDirectory(nullptr);
  NormaliseToBinWidth(houtput_draw);
  houtput_draw -> SetMinimum(0.0);
  coutput_stack -> Add(houtput_draw);
  houtput_draw -> SetLineColor(kGreen);
  coutput_stack -> Draw("NOSTACK");
  TLegend legend_output(0.6, 0.7, 1.0, 1.0);
  legend_output.SetTextSize(0.03);
  legend_output.AddEntry(houtput_draw, "unfolded");
  legend_output.AddEntry(py, TString(GetLevel(IN) -> GetHU(SIGNALMO) -> GetTitle()) + " gen");
  legend_output.Draw("SAME");
  coutput -> SaveAs(TString(_folder) + "/" + "coutput.png");
  
  GetLevel(OUT) -> GetHURef(SIGNALMO) = new HistoUnfoldingTH1(GetLevel(IN) -> GetHURef(SIGNALMO));
  GetLevel(OUT) -> GetHU(SIGNALMO) -> GetTH1Ref(GEN) = houtput;
  TCanvas * cfolded_output = new TCanvas(CreateName("folded_output"), CreateTitle("folded_output"));
  THStack * cfolded_output_stack = new THStack(CreateName("cfolded_output_stack"), CreateTitle("cfolded output"));
  TH1F * hfolded_output = (TH1F*) unfold.GetFoldedOutput(CreateName("folded_output"));
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
  cfolded_output_stack -> Add(hfolded_output_draw);
  hfolded_output_draw -> SetLineColor(kGreen);
  hfolded_output_draw -> SetMinimum(0.0);
  cfolded_output_stack -> Draw("NOSTACK");
  TLegend legend_input(0.6, 0.7, 1.0, 1.0);
  legend_input.SetTextSize(0.03);
  legend_input.AddEntry(input_draw, "dataminusbckg");
  legend_input.AddEntry(px, TString(GetLevel(IN) -> GetHU(SIGNALMO) -> GetTitle()) + " reco no fakes");
  legend_input.AddEntry(hfolded_output_draw, "folded_output");
  legend_input.Draw("SAME");
  cfolded_output -> SaveAs(TString(_folder) + "/" + "cfolded_output.png");
  GetLevel(OUT) -> GetHU(SIGNALMO) -> GetTH1Ref(RECO) = hfolded_output; 
  GetLevel(OUT) -> GetHU(SIGNALMO) -> GetTH1Ref(GEN) = houtput;
  for (vector<HistoUnfolding *>::iterator it = GetLevel(IN) -> GetV(SYSMO) -> begin(); it != GetLevel(IN) -> GetV(SYSMO) -> end(); it ++)
    {
      TUnfoldDensity unfold((*it) -> GetTH2(), TUnfold::kHistMapOutputVert, TUnfold::kRegModeCurvature, TUnfold::kEConstraintArea, TUnfoldDensity::kDensityModeUser);
      unfold.SetInput(input);
      unfold.DoUnfold(0.0);
      HistoUnfoldingTH1 * out = new HistoUnfoldingTH1((SampleDescriptor * ) * it);
      GetLevel(OUT) -> GetV(SYSMO) -> push_back(out);
      out -> GetTH1Ref(RECO) = (TH1F*) unfold.GetFoldedOutput(CreateName(TString((*it) -> GetTag()) + "_output"));
      out -> GetTH1Ref(GEN) = (TH1F*) unfold.GetOutput(CreateName(TString((*it) ->GetTag()) + "_folded_output"));
    }
  GetLevel(OUT) -> GetHURef(DATAMO) = new HistoUnfoldingTH2((HistoUnfoldingTH2*) GetLevel(IN) -> GetHU(DATAMO), CreateName("OUTData"));
  
}
void CompoundHistoUnfolding::LoadHistos(const char * json, SysTypeCode_t sys)
{
  JsonParser parser;
  parser.SetSignalTag(signaltag);
  parser.Load(json, sys);
  //parser.ls();
  const unsigned long ssize = parser.GetSize();
  for (unsigned long sind = 0; sind < ssize; sind ++)
    {
      HistoUnfoldingTH2 *h = nullptr;
      if (TString(signaltag) != "MC13TeV_TTJets2l2nu_amcatnlo" and TString(parser.GetSample(sind) -> GetTag()) == "MC13TeV_TTJets2l2nu_amcatnlo") 
	continue;
      if (TString(parser.GetSample(sind) -> GetTag()) == TString(signaltag))
	parser.GetSample(sind) -> SetSysTypeCode(NOMINAL);
      if (TString(parser.GetSample(sind) -> GetTag()) == "MC13TeV_TTJets" and TString(signaltag) != "MC13TeV_TTJets")
	parser.GetSample(sind) -> SetSysTypeCode(THEORSYS);
      if (sys == EXPSYS)
	parser.GetSample(sind) -> SetTag(TString(parser.GetSample(sind) -> GetTag()).ReplaceAll("MC13TeV_TTJets", signaltag));
      if (IsRegular())
	{
	  h = new HistoUnfoldingTH2(GetName(), CreateTitle(_title), _nbinsx, _xlow, _xup, _nbinsy, _ylow, _yup, parser.GetSample(sind)); 
	  h -> GetTH2() -> RebinY(2);
	}
      else
	{
	  h = new HistoUnfoldingTH2(GetName(), CreateTitle(_title), _nentriesx - 1, _xbins, _nentriesy - 1, _ybins, parser.GetSample(sind));
	}
      h -> FillFromTree(sys != EXPSYS ? "MC13TeV_TTJets" : signaltag, tag_jet_types_[jetcode], tag_charge_types_[chargecode], observable);
      AddHisto(h);
      parser.UnsetSample(sind);

    }
}
void CompoundHistoUnfolding::ApproximateTheorSys()
{
  if (string(signaltag).compare("MC13TeV_TTJets") == 0)
    return;
  const TString binfilename = TString("/eos/user/v/vveckaln/unfolding_") + method + "/" + tag_jet_types_[jetcode] + '_' + tag_charge_types_[chargecode] + '_' + "MC13TeV_TTJets" + '_' + observable + '_' + tag_opt[optcode] + '_' + method + "/save.root";
  TFile * fMC13TeV_TTJets = TFile::Open(binfilename);
  TH2F * hMC13TeV_TTJets = (TH2F*) fMC13TeV_TTJets -> Get(TString(tag_opt[optcode]) + "_MC13TeV_TTJets_aggr");
  for (vector<HistoUnfolding *>::iterator it = GetLevel(IN) -> _vsyshistos.begin(); it != GetLevel(IN) -> _vsyshistos.end(); it ++)
    {
      if ((*it) -> GetSysType() != THEORSYS)
	continue;
      TH2F * h = (*it) -> GetTH2();
      for (unsigned char xbin = 0; xbin < h -> GetNbinsX() + 2; xbin ++)
	{
	  for (unsigned char ybin = 0; ybin < h -> GetNbinsY() + 2; ybin ++)
	    {
	      if (hMC13TeV_TTJets -> GetBinContent(xbin, ybin) < 10.0)
		continue;
	      h -> SetBinContent(xbin, ybin, h -> GetBinContent(xbin, ybin) * GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> GetBinContent(xbin, ybin) / hMC13TeV_TTJets -> GetBinContent(xbin, ybin));
	    }
	}
    }
  fMC13TeV_TTJets -> Close();
}


void CompoundHistoUnfolding::AddHisto(HistoUnfoldingTH2 * histo)
{
  //printf("Adding \n");
  //  histo -> ls();
  //histo -> _th1 -> SetMinimum(0.0);
  if (histo -> GetSampleType() != DATA)
    histo -> GetTH2() -> Scale(luminosity * histo -> GetXsec());

  if (histo -> IsSys())
    {
      if (TString(histo -> GetTitle()).Contains("t#bar{t}"))
	{
	  GetV(IN, SYSMO) -> push_back(histo);
	for (unsigned char ind = 0; ind < histo -> GetTH2() -> GetNbinsX() + 2; ind ++)
	  {
	    histo -> GetTH2() -> SetBinContent(ind, 0, 0.0);
	  }
	}
    return;
    }

  GetV(IN, histo -> GetSampleType()) -> push_back(histo);
  printf("sample type %u title %s\n", histo -> GetSampleType(), histo -> GetTitle());
  if (histo -> GetSampleType() != BACKGROUNDMO)
    {
      HistoUnfolding *& h = GetLevel(IN) -> GetHURef(histo -> GetSampleType());
      if (h)
	{
	  h -> GetTH2() -> Add(histo -> GetTH2());
	}
      else
	{
	  h = new HistoUnfoldingTH2(histo, TString(histo -> GetTH2() -> GetName()) + "_aggr");
	  h -> GetTH2() -> SetTitle(CreateTitle(histo -> GetTitle()));
	}
    }
  
}

 CompoundHistoUnfolding::Level * CompoundHistoUnfolding::GetLevel(ResultLevelCode_t resultcode)
 {
   switch(resultcode)
     {
     case IN:
       return & INLEVEL;
     case OUT:
       return & OUTLEVEL;
     }
 }

 vector<HistoUnfolding *> * CompoundHistoUnfolding::GetV(ResultLevelCode_t resultcode, MOCode_t mo)
 {
  Level * level = GetLevel(resultcode);
  switch(mo)
    {
    case BACKGROUNDMO:
      return & _vbackgroundhistos;
    default:
      return level -> GetV(mo);
    }
}

void CompoundHistoUnfolding::AggregateBackgroundMCs()
{
  printf("Aggregating backgrounds \n");
  for(vector<HistoUnfolding *> :: iterator it = _vbackgroundhistos.begin(); it != _vbackgroundhistos.end(); it ++)
    {
      const TString title ((*it) -> GetTitle());
      if (aggrbackgroundMC.find(title) == aggrbackgroundMC.end())
	{
	  HistoUnfoldingTH2 *check = new HistoUnfoldingTH2(* (HistoUnfoldingTH2*) * it);
	  aggrbackgroundMC[title] = check;
	  
	}
      else
	{
	  aggrbackgroundMC[title] -> GetTH2() -> Add((*it) -> GetTH2());
	}
    }
}


void CompoundHistoUnfolding::CreateBackgroundTotal()
{
  for (map<TString, HistoUnfolding *>::iterator it = aggrbackgroundMC.begin(); it != aggrbackgroundMC.end(); it ++)
    {
      if (not totalbackground)
	{
	totalbackground = new HistoUnfoldingTH2();
	totalbackground -> GetTH2Ref() = (TH2F*) it -> second -> GetTH2() -> Clone(CreateName("totalbackground"));
	totalbackground -> GetTH2() -> SetDirectory(nullptr);
	}
      else
        totalbackground -> GetTH2() -> Add(it -> second -> GetTH2());
    }
}


void CompoundHistoUnfolding::CreateMCTotal(ResultLevelCode_t resultcode)
{
  HistoUnfolding *& totalMC = GetLevel(resultcode) -> GetHURef(TOTALMC);
  if (resultcode == IN)
    {
      totalMC = new HistoUnfoldingTH2();
      totalMC -> SetTitle("MCTotal");
      totalMC -> GetTH2Ref() = (TH2F*) GetLevel(resultcode) -> GetHU(SIGNALMO) -> GetTH2() -> Clone(CreateName("MCtotalIN"));
      totalMC -> GetTH2() -> SetDirectory(nullptr);
      totalMC -> GetTH2() -> Add(totalbackground -> GetTH2());
    }
  if (resultcode == OUT)
    {
      totalMC = new HistoUnfoldingTH1();
      totalMC -> SetTitle("MCTotal");
      totalMC -> GetTH1Ref(RECO) = GetLevel(resultcode) -> GetHU(SIGNALMO) -> Project(RECO, CreateName("MCtotalOUTRECO"));
      TH1F *bckgsfRECO = totalbackground -> Project(RECO);
      ApplyScaleFactor(bckgsfRECO);
      totalMC -> GetTH1(RECO) -> Add(bckgsfRECO);
      delete bckgsfRECO;
      totalMC -> GetTH1Ref(GEN) = GetLevel(resultcode) -> GetHU(SIGNALMO) -> Project(GEN, CreateName("MCtotalOUTGEN"));
      TH1F *bckgGEN = totalbackground -> Project(GEN);
      totalMC -> GetTH1(GEN) -> Add(bckgGEN);
      totalMC -> lsbins(RECO);
      delete bckgGEN;
      
    }
}



void CompoundHistoUnfolding::CreateDataMinusBackground(ResultLevelCode_t resultcode)
{
  
  HistoUnfolding *& datambackground = GetLevel(resultcode) -> GetHURef(DATAMBCKG);
  if (datambackground)
    return;
  datambackground = new HistoUnfoldingTH2();
  datambackground -> GetTH2Ref() = (TH2F * ) GetLevel(resultcode) -> GetHU(DATAMO) -> GetTH2() -> Clone(CreateName("datambackground"));
  datambackground -> GetTH2() -> SetDirectory(nullptr);
  datambackground -> GetTH2() -> Add(totalbackground -> GetTH2(), -1);
}

void CompoundHistoUnfolding::CreateTotalMCUnc(ResultLevelCode_t resultcode, RecoLevelCode_t recocode, bool shape)
{
  printf("void CompoundHistoUnfolding::CreateTotalMCUnc(ResultLevelCode_t resultcode %u, RecoLevelCode_t recocode %u, bool shape %s)\n", resultcode, recocode, shape ? "shape" : "noshape");

  MOCode_t mo = shape ? TOTALMCUNCSHAPE : TOTALMCUNC;
  if (not GetLevel(resultcode) -> GetHU(mo))
    GetLevel(resultcode) -> GetHURef(mo) = new HistoUnfoldingTH1();
  TH1F *& htotalMCUnc = GetLevel(resultcode) -> GetHU(mo) -> GetTH1Ref(recocode);
  htotalMCUnc = GetLevel(resultcode) -> GetHU(TOTALMC) -> Project(recocode, CreateName(shape ? "totalMCUncShape" : "totalMCUnc"));

  const unsigned char nbins = htotalMCUnc -> GetNbinsX();

  if (recocode == RECO and resultcode == IN)
      ApplyScaleFactor(htotalMCUnc);
  TH1F * hsig = GetLevel(resultcode) -> GetHU(SIGNALMO) -> Project(recocode, CreateName("sig"));
  /*  if (recocode == RECO)
      ApplyScaleFactor(hsig);*/
  const double integralnominal = hsig -> Integral();
  float sysUp[nbins + 2];
  float sysDown[nbins + 2];
  for (unsigned char ind = 0; ind < nbins + 2; ind ++)
    {
      sysUp[ind] = 0.0;
      sysDown[ind] = 0.0;
    }
  for (vector<HistoUnfolding *>::iterator it = GetLevel(resultcode) -> GetV(SYSMO) -> begin(); it != GetLevel(resultcode) -> GetV(SYSMO) -> end(); it ++)
    {
      TH1F* hsys = (*it) -> Project(recocode);
      if (shape)
	{
	  const double integral = hsys -> Integral();
	  hsys -> Scale(integralnominal/integral);
	}
      //    (*it) -> ls();
      for (unsigned char bin_ind = 0; bin_ind < nbins + 2; bin_ind ++)
	{
	  const float diff = hsys -> GetBinContent(bin_ind) - hsig -> GetBinContent(bin_ind);
	  //  printf("bin_ind %u diff %f\n", bin_ind, diff);
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
      htotalMCUnc -> SetBinContent(bin_ind, htotalMCUnc -> GetBinContent(bin_ind) + (sysUp[bin_ind] - sysDown[bin_ind])/2.0);
      htotalMCUnc -> SetBinError(bin_ind, TMath::Sqrt(TMath::Power(htotalMCUnc -> GetBinError(bin_ind), 2) + TMath::Power((sysUp[bin_ind] + sysDown[bin_ind])/2.0, 2)));
    }
  delete hsig;

  NormaliseToBinWidth(htotalMCUnc);
}


void CompoundHistoUnfolding::ScaleFactor()
{
  _size_sf  = GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> GetNbinsX() + 2;
  _sf = new float[_size_sf];
  TH1F * sigReco = GetLevel(IN) -> GetHU(SIGNALMO) -> Project(RECO, CreateName("sigReco"));
  sigReco -> SetTitle(CreateTitle("signal RECO"));
  TH1F * sigRecoNonGen = (TH1F*) GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> ProjectionX(CreateName("sigRecoNonGen"), 0, 0);
  sigRecoNonGen -> SetDirectory(nullptr);
  TCanvas *c = new TCanvas(CreateName("checkproj"), CreateTitle("checkproj"));
  
  sigReco -> SetLineColor(kRed);
  sigReco -> SetMinimum(0.0);
  sigReco -> Draw();
  for (unsigned char ind = 0; ind < _size_sf; ind ++)
    {
      float sfi = 1.0;
      if (sigReco -> GetBinContent(ind) > 0.0)
	{
	  sfi = sigRecoNonGen -> GetBinContent(ind) / sigReco -> GetBinContent(ind);
	}
      _sf[ind] = sfi;
      GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2() -> SetBinContent(ind, 0, 0.0);
    }
  for (unsigned char ind = 0; ind < _size_sf; ind ++)
    {
      printf("ind %u sf %f\n", ind, _sf[ind]);
    }
  TH1F * after = GetLevel(IN) -> GetHU(SIGNALMO) -> Project(RECO, CreateName("after"));
  after -> SetTitle("after sf");
  after -> Draw("SAME");
  after -> SetMinimum(0.0);
  TLegend * legend = new TLegend(0.6, 0.6, 1.0, 1.0);
  legend -> AddEntry(sigReco);
  legend -> AddEntry(after);
  legend -> Draw("SAME");

 //delete sigReco;
  delete sigRecoNonGen;
}


void CompoundHistoUnfolding::ApplyScaleFactor(TH1 *h)
{
  for (unsigned char ind = 0; ind < _size_sf; ind ++)
    {
      h -> SetBinContent(ind, (1 - _sf[ind]) * h -> GetBinContent(ind));
      h -> SetBinError(ind, (1 - _sf[ind]) * h -> GetBinError(ind));
    }
}

void CompoundHistoUnfolding::NormaliseToBinWidth(TH1 *h)
{
  for (unsigned char ind = 1; ind < h -> GetNbinsX() + 1; ind ++)
    {
      h -> SetBinContent(ind, h -> GetBinContent(ind) * _orig_bin_width / h -> GetBinWidth(ind));
      h -> SetBinError(ind, h -> GetBinError(ind) * _orig_bin_width / h -> GetBinWidth(ind));

    }
}


/*
void CompoundHisto::PlotDataMC()
{
  TCanvas c;
  _datath1 -> SetMinimum(0.0);
  _datath1 -> Draw();
  TH1F *hMCTotal = NewMCTotal();
  hMCTotal -> SetMinimum(0.0);
  hMCTotal -> SetLineColor(kRed);
  hMCTotal-> Draw("HISTOSAME");
  c.SaveAs("test_output/DataMC.png");
  delete hMCTotal;
}

*/

THStack * CompoundHistoUnfolding::CreateTHStack(RecoLevelCode_t recocode, ResultLevelCode_t resultcode)
{
  Format();
  Level * level = GetLevel(resultcode);
  Level::ProjectionDeco * projdeco = level -> GetProjectionDeco(recocode);
  TLegend *& legend = projdeco -> legend;
  legend = new TLegend(0.6, 0.6, 1.0, 1.0);
  THStack * stack = new THStack("mc", "mc");
  for (map<TString, HistoUnfolding*> :: iterator it = aggrbackgroundMC.begin(); it != aggrbackgroundMC.end(); it ++)
    {
      TH1F * hbackground = it -> second  -> Project(recocode, CreateName(TString(it -> second -> GetTag()) + "background"));
      if (recocode == RECO)
	  ApplyScaleFactor(hbackground);
      NormaliseToBinWidth(hbackground);
      stack -> Add(hbackground);
      if (hbackground -> Integral())
	legend -> AddEntry(hbackground, it -> second -> GetTitle());
      //delete hbackground;
    }
  TH1F * hsig = level -> GetHU(SIGNALMO) -> Project(recocode, CreateName("sigstack"));
  NormaliseToBinWidth(hsig);
  stack -> Add(hsig);
  legend -> AddEntry(hsig, level -> GetHU(SIGNALMO) -> GetTitle());
  return stack;
}
 

   
TPad * CompoundHistoUnfolding::CreateMainPlot(RecoLevelCode_t recocode, ResultLevelCode_t resultcode)
{ 

  Level * level = GetLevel(resultcode);
  Level::ProjectionDeco * projdeco = level -> GetProjectionDeco(recocode);
  THStack * stack = CreateTHStack(recocode, resultcode);
  float max = 0.0;
  TH1F * totalMC = level -> GetHU(TOTALMC) -> Project(recocode, CreateName("totalMCmp"));
  //  TH1F * data = nullptr;
  TH1F * totalMCUnc = level -> GetHU(TOTALMCUNC) -> GetTH1(recocode);
  TH1F * totalMCUncShape = level -> GetHU(TOTALMCUNCSHAPE) -> GetTH1(recocode);;
  TH1F * hoverlay = nullptr;
  TLegend * legend = projdeco -> legend;
  if (recocode == RECO and resultcode == IN)
    ApplyScaleFactor(totalMC);
  if (resultcode == OUT and recocode == RECO)
    {
      hoverlay = level -> GetHU(SIGNALMO) -> Project(RECO, CreateName("overlay"));
      hoverlay -> SetLineColor(kRed);
      TH1F *bckgsf = (TH1F*) totalbackground  -> Project(RECO, CreateName("bckgsf"));
      ApplyScaleFactor(bckgsf);
      hoverlay -> Add(bckgsf);
      NormaliseToBinWidth(hoverlay);
      for (unsigned char bin_ind = 0; bin_ind < hoverlay -> GetNbinsX(); bin_ind ++)
	{
	  printf("bin_ind %u overlay %f\n", bin_ind, hoverlay -> GetBinContent(bin_ind));
	}
      delete bckgsf;
    }
  NormaliseToBinWidth(totalMC);
  if (resultcode == OUT)
    {
      projdeco -> _grtotalMC = new TGraphAsymmErrors(totalMC);
    }
  double maximum = 0.0;
  if (recocode == RECO)
    {
      const double grmaximum = TMath::MaxElement(projdeco -> _datagr -> GetN(), projdeco -> _datagr -> GetY());
      maximum = stack -> GetMaximum() > grmaximum ? stack -> GetMaximum() : grmaximum;
      
    }
  else
    maximum = stack -> GetMaximum();
  stack -> SetMaximum(1.4 * maximum);
  TPad *pad = new TPad(CreateName("pad1"), CreateTitle("pad1"), 0.0, 0.2, 1.0, 1.0);
  pad -> Draw();
  pad -> cd();
  stack -> Draw("HISTO");
  if (hoverlay)
    {
      hoverlay -> Draw("SAME");
      legend -> AddEntry(hoverlay, TString(GetLevel(IN) -> GetHU(SIGNALMO) -> GetTitle()) + " + bckg");
    }

  totalMCUnc -> Draw("E2SAME");
  totalMCUncShape -> Draw("E2SAME");
  if (recocode == RECO)
    {
      projdeco -> _datagr -> Draw("P");
      legend -> AddEntry(projdeco -> _datagr, "data");
    }
  legend -> Draw("SAME");
  Format();
  level -> Format();
  projdeco -> Format();
  delete totalMC;
  //if (data) delete data;
  return pad;
}


void CompoundHistoUnfolding::CreateDataGraph(ResultLevelCode_t resultcode)
{
  printf("void CompoundHistoUnfolding::CreateDataGraph(ResultLevelCode_t resultcode) %u\n", resultcode);
  Level * level = GetLevel(resultcode);
  Level::ProjectionDeco * projdeco = level -> GetProjectionDeco(RECO);

  TH1F * data = level -> GetHU(DATAMO) -> Project(RECO, CreateName("datadg"));
  ApplyScaleFactor(data);
  NormaliseToBinWidth(data);
  projdeco -> _datagr = new TGraphErrors(data);
  delete data;
}

 
TPad * CompoundHistoUnfolding::CreateRatioGraph(RecoLevelCode_t recocode, ResultLevelCode_t resultcode)
{
  Level * level = GetLevel(resultcode);
  Level::ProjectionDeco * projdeco = level -> GetProjectionDeco(recocode);
  TH1F * htotalMC = level -> GetHU(TOTALMC) -> Project(recocode, CreateName("totalmcrg"));
  TH1F * htotalMCUnc = level -> GetHU(TOTALMCUNC) -> GetTH1(recocode);
  TH1F * htotalMCUncShape = level -> GetHU(TOTALMCUNCSHAPE) -> GetTH1(recocode);
  if (recocode == RECO and resultcode == IN)
    ApplyScaleFactor(htotalMC);
  NormaliseToBinWidth(htotalMC);
  projdeco -> _ratioframe = (TH1F*) htotalMC -> Clone(CreateName("ratioframe"));
  projdeco -> _ratioframe -> SetDirectory(nullptr);
  projdeco -> _ratioframe -> Reset("ICE");
  projdeco -> _ratioframe -> GetYaxis() -> SetRangeUser(0.4, 1.6);
  projdeco -> _ratioframe -> GetYaxis() -> SetTitle("Data/MC");
  projdeco -> _ratioframeshape = (TH1F*) projdeco -> _ratioframe -> Clone(CreateName("ratioframeshape"));
  projdeco -> _ratioframeshape -> SetDirectory(nullptr);
  TH1F * htotalMCnoUnc = (TH1F*) htotalMC -> Clone(CreateName("totalMCnounc"));
  htotalMCnoUnc -> SetDirectory(nullptr);
  for (unsigned char xbin = 0; xbin < htotalMC -> GetNbinsX() + 1; xbin ++)
    {    
      projdeco -> _ratioframe -> SetBinContent(xbin, 1.0);
      const float val = htotalMC -> GetBinContent(xbin);
      htotalMCnoUnc -> SetBinError(xbin, 0.0);
      if (val == 0)
	{
	  continue;
	}
      const float totalUnc = htotalMCUnc -> GetBinError(xbin)/val; 
      const float totalUncShape = htotalMCUncShape -> GetBinError(xbin)/val; 
      projdeco -> _ratioframeshape -> SetBinContent(xbin, htotalMCUncShape -> GetBinContent(xbin)/val);
      projdeco -> _ratioframeshape -> SetBinError(xbin, totalUncShape);
      projdeco -> _ratioframe -> SetBinContent(xbin, htotalMCUnc -> GetBinContent(xbin)/val);
      projdeco -> _ratioframe -> SetBinError(xbin, totalUnc);
    }
  TPad * pad = new TPad(CreateName("pad2"), CreateTitle("pad2"), 0.0, 0.0, 1.0, 0.2);
  pad -> Draw();
  pad -> cd();
  projdeco -> _ratioframe -> Draw("E2");
  projdeco -> _ratioframeshape -> Draw("e2 same");
  if (recocode == RECO)
    {
      TH1F * ratio = level -> GetHU(DATA) -> Project(RECO, "datamp");
      ApplyScaleFactor(ratio);
      NormaliseToBinWidth(ratio);
      ratio -> Divide(htotalMCnoUnc);
      projdeco -> _ratiogr = new TGraphAsymmErrors(ratio);
      
      projdeco -> _ratiogr -> Draw("P");
      delete ratio;
    }
  printf("created ratio graph reco %u result %u\n", recocode, resultcode);
  projdeco -> Format();
  delete htotalMC;
  delete htotalMCnoUnc;
  return pad;
}
 
TCanvas * CompoundHistoUnfolding::CreateCombinedPlot(RecoLevelCode_t recocode, ResultLevelCode_t resultcode)
{
  TCanvas *c = new TCanvas(CreateName(TString("compound") + "_" + tag_recolevel[recocode] + "_" + tag_resultlevel[resultcode]), CreateTitle(TString("compound") + "_" + tag_recolevel[recocode] + "_" + tag_resultlevel[resultcode]), 500, 500);
  printf("probe CombinePlot A\n");
  TPad * p1 = CreateMainPlot(recocode, resultcode);
  printf("probe CombinePlot B\n");
  p1 -> SetRightMargin(0.05);
  p1 -> SetLeftMargin(0.12);
  p1 -> SetTopMargin(0.06);
  p1 -> SetBottomMargin(0.01);
  //p1 -> Draw();
  c -> cd();
  //  printf("probe B %f\n", _signalh -> _th2 -> Integral());

  TPad * p2 = CreateRatioGraph(recocode, resultcode);
  p2 -> SetBottomMargin(0.4);
  p2 -> SetRightMargin(0.05);
  p2 -> SetLeftMargin(0.12);
  p2 -> SetTopMargin(0.01);
  p2 -> SetGridx(kFALSE);
  p2 -> SetGridy(kTRUE);
   c -> SetBottomMargin(0.0);
  c -> SetLeftMargin(0.0);
  c -> SetTopMargin(0);
  c -> SetRightMargin(0.00);
  //p2 -> Draw();
  printf("created combined plot reco %u result %u\n", recocode, resultcode);
  c -> SaveAs(TString(_folder) + "/" + c -> GetName() +  ".png");
  //  printf("probe C %f\n", _signalh -> _th2 -> Integral());
  return c;
}

void CompoundHistoUnfolding::WriteHistograms()
{
  /*  for(vector<Histo *>::iterator it = _vsyshistosGEN_OUT.begin(); it != _vsyshistosGEN_OUT.end(); it ++)
    {
      (*it) -> _th1 -> Write();
    }
  _totalMCGEN_OUT -> Write();
  for(vector<HistoUnfolding *>::iterator it = _vdatahistos.begin(); it != _vdatahistos.end(); it ++)
    {
      (*it) -> _th2 -> Write();
    }
  for(vector<HistoUnfolding *>::iterator it = _vbackgroundhistos.begin(); it != _vbackgroundhistos.end(); it ++)
    {
      (*it) -> _th2 -> Write();
    }

  _signalhunfolded -> _th1 -> Write();
  */
}

void CompoundHistoUnfolding::SetFolder(const char * folder)
{
  
  sprintf(_folder, "%s", folder);
  //system(TString("rm -r ") + _folder);
  system(TString("mkdir - p ") + _folder);

}

void CompoundHistoUnfolding::SetSignalTag(const char * tag)
{
  //  sprintf(_signal_tag, "%s", tag);
}

void CompoundHistoUnfolding::SetOrigBinWidth(float orig)
{
  _orig_bin_width = orig;
}

float CompoundHistoUnfolding::GetOrigBinWidth() const
{
  return _orig_bin_width;
}


void CompoundHistoUnfolding::Format()
{
  for (map<TString, HistoUnfolding *> :: iterator it = aggrbackgroundMC.begin(); it != aggrbackgroundMC.end(); it ++)
    {
      //it -> second -> ls();
      it -> second -> GetTH2() -> SetFillColor(TColor::GetColor(it -> second -> GetColor()));
    }
  //_signalh -> ls();
}

CompoundHistoUnfolding::CompoundHistoUnfolding(): 
  _isRegular(false),
  _nbinsx(0),
  _xlow(0.0),
  _xup(0.0),
  _nbinsy(0),
  _ylow(0.0),
  _yup(0.0),
  _nentriesx(0),
  _xbins(nullptr),
  _nentriesy(0),
  _ybins(nullptr),
_size_sf(0),
  _sf(nullptr),
  totalbackground(nullptr)
  
{

}

CompoundHistoUnfolding::CompoundHistoUnfolding(const char* name, const char* title, Int_t nbinsx, const Float_t* xbins, Int_t nbinsy, const Float_t* ybins): 
_nbinsx(nbinsx), 
_nentriesx(nbinsx + 1), 
_xbins(xbins), 
_nbinsy(nbinsy), 
_nentriesy(nbinsy + 1), 
_ybins(ybins), 
_isRegular(false),
_xlow(0.0),
_xup(0.0),
_ylow(0.0),
_yup(0.0),
_size_sf(0),
_sf(nullptr),
totalbackground(nullptr)
{
  SetName(name);
  SetTitle(title);
}

CompoundHistoUnfolding::CompoundHistoUnfolding(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup): 
_nbinsx(nbinsx), 
_nentriesx(nbinsx + 1), 
_xlow(xlow), 
_xup(xup), _nbinsy(nbinsy), 
_nentriesy(nbinsy + 1), 
_ylow(ylow), 
_yup(yup), 
_isRegular(true),
_xbins(nullptr),
_ybins(nullptr),
_size_sf(0),
_sf(nullptr),
totalbackground(nullptr)
{
  SetName(name);
  SetTitle(title);
  _orig_bin_width = (xup - xlow)/nbinsx;

}

bool CompoundHistoUnfolding::IsRegular() const
{
  return _isRegular;
}

TString CompoundHistoUnfolding::CreateName(const char * name)
{
  return TString(GetName()) + "_" + name;
}

TString CompoundHistoUnfolding::CreateTitle(const char * title)
{
  return TString(GetName()) + " " + title;
}

void CompoundHistoUnfolding::Do()
{

  /*
  for(vector<HistoUnfolding *> :: iterator it = _vbackgroundhistos.begin(); it != _vbackgroundhistos.end(); it ++)
    {
      (*it) -> ls();
      TFile *f = TFile::Open(TString("$EOS/analysis/") + (*it) -> _tag + ".root");
      TH1F * hreco = f ? (TH1F*) f -> Get("L_pull_angle_allconst_reco_leading_jet_2nd_leading_jet_DeltaRTotal") : nullptr; 
      TH1F * hgen = f ? (TH1F*) f -> Get("L_pull_angle_allconst_gen_leading_jet_2nd_leading_jet_DeltaRTotal") : nullptr; 
      TH1F * px = (TH1F*)(*it) -> _th2 -> ProjectionX();
      TH1F * py = (TH1F*)(*it) -> _th2 -> ProjectionY();

      printf("integral %f projx %f projy %f hgen %p hreco %p\n", (*it) -> _th2 ->  Integral(), px -> Integral(), py -> Integral(), hgen, hreco);
      if (hreco)
	{
	  hreco -> Scale(luminosity * (*it) -> _xsec);
	  printf("hreco Integral %f\n", hreco -> Integral());
	  if (TMath::Abs(hreco -> Integral() - px -> Integral() > 5.0))
	    {
	      printf("attention reco\n");
	      getchar();
	    }

	}
      if (hgen)
	{
	  hgen -> Scale(luminosity * (*it) -> _xsec);
	  printf("hgen Integral %f \n", hgen -> Integral());
	  if (TMath::Abs(hgen -> Integral() - py -> Integral() > 5.0))
	    {
	      printf("attention gen\n");
	      getchar();
	    }

	}
      delete px;
      delete py;

      if (f) f -> Close();
    }
  
  for(vector<HistoUnfolding *> :: iterator it = _vdatahistos.begin(); it != _vdatahistos.end(); it ++)
    {
      (*it) -> ls();
      TFile *f = TFile::Open(TString("$EOS/analysis/") + (*it) -> _tag + ".root");
      TH1F * hreco = f ? (TH1F*) f -> Get("L_pull_angle_allconst_reco_leading_jet_2nd_leading_jet_DeltaRTotal") : nullptr; 
      TH1F * hgen = f ? (TH1F*) f -> Get("L_pull_angle_allconst_gen_leading_jet_2nd_leading_jet_DeltaRTotal") : nullptr; 
      TH1F * px = (TH1F*)(*it) -> _th2 -> ProjectionX();
      TH1F * py = (TH1F*)(*it) -> _th2 -> ProjectionY();

      printf("integral %f projx %f projy %f hgen %p hreco %p\n", (*it) -> _th2 ->  Integral(), px -> Integral(), py -> Integral(), hgen, hreco);
      if (hreco)
	{
	  printf("hreco Integral %f\n", hreco -> Integral());
	  if (TMath::Abs(hreco -> Integral() - px -> Integral() > 5.0))
	    {
	      printf("attention reco\n");
	      getchar();
	    }

	}
      if (hgen)
	{
	  printf("hgen Integral %f \n", hgen -> Integral());
	  if (TMath::Abs(hgen -> Integral() - py -> Integral() > 5.0))
	    {
	      printf("attention gen\n");
	      getchar();
	    }

	}
      delete px;
      delete py;

      if (f) f -> Close();
    }*/
  /*
  for(vector<HistoUnfolding *> :: iterator it = _vsignalhistos.begin(); it != _vsignalhistos.end(); it ++)
    {
      //(*it) -> ls();
      TFile *f = TFile::Open(TString("$EOS/analysis/") + (*it) -> _tag + ".root");
      TH1F * hreco = f ? (TH1F*) f -> Get("L_pull_angle_allconst_reco_leading_jet_2nd_leading_jet_DeltaRTotal") : nullptr; 
      TH1F * hgen = f ? (TH1F*) f -> Get("L_pull_angle_allconst_gen_leading_jet_2nd_leading_jet_DeltaRTotal") : nullptr; 
      TH1F * px = (TH1F*)(*it) -> _th2 -> ProjectionX();
      TH1F * py = (TH1F*)(*it) -> _th2 -> ProjectionY();

      printf("integral %f projx %f projy %f hgen %p hreco %p\n", (*it) -> _th2 ->  Integral(), px -> Integral(), py -> Integral(), hgen, hreco);
      if (hreco)
	{
	  hreco -> Scale(luminosity * (*it) -> _xsec);
	  printf("hreco Integral %f\n", hreco -> Integral());
	  if (TMath::Abs(hreco -> Integral() - px -> Integral() > 5.0))
	    {
	      printf("attention reco\n");
	      getchar();
	    }

	}
      if (hgen)
	{
	  hgen -> Scale(luminosity * (*it) -> _xsec);
	  printf("hgen Integral %f \n", hgen -> Integral());
	  if (TMath::Abs(hgen -> Integral() - py -> Integral() > 5.0))
	    {
	      printf("attention gen\n");
	      getchar();
	    }

	}
      delete px;
      delete py;

      if (f)  f -> Close();
    }
  for(vector<HistoUnfolding *> :: iterator it = _vsyshistos.begin(); it != _vsyshistos.end(); it ++)
    {
      (*it) -> ls();
      TFile *f = TFile::Open(TString("$EOS/analysis/") + (*it) -> _tag + ".root");
      TH1F * hreco = f ? (TH1F*) f -> Get("L_pull_angle_allconst_reco_leading_jet_2nd_leading_jet_DeltaRTotal") : nullptr; 
      TH1F * hgen = f ? (TH1F*) f -> Get("L_pull_angle_allconst_gen_leading_jet_2nd_leading_jet_DeltaRTotal") : nullptr; 
      TH1F * px = (TH1F*)(*it) -> _th2 -> ProjectionX();
      TH1F * py = (TH1F*)(*it) -> _th2 -> ProjectionY();

      printf("integral %f projx %f projy %f hgen %p hreco %p\n", (*it) -> _th2 ->  Integral(), px -> Integral(), py -> Integral(), hgen, hreco);
      if (hreco)
	{
	  hreco -> Scale(luminosity * (*it) -> _xsec);
	  printf("hreco Integral %f\n", hreco -> Integral());
	  if (TMath::Abs(hreco -> Integral() - px -> Integral() > 5.0))
	    {
	      printf("attention reco\n");
	      getchar();
	    }

	}
      if (hgen)
	{
	  hgen -> Scale(luminosity * (*it) -> _xsec);
	  printf("hgen Integral %f \n", hgen -> Integral());
	  if (TMath::Abs(hgen -> Integral() - py -> Integral() > 5.0))
	    {
	      printf("attention gen\n");
	      getchar();
	    }

	}
      delete px;
      delete py;

      if (f)f -> Close();
    }
  */
}
