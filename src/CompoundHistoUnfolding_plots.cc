#include "CompoundHistoUnfolding.hh"
#include "TFile.h"
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
#include <regex>
ClassImp(CompoundHistoUnfolding);

THStack * CompoundHistoUnfolding::CreateTHStack(RecoLevelCode_t recocode, ResultLevelCode_t resultcode)
{
  Format();
  // for (vector<HistoUnfolding *>::iterator it = _vbackgroundhistos.begin(); it !=  _vbackgroundhistos.end(); it ++)
  //   {
  //     ApplyScaleFactor((*it) -> GetTH2());
  //   }
  // ApplyScaleFactor(GetLevel(IN) -> GetHU(DATA) -> GetTH2());

  Level * level = GetLevel(resultcode);
  Level::ProjectionDeco * projdeco = level -> GetProjectionDeco(recocode);
  TLegend *& legend = projdeco -> legend;
  if (legend)
    {
      delete legend;
      legend = nullptr;
    }

  legend = new TLegend(0.52, 0.47, 0.94, 0.93);
  if (projdeco -> _datagr)
    {
      legend -> AddEntry(projdeco -> _datagr, resultcode == IN ? "data" : level -> GetHU(DATAMO) -> GetTH1(recocode) -> GetTitle());
    }
  TH1 * hsig = GetLevel(IN)/*level*/ -> GetHU(SIGNAL/*SIGNALPROXY*/, resultcode) -> Project(recocode, CreateName("sigstack"), "", _YaxisTitle);
  TH1 * htotalmc = GetLevel(IN) -> GetHU(TOTALMCSIGNALPROXY, IN/*resultcode*/) -> Project(recocode, CreateName("totalmc"), "", _YaxisTitle);
  if (recocode == RECO and resultcode == IN)
    {
      ApplyScaleFactor(hsig);
      ApplyScaleFactor(htotalmc);
    }
  NormaliseToBinWidth(htotalmc);
  NormaliseToBinWidth(hsig);
  // RejectNonReco(htotalmc);
  // RejectNonReco(hsig);

  const float totalmcarea(calculatearea(htotalmc));
  // if (recocode == RECO and resultcode == IN)
  //   {
  //     printf("printing signal\n");
  //     hsig -> Print("all");
  //     printf("printing total mc\n");
  //     htotalmc -> Print("all");
  //     //getchar();
  //   }
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
      if (recocode == RECO and resultcode == IN)
	ApplyScaleFactor(hbackground);
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
  //totalMCUnc -> Print("all");
 
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
    {
      maximum = 1.5 *stack -> GetMaximum();
    }
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
    {
      hframe -> SetMinimum(0.85 * totalMCUnc -> GetMinimum());
    }
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
  //  totalMCUnc -> Print("all");
  totalMCUncShape -> Draw("E2SAME");
  if (projdeco -> _datagr)
    {
      //  projdeco -> _datagr -> Print("all");
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
    {
      return;
    }
  Level * level = GetLevel(resultcode);
  Level::ProjectionDeco * projdeco = level -> GetProjectionDeco(recocode);
  if (projdeco -> _datagr)
    {
      delete projdeco -> _datagr;
      projdeco -> _datagr = nullptr;
    }

  TH1 * data = level -> GetHU(DATAMO) -> Project(recocode, CreateName("datadg"), "", _YaxisTitle); //Equalised with DATAMBCKG in CreateTotalMC()
  //ApplyScaleFactor(data);

  NormaliseToBinWidth(data);			
  //  RejectNonReco(data);
  data -> Scale(1.0/calculatearea(data));
  // if (recocode == GEN and resultcode == OUT)
  //   {
  //     float integral = 0;
  //     for (unsigned char bind = 1; bind <= data -> GetNbinsX(); bind ++)
  // 	{
  // 	  const float w = data -> GetBinWidth(bind);
  // 	  const float c = data -> GetBinContent(bind);
  // 	  printf("%f %f\n", w, c);
  // 	  integral += w*c;
  // 	}
  //     printf("integral check %f\n", integral);
  //   }
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
  TH1 * ratio = resultcode == IN ? 
    level -> GetHU(TOTALMCSIGNALPROXY, resultcode) -> Project(recocode, "datamp"):
    GetLevel(IN) -> GetHU(TOTALMC, resultcode) -> Project(recocode, "datamp");
  if (not projdeco -> _ratioframe)
    {
      TH1 * htotalMC = level -> GetHU(TOTALMCSIGNALPROXY, resultcode) -> Project(recocode, CreateName("totalmcrg"), _XaxisTitle, "MC/data");
      TH1 * hdata = level -> GetHU(DATAMO) -> Project(recocode, CreateName("datadg"), "", _YaxisTitle);
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
	  const float val = hdata -> GetBinContent(xbin);//htotalMC -> GetBinContent(xbin);
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
      delete htotalMC;
      delete htotalMCnoUnc;
      delete hdata;
    }
  projdeco -> _ratioframe -> Draw("E2");
  projdeco -> _ratioframeshape -> Draw("e2 same");
  if (recocode == RECO and resultcode == IN or resultcode == OUT)
    {
	  
	  
      TH1 * data = level -> GetHU(DATAMO) -> Project(recocode, CreateName("datadg"), "", _YaxisTitle);
      ratio -> Divide(data);
      //ApplyScaleFactor(ratio);
      //NormaliseToBinWidth(ratio);
      for (unsigned char bind = 1; bind <= data -> GetNbinsX(); bind ++)
	{
	  data -> SetBinError(bind, data -> GetBinError(bind)/data -> GetBinContent(bind));
	  data -> SetBinContent(bind, 1.0);
	  data -> SetLineWidth(0);
	  data -> SetFillColor(17);
	}
      // data -> Print("all");
      data -> Draw("E2Same");
	  
      // projdeco -> _ratiogr = new TGraphAsymmErrors(ratio);
      // //delete data;
      // delete ratio;
    }
  if (projdeco -> _ratiogr)
    {
      delete projdeco -> _ratiogr;
      projdeco -> _ratiogr = nullptr;
    }
  ratio -> SetLineWidth(2);
   // ratio -> Print("all");
  ratio -> Draw("SAME");
  // printf("ratio line color %u width %u\n", ratio -> GetLineColor(), ratio -> GetLineWidth());
  // getchar();
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
  TIter next(p1 -> GetListOfPrimitives());
  TObject * obj = nullptr;
  while ((obj = next()) and TString(obj -> ClassName()) != "TLegend")
    {
    }
  TLegend * legend = (TLegend *)obj;
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

void CompoundHistoUnfolding::BottomLineTest()
{
  TH1 * hdatambckgin = GetLevel(IN)/*level*/ -> GetHU(DATAMBCKG) -> Project(RECO, "datambckginbottomline", "", _YaxisTitle);
  ApplyScaleFactor(hdatambckgin);
  hdatambckgin -> Print("all");
  hdatambckgin -> SetLineWidth(2);
  NormaliseToBinWidth(hdatambckgin);
  TH1 * hdatambckgout = GetLevel(OUT)/*level*/ -> GetHU(DATAMBCKG) -> Project(RECO, "datambckgoutbottomline", "", _YaxisTitle);
  hdatambckgout -> Print("all");
  hdatambckgout -> SetLineColor(kRed);
  hdatambckgout -> SetLineWidth(2);
  NormaliseToBinWidth(hdatambckgout);
  TH1 * hdataframe = (TH1*) hdatambckgin -> Clone();
  hdataframe -> Reset("ICE");
  TCanvas * canvas = new TCanvas;
  TPad * pad1 = new TPad(CreateName("bottomlinetestpad1"), CreateTitle("bottomlinetestpad1"), 0.0, 0.3, 1.0, 1.0);
  pad1 -> Draw();
  pad1 -> cd();
  pad1 -> SetRightMargin(0.05);
  pad1 -> SetLeftMargin(0.2);
  pad1 -> SetTopMargin(0.06);
  pad1 -> SetBottomMargin(0.03);

  THStack * stack = new THStack;
  stack -> Add(hdatambckgin, "HIST");
  stack -> Add(hdatambckgout, "HIST");
  hdataframe -> SetMinimum(0.0);
  hdataframe -> SetMaximum(stack -> GetMaximum("nostack") * 1.4);
  hdataframe -> GetYaxis() -> SetNdivisions(5);
  hdataframe -> GetXaxis() -> SetLabelSize(0.0);
  hdataframe -> GetYaxis() -> SetLabelSize(0.20*0.3/0.7);
  hdataframe -> GetYaxis() -> SetTitleSize(0.21*0.3/0.7);
  
  hdataframe -> GetYaxis() -> SetTitleOffset(0.42*0.7/0.3);
  hdataframe -> Draw();
  stack -> Draw("SAMEnostack");
  TLegend * legend = new TLegend(0.6, 0.6, 0.8, 0.8);
  legend -> AddEntry(hdatambckgin, "data - b IN", "l");
  legend -> AddEntry(hdatambckgout, "data - b f unf", "l");
  legend -> SetLineWidth(0);
  legend -> SetTextSize(0.06);
  legend -> Draw("SAME");
  TLatex * chi2s = new TLatex;
  char chi[64];
  sprintf(chi, "#chi_{in}^{2}=%.2f, dof=%u", GetChi(signaltag, IN, RECO, NOMINAL), _nbinsx);
  chi2s -> DrawLatex(0.2, 6000, chi);
  sprintf(chi, "#chi_{unf}^{2}=%.2f, dof=%u", GetChi(signaltag, OUT, GEN, NOMINAL), _nbinsy);
  chi2s -> DrawLatex(0.2, 4000, chi);
  
  TPad * pad2 = new TPad(CreateName("bottomlinetestpad2"), CreateTitle("bottomlinetestpad2"), 0.0, 0.0, 1.0, 0.3);
  canvas -> cd();
  pad2 -> Draw();
  pad2 -> cd();
  pad2 -> SetBottomMargin(0.5);
  pad2 -> SetRightMargin(0.05);
  pad2 -> SetLeftMargin(0.2);
  pad2 -> SetTopMargin(0.1);
  pad2 -> SetGridx(kFALSE);
  pad2 -> SetGridy(kTRUE);
  canvas -> SetBottomMargin(0.0);
  canvas -> SetLeftMargin(0.0);
  canvas -> SetTopMargin(0);
  canvas -> SetRightMargin(0.00);
  TH1 * hdatambckginframe = (TH1*) hdatambckgin -> Clone();
  hdatambckginframe -> Divide(hdatambckgout);
  hdatambckginframe -> GetYaxis() -> SetTitle("#frac{data - b}{data - b f unf}");
  hdatambckginframe -> SetMaximum(1.2);
  hdatambckginframe -> SetMinimum(0.8);
  hdatambckginframe -> GetYaxis() -> SetNdivisions(3);
  hdatambckginframe -> GetXaxis() -> SetTitle(_XaxisTitle);
  hdatambckginframe -> GetXaxis() -> SetLabelSize(0.2);
  hdatambckginframe -> GetXaxis() -> SetTitleSize(0.2);
  hdatambckginframe -> GetYaxis() -> SetLabelSize(0.2);
  hdatambckginframe -> GetYaxis() -> SetTitleSize(0.1);
  
  hdatambckginframe -> GetYaxis() -> SetTitleOffset(0.42*0.21/0.1);
  hdatambckginframe -> Draw();

  canvas -> SaveAs(TString(_folder) + "/bottomlinetest.png");
}

void CompoundHistoUnfolding::CRComparisonPlots()
{
    TH1 * hdatambckgout = GetLevel(OUT)/*level*/ -> GetHU(DATAMBCKG) -> Project(GEN, "datambckgoutbottomline", "", _YaxisTitle);
    
    NormaliseToBinWidth(hdatambckgout);
    hdatambckgout -> Scale(1.0/calculatearea(hdatambckgout));
    TGraphErrors * datagr = new TGraphErrors(hdatambckgout);
    datagr -> SetMarkerStyle(20);
    datagr -> SetMarkerSize(1.1);
    TH1 * MCsignalpred = GetLevel(IN) -> GetHU(SIGNALMO) -> Project(GEN, "signal");
    NormaliseToBinWidth(MCsignalpred);
    MCsignalpred -> Scale(1.0/calculatearea(MCsignalpred));
    MCsignalpred -> SetLineWidth(2);
    Color_t colors[3] = {kGreen, kRed, kMagenta};
    const char * tags[3] = {"MC13TeV_TTJets_erdON", "MC13TeV_TTJets_qcdBased", "MC13TeV_TTJets_gluonMove"}; 
    for (ResultLevelCode_t resultlevel = 0; resultlevel < 2; resultlevel ++)
      {
	TH1 * CRsyst[3] = {nullptr, nullptr, nullptr};
	TCanvas * canvas = new TCanvas;
	TPad * pad1 = new TPad(CreateName(TString("CRComparisonplots_p1_") + tag_resultlevel[resultlevel]), CreateTitle(TString("bottomlinetestpad1 ") + tag_resultlevel[resultlevel]), 0.0, 0.3, 1.0, 1.0);
	pad1 -> Draw();
	pad1 -> cd();
	pad1 -> SetRightMargin(0.05);
	pad1 -> SetLeftMargin(0.2);
	pad1 -> SetTopMargin(0.15);
	pad1 -> SetBottomMargin(0.03);
	TH1 * framepad1 = (TH1*) MCsignalpred -> Clone();
	framepad1 -> Reset("ICE");
	framepad1 -> GetYaxis() -> SetTitle(_YaxisTitle);
	framepad1 -> GetYaxis() -> SetLabelSize(0.2*0.3/0.7);
	framepad1 -> GetYaxis() -> SetTitleSize(0.2*0.3/0.7);
	framepad1 -> GetYaxis() -> SetTitleOffset(0.3*0.7/0.3);
	framepad1 -> GetXaxis() -> SetLabelSize(0);
	
	TPad * pad2 = new TPad(CreateName(TString("bottomlinetest_p2_") + tag_resultlevel[resultlevel]), CreateTitle(TString("bottomlinetestpad2 ") + tag_resultlevel[resultlevel]), 0.0, 0.0, 1.0, 0.3);
	canvas -> cd();
	pad2 -> Draw();
	pad2 -> cd();
	pad2 -> SetBottomMargin(0.5);
	pad2 -> SetRightMargin(0.05);
	pad2 -> SetLeftMargin(0.2);
	pad2 -> SetTopMargin(0.1);
	pad2 -> SetGridx(kFALSE);
	pad2 -> SetGridy(kTRUE);

	THStack * stack = new THStack;
	stack -> Add(MCsignalpred, "HIST");
	TLegend * legend = new TLegend(0.5, 0.5, 0.8, 0.8);
	legend -> SetLineWidth(0);
	legend -> AddEntry(datagr, resultlevel == IN ? "data unf" : "data unf", "p");
	legend -> AddEntry(MCsignalpred, GetLevel(IN) -> GetHU(SIGNALMO) -> GetTitle(), "l");
	for (unsigned char crind = 0; crind < 3; crind ++)
	  {
	    CRsyst[crind] = GetLevel(resultlevel) -> GetSys(
							    regex_replace(tags[crind], regex("MC13TeV_TTJets"), signaltag).c_str(), signaltag) -> Project(GEN);
	    CRsyst[crind] -> SetLineColor(colors[crind]); 
	    CRsyst[crind] -> SetLineWidth(2);
	    NormaliseToBinWidth(CRsyst[crind]);
	    CRsyst[crind] -> Scale(1.0/calculatearea(CRsyst[crind]));
	    legend -> AddEntry(CRsyst[crind], 
			       resultlevel == IN ? 
			       GetLevel(resultlevel) -> GetSys(regex_replace(tags[crind], regex("MC13TeV_TTJets"), signaltag).c_str(), signaltag) -> GetTitle() :
			       (TString(GetLevel(resultlevel) -> GetSys(regex_replace(tags[crind], regex("MC13TeV_TTJets"), signaltag).c_str(), signaltag) -> GetTitle()) + " unf").Data(), 
			       "l");
	    stack -> Add(CRsyst[crind], "HIST");
	  }
	legend -> SetTextSize(0.06);
	pad1 -> cd();
	framepad1 -> SetMinimum(0.8);
	framepad1 -> SetMaximum(stack -> GetMaximum("nostack") * 1.6);
	framepad1 -> GetYaxis() -> SetNdivisions(5);
	framepad1 -> Draw();
	stack -> Draw("nostackSAME");
	const unsigned int Npoints = datagr->GetN();
	double ey;
	for (unsigned int ind = 0; ind < Npoints; ind++) 
	  {
	    ey = datagr -> GetErrorY(ind);
	    datagr -> SetPointError ( ind, 0.0, ey);	
	  }
	datagr -> Draw("SAMEP");
	
	legend -> Draw("SAME");
	TH1* pad2h[4] = {(TH1 *) MCsignalpred -> Clone(), (TH1 *) CRsyst[0] -> Clone(), (TH1 * ) CRsyst[1] -> Clone(), (TH1 *) CRsyst[2] -> Clone()};
	TH1 * pad2frame = (TH1*) MCsignalpred -> Clone("pad2frame");
	pad2frame -> Reset("ICE");
	pad2frame -> SetMinimum(0.8);
	pad2frame -> SetMaximum(1.2);
	pad2frame -> GetXaxis() -> SetTitle(_XaxisTitle);
	pad2frame -> GetXaxis() -> SetLabelSize(0.2);
	pad2frame -> GetXaxis() -> SetTitleSize(0.2);
	pad2frame -> GetXaxis() -> SetTitleOffset(0.9);
	pad2frame -> GetYaxis() -> SetTitle("MC/data");
	pad2frame -> GetYaxis() -> SetTitleSize(0.2);
	pad2frame -> GetYaxis() -> SetLabelSize(0.2);

	pad2frame -> GetYaxis() -> SetTitleOffset(0.3);
	pad2frame -> GetYaxis() -> SetNdivisions(3);
	pad2 -> cd();
	pad2frame -> Draw();
	for (unsigned char pad2ind = 0; pad2ind < 4; pad2ind ++)
	  {
	    pad2h[pad2ind] -> Divide(hdatambckgout);
	    pad2h[pad2ind] -> Draw("SAME");
	  }
	canvas -> SetBottomMargin(0.0);
	canvas -> SetLeftMargin(0.0);
	canvas -> SetTopMargin(0);
	canvas -> SetRightMargin(0.00);

	canvas -> SaveAs(TString(_folder) + "/CRcomparison_" + tag_resultlevel[resultlevel] + ".png");
      }

}
