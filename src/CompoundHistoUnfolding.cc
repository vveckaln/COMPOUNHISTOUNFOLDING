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
const unsigned char nbckg = 6;
const char * bckgtitles[nbckg] = {"QCD", "t#bar{t}+V", "Multiboson", "DY", "W", "Single top"};
ClassImp(CompoundHistoUnfolding);

float calculatearea(TH1 *h)
{
  float area(0.0);
  for (unsigned char bind = 1; bind <= h -> GetNbinsX(); bind ++)
    {
      area += h -> GetBinContent(bind) * h -> GetBinWidth(bind);
    }
  return area;
}

void CompoundHistoUnfolding::Process()
{
  ScaleFactor();
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
  CreateDataGraph(OUT, RECO);
  CreateDataGraph(IN, RECO);
  CreateDataGraph(OUT, GEN);

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
  CreateDataMinusBackground();
  TCanvas cmigration("cmigration", "cmigration");
  cmigration.SetRightMargin(0.18);

  TH2F * hsignal = GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2();
 
  hsignal -> Draw("COLZ");
  cmigration.SaveAs(TString(_folder) + "/migrationmatrix.png");
  TUnfoldDensity unfold(hsignal, TUnfold::kHistMapOutputVert, TUnfold::kRegModeCurvature, TUnfold::kEConstraintArea, TUnfoldDensity::kDensityModeBinWidthAndUser);
  input = /*(TH1F*) _signalh -> _th2 -> ProjectionX(); */GetLevel(IN) -> GetHU(DATAMBCKG) -> Project(RECO, CreateName("input"));
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
  unfold.DoUnfold(0.0);
  TCanvas *coutput = new TCanvas(CreateName("coutput"), CreateTitle("coutput"));
  THStack *coutput_stack = new THStack(CreateName("output"), CreateTitle("output"));
  TH1 * houtput = unfold.GetOutput(CreateName("output"));
  houtput -> SetDirectory(nullptr);
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
  if ( fN != 0 ) {
    if ( h2Array->fN != fN ) {
      printf("DifferentBinLimits %u %u\n", fN, h2Array -> fN);
    }
    else {
      for ( int i = 0; i < fN; ++i ) {
	if ( ! TMath::AreEqualRel( h1Array->GetAt(i), h2Array->GetAt(i), 1E-10 ) ) {
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
  for (vector<HistoUnfolding *>::iterator it = GetLevel(IN) -> GetV(SYSMO) -> begin(); it != GetLevel(IN) -> GetV(SYSMO) -> end(); it ++)
    {
      TUnfoldDensity unfold((*it) -> GetTH2(), TUnfold::kHistMapOutputVert, TUnfold::kRegModeCurvature, TUnfold::kEConstraintArea, TUnfoldDensity::kDensityModeUser);
      //      (*it) -> ls();
      unfold.SetInput(input);
      unfold.DoUnfold(0.0);
      HistoUnfoldingTH1 * out = new HistoUnfoldingTH1((SampleDescriptor * ) * it);
      GetLevel(OUT) -> GetV(SYSMO) -> push_back(out);
      out -> GetTH1Ref(RECO) = (TH1F*) unfold.GetFoldedOutput(CreateName(TString((*it) -> GetTag()) + "_folded_output"));
      out -> GetTH1Ref(GEN) = (TH1F*) unfold.GetOutput(CreateName(TString((*it) ->GetTag()) + "_output"));
    }
  GetLevel(OUT) -> GetHURef(SIGNALMO) = GetLevel(IN) -> GetHU(SIGNALMO); 
  
}
void CompoundHistoUnfolding::LoadHistos(const char * json, SysTypeCode_t sys)
{
  const char * sampletag = sys != EXPSYS ? "MC13TeV_TTJets" : signaltag;
  char * temp = nullptr;
  if (TString(method) == "dire2002" or TString(method) == "sherpa" or TString(method) == "herwig7")
    {
      temp = new char[128];
      sprintf(temp, "%s",  (TString(sampletag) + "_" + method).Data());
      sampletag = temp;
    }
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
      if (TString(method) == "nominal" and TString(parser.GetSample(sind) -> GetTag()) == "MC13TeV_TTJets_cflip")
	continue;
      if (TString(parser.GetSample(sind) -> GetTag()) == TString(signaltag))
	parser.GetSample(sind) -> SetSysType(NOMINAL);
      else if (calculate_bins)
	continue;
      if (TString(parser.GetSample(sind) -> GetTag()) == "MC13TeV_TTJets" and TString(signaltag) != "MC13TeV_TTJets")
	parser.GetSample(sind) -> SetSysType(THEORSYS);
      if (sys == EXPSYS)
	parser.GetSample(sind) -> SetTag(TString(parser.GetSample(sind) -> GetTag()).ReplaceAll("MC13TeV_TTJets", signaltag));
      if (IsRegular())
	{
	  h = new HistoUnfoldingTH2(GetName(), _XaxisTitle, _nbinsx, _xlow, _xup, _nbinsy, _ylow, _yup, parser.GetSample(sind)); 
	  h -> GetTH2() -> GetZaxis() -> SetTitle("N");
	  h -> GetTH2() -> RebinY(2);
	}
      else
	{
	  h = new HistoUnfoldingTH2(GetName(), _XaxisTitle, _nentriesx - 1, _xbins, _nentriesy - 1, _ybins, parser.GetSample(sind));
	}
      TChain chain("migration");
      printf("tag %s \n", h-> GetTag());
      chain.Add(TString("root://eosuser.cern.ch//$EOS/analysis_") + sampletag + ("/HADDmigration/migration_") + h -> GetTag() + ".root/E_" + tag_charge_types_[chargecode] + "_" + tag_jet_types_[jetcode] + "_" + "migration");
      chain.Add(TString("root://eosuser.cern.ch//$EOS/analysis_") + sampletag + ("/HADDmigration/migration_") + h -> GetTag() + ".root/M_" + tag_charge_types_[chargecode] + "_" + tag_jet_types_[jetcode] + "_" + "migration");
      Float_t reco;
      Float_t gen;
      vector<double> * weights = nullptr;
      chain.SetBranchAddress(TString(observable) + "_reco", & reco);
      chain.SetBranchAddress(TString(observable) + "_gen",  & gen);
      chain.SetBranchAddress("weight",          & weights);
      const unsigned int dbcut = 1;
      //      h -> FillFromTree(, tag_jet_types_[jetcode], tag_charge_types_[chargecode], observable);
      for (unsigned long event_ind = 0; event_ind < chain.GetEntries()/dbcut; event_ind ++)
	{
	  if (event_ind % 10000 == 0)
	    printf("%u\r", event_ind);
	  chain.GetEntry(event_ind);
	  if (string(observable).compare("pull_angle") == 0)
	    h -> GetTH2() -> Fill(reco == -10 ? reco : TMath::Abs(reco)/TMath::Pi(), gen == -10.0 ? gen : TMath::Abs(gen)/TMath::Pi(), (*weights)[0]);
	  /*      if (string(GetTag()).compare("MC13TeV_TTJets") == 0)
		  {
		  printf("%lu %f\n", event_ind, reco);
		  getchar();
		  }*/
	  if (string(observable).compare("pvmag") == 0)
	    h -> GetTH2() -> Fill(reco, gen, (*weights)[0]);
	  //      _th2 -> Fill(pull_angle_reco == -10 ? pull_angle_reco : pull_angle_reco, pull_angle_gen == -10.0 ? pull_angle_gen : pull_angle_gen, weight);
	  
	}
      printf("Filled %lu events\n", chain.GetEntries());
      AddHisto(h);
      // if (TString(parser.GetSample(sind) -> GetTag()) == "MC13TeV_TTJets_herwig")
      // 	{
      // 	  printf("~~~~~~~~~~~~ after loading MC13TeV_TTJets_herwig\n");
      // 	  h -> GetTH2() -> ProjectionX("test") -> Print ("all");
      // 	  getchar();
      // 	}
      bool nondedicated_end =  false;
      
      if (string(parser.GetSample(sind)->GetTag()).compare(signaltag) == 0 and not calculate_bins)
	{
	  vector<HistoUnfoldingTH2*> nondedicatedth2d;
	  static const unsigned char Nnondedicated = 20;
	  static const char * nondedicated_titles[Nnondedicated] = {"pileup up", "pileup down",
								    "trig efficiency correction up", "trig efficiency correction down",
								    "sel efficiency correction up", "sel efficiency correction down",
								    "b fragmentation up", "b fragmentation down",
								    "Peterson Frag", 
								    "semilep BR up", "semilep BR down", 
								    "id1002muR1muF2hdampmt272.7225", "id1003muR1muF0.5hdampmt272.7225", "id1004muR2muF1hdampmt272.7225", "id1005muR2muF2hdampmt272.7225", "id1007muR0.5muF1hdampmt272.7225", "id1009muR0.5muF0.5hdampmt272.7225", "id1010", "", ""};
	  static const char * nondedicated_names[Nnondedicated] = {"pileup_up", "pileup_down",
								   "trig_efficiency_correction_up", "trig_efficiency_correction_down",
								   "sel_efficiency_correction_up", "sel_efficiency_correction_down",
								   "b_fragmentation_up", "b_fragmentation_down",
								   "Peterson_frag", 
								   "semilep_BR_up", "semilep_BR_down", 
								   "id1002muR1muF2hdampmt272.7225", "id1003muR1muF0.5hdampmt272.7225", "id1004muR2muF1hdampmt272.7225", "id1005muR2muF2hdampmt272.7225", "id1007muR0.5muF1hdampmt272.7225", "id1009muR0.5muF0.5hdampmt272.7225", "id1010", "", ""};
	  for (unsigned char nondedicated_ind = 0; nondedicated_ind < Nnondedicated and not nondedicated_end; nondedicated_ind ++)
	    {
	      printf("%s\n", nondedicated_names[nondedicated_ind]);
	      HistoUnfoldingTH2 *h = nullptr;
	      if (IsRegular())
		{
		  h = new HistoUnfoldingTH2(GetName(), _XaxisTitle, _nbinsx, _xlow, _xup, _nbinsy, _ylow, _yup, parser.GetSample(sind)); 
		  h -> GetTH2() -> GetZaxis() -> SetTitle("N");
		  h -> GetTH2() -> RebinY(2);
		}
	      else
		{
		  h = new HistoUnfoldingTH2(GetName(), _XaxisTitle, _nentriesx - 1, _xbins, _nentriesy - 1, _ybins, parser.GetSample(sind));
		}
	      h -> SetTag(TString(h -> GetTag()) + nondedicated_names[nondedicated_ind]);
	      h -> GetTH2() -> SetName(nondedicated_names[nondedicated_ind]);
	      h -> GetTH2() -> SetTitle(nondedicated_titles[nondedicated_ind]);
	      h -> SetTitle((TString("t#bar{t} ") + nondedicated_titles[nondedicated_ind]).Data());
	      h -> SetSysType(EXPSYS);

	      for (unsigned long event_ind = 0; event_ind < chain.GetEntries()/dbcut; event_ind ++)
		{
		  if (event_ind % 10000 == 0)
		    {
		    printf("%u\r", event_ind);
		    }
		  chain.GetEntry(event_ind);
		  if (nondedicated_ind + 1 > weights -> size() - 1)
		    {
		      nondedicated_end = true;
		      delete h;
		      h = nullptr;
		      printf("breaking\n");
		      
		      break;
		    }
		  if (string(observable).compare("pull_angle") == 0)
		    {
		      //		      printf("%f nondedicated_ind %u\n",  (*weights)[nondedicated_ind + 1], nondedicated_ind);
		      h -> GetTH2() -> Fill(reco == -10 ? reco : TMath::Abs(reco)/TMath::Pi(), gen == -10.0 ? gen : TMath::Abs(gen)/TMath::Pi(), (*weights)[0] * (*weights)[nondedicated_ind + 1]);
		    }
		  /*      if (string(GetTag()).compare("MC13TeV_TTJets") == 0)
			  {
			  printf("%lu %f\n", event_ind, reco);
			  getchar();
			  }*/
		  if (string(observable).compare("pvmag") == 0)
		    h -> GetTH2() -> Fill(reco, gen, (*weights)[0] * (*weights)[nondedicated_ind + 1]);
		  //      _th2 -> Fill(pull_angle_reco == -10 ? pull_angle_reco : pull_angle_reco, pull_angle_gen == -10.0 ? pull_angle_gen : pull_angle_gen, weight);

		}
	      if (h)
		{
		  AddHisto(h);
		  
		  // printf("printing nondedicated %s\n", h -> GetTH2() -> GetName());
		  // TH1 * hp = h -> GetTH2() -> ProjectionX();
		  // hp -> Print("all");
		  // delete hp;
		  // getchar();
		}
	      
	    }
	}
      parser.UnsetSample(sind);
    }
  // TH1F * testsignal = GetLevel(IN) -> GetHU(SIGNALPROXY, IN) -> Project(RECO, "testsignal");
  // printf("printing reco signal\n");
  // testsignal -> Print("all");
  // TH1F * testdata = GetLevel(IN) -> GetHU(DATAMO, IN) -> Project(RECO, "testdata");
  // printf("printing data\n");
  // testdata -> Print("all");
  // getchar();
  // delete testsignal;
  // delete testdata;
    if (TString(method) == "dire2002" or TString(method) == "sherpa" or TString(method) == "herwig7")
    {
      delete [] temp;
    }
}
void CompoundHistoUnfolding::ApproximateTheorSys()
{
  if (string(signaltag).compare("MC13TeV_TTJets") == 0)
    return;
  TH2F * hMC13TeV_TTJets = _chunominal -> GetLevel(IN) -> GetHU(SIGNALMO) -> GetTH2();
  for (vector<HistoUnfolding *>::iterator it = GetLevel(IN) -> _vsyshistos.begin(); it != GetLevel(IN) -> _vsyshistos.end(); it ++)
    {
      if ((*it) -> GetSysType() != THEORSYS)
	continue;
      if (TString((*it) -> GetTag()) == "MC13TeV_TTJets")
	{
	  continue;
	}
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
	  h -> GetTH2() -> SetTitle(histo -> GetTitle());
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
      // TH1F * totalMCtest = totalMC -> Project(RECO, "test");
      // printf("printing totalMC\n");
      // totalMCtest -> Print("all");
      //getchar();
      // delete totalMCtest;
    }
  if (resultcode == OUT)
    {
      totalMC = new HistoUnfoldingTH1();
      totalMC -> SetTitle("MCTotal");
      totalMC -> GetTH1Ref(RECO) = GetLevel(resultcode) -> GetHU(SIGNALPROXY, OUT) -> Project(RECO, CreateName("MCtotalOUTRECO"));
      TH1F *bckgsfRECO = totalbackground -> Project(RECO);
      //ApplyScaleFactor(bckgsfRECO);
      totalMC -> GetTH1(RECO) -> Add(bckgsfRECO);
      GetLevel(resultcode) -> GetHURef(DATAMO)  = new HistoUnfoldingTH1((HistoUnfoldingTH1*) GetLevel(resultcode) -> GetHU(DATAMBCKG), "data_out");
      GetLevel(resultcode) -> GetHU(DATAMO) -> GetTH1(RECO) -> Add(bckgsfRECO);
      GetLevel(resultcode) -> GetHU(DATAMO) -> GetTH1(RECO) -> SetTitle(TString("data (") + 
									GetLevel(IN) -> GetHU(SIGNALMO) -> GetTitle() + 
									") folded back + bckg");
      delete bckgsfRECO;
      totalMC -> GetTH1Ref(GEN) = GetLevel(resultcode) -> GetHU(SIGNALPROXY, OUT) -> Project(GEN, CreateName("MCtotalOUTGEN"));
      TH1F *bckgGEN = totalbackground -> Project(GEN);
      totalMC -> GetTH1(GEN) -> Add(bckgGEN);
      GetLevel(resultcode) -> GetHU(DATAMO) -> GetTH1(GEN) -> Add(bckgGEN);
      delete bckgGEN;
    }
}



void CompoundHistoUnfolding::CreateDataMinusBackground(ResultLevelCode_t resultcode)
{
  
  HistoUnfolding *& datambackground = GetLevel(resultcode) -> GetHURef(DATAMBCKG);
  if (datambackground)
    return;
  datambackground = new HistoUnfoldingTH2();
  datambackground -> SetTitle("data - bckg");
  datambackground -> SetName("data_minus_bckg");
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
  TH1 *& htotalMCUnc = GetLevel(resultcode) -> GetHU(mo) -> GetTH1Ref(recocode);
  htotalMCUnc = GetLevel(resultcode) -> GetHU(TOTALMCSIGNALPROXY, resultcode) -> Project(recocode, CreateName(shape ? "totalMCUncShape" : "totalMCUnc"), "", _YaxisTitle);

  const unsigned char nbins = htotalMCUnc -> GetNbinsX();

  /*  if (recocode == RECO and resultcode == IN)
      ApplyScaleFactor(htotalMCUnc);*/
  TH1F * hsig = GetLevel(resultcode) -> GetHU(SIGNALPROXY, resultcode) -> Project(recocode, CreateName("sig"));
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
  for (vector<HistoUnfolding *>::iterator it = GetLevel(resultcode) -> GetV(SYSMO) -> begin(); it != GetLevel(resultcode) -> GetV(SYSMO) -> end(); it ++)
    {
      TH1F* hsys = (*it) -> Project(recocode);
      // if (resultcode == IN and  recocode == RECO and TString((*it) -> GetTag()) == "MC13TeV_TTJets_herwig")
      // 	{
      // 	  printf("~~~~~~~~~~~~ when calculating unc MC13TeV_TTJets_herwig\n");
      // 	  hsys -> Print ("all");
      // 	  printf ("integralnominal %f hsys -> Integral() %f\n", integralnominal, hsys -> Integral());
      // 	  getchar();
      // 	}
      //      (*it) -> ls();
      if (shape)
	{
	  const double integral = hsys -> Integral();
	  hsys -> Scale(integralnominal/integral);
	}
      //    (*it) -> ls();
      for (unsigned char bin_ind = 0; bin_ind < nbins + 2; bin_ind ++)
	{
	  const float diff = hsys -> GetBinContent(bin_ind) - hsig -> GetBinContent(bin_ind);
	  // if (not shape and resultcode == IN and recocode == RECO)
	  //   printf("%f %f %f", diff, hsys -> GetBinContent(bin_ind), hsig -> GetBinContent(bin_ind));
	  if (bin_ind == 1 and not shape)
	    {
	      float diffnom = hsys -> GetBinContent(bin_ind)/hsys -> Integral() - hsig -> GetBinContent(bin_ind)/hsig -> Integral();
	      diffnom = fabs(diffnom) * hsig -> Integral()/hsig -> GetBinContent(bin_ind) * 100;
	      //      printf("uncertainty %s diff %f %\n", (*it) -> GetTag(), diffnom);
	      fprintf(uncfilefull, "%s,\t%.9f\t%.9f", (*it) -> GetTitle(), hsys -> GetBinContent(bin_ind)/hsys -> Integral(), hsig -> GetBinContent(bin_ind)/hsig -> Integral());
	   
	    }
	  if (bin_ind > 1 and bin_ind < nbins + 1 and not shape)
	    {
	      fprintf(uncfilefull, "\t%.9f\t%.9f", hsys -> GetBinContent(bin_ind)/hsys -> Integral(), hsig -> GetBinContent(bin_ind)/hsig -> Integral());
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
    }
  delete hsig;
  //  NormaliseToBinWidth(htotalMCUnc);
  htotalMCUnc -> SetName(CreateName(TString("unc_") + tag_resultlevel[resultcode] + "_" + tag_recolevel[recocode] + (shape ? "_shape" : "")));
  htotalMCUnc -> SetTitle(CreateTitle(TString("unc ") + tag_resultlevel[resultcode] + " " + tag_recolevel[recocode] + (shape ? " shape" : "")));
  // printf("final");
  // htotalMCUnc -> Print("all");
  // printf("normalised\n");
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
      for (vector<HistoUnfolding *>::iterator it = GetLevel(resultcode) -> GetV(SYSMO) -> begin(); it != GetLevel(resultcode) -> GetV(SYSMO) -> end(); it ++)
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
  for (vector<HistoUnfolding *>::iterator it = _vbackgroundhistos.begin(); it !=  _vbackgroundhistos.end(); it ++)
    {
      ApplyScaleFactor((*it) -> GetTH2());
    }
  ApplyScaleFactor(GetLevel(IN) -> GetHU(DATA) -> GetTH2());
}


void CompoundHistoUnfolding::ApplyScaleFactor(TH1 *h)
{
  for (unsigned char ind = 0; ind < _size_sf; ind ++)
    {
      h -> SetBinContent(ind, (1 - _sf[ind]) * h -> GetBinContent(ind));
      h -> SetBinError(ind, (1 - _sf[ind]) * h -> GetBinError(ind));
    }
}

void CompoundHistoUnfolding::ApplyScaleFactor(TH2 *h)
{
  for (unsigned char ind = 0; ind < _size_sf; ind ++)
    {
      h -> SetBinContent(ind, 0, (1 - _sf[ind]) * h -> GetBinContent(ind));
      h -> SetBinError(ind, 0, (1 - _sf[ind]) * h -> GetBinError(ind));
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
  legend = new TLegend(0.3, 0.47, 0.94, 0.93);
  if (projdeco -> _datagr)
    {
      legend -> AddEntry(projdeco -> _datagr, resultcode == IN ? "data" : level -> GetHU(DATAMO) -> GetTH1(recocode) -> GetTitle());
    }
  TH1F * hsig = GetLevel(IN)/*level*/ -> GetHU(SIGNAL/*SIGNALPROXY*/, resultcode) -> Project(recocode, CreateName("sigstack"), "", _YaxisTitle);
  TH1F * htotalmc = level -> GetHU(TOTALMCSIGNALPROXY, resultcode) -> Project(recocode, CreateName("totalmc"), "", _YaxisTitle);
  NormaliseToBinWidth(htotalmc);
  NormaliseToBinWidth(hsig);
  const float totalmcarea(calculatearea(htotalmc));
  hsig -> Scale(1.0/totalmcarea);
  char legendtitle[128];
  if (recocode == RECO)
    sprintf(legendtitle, "%s", level -> GetHU(SIGNALMO) -> GetTitle());
  else
    {
      if (string(method).compare("nominal") == 0)
	sprintf(legendtitle, "%s", "POWHEG+PYTHIA8");
      else if (string(method).compare("cflip"))
	sprintf(legendtitle, "%s", "POWHEG+PYTHIA8 cflip");
      else
	sprintf(legendtitle, "%s", level -> GetHU(SIGNALMO) -> GetTitle());
    }
  hsig -> SetLineWidth(2.0);
    legend -> AddEntry(hsig, legendtitle, "f");

  THStack * stack = new THStack("mc", "mc");
  TH1F * hbackgrounds[nbckg];
  for (unsigned char bin_ind = 0; bin_ind < nbckg; bin_ind ++)
    {
      HistoUnfolding * it = aggrbackgroundMC[bckgtitles[bin_ind]];
      TH1F * hbackground = it -> Project(recocode, CreateName(TString(it -> GetTag()) + "background"), "", _YaxisTitle);
      hbackgrounds[bin_ind] = hbackground;
      /*if (recocode == RECO)
	ApplyScaleFactor(hbackground);*/
      NormaliseToBinWidth(hbackground);
      hbackground -> Scale(1.0/totalmcarea);
      stack -> Add(hbackground);
      //delete hbackground;
    }
  for (char bin_ind = nbckg - 1; bin_ind > -1; bin_ind --)
    {
      
      TH1F * hbackground = hbackgrounds[bin_ind];
      if (hbackground -> Integral() > 0.0)

	legend -> AddEntry(hbackground, bckgtitles[bin_ind], "f");
    }  
  stack -> Add(hsig);
  delete htotalmc;
  return stack;
}
 

   
TPad * CompoundHistoUnfolding::CreateMainPlot(RecoLevelCode_t recocode, ResultLevelCode_t resultcode)
{ 

  Format();
  Level * level = GetLevel(resultcode);
  level -> Format();
  Level::ProjectionDeco * projdeco = level -> GetProjectionDeco(recocode);
  THStack * stack = CreateTHStack(recocode, resultcode);
  //TH1F * totalMC = level -> GetHU(TOTALMC) -> Project(recocode, CreateName("totalMCmp"));
  //  TH1F * data = nullptr;
  TH1F * htotalmc = level -> GetHU(TOTALMC) -> Project(recocode, CreateName("totalmc"), "", _YaxisTitle);
  NormaliseToBinWidth(htotalmc);
  const double totalmcarea(calculatearea(htotalmc));
  TH1 * totalMCUnc = (TH1 *) level -> GetHU(TOTALMCUNC) -> GetTH1(recocode) -> Clone(TString(level -> GetHU(TOTALMCUNC) -> GetTH1(recocode) -> GetName()) + "drawingclone");
  totalMCUnc -> SetDirectory(nullptr);
  _garbage.push_back(totalMCUnc);
  NormaliseToBinWidth(totalMCUnc);
  totalMCUnc -> Scale(1.0/calculatearea(totalMCUnc));
  //  totalMCUnc -> Print("all");
  TH1 * totalMCUncShape = (TH1 *) level -> GetHU(TOTALMCUNCSHAPE) -> GetTH1(recocode) -> Clone(TString(level -> GetHU(TOTALMCUNCSHAPE) -> GetTH1(recocode) -> GetName()) + "drawingclone");
  totalMCUncShape -> SetDirectory(nullptr);
  _garbage.push_back(totalMCUncShape);
  //printf("$$$$$$$$$$$$$ unc %p uncshape %p\n", totalMCUnc, totalMCUncShape);
  NormaliseToBinWidth(totalMCUncShape);
  totalMCUncShape -> Scale(1.0/calculatearea(totalMCUncShape));
  //TH1F * hoverlay = nullptr;
  TLegend * legend = projdeco -> legend;
  /*if (recocode == RECO and resultcode == IN)
    ApplyScaleFactor(totalMC);*/
  /*if (resultcode == OUT )
    {
      hoverlay = GetLevel(IN) -> GetHU(SIGNALMO) -> Project(recocode, CreateName("overlay"), "", _YaxisTitle);
      hoverlay -> SetLineColor(kRed);
      TH1F *bckgsf = (TH1F*) totalbackground  -> Project(recocode, CreateName("bckgsf"), "", _YaxisTitle);
      //ApplyScaleFactor(bckgsf);
      hoverlay -> Add(bckgsf);
      NormaliseToBinWidth(hoverlay);
      delete bckgsf;
      }*/
  //NormaliseToBinWidth(totalMC);
  /*if (resultcode == OUT)
    {
      projdeco -> _grtotalMC = new TGraphAsymmErrors(totalMC);
    }*/
  double maximum = 0.0;
  if (recocode == RECO)
    {
      const double grmaximum = TMath::MaxElement(projdeco -> _datagr -> GetN(), projdeco -> _datagr -> GetY());
      maximum = stack -> GetMaximum() > grmaximum ? 2.5 * stack -> GetMaximum() : 2.5 * grmaximum;
      
    }
  else
    maximum = 1.85 *stack -> GetMaximum();
  // stack -> SetMaximum(maximum);
  // stack -> SetMinimum(0.8 * stack -> GetMinimum();
  TPad *pad = new TPad(CreateName("pad1"), CreateTitle("pad1"), 0.0, 0.2, 1.0, 1.0);
  pad -> Draw();
  pad -> cd();
  TH1 * hframe = (TH1 *) ((TH1*)stack -> GetHists() -> At(0)) -> Clone(CreateName("hframe"));
  hframe -> Reset("ICE");
  hframe -> SetMaximum(maximum);
  //stack -> SetMaximum(maximum);
  if (TString(observable) == "pull_angle" and recocode == GEN)
    hframe -> SetMinimum(0.7 * totalMCUnc -> GetMinimum());
  // hframe -> GetYaxis() -> SetLabelSize(0.20);
  // hframe -> GetYaxis() -> SetTitleSize(0.21);
  // hframe -> GetYaxis() -> SetTitleOffset(0.35);
  // hframe -> GetXaxis() -> SetLabelSize(0.20);
  // hframe -> GetXaxis() -> SetTitleSize(0.21);
  hframe -> GetXaxis() -> SetLabelSize(0.0);
  hframe -> GetYaxis() -> SetLabelSize(0.20*0.2/0.8);
  hframe -> GetYaxis() -> SetTitleSize(0.21*0.2/0.8);
  
  hframe -> GetYaxis() -> SetTitleOffset(0.30*0.8/0.2);
  hframe -> Draw();
  stack -> Draw("SAMEHISTO");
  /*  stack -> GetXaxis() -> SetTitle("");
      stack -> GetYaxis() -> SetTitle(_YaxisTitle);*/
  TH1 *h = nullptr;
  TIter next(stack -> GetHists());
  /*  if (hoverlay)
    {
      hoverlay -> Draw("SAME");
      legend -> AddEntry(hoverlay, hoverlay -> GetTitle());
      }*/

  totalMCUnc -> Draw("E2SAME");
  totalMCUnc -> Print("all");
  totalMCUncShape -> Draw("E2SAME");
  if (projdeco -> _datagr)
    {
      projdeco -> _datagr -> Print("all");
      const unsigned int n = projdeco -> _datagr -> GetN();
      double ey;
      for (int i = 0; i < n; i ++) 
	{
	  ey = projdeco -> _datagr -> GetErrorY(i);
	  projdeco -> _datagr -> SetPointError(i, 0, ey);
	}  
      projdeco -> _datagr -> Draw("EP");
    }
  legend -> Draw("SAME");
  projdeco -> Format();
  //  delete totalMC;
  TLatex * txt = new TLatex();
  txt -> SetNDC(kTRUE);
  txt -> SetTextFont(42);
  txt -> SetTextSize(0.06);
  txt -> SetTextAlign(12);
  const float inixlumi = 0.7;
  char text[128];
  sprintf(text, "#scale[0.8]{%3.1f fb^{-1} (%s)}", _luminosity/1000.0, _com);
  txt -> DrawLatex(inixlumi, 0.97, text);
  return pad;
}


void CompoundHistoUnfolding::CreateDataGraph(ResultLevelCode_t resultcode, RecoLevelCode_t recocode)
{
  if (recocode == GEN and resultcode == IN)
    return;
  Level * level = GetLevel(resultcode);
  Level::ProjectionDeco * projdeco = level -> GetProjectionDeco(recocode);
  if (projdeco -> _datagr)
    return;

  TH1F * data = level -> GetHU(DATAMO) -> Project(recocode, CreateName("datadg"), "", _YaxisTitle);
  //ApplyScaleFactor(data);
  NormaliseToBinWidth(data);
  data -> Scale(1.0/calculatearea(data));
  projdeco -> _datagr = new TGraphErrors(data);
  projdeco -> _datagr -> GetXaxis() -> SetTitle("");
  projdeco -> _datagr -> GetYaxis() -> SetTitle(_YaxisTitle);
  const unsigned int n = projdeco -> _datagr -> GetN();
  double ey;
  for (int i = 0; i < n; i ++) 
    {
      ey = projdeco -> _datagr -> GetErrorY(i);
      projdeco -> _datagr -> SetPointError(i, 0, ey);
    }  
// printf("datagr\n");
  // data -> Print("all");
  // getchar();
  delete data;
}

 
TPad * CompoundHistoUnfolding::CreateRatioGraph(RecoLevelCode_t recocode, ResultLevelCode_t resultcode)
{
  TPad * pad = new TPad(CreateName("pad2"), CreateTitle("pad2"), 0.0, 0.0, 1.0, 0.2);
  pad -> Draw();
  pad -> cd();
  
  Level * level = GetLevel(resultcode);
  Level::ProjectionDeco * projdeco = level -> GetProjectionDeco(recocode);
  if (not projdeco -> _ratioframe)
    {
      TH1 * htotalMC = level -> GetHU(TOTALMCSIGNALPROXY, resultcode) -> Project(recocode, CreateName("totalmcrg"), _XaxisTitle, "MC/data");
      TH1 * htotalMCUnc = level -> GetHU(TOTALMCUNC) -> GetTH1(recocode);
      TH1 * htotalMCUncShape = level -> GetHU(TOTALMCUNCSHAPE) -> GetTH1(recocode);
      /*if (recocode == RECO and resultcode == IN)
	ApplyScaleFactor(htotalMC);*/
      //NormaliseToBinWidth(htotalMC);
      projdeco -> _ratioframe = (TH1F*) htotalMC -> Clone(CreateName("ratioframe"));
      projdeco -> _ratioframe -> SetDirectory(nullptr);
      projdeco -> _ratioframe -> Reset("ICE");
      
      projdeco -> _ratioframe -> GetYaxis() -> SetRangeUser(0.7, 1.3);
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
      if (recocode == RECO and resultcode == IN or resultcode == OUT)
	{
	  TH1F * ratio = level -> GetHU(TOTALMCSIGNALPROXY, resultcode) -> Project(recocode, "datamp");
	  TH1F * data = level -> GetHU(DATAMO) -> Project(recocode, CreateName("datadg"), "", _YaxisTitle);
	  //ApplyScaleFactor(ratio);
	  //NormaliseToBinWidth(ratio);
	  ratio -> Divide(data);
	  projdeco -> _ratiogr = new TGraphAsymmErrors(ratio);
	  delete data;
	  delete ratio;
	}
      delete htotalMC;
      delete htotalMCnoUnc;

    }
  projdeco -> _ratioframe -> Draw("E2");
  projdeco -> _ratioframeshape -> Draw("e2 same");
  if (projdeco -> _ratiogr)
    projdeco -> _ratiogr -> Draw("PSAME");
  /*  if (resultcode == OUT and recocode == GEN)
    {
      TH1 * hoverlay = GetLevel(IN) -> GetHU(SIGNALMO) -> Project(GEN);
      hoverlay -> Divide(GetLevel(OUT) -> GetHU(TOTALMC) -> GetTH1(GEN));
      hoverlay -> SetLineColor(kRed);
      hoverlay -> Draw("SAME");
      }*/
  projdeco -> Format();
  return pad;
}
 
TCanvas * CompoundHistoUnfolding::CreateCombinedPlot(RecoLevelCode_t recocode, ResultLevelCode_t resultcode)
{
  TCanvas *c = new TCanvas(CreateName(TString("compound") + "_" + tag_recolevel[recocode] + "_" + tag_resultlevel[resultcode]), CreateTitle(TString("compound") + "_" + tag_recolevel[recocode] + "_" + tag_resultlevel[resultcode]), 500, 500);
  TPad * p1 = CreateMainPlot(recocode, resultcode);
  p1 -> SetRightMargin(0.05);
  p1 -> SetLeftMargin(0.16);
  p1 -> SetTopMargin(0.06);
  p1 -> SetBottomMargin(0.01);
  //p1 -> Draw();
  c -> cd();
  //  printf("probe B %f\n", _signalh -> _th2 -> Integral());
  
  TPad * p2 = CreateRatioGraph(recocode, resultcode);
  p2 -> SetBottomMargin(0.5);
  p2 -> SetRightMargin(0.05);
  p2 -> SetLeftMargin(0.16);
  p2 -> SetTopMargin(0.01);
  p2 -> SetGridx(kFALSE);
  p2 -> SetGridy(kTRUE);
  c -> SetBottomMargin(0.0);
  c -> SetLeftMargin(0.0);
  c -> SetTopMargin(0);
  c -> SetRightMargin(0.00);
  //p2 -> Draw();
  //  c -> SaveAs(TString(_folder) + "/" + c -> GetName() +  ".png");
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
  sprintf(signaltag, "%s", tag);
}

void CompoundHistoUnfolding::SetMethod(const char * method)
{
  sprintf(this -> method, "%s", method);
}

void CompoundHistoUnfolding::SetObservable(const char * observable)
{
  sprintf(this -> observable, "%s", observable);
}


void CompoundHistoUnfolding::SetOrigBinWidth(float orig)
{
  _orig_bin_width = orig;
}

void CompoundHistoUnfolding::SetCHUnominal(CompoundHistoUnfolding * chunominal)
{
  _chunominal = chunominal;
}

void CompoundHistoUnfolding::SetXaxisTitle(const char * title)
{
  sprintf(_XaxisTitle, "%s", title);
}

void CompoundHistoUnfolding::SetYaxisTitle(const char * title)
{
  sprintf(_YaxisTitle, "%s", title);
}

void CompoundHistoUnfolding::SetLuminosity(float l)
{
  _luminosity = l;
}

void CompoundHistoUnfolding::SetCOM(const char * com)
{
  sprintf(_com, "%s", com);
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
  totalbackground(nullptr),
  input(nullptr),
  _chunominal(nullptr)
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
totalbackground(nullptr),
input(nullptr),
_chunominal(nullptr)
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
totalbackground(nullptr),
input(nullptr),
_chunominal(nullptr)
{
  SetName(name);
  SetTitle(title);
  _orig_bin_width = (xup - xlow)/nbinsx;

}

CompoundHistoUnfolding::~CompoundHistoUnfolding()
{
  if (_sf)
    delete [] _sf;
  if (_xbins)
    delete [] _xbins;
  if (_ybins)
    delete [] _ybins;
  for(vector<TObject *>::iterator it = _garbage.begin(); it != _garbage.end(); it ++)
    {
      //printf(" check %p  %c\n", *it, "hh");//(*it) -> ClassName());
        delete (*it);
    }
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

HistoUnfoldingTH2 * CompoundHistoUnfolding::GetBackgroundTotal()
{
  return totalbackground;
}

void CompoundHistoUnfolding::Do()
{
  printf("CompoundHistoUnfolding doing\n");
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
