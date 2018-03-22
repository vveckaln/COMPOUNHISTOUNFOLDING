#include "CompoundHisto.hh"
#include "TFile.h"
#include "JsonParser.hh"
#include "TCanvas.h"
#include "TMath.h"
#include "TColor.h"
#include "TStyle.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TROOT.h"
ClassImp(Histo);

Histo::Histo(): size(1), _th1(nullptr)
{

}

Histo::Histo(TH1F * h, SampleDescriptor sd): size(1), SampleDescriptor(sd),  _th1(h)
{
 
}

Histo::Histo(TH1F * h, SampleDescriptor * sd): size(1), SampleDescriptor(*sd), _th1(h)
{
}

Histo::~Histo()
{

}

ClassImp(CompoundHisto);

void CompoundHisto::LoadHistos(const char * json, SysTypeCode_t sys)
{
  JsonParser parser;
  parser.Load(json, sys);
  parser.ls();
  const unsigned long ssize = parser.GetSize();
  for (unsigned long sind = 0; sind < ssize; sind ++)
    {
      TFile * f = TFile::Open(TString("$EOS/analysis_old/") + parser.GetTag(sind) + ".root");
      if (not f)
	{
	  printf("no file for %s\n", parser.GetTag(sind));
	  continue;
	}
      TH1F *h = (TH1F*) f -> Get("L_pull_angle_allconst_reco_leading_jet_2nd_leading_jet_DeltaRTotal");
      h -> SetDirectory(0);
      f -> Close();
      AddHisto(new Histo(h, parser.GetSample(sind)));
      parser.UnsetSample(sind);

    }
}

void CompoundHisto::AddHisto(Histo * histo)
{
  //printf("Adding \n");
  //  histo -> ls();
  //histo -> _th1 -> SetMinimum(0.0);
  if (TString(histo -> _tag) == "MC13TeV_TTJets_cflip")
    return;
  if (histo -> _sample_type != DATA)
    histo -> _th1 -> Scale(luminosity * histo -> _xsec);

  if (histo -> _sys_type == SYS)
    {
      if (TString(histo -> _title).Contains("t#bar{t}"))
	_vsyshistos.push_back(histo);
    return;
    }

  vector<Histo *> * vvhisto[3] = {&_vdatahistos, &_vsignalhistos, & _vbackgroundhistos};
  vector<Histo *> * vhistosel = vvhisto[histo -> _sample_type]; 

  vhistosel -> push_back(histo);
  switch(histo -> _sample_type)
    {
    case SIGNAL:
      if (_signalh)
	{
	  _signalh -> _th1 -> Add(histo -> _th1);
	}
      else
	{
	  _signalh = new Histo((TH1F*)histo -> _th1 -> Clone(), (SampleDescriptor*) histo);
	}
      break;
    case DATA:
      {
	if (_datath1)
	  {
	    _datath1 -> Add(histo -> _th1);
	  }
	else
	  {
	    _datath1 = (TH1F*) histo -> _th1 -> Clone();
	  }
      }
      break;
    case BACKGROUND:
      {
	if (_backgroundth1)
	  {
	    _backgroundth1 -> Add(histo -> _th1);
	  }
	else
	  {
	    _backgroundth1 = (TH1F*) histo -> _th1 -> Clone();
	  }
      }
      break;
    }
}

CompoundHisto::CompoundHisto(): 
size(1), 
_datagr(nullptr), 
_datath1(nullptr), 
_signalh(nullptr), 
_backgroundth1(nullptr), 
_totalMCth1(nullptr),
_totalMCUncth1(nullptr),
_totalMCUncShapeth1(nullptr),
_ratiogr(nullptr), 
_ratioframe(nullptr), 
_ratioframeshape(nullptr)
{

}

TH1F * CompoundHisto::NewMCTotal(const char * name)
{
  TH1F * hMCTotal = (TH1F*) _signalh -> _th1 -> Clone(name);
  hMCTotal -> Add(_backgroundth1);
  return hMCTotal;
}

TH1F * CompoundHisto::NewDataMinusBackground()
{
  TH1F * hDataMBckg = (TH1F*) _datath1 -> Clone();
  hDataMBckg -> Add(_backgroundth1, -1);
  return hDataMBckg;
}


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

void CompoundHisto::CreateTotalMCUnc()
{
  if ( _totalMCUncth1)
    return;
  TH1F * htotalMCUnc = NewMCTotal("totalMCUnc");
  const unsigned char nbins = htotalMCUnc -> GetNbinsX();
  float sysUp[nbins + 2];
  float sysDown[nbins + 2];
  for (unsigned char ind = 0; ind < nbins + 2; ind ++)
    {
      sysUp[ind] = 0.0;
      sysDown[ind] = 0.0;
    }
  for (vector<Histo *>::iterator it = _vsyshistos.begin(); it != _vsyshistos.end(); it ++)
    {
	  (*it) -> ls();

      for (unsigned char bin_ind = 0; bin_ind < nbins + 2; bin_ind ++)
	{
	  const float diff = (*it) -> _th1 -> GetBinContent(bin_ind) - _signalh -> _th1 -> GetBinContent(bin_ind);
	  if (bin_ind == 4)
	    printf("bin_ind %u cont %f diff %f nom %f\n", bin_ind, (*it) -> _th1 -> GetBinContent(bin_ind), diff, _signalh -> _th1 -> GetBinContent(bin_ind));

	  if (diff > 0)
	    {
	      sysUp[bin_ind] = TMath::Sqrt(TMath::Power(sysUp[bin_ind], 2) + TMath::Power(diff, 2));
		}
	  else
	    {
	      sysDown[bin_ind] = TMath::Sqrt(TMath::Power(sysDown[bin_ind], 2) + TMath::Power(diff, 2));

	    }
	    
	}
    }
  printf("sysUp[4] %f sysDown[4] %f\n", sysUp[4], sysDown[4]);
  for (unsigned char bin_ind = 0; bin_ind < nbins + 2; bin_ind ++)
    {
      htotalMCUnc -> SetBinContent(bin_ind, htotalMCUnc -> GetBinContent(bin_ind) + (sysUp[bin_ind] - sysDown[bin_ind])/2.0);
      htotalMCUnc -> SetBinError(bin_ind, TMath::Sqrt(TMath::Power(htotalMCUnc -> GetBinError(bin_ind), 2) + TMath::Power((sysUp[bin_ind] + sysDown[bin_ind])/2.0, 2)));
    }
  _totalMCUncth1 = htotalMCUnc;
}

void CompoundHisto::CreateTotalMCUncShape()
{
  if (_totalMCUncShapeth1)
    return;
  const float nominalIntegral = _signalh -> _th1 -> Integral();
  TH1F * htotalMCUncShape = NewMCTotal("totalMCUncShape");
  const unsigned char nbins = htotalMCUncShape -> GetNbinsX();
  float sysUp[nbins + 2];
  float sysDown[nbins + 2];
  for (unsigned char ind = 0; ind < nbins + 2; ind ++)
    {
      sysUp[ind] = 0.0;
      sysDown[ind] = 0.0;
    }
  for (vector<Histo *>::iterator it = _vsyshistos.begin(); it != _vsyshistos.end(); it ++)
    {
      
      (*it) -> _th1 -> Scale(nominalIntegral/(*it) -> _th1 -> Integral());
      for (unsigned char bin_ind = 0; bin_ind < nbins + 2; bin_ind ++)
	{
	  const float diff = (*it) -> _th1 -> GetBinContent(bin_ind) - _signalh -> _th1 -> GetBinContent(bin_ind);
	  if (diff > 0)
	    {
	      sysUp[bin_ind] = TMath::Sqrt(TMath::Power(sysUp[bin_ind], 2) + TMath::Power(diff, 2));
		}
	  else
	    {
	      sysDown[bin_ind] = TMath::Sqrt(TMath::Power(sysDown[bin_ind], 2) + TMath::Power(diff, 2));

	    }
	    
	}
    }

  for (unsigned char bin_ind = 0; bin_ind < nbins + 2; bin_ind ++)
    {
      htotalMCUncShape -> SetBinContent(bin_ind, htotalMCUncShape -> GetBinContent(bin_ind) + (sysUp[bin_ind] - sysDown[bin_ind])/2.0);
      htotalMCUncShape -> SetBinError(bin_ind, TMath::Sqrt(TMath::Power(htotalMCUncShape -> GetBinError(bin_ind), 2) + TMath::Power((sysUp[bin_ind] + sysDown[bin_ind])/2.0, 2)));
    }
  _totalMCUncShapeth1 = htotalMCUncShape;
}

void CompoundHisto::AggregateBackgroundMCs()
{
  if (_vbackgroundhistos.size() != 0)
    return;
  for(vector<Histo *> :: iterator it = _vbackgroundhistos.begin(); it != _vbackgroundhistos.end(); it ++)
    {
      const TString title = (*it) -> _title;
      if (_mbackgroundMC.find(title) == _mbackgroundMC.end())
	{
	  Histo *check = new Histo((TH1F*)(*it) -> _th1 -> Clone(TString((*it) -> _th1 -> GetName()) + "_aggregate"), (SampleDescriptor*) (*it));
	  _mbackgroundMC[title] = check;
	}
      else
	{
	  _mbackgroundMC[title] -> _th1 -> Add((*it) -> _th1);
	}
    }
}

void CompoundHisto::FormatHistograms()
{
  for (map<TString, Histo*> :: iterator it = _mbackgroundMC.begin(); it != _mbackgroundMC.end(); it ++)
    {
      it -> second -> ls();
      it -> second -> _th1 -> SetFillColor(TColor::GetColor(it -> second -> _color));
    }
  _signalh -> ls();
  _signalh -> _th1 -> SetFillColor(_signalh -> _color ? TColor::GetColor(_signalh -> _color) : _signalh -> _colornum);
  TH1F * totalMC = NewMCTotal("temp");
  delete totalMC;
  _totalMCUncth1 -> SetFillColor(TColor::GetColor("#99d8c9"));
  gStyle -> SetHatchesLineWidth(1);
  _totalMCUncth1 -> SetFillStyle(3254);

  _totalMCUncShapeth1 -> SetFillColor(TColor::GetColor("#d73027"));
  _totalMCUncShapeth1 -> SetFillStyle(3254);
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
  if (_ratiogr)
    {
      _ratiogr -> SetMarkerStyle(_datath1 -> GetMarkerStyle());
      _ratiogr -> SetMarkerSize(_datath1 -> GetMarkerSize());
      _ratiogr -> SetMarkerColor(_datath1  -> GetMarkerColor());
      _ratiogr -> SetLineColor(_datath1  -> GetLineColor());
      _ratiogr -> SetLineWidth(_datath1  -> GetLineWidth());
    }
}


THStack * CompoundHisto::CreateTHStack()
{
  AggregateBackgroundMCs();
  //FormatHistograms();
  THStack * stack = new THStack("mc", "mc");
  for (map<TString, Histo*> :: iterator it = _mbackgroundMC.begin(); it != _mbackgroundMC.end(); it ++)
    {
      stack -> Add(it -> second -> _th1); 
    }
  stack -> Add(_signalh -> _th1);

  return stack;
}

void CompoundHisto::Process()
{
  CreateTotalMCUnc();
  CreateTotalMCUncShape();
  CreateDataGraph();
  _totalMCth1 = NewMCTotal();
}

TPad * CompoundHisto::CreateMainPlot()
{
  THStack * stack = CreateTHStack();
  float max = 0.0;
  TH1 *hmax[4] = {_totalMCUncth1, _totalMCUncShapeth1, _datath1, _totalMCth1};

  for (unsigned char ind = 0; ind < 4; ind ++)
    {
      if (hmax[ind] -> GetMaximum() > max)
	max = hmax[ind] -> GetMaximum();
    }
   for (unsigned char ind = 0; ind < 4; ind ++)
    {
      hmax[ind] -> SetMaximum(1.2 * max);
    }

   stack -> SetMaximum(1.2 * max);
   TPad *pad = new TPad("pad1", "pad1", 0.0, 0.2, 1.0, 1.0);
   pad -> Draw();
   pad -> cd();
     stack -> Draw("HISTO");
   _totalMCUncth1 -> Draw("E2SAME");
   _totalMCUncShapeth1 -> Draw("E2SAME");
   _datagr -> Draw("P");
   /*   TCanvas *c = new TCanvas("test", "test");
   stack -> Draw("HISTO");
   _totalMCUncth1 -> Draw("E2SAME");
   _totalMCUncShapeth1 -> Draw("E2SAME");
   _datagr -> Draw("P");
   FormatHistograms();
   */return pad;//pad;
}

void CompoundHisto::CreateDataGraph()
{
  _datagr = new TGraphErrors(_datath1);
}


TPad * CompoundHisto::CreateRatioGraph()
{
  _ratioframe = (TH1F*) _totalMCth1 -> Clone("ratioframe");
  _ratioframe -> Reset("ICE");
  _ratioframe -> GetYaxis() -> SetRangeUser(0.4, 1.6);
  _ratioframe -> GetYaxis() -> SetTitle("Data/MC");
  _ratioframeshape = (TH1F*) _ratioframe -> Clone("ratioframeshape");

  TH1F * totalMCnoUnc = (TH1F*) _totalMCth1 -> Clone("totalMCnounc");
  for (unsigned char xbin = 0; xbin < _totalMCth1 -> GetNbinsX()+1; xbin ++)
    {    
      _ratioframe -> SetBinContent(xbin, 1.0);
      const float val = _totalMCth1 -> GetBinContent(xbin);
      totalMCnoUnc -> SetBinError(xbin, 0.0);
      if (val == 0)
	{
	  continue;
	}
      const float totalUnc = _totalMCUncth1 -> GetBinError(xbin)/val; 
      const float totalUncShape = _totalMCUncShapeth1 -> GetBinError(xbin)/val; 
      _ratioframeshape -> SetBinContent(xbin, _totalMCUncShapeth1 -> GetBinContent(xbin)/val);
      _ratioframeshape -> SetBinError(xbin, totalUncShape);
      _ratioframe -> SetBinContent(xbin, _totalMCUncth1 -> GetBinContent(xbin)/val);
      _ratioframe -> SetBinError(xbin, totalUnc);
    }
    
  TH1F * ratio = (TH1F*) _datath1 -> Clone("ratio");
  ratio -> Divide(totalMCnoUnc);
  _ratiogr = new TGraphAsymmErrors(ratio);
  TPad * pad = new TPad("pad2", "pad2", 0.0, 0.0, 1.0, 0.2);
  pad -> Draw();
  pad -> cd();
 _ratioframe -> Draw("E2");
  _ratioframeshape -> Draw("e2 same");
  _ratiogr -> Draw("P");

  //  delete ratio;
  //delete totalMCnoUnc;
  return pad;
}

TCanvas * CompoundHisto::CreateCombinedPlot()
{
  //  TCanvas *c = new TCanvas("compound", "compound", 500, 500);
  TCanvas *c = new TCanvas("compound", "compound", 500, 500);

  
  TPad * p1 = CreateMainPlot();
  p1 -> SetRightMargin(0.05);
  p1 -> SetLeftMargin(0.12);
  p1 -> SetTopMargin(0.06);
  p1 -> SetBottomMargin(0.01);
  //p1 -> Draw();
  c -> cd();
  TPad * p2 = CreateRatioGraph();
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
  return c;
}

 void CompoundHisto::Do()
{
    for(vector<Histo *> :: iterator it = _vbackgroundhistos.begin(); it != _vbackgroundhistos.end(); it ++)
    {
      (*it) -> ls();
      printf("integral %f\n", (*it) -> _th1 -> Integral());
    }
}

MinCompoundHisto::MinCompoundHisto(): size(1), h(nullptr)
{
}
 void MinCompoundHisto::Do()
{
    for(vector<Histo *> :: iterator it = _vbackgroundhistos.begin(); it != _vbackgroundhistos.end(); it ++)
    {
      (*it) -> ls();
      printf("integral %f\n", (*it) -> _th1 -> Integral());
    }
}


MinCompoundHisto::~MinCompoundHisto()
{

}
