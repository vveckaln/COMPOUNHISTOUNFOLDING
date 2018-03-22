#include "CompoundHistoUnfolding.hh"
#include "TFile.h"
#include "JsonParser.hh"
#include "TCanvas.h"
#include "TMath.h"
#include "TColor.h"
#include "TStyle.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TROOT.h"
#include "TF1.h"
#include "TUnfoldDensity.h"
#include "TLegend.h"
ClassImp(CompoundHistoUnfolding);

float* CompoundHistoUnfolding::splitForMinSigmaM(unsigned long & size, float factor)
{
  printf("splitForMinSigmaM\n");
  TH2F & h = *_signalh -> _th2;
  TAxis * Xaxis = h.GetXaxis();
  //TAxis * Yaxix = h.GetYaxis();                                                                                                                                                                           

  TF1 f1 = TF1("f1", "gaus", Xaxis -> GetXmin(), Xaxis -> GetXmax());
  f1.SetParameters(100, (Xaxis -> GetXmax() - Xaxis -> GetXmin())/2., 0.01);
  TObjArray slices;
  h.FitSlicesX(& f1, 1, h.GetNbinsX(), 0, "QNRLM", &slices);

  TCanvas c2("c2", "c2");
  ((TH1D*)slices[0]) -> Draw();
  c2.Print(pic_folder + "/slices_N.eps");
  ((TH1D*)slices[1]) -> GetYaxis() -> SetRangeUser(Xaxis -> GetXmin(), Xaxis -> GetXmax());
  ((TH1D*)slices[1]) -> Draw();
  c2.Print(pic_folder + "/slices_mean.eps");
  ((TH1D*)slices[2]) -> GetYaxis() -> SetRangeUser(0.0, Xaxis -> GetXmax());
  ((TH1D*)slices[2]) -> Draw();
  c2.Print(pic_folder + "/slices_sigma.eps");

  vector<float> bins;
  bins.push_back(Xaxis -> GetXmin());
  vector<float> exact;
  exact.push_back(Xaxis -> GetXmin());
  const float integral = h.Integral();
  float binfraction = 0.0;
  for (unsigned char ind = 0; ind < h.GetNbinsX() + 1; ind ++)
    {
      binfraction += h.ProjectionX("px", ind, ind) -> Integral()/integral;
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
  _signalh -> _th2 -> Draw("COLZ");
  cmigration.SaveAs(TString(_folder) + "/" + GetName() + "/migrationmatrix.png");
  TUnfoldDensity unfold(_signalh -> _th2, TUnfold::kHistMapOutputVert, TUnfold::kRegModeCurvature, TUnfold::kEConstraintArea, TUnfoldDensity::kDensityModeUser);
  TFile *f = TFile::Open("test.root", "RECREATE");
  f -> cd();
  _signalh -> _th2 -> Write();
  _signalh -> _th2 -> SetDirectory(nullptr);
  
  TH1F * input = (TH1F *) _datambackgroundth2 -> ProjectionX(CreateName("datambackground"));
  ApplyScaleFactor(input);
  input -> SetMinimum(0.0);
  input -> Write();
  input -> SetDirectory(nullptr);
  f -> Close();
  unfold.SetInput(input);
  unfold.DoUnfold(0.0);
  TCanvas *coutput = new TCanvas(CreateName("coutput"), CreateTitle("coutput"));
  
  TH1F * houtput = (TH1F*) unfold.GetOutput(CreateName("output"));
  TH1F * py = (TH1F*)_signalh -> _th2 -> ProjectionY();
  py -> SetMinimum(0.0);
  NormaliseToBinWidth(py);
  py -> Draw();
  TH1F * houtput_draw = (TH1F*) houtput -> Clone();
  NormaliseToBinWidth(houtput_draw);
  houtput_draw -> SetMinimum(0.0);
  houtput_draw -> Draw("SAME");
  houtput_draw -> SetLineColor(kGreen);
  TLegend legend_output(0.5, 0.6, 1.0, 0.9);
  legend_output.AddEntry(houtput_draw, "unfolded");
  legend_output.AddEntry(py, "ttbar gen");
  legend_output.Draw("SAME");
  coutput -> SaveAs(TString(_folder) + "/" + GetName() + "/coutput.png");
  
  _signalhunfolded = new Histo(houtput, (SampleDescriptor*)_signalh);
  _totalMCGEN_OUT = (TH1F*)_backgroundth2 -> ProjectionY(CreateName("totalMCGEN_OUT"));
  _totalMCGEN_OUT -> Add(houtput);
  TCanvas * cfolded_output = new TCanvas(CreateName("cfolded_output"), CreateTitle("cfolded_output"));
  TH1F * hfolded_output = (TH1F*) unfold.GetFoldedOutput(CreateName("folded_output"));
  TH1F * px = (TH1F *)_signalh -> _th2 -> ProjectionX();
  NormaliseToBinWidth(px);
  px -> SetMinimum(0.0);
 
  px -> Draw();
  TH1F * input_draw = (TH1F*) input -> Clone();
  NormaliseToBinWidth(input_draw);
  input_draw -> SetLineColor(kRed);
  input_draw -> SetMinimum(0.0);
  input_draw -> Draw("SAME");
  TH1F * hfolded_output_draw = (TH1F*) hfolded_output -> Clone();
  NormaliseToBinWidth(hfolded_output_draw);
  hfolded_output_draw -> Draw("SAME");
  hfolded_output_draw -> SetLineColor(kGreen);
  hfolded_output_draw -> SetMinimum(0.0);
  TLegend legend_input(0.5, 0.6, 1.0, 0.9);
  legend_input.AddEntry(input_draw, "dataminusbckg");
  legend_input.AddEntry(px, "ttbar reco no fakes");
  legend_input.AddEntry(hfolded_output_draw, "folded_output");
  legend_input.Draw("SAME");
  cfolded_output -> SaveAs(TString(_folder) + "/" + GetName() + "/cfolded_output.png");
  _signalhfoldedback = new Histo(hfolded_output, (SampleDescriptor*)_signalh);
  _totalMCRECO_OUT = (TH1F *) _backgroundth2 -> ProjectionX(CreateName("totalMCRECO_OUT"));
  ApplyScaleFactor(_totalMCRECO_OUT);
  _totalMCRECO_OUT -> Add(hfolded_output);

  for (unsigned char ind = 0; ind < _signalh -> _th2 -> GetNbinsX() + 1; ind ++)
    {
      printf("matrix x %u upedge %f \n", ind, _signalh -> _th2 -> GetXaxis() -> GetBinUpEdge(ind) );

    }
 for (unsigned char ind = 0; ind < _signalh -> _th2 -> GetNbinsY() + 1; ind ++)
    {
      printf("matrix y %u upedge %f \n", ind, _signalh -> _th2 -> GetYaxis() -> GetBinUpEdge(ind) );

    }

 for (unsigned char ind = 0; ind < houtput -> GetNbinsX() + 1; ind ++)
    {
      printf("output %u upedge %f \n", ind, houtput -> GetXaxis() -> GetBinUpEdge(ind) );

    }

 for (unsigned char ind = 0; ind < hfolded_output -> GetNbinsX() + 1; ind ++)
    {
      printf("folded output %u upedge %f \n", ind, hfolded_output -> GetXaxis() -> GetBinUpEdge(ind) );

    }
 
}

void CompoundHistoUnfolding::ScaleFactor()
{
  _size_sf  = _signalh -> _th2 -> GetNbinsX() + 2;
  _sf = new float[_size_sf];
  TH1F * sigReco = (TH1F*) _signalh -> _th2 -> ProjectionX(CreateName("sigReco"));
  TH1F * sigRecoNonGen = (TH1F*) _signalh -> _th2 -> ProjectionX(CreateName("sigRecoNonGen"), 0, 0);
  TCanvas *c = new TCanvas(CreateName("checkproj"), CreateTitle("checkproj"));
  
  sigReco -> SetLineColor(kRed);
  sigReco -> Draw();
  for (unsigned char ind = 0; ind < _size_sf; ind ++)
    {
      float sfi = 1.0;
      if (sigReco -> GetBinContent(ind) > 0.0)
	{
	  sfi = sigRecoNonGen -> GetBinContent(ind) / sigReco -> GetBinContent(ind);
	}
      _sf[ind] = sfi;
      _signalh -> _th2 -> SetBinContent(ind, 0, 0.0);
    }
  TH1F * after = (TH1F*) _signalh -> _th2 -> ProjectionX();
  after -> Draw("SAME");
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
void CompoundHistoUnfolding::LoadHistos(const char * json, SysTypeCode_t sys)
{
  JsonParser parser;
  parser.Load(json, sys);
  //parser.ls();
  const unsigned long ssize = parser.GetSize();
  for (unsigned long sind = 0; sind < ssize; sind ++)
    {
      HistoUnfolding *h = nullptr;
      if (_isRegular)
	{
	  h = new HistoUnfolding(GetName(), CreateTitle(_title), _nbinsx, _xlow, _xup, _nbinsy, _ylow, _yup, parser.GetSample(sind)); 
	  h -> _th2 -> RebinY(2);
	}
      else
	h = new HistoUnfolding(GetName(), CreateTitle(_title), _nentriesx - 1, _xbins, _nentriesy - 1, _ybins, parser.GetSample(sind));
      //  printf("Filling from tree\n");
      //h-> ls();
      h -> FillFromTree();
      AddHisto(h);
      parser.UnsetSample(sind);

    }
}

void CompoundHistoUnfolding::AddHisto(HistoUnfolding * histo)
{
  //printf("Adding \n");
  //  histo -> ls();
  //histo -> _th1 -> SetMinimum(0.0);
  if (TString(histo -> _tag) == "MC13TeV_TTJets_cflip")
    return;
  if (histo -> _sample_type != DATA)
    histo -> _th2 -> Scale(luminosity * histo -> _xsec);

  if (histo -> _sys_type == SYS)
    {
      if (TString(histo -> _title).Contains("t#bar{t}"))
	_vsyshistos.push_back(histo);
    return;
    }

  vector<HistoUnfolding *> * vvhisto[3] = {&_vdatahistos, &_vsignalhistos, & _vbackgroundhistos};
  vector<HistoUnfolding *> * vhistosel = vvhisto[histo -> _sample_type]; 

  vhistosel -> push_back(histo);
  switch(histo -> _sample_type)
    {
    case SIGNAL:
      if (_signalh)
	{
	  _signalh -> _th2 -> Add(histo -> _th2);
	}
      else
	{
	  _signalh = new HistoUnfolding((TH2F*)histo -> _th2 -> Clone(), (SampleDescriptor*) histo);
	}
      break;
    case DATA:
      {
	if (_datath2)
	  {
	    _datath2 -> Add(histo -> _th2);
	  }
	else
	  {
	    _datath2 = (TH2F*) histo -> _th2 -> Clone();
	  }
      }
      break;
    case BACKGROUND:
      {
	if (_backgroundth2)
	  {
	    _backgroundth2 -> Add(histo -> _th2);
	  }
	else
	  {
	    _backgroundth2 = (TH2F*) histo -> _th2 -> Clone();
	  }
      }
      break;
    }

}

void CompoundHistoUnfolding::CreateMCTotal()
{
  
  TH2F * hMCTotal = (TH2F*)_signalh -> _th2 -> Clone(CreateName("mctotal"));
  hMCTotal -> Add(_backgroundth2);
  _totalMCth2 = hMCTotal;
}

void CompoundHistoUnfolding::NormaliseToBinWidth(TH1 *h)
{
  for (unsigned char ind = 1; ind < h -> GetNbinsX() + 1; ind ++)
    {
      h -> SetBinContent(ind, h -> GetBinContent(ind) * _orig_bin_width / h -> GetBinWidth(ind));
      h -> SetBinError(ind, h -> GetBinError(ind) * _orig_bin_width / h -> GetBinWidth(ind));

    }
}

void CompoundHistoUnfolding::CreateBackgroundTotal()
{
  if (_backgroundth2)
    return;
  for (map<TString, HistoUnfolding *>::iterator it = _mbackgroundMC.begin(); it != _mbackgroundMC.end(); it ++)
    {
      if (not _backgroundth2)
	_backgroundth2 = (TH2F*) it -> second -> _th2 -> Clone();
      else
	_backgroundth2 -> Add(it -> second -> _th2);
    }
}

void CompoundHistoUnfolding::CreateDataMinusBackground()
{
  _datambackgroundth2 = (TH2F*) _datath2 -> Clone();
  _datambackgroundth2 -> Add(_backgroundth2, -1);
  
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

void CompoundHistoUnfolding::CreateTotalMCUnc(RecoLevelCode_t recocode, ResultLevelCode_t resultcode)
{
  TH1F **totalMCUncret = nullptr;
  TH1F * htotalMCUnc = nullptr;
  TH1F * hsig = nullptr;
  unsigned char nbins = 0;

  switch(recocode)
    {
    case RECO:
      switch (resultcode)
	{
	case IN:
	  totalMCUncret = &_totalMCUncth1RECO_IN;
	  break;
	case OUT:
	  totalMCUncret = &_totalMCUncth1RECO_OUT;
	  break;
	}
      break;
    case GEN:
      switch (resultcode)
	{
	case IN:
	  totalMCUncret = &_totalMCUncth1GEN_IN;
	  break;
	case OUT:
	  totalMCUncret = &_totalMCUncth1GEN_OUT;
	  break;
	}
      break;
    }
  if ( *totalMCUncret)
    return;

  switch(recocode)
    {
    case RECO:
      switch(resultcode)
	{
	case IN:
	  htotalMCUnc = (TH1F*) _totalMCth2 -> ProjectionX(CreateName("htotalMCUnc2Fpx"));
	  ApplyScaleFactor(htotalMCUnc);
	  hsig = (TH1F*) _signalh -> _th2 -> ProjectionX();
	  break;
	case OUT:
	  htotalMCUnc = (TH1F*) _totalMCRECO_OUT -> Clone(CreateName("htotalMCUncRECO_OUT"));
	  hsig = (TH1F*) _signalhfoldedback -> _th1 -> Clone();
	  break;
	}
      break;
    case GEN:
      switch(resultcode)
	{
	case IN:
	  htotalMCUnc = (TH1F*) _totalMCth2 -> ProjectionY(CreateName("htotalMCUnc2Fpy"));
	  hsig = (TH1F*) _signalh -> _th2 -> ProjectionY();
	  break;
	case OUT:
	  htotalMCUnc = (TH1F*) _totalMCGEN_OUT -> Clone(CreateName("htotalMCUncGEN_OUT"));
	  hsig = (TH1F*) _signalhunfolded -> _th1 -> Clone();
	  break;
	}
      break;
    }
  nbins = htotalMCUnc -> GetNbinsX();

  float sysUp[nbins + 2];
  float sysDown[nbins + 2];
  for (unsigned char ind = 0; ind < nbins + 2; ind ++)
    {
      sysUp[ind] = 0.0;
      sysDown[ind] = 0.0;
    }
  for (vector<HistoUnfolding *>::iterator it = _vsyshistos.begin(); it != _vsyshistos.end(); it ++)
    {
      //(*it) -> ls();
      TH1F * hsys = nullptr;
      switch(recocode)
	{
	case RECO:
	  hsys = (TH1F*) (*it) -> _th2 -> ProjectionX(CreateName(TString((*it) -> _tag) + "_hsyspx"));
	  ApplyScaleFactor(hsys);
	  break;
	case GEN:
	  hsys = (TH1F*) (*it) -> _th2 -> ProjectionY(CreateName(TString((*it) -> _tag) + "_hsyspy"));
	  break;
	}
      for (unsigned char bin_ind = 0; bin_ind < nbins + 2; bin_ind ++)
	{
	  const float diff = hsys -> GetBinContent(bin_ind) - hsig -> GetBinContent(bin_ind);
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
  *totalMCUncret = htotalMCUnc;
}

void CompoundHistoUnfolding::CreateTotalMCUncShape(RecoLevelCode_t recocode, OptLevelCode_t resultcode)
{
  TH1F **totalMCUncShaperet = nullptr;
  switch(recocode)
    {
    case RECO:
      switch (resultcode)
	{
	case IN:
	  totalMCUncShaperet = &_totalMCUncShapeth1RECO_IN;
	  break;
	case OUT:
	  totalMCUncShaperet = &_totalMCUncShapeth1RECO_OUT;
	  break;
	}
      break;
    case GEN:
      switch (resultcode)
	{
	case IN:
	  totalMCUncShaperet = &_totalMCUncShapeth1GEN_IN;
	  break;
	case OUT:
	  totalMCUncShaperet = &_totalMCUncShapeth1GEN_OUT;
	  break;
	}
      break;
    }
  if ( *totalMCUncShaperet)
    return;
  TH1F * htotalMCUncShape = nullptr;
  TH1F * hsig = nullptr;
  unsigned char nbins = 0;
  switch(recocode)
    {
    case RECO:
      switch(resultcode)
	{
	case IN:
	  htotalMCUncShape = (TH1F*) _totalMCth2 -> ProjectionX(CreateName("htotalMCUncShape2Fpx"));
	  ApplyScaleFactor(htotalMCUncShape);
	  hsig = (TH1F*) _signalh -> _th2 -> ProjectionX(CreateName("hsigpx"));
	  break;
	case OUT:
	  htotalMCUncShape = (TH1F*) _totalMCRECO_OUT -> Clone(CreateName("htotalMCUncRECO_OUT"));
	  hsig = (TH1F*)_signalhfoldedback -> _th1 -> Clone();
	}
      break;
    case GEN:
      switch(resultcode)
	{
	case IN:
	  htotalMCUncShape = (TH1F*) _totalMCth2 -> ProjectionY(CreateName("htotalMCUncShape2Fpy"));
	  hsig = (TH1F*) _signalh -> _th2 -> ProjectionY(CreateName("hsigpy"));
	  break;
	case OUT:
	  htotalMCUncShape = (TH1F*) _totalMCGEN_OUT -> Clone(CreateName("htotalMCUncShapeGEN_OUT"));
	  hsig = (TH1F*) _signalhunfolded -> _th1 -> Clone();
	  break;
	}
      break;
    }
  nbins = htotalMCUncShape -> GetNbinsX();

  const float nominalIntegral = hsig -> Integral();
  float sysUp[nbins + 2];
  float sysDown[nbins + 2];
  for (unsigned char ind = 0; ind < nbins + 2; ind ++)
    {
      sysUp[ind] = 0.0;
      sysDown[ind] = 0.0;
    }
  for (vector<HistoUnfolding *>::iterator it = _vsyshistos.begin(); it != _vsyshistos.end(); it ++)
    {
      TH1F * hsys = nullptr;
      switch(recocode)
	{
	case RECO:
	  hsys = (TH1F*) (*it) -> _th2 ->ProjectionX(CreateName(TString((*it) -> _tag) + "_hsysShapepx"));
	  ApplyScaleFactor(hsys);
	  break;
	case GEN:
	  hsys = (TH1F*) (*it) -> _th2 -> ProjectionY(CreateName(TString((*it) -> _tag) + "hsysShapepy"));
	  break;
	}
      
      hsys -> Scale(nominalIntegral/hsys -> Integral());
      for (unsigned char bin_ind = 0; bin_ind < nbins + 2; bin_ind ++)
	{
	  const float diff = hsys -> GetBinContent(bin_ind) - hsig -> GetBinContent(bin_ind);
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
      htotalMCUncShape -> SetBinContent(bin_ind, htotalMCUncShape -> GetBinContent(bin_ind) + (sysUp[bin_ind] - sysDown[bin_ind])/2.0);
      htotalMCUncShape -> SetBinError(bin_ind, TMath::Sqrt(TMath::Power(htotalMCUncShape -> GetBinError(bin_ind), 2) + TMath::Power((sysUp[bin_ind] + sysDown[bin_ind])/2.0, 2)));
    }
  delete hsig;
  NormaliseToBinWidth(htotalMCUncShape);
  *totalMCUncShaperet = htotalMCUncShape;
}

void CompoundHistoUnfolding::AggregateBackgroundMCs()
{
  if (_mbackgroundMC.size() != 0)
    return;
  for(vector<HistoUnfolding *> :: iterator it = _vbackgroundhistos.begin(); it != _vbackgroundhistos.end(); it ++)
    {
      const TString title = (*it) -> _title;
      if (_mbackgroundMC.find(title) == _mbackgroundMC.end())
	{
	  HistoUnfolding *check = new HistoUnfolding((TH2F*)(*it) -> _th2 -> Clone(TString((*it) -> _th2 -> GetName()) + "_aggregate"), (SampleDescriptor*) (*it));
	  _mbackgroundMC[title] = check;
	}
      else
	{
	  _mbackgroundMC[title] -> _th2 -> Add((*it) -> _th2);
	}
    }
}
 
THStack * CompoundHistoUnfolding::CreateTHStack(RecoLevelCode_t recocode, ResultLevelCode_t resultcode)
{
  AggregateBackgroundMCs();
  FormatHistograms();
  THStack * stack = new THStack("mc", "mc");
  for (map<TString, HistoUnfolding*> :: iterator it = _mbackgroundMC.begin(); it != _mbackgroundMC.end(); it ++)
    {
      TH1F * hbackground = nullptr;
      switch(recocode)
	{
	case RECO:
	  hbackground = (TH1F*)it -> second -> _th2 -> ProjectionX(CreateName(TString(it -> second -> _tag) + "_backgroundpx"));
	  ApplyScaleFactor(hbackground);
	  break;
	case GEN:
	  hbackground = (TH1F*)it -> second -> _th2 -> ProjectionY(CreateName(TString(it -> second -> _tag) + "backgroundpy"));
	  break;
	}
      NormaliseToBinWidth(hbackground);
      stack -> Add(hbackground);
      //delete hbackground;
    }
  TH1F * hsig = nullptr;
  switch(recocode)
    {
    case RECO:
      switch(resultcode)
	{
	case IN:
	  hsig = (TH1F*) _signalh ->  _th2 -> ProjectionX(CreateName("sigpx"));
	  break;
	case OUT:
	  hsig = (TH1F*) _signalhfoldedback -> _th1 -> Clone(CreateName("sigRECO_OUT"));
	  break;
	}
      break;
    case GEN:
      switch(resultcode)
	{
	case IN:
	  hsig = (TH1F*) _signalh -> _th2 -> ProjectionY(CreateName("sigpy"));
	  break;
	case OUT:
	  hsig = (TH1F*) _signalhunfolded -> _th1 -> Clone(CreateName("sigGEN_OUT"));
	  break;
	}
      break;
    }
  NormaliseToBinWidth(hsig);
  printf("probe A\n");
  stack -> Add(hsig);
  //  delete hsig;
  printf("THStack created level %u result %u\n", recocode, resultcode);
  return stack;
}
 

void CompoundHistoUnfolding::Process()
{
  CreateMCTotal();
  unfold();
  CreateTotalMCUnc(RECO, IN);
  CreateTotalMCUncShape(RECO, IN);
  CreateTotalMCUnc(GEN, IN);
  CreateTotalMCUncShape(GEN, IN);
  CreateTotalMCUnc(RECO, OUT);
  CreateTotalMCUncShape(RECO, OUT);
  CreateTotalMCUnc(GEN, OUT);
  CreateTotalMCUncShape(GEN, OUT);

  CreateDataGraph();
}

   
TPad * CompoundHistoUnfolding::CreateMainPlot(RecoLevelCode_t recocode, ResultLevelCode_t resultcode)
{
  THStack * stack = CreateTHStack(recocode, resultcode);
  float max = 0.0;
  TH1F * totalMC = nullptr;
  TH1F * data = nullptr;
  TH1F * totalMCUnc = nullptr;
  TH1F * totalMCUncShape = nullptr;
  TH1F * hoverlay = nullptr;
  switch(recocode)
    {
    case RECO:
      {
	data = (TH1F *) _datath2 -> ProjectionX(CreateName("datapx"));
	ApplyScaleFactor(data);
	
	NormaliseToBinWidth(data);
	switch(resultcode)
	  {
	  case IN:
	    totalMC        = (TH1F*) _totalMCth2 -> ProjectionX(CreateName("totalMCpx"));
	    ApplyScaleFactor(totalMC);
	    totalMCUnc      = _totalMCUncth1RECO_IN;
	    totalMCUncShape = _totalMCUncShapeth1RECO_IN;
	    break;
	  case OUT:
	    totalMC = (TH1F*) _totalMCRECO_OUT -> Clone();
	    totalMCUnc = _totalMCUncth1RECO_OUT;
	    totalMCUncShape = _totalMCUncShapeth1RECO_OUT;
	    hoverlay = (TH1F*) _signalh ->  _th2 -> ProjectionX(CreateName("sigpx"));
	    hoverlay -> SetLineColor(kRed);

	    break;
	  }
	NormaliseToBinWidth(totalMC);

	TH1 * hmax[4] = {totalMCUnc, totalMCUncShape, data, totalMC};
	for (unsigned char ind = 0; ind < 4; ind ++)
	  {
	    if (hmax[ind] -> GetMaximum() > max)
	      max = hmax[ind] -> GetMaximum();
	  }
	for (unsigned char ind = 0; ind < 4; ind ++)
	  {
	    hmax[ind] -> SetMaximum(1.2 * max);
	  }
	break;
      }
    case GEN:
      {
	switch(resultcode)
	  {
	  case IN:
	    totalMC = (TH1F*) _totalMCth2 -> ProjectionY(CreateName("totalMCpy"));
	    totalMCUnc = _totalMCUncth1GEN_IN;
	    totalMCUncShape = _totalMCUncShapeth1GEN_IN;
	    break;
	  case OUT:
	    totalMC = (TH1F*) _totalMCGEN_OUT -> Clone();
	    totalMCUnc = _totalMCUncth1GEN_OUT;
	    totalMCUncShape = _totalMCUncShapeth1GEN_OUT;
	    hoverlay = (TH1F*) _signalh -> _th2 -> ProjectionY(CreateName("sigpy"));
	    hoverlay -> SetLineColor(kRed);

	  }
	NormaliseToBinWidth(totalMC);
	if (hoverlay)
	  NormaliseToBinWidth(hoverlay);
	TH1 * hmax[3] = {totalMCUnc, totalMCUncShape, totalMC};
	for (unsigned char ind = 0; ind < 3; ind ++)
	  {
	    if (hmax[ind] -> GetMaximum() > max)
	      max = hmax[ind] -> GetMaximum();
	  }
	for (unsigned char ind = 0; ind < 3; ind ++)
	  {
	    hmax[ind] -> SetMaximum(1.2 * max);
	  }
      }
      break;
    }
   stack -> SetMaximum(1.2 * max);
   TPad *pad = new TPad(CreateName("pad1"), CreateTitle("pad1"), 0.0, 0.2, 1.0, 1.0);
   pad -> Draw();
   pad -> cd();
   stack -> Draw("HISTO");
   if (hoverlay)
     hoverlay -> Draw("SAME");
   totalMCUnc -> Draw("E2SAME");
   for (unsigned char ind = 0; ind < totalMCUnc -> GetNbinsX() + 1; ind ++)
     {
       printf("Unc ind %u cont %f err %f\n", ind, totalMCUnc -> GetBinContent(ind), totalMCUnc -> GetBinError(ind));
     }
   totalMCUncShape -> Draw("E2SAME");
    for (unsigned char ind = 0; ind < totalMCUnc -> GetNbinsX() + 1; ind ++)
     {
       printf("UncShape ind %u cont %f err %f\n", ind, totalMCUncShape -> GetBinContent(ind), totalMCUncShape -> GetBinError(ind));
     }
 if (recocode == RECO)
     _datagr -> Draw("P");
 printf("created main plot reco %u result %u\n", recocode, resultcode);
   delete totalMC;
   //if (data) delete data;
   return pad;
}


void CompoundHistoUnfolding::CreateDataGraph()
{
  TH1F * data = (TH1F *) _datath2 -> ProjectionX(CreateName("datapx"));
  ApplyScaleFactor(data);
  NormaliseToBinWidth(data);
  _datagr = new TGraphErrors(data);
  delete data;
}

 
TPad * CompoundHistoUnfolding::CreateRatioGraph(RecoLevelCode_t recocode, ResultLevelCode_t resultcode)
{
  TH1F * htotalMC = nullptr;
  TH1F * htotalMCUnc = nullptr;
  TH1F * htotalMCUncShape = nullptr;
  switch(recocode)
    {
    case RECO:
      switch(resultcode)
	{
	case IN:
	  htotalMC =  (TH1F *) _totalMCth2 -> ProjectionX(CreateName("ratiograpphtotalMCpx"));
	  htotalMCUnc = _totalMCUncth1RECO_IN;
	  htotalMCUncShape = _totalMCUncShapeth1RECO_IN;
	  ApplyScaleFactor(htotalMC);
	  break;
	case OUT:
	  htotalMC = (TH1F*) _totalMCRECO_OUT -> Clone();
	  htotalMCUnc = _totalMCUncth1RECO_OUT;
	  htotalMCUncShape = _totalMCUncShapeth1RECO_OUT;
	  break;
	}
      break;
    case GEN:
      switch(resultcode)
	{
	case IN:
	  htotalMC =  (TH1F *) _totalMCth2 -> ProjectionY(CreateName("ratiographtotalMCpy"));
	  htotalMCUnc = _totalMCUncth1GEN_IN;
	  htotalMCUncShape = _totalMCUncShapeth1GEN_IN;
	  break;
	case OUT:
	  htotalMC = (TH1F*) _totalMCGEN_OUT -> Clone();
	  htotalMCUnc = _totalMCUncth1GEN_OUT;
	  htotalMCUncShape = _totalMCUncth1GEN_OUT;
	  break;
	}
      break;
    }
  NormaliseToBinWidth(htotalMC);
  _ratioframe = (TH1F*) htotalMC -> Clone(CreateName("ratioframe"));
  _ratioframe -> Reset("ICE");
  _ratioframe -> GetYaxis() -> SetRangeUser(0.4, 1.6);
  _ratioframe -> GetYaxis() -> SetTitle("Data/MC");
  _ratioframeshape = (TH1F*) _ratioframe -> Clone(CreateName("ratioframeshape"));

  TH1F * htotalMCnoUnc = (TH1F*) htotalMC -> Clone(CreateName("totalMCnounc"));
  for (unsigned char xbin = 0; xbin < htotalMC -> GetNbinsX() + 1; xbin ++)
    {    
      _ratioframe -> SetBinContent(xbin, 1.0);
      const float val = htotalMC -> GetBinContent(xbin);
      htotalMCnoUnc -> SetBinError(xbin, 0.0);
      if (val == 0)
	{
	  continue;
	}
      const float totalUnc = htotalMCUnc -> GetBinError(xbin)/val; 
      const float totalUncShape = htotalMCUncShape -> GetBinError(xbin)/val; 
      _ratioframeshape -> SetBinContent(xbin, htotalMCUncShape -> GetBinContent(xbin)/val);
      _ratioframeshape -> SetBinError(xbin, totalUncShape);
      _ratioframe -> SetBinContent(xbin, htotalMCUnc -> GetBinContent(xbin)/val);
      _ratioframe -> SetBinError(xbin, totalUnc);
    }
  TPad * pad = new TPad(CreateName("pad2"), CreateTitle("pad2"), 0.0, 0.0, 1.0, 0.2);
  pad -> Draw();
  pad -> cd();
 _ratioframe -> Draw("E2");
  _ratioframeshape -> Draw("e2 same");
  if (recocode == RECO)
    {
      TH1F * ratio = (TH1F*) _datath2 -> ProjectionX("ratio");
      ApplyScaleFactor(ratio);
      NormaliseToBinWidth(ratio);
      ratio -> Divide(htotalMCnoUnc);
      _ratiogr = new TGraphAsymmErrors(ratio);
      
      _ratiogr -> Draw("P");
      delete ratio;
    }
  printf("created ratio graph reco %u result %u\n", recocode, resultcode);
  delete htotalMC;
  delete htotalMCnoUnc;
  return pad;
}
 
TCanvas * CompoundHistoUnfolding::CreateCombinedPlot(RecoLevelCode_t recocode, ResultLevelCode_t resultcode)
{
  //  TCanvas *c = new TCanvas("compound", "compound", 500, 500);
  TCanvas *c = new TCanvas(CreateName(TString("compound") + "_" + tag_recolevel[recocode] + "_" + tag_resultlevel[resultcode]), CreateTitle(TString("compound") + "_" + tag_recolevel[recocode] + "_" + tag_resultlevel[resultcode]), 500, 500);
  
  TPad * p1 = CreateMainPlot(recocode, resultcode);
  p1 -> SetRightMargin(0.05);
  p1 -> SetLeftMargin(0.12);
  p1 -> SetTopMargin(0.06);
  p1 -> SetBottomMargin(0.01);
  //p1 -> Draw();
  c -> cd();

  TPad * p2 = CreateRatioGraph(recocode, resultcode);
  p2 -> SetBottomMargin(0.4);
  p2 -> SetRightMargin(0.05);
  p2 -> SetLeftMargin(0.12);
  p2 -> SetTopMargin(0.01);
  p2 -> SetGridx(kFALSE);
  p2 -> SetGridy(kTRUE);
  FormatHistograms();
   c -> SetBottomMargin(0.0);
  c -> SetLeftMargin(0.0);
  c -> SetTopMargin(0);
  c -> SetRightMargin(0.00);
  //p2 -> Draw();
  printf("created combined plot reco %u result %u\n", recocode, resultcode);
  c -> SaveAs(TString(_folder) + "/" + GetName() + "/" + c -> GetName() +  ".png");
  return c;
}

void CompoundHistoUnfolding::SetFolder(const char * folder)
{
  
  sprintf(_folder, "%s", folder);
  //system(TString("rm -r ") + _folder);
  system(TString("mkdir - p ") + _folder);
  system(TString("mkdir - p ") + _folder + "/" + GetName());

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

}

void CompoundHistoUnfolding::FormatHistograms()
{
  for (map<TString, HistoUnfolding*> :: iterator it = _mbackgroundMC.begin(); it != _mbackgroundMC.end(); it ++)
    {
      //it -> second -> ls();
      it -> second -> _th2 -> SetFillColor(TColor::GetColor(it -> second -> _color));
    }
  //_signalh -> ls();
  _signalh -> _th2 -> SetFillColor(_signalh -> _color ? TColor::GetColor(_signalh -> _color) : _signalh -> _colornum);
  gStyle -> SetHatchesLineWidth(1);

  if (_totalMCUncth1RECO_IN)
    {
      _totalMCUncth1RECO_IN -> SetFillColor(TColor::GetColor("#99d8c9"));
      _totalMCUncth1RECO_IN -> SetFillStyle(3254);
    }
  if (_totalMCUncShapeth1RECO_IN)
    {
      _totalMCUncShapeth1RECO_IN -> SetFillColor(TColor::GetColor("#d73027"));
      _totalMCUncShapeth1RECO_IN -> SetFillStyle(3254);
    }
  if( _totalMCUncth1GEN_IN)
    {
      _totalMCUncth1GEN_IN -> SetFillColor(TColor::GetColor("#99d8c9"));
      _totalMCUncth1GEN_IN -> SetFillStyle(3254);
    }
  if(_totalMCUncShapeth1GEN_IN)
    {
      _totalMCUncShapeth1GEN_IN -> SetFillColor(TColor::GetColor("#d73027"));
      _totalMCUncShapeth1GEN_IN -> SetFillStyle(3254);
    }

  if (_totalMCUncth1RECO_OUT)
    {
      _totalMCUncth1RECO_OUT -> SetFillColor(TColor::GetColor("#99d8c9"));
      _totalMCUncth1RECO_OUT -> SetFillStyle(3254);
    }
  if (_totalMCUncShapeth1RECO_OUT)
    {
      _totalMCUncShapeth1RECO_OUT -> SetFillColor(TColor::GetColor("#d73027"));
      _totalMCUncShapeth1RECO_OUT -> SetFillStyle(3254);
    }
  if( _totalMCUncth1GEN_OUT)
    {
      _totalMCUncth1GEN_OUT -> SetFillColor(TColor::GetColor("#99d8c9"));
      _totalMCUncth1GEN_OUT -> SetFillStyle(3254);
    }
  if(_totalMCUncShapeth1GEN_OUT)
    {
      _totalMCUncShapeth1GEN_OUT -> SetFillColor(TColor::GetColor("#d73027"));
      _totalMCUncShapeth1GEN_OUT -> SetFillStyle(3254);
    }


  if (_ratioframe)
    {
 
      _ratioframe -> GetYaxis() -> SetNdivisions(503);
      _ratioframe -> GetYaxis() -> SetLabelSize(0.18);
      _ratioframe -> GetYaxis() -> SetTitleSize(0.2);
      _ratioframe -> GetYaxis() -> SetTitleOffset(0.3);
      _ratioframe -> GetXaxis() -> SetLabelSize(0.18);
      _ratioframe -> GetXaxis() -> SetTitleSize(0.2);
      _ratioframe -> GetXaxis() -> SetTitleOffset(0.8);
      _ratioframe -> SetFillStyle(3254);
      _ratioframe -> SetFillColor(TColor::GetColor("#99d8c9"));
    }
  if (_ratioframeshape)
    {
     _ratioframeshape -> GetYaxis() -> SetNdivisions(503);
      _ratioframeshape -> GetYaxis() -> SetLabelSize(0.18);
      _ratioframeshape -> GetYaxis() -> SetTitleSize(0.2);
      _ratioframeshape -> GetYaxis() -> SetTitleOffset(0.3);
      _ratioframeshape -> GetXaxis() -> SetLabelSize(0.18);
      _ratioframeshape -> GetXaxis() -> SetTitleSize(0.2);
      _ratioframeshape -> GetXaxis() -> SetTitleOffset(0.8);
      _ratioframeshape -> SetFillStyle(3254);
  
     _ratioframeshape -> SetFillColor(TColor::GetColor("#d73027"));
    }

  if (_datath2)
    {

      _datath2 -> SetMarkerStyle(20);
      _datath2 -> SetMarkerSize(1.);
      _datath2 -> SetMarkerColor(kBlue);
      _datath2 -> SetLineColor(kBlack);
      _datath2 -> SetLineWidth(2);
      _datath2 -> SetFillColor(0);
      _datath2 -> SetFillStyle(0);

    }
  if (_ratiogr)
    {
      _ratiogr -> SetMarkerStyle(_datath2 -> GetMarkerStyle());
      _ratiogr -> SetMarkerSize(_datath2 -> GetMarkerSize());
      _ratiogr -> SetMarkerColor(_datath2  -> GetMarkerColor());
      _ratiogr -> SetLineColor(_datath2 -> GetLineColor());
      _ratiogr -> SetLineWidth(_datath2  -> GetLineWidth());
    }
}

CompoundHistoUnfolding::CompoundHistoUnfolding(): 
size_IN(0),
size_RECO_IN(0),
size_RECO_OUT(0),
size_GEN_IN(0),
size_GEN_OUT(0), 
_datambackgroundth2(nullptr),
_signalhunfolded(nullptr),
_signalhfoldedback(nullptr),

_datagr(nullptr), 
_datath2(nullptr), 
_signalh(nullptr), 
_backgroundth2(nullptr), 
_totalMCth2(nullptr),
_totalMCUncth1RECO_IN(nullptr),
_totalMCUncShapeth1RECO_IN(nullptr),
_totalMCUncth1RECO_OUT(nullptr),
_totalMCUncShapeth1RECO_OUT(nullptr),

_totalMCUncth1GEN_IN(nullptr),
_totalMCUncShapeth1GEN_IN(nullptr),
_totalMCUncth1GEN_OUT(nullptr),
_totalMCUncShapeth1GEN_OUT(nullptr),

_ratiogr(nullptr), 
_ratioframe(nullptr), 
_ratioframeshape(nullptr),
_isRegular(true), 
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
_orig_bin_width(1),
_size_sf(0),
_sf(nullptr)
{

}

CompoundHistoUnfolding::CompoundHistoUnfolding(const char* name, const char* title, Int_t nbinsx, const Float_t* xbins, Int_t nbinsy, const Float_t* ybins): 
size_IN(0),
size_RECO_IN(0),
size_RECO_OUT(0),
size_GEN_IN(0),
size_GEN_OUT(0), 
_datambackgroundth2(nullptr),
_signalhunfolded(nullptr),
_signalhfoldedback(nullptr),

_datagr(nullptr), 
_datath2(nullptr), 
_signalh(nullptr), 
_backgroundth2(nullptr), 
_totalMCth2(nullptr),
_totalMCUncth1RECO_IN(nullptr),
_totalMCUncShapeth1RECO_IN(nullptr),
_totalMCUncth1RECO_OUT(nullptr),
_totalMCUncShapeth1RECO_OUT(nullptr),

_totalMCUncth1GEN_IN(nullptr),
_totalMCUncShapeth1GEN_IN(nullptr),
_totalMCUncth1GEN_OUT(nullptr),
_totalMCUncShapeth1GEN_OUT(nullptr),
_ratiogr(nullptr), 
_ratioframe(nullptr), 
_ratioframeshape(nullptr),
_isRegular(false), 
_nbinsx(0),
_xlow(0.0),
_xup(0.0),
_nbinsy(0),
_ylow(0.0),
_yup(0.0),
_nentriesx(nbinsx + 1),
_xbins(xbins), 
_nentriesy(nbinsy + 1),
_ybins(ybins),
_orig_bin_width(1),
_size_sf(0),
_sf(nullptr)
{
  SetName(name);
  sprintf(_title, "%s", title);
}

CompoundHistoUnfolding::CompoundHistoUnfolding(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup): 
size_IN(0),
size_RECO_IN(0),
size_RECO_OUT(0),
size_GEN_IN(0),
size_GEN_OUT(0), 
_datambackgroundth2(nullptr),
_signalhunfolded(nullptr),
_signalhfoldedback(nullptr),

_datagr(nullptr), 
_datath2(nullptr), 
_signalh(nullptr), 
_backgroundth2(nullptr), 
_totalMCth2(nullptr),
_totalMCUncth1RECO_IN(nullptr),
_totalMCUncShapeth1RECO_IN(nullptr),
_totalMCUncth1RECO_OUT(nullptr),
_totalMCUncShapeth1RECO_OUT(nullptr),

_totalMCUncth1GEN_IN(nullptr),
_totalMCUncShapeth1GEN_IN(nullptr),
_totalMCUncth1GEN_OUT(nullptr),
_totalMCUncShapeth1GEN_OUT(nullptr),


_ratiogr(nullptr), 
_ratioframe(nullptr), 
_ratioframeshape(nullptr),
_isRegular(true), 
_nbinsx(nbinsx),
_xlow(xlow),
_xup(xup),
_nbinsy(nbinsy),
_ylow(ylow),
_yup(yup),
_nentriesx(0),
_xbins(nullptr), 
_nentriesy(0),
_ybins(nullptr),
_size_sf(0),
_sf(nullptr)
{
  SetName(name);
  sprintf(_title, "%s", title);
  _orig_bin_width = (xup - xlow)/nbinsx;

}

TString CompoundHistoUnfolding::CreateName(const char * name)
{
  return TString(GetName()) + "_" + name;
}

TString CompoundHistoUnfolding::CreateTitle(const char * title)
{
  return TString(GetName()) + " " + title;
}
