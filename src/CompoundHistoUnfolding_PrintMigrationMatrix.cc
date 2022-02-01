#include "CompoundHistoUnfolding.hh"
#include "TH1D.h"
#include "TRandom3.h"
#include "TMath.h"
#include "TPaveText.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TExec.h"
#include "TROOT.h"
ClassImp(CompoundHistoUnfolding)

void CompoundHistoUnfolding::PrintMigrationMatrix()
{
  TH2 * migrationmatrix = (TH2*) GetLevel(IN) -> GetHU(SIGNALPROXY, IN) -> GetTH2() -> Clone("migrationmatrix");
  printf("before %u %u\n", migrationmatrix -> GetNbinsX(), migrationmatrix -> GetNbinsY());
  const unsigned char ratio = migrationmatrix -> GetNbinsX()/migrationmatrix -> GetNbinsY();
  migrationmatrix -> RebinX(ratio);
  printf("after  %u %u\n", migrationmatrix -> GetNbinsX(), migrationmatrix -> GetNbinsY());
  TH1 * recoonly = migrationmatrix -> ProjectionX("recoonly");
  recoonly -> Reset("ICE");
  recoonly -> SetTitle("#frac{N_{RECO#wedge GEN}}{N_{RECO}}");
  TH1 * genonly = migrationmatrix -> ProjectionX("genonly");
  genonly -> Reset("ICE");
  TH1 * hframecorrectioncoefficients = (TH1*) genonly -> Clone("correctioncoefficients");
  hframecorrectioncoefficients -> SetXTitle(_XaxisTitle);
  hframecorrectioncoefficients -> SetYTitle("ratio");
  genonly -> SetLineColor(kRed);
  genonly -> SetTitle("#frac{N_{RECO#wedge GEN}}{N_{GEN}}");
  const unsigned char nbins = migrationmatrix -> GetNbinsX();
  for (unsigned char bind = 1; bind <= nbins; bind ++)
    {
      recoonly -> SetBinContent(bind, migrationmatrix -> Integral(bind, bind, 1, migrationmatrix -> GetNbinsY())/migrationmatrix -> Integral(bind, bind, 0, migrationmatrix -> GetNbinsY() + 1));
      genonly  -> SetBinContent(bind, migrationmatrix -> Integral(1, migrationmatrix -> GetNbinsX(), bind, bind)/migrationmatrix -> Integral(0, migrationmatrix -> GetNbinsX() + 1, bind, bind));
    }
  TLegend legend(0.6, 0.6, 0.85, 0.85);
  legend.SetBorderSize(0);
  legend.AddEntry(recoonly, recoonly -> GetTitle()); 
  legend.AddEntry(genonly,  genonly -> GetTitle()); 
  legend.SetTextSize(0.05);
  TCanvas * c = new TCanvas;

  THStack stack;

  stack.Add(recoonly, "HIST");
  stack.Add(genonly, "HIST");
  hframecorrectioncoefficients -> SetMaximum(stack.GetMaximum("nostack") * 1.6);
  hframecorrectioncoefficients -> SetMinimum(0.0);
  hframecorrectioncoefficients -> Draw();
  recoonly -> Print("all");
  genonly -> Print("all");
  stack.Draw("nostackSAME");
  legend.Draw("SAME");
  c -> SaveAs("output/correctioncoefficients.png");
  delete c;
  delete genonly;
  delete recoonly;
  TCanvas * cmigrationmatrix = new TCanvas("cmm", "cmm", 600, 600);
  cmigrationmatrix -> SetLeftMargin(0.15);
  cmigrationmatrix -> SetRightMargin(0.2);
  TExec * exgreen = new TExec("exgreen", "palettegreen()");
  migrationmatrix -> Draw("COLZ");
  exgreen -> Draw();
  migrationmatrix -> Draw("COLZSAME");
  migrationmatrix -> GetZaxis() -> SetTitle("%");
  migrationmatrix -> SetMinimum(0.0);
  migrationmatrix -> SetMaximum(100.0);
  for (unsigned char bindx = 1; bindx <= migrationmatrix -> GetNbinsX(); bindx ++)
    {
      const float integral = migrationmatrix -> Integral(bindx, bindx, 1, migrationmatrix -> GetNbinsX());
      for (unsigned char bindy = 1; bindy <= migrationmatrix -> GetNbinsY(); bindy ++)
	{
	  const float error = migrationmatrix -> GetBinError(bindx, bindy);
	  //	  printf("error %f %f\n", error, TMath::Sqrt(migrationmatrix-> GetBinError(bindx, bindy)));
	  migrationmatrix -> SetBinContent(bindx, bindy, 100.0*migrationmatrix -> GetBinContent(bindx, bindy) / integral);
	  migrationmatrix -> SetBinError(bindx, bindy, 100.0*error/ integral);
	  TAxis * xaxis = migrationmatrix -> GetXaxis();
	  TAxis * yaxis = migrationmatrix -> GetYaxis();
	  TString pavestring = TString(Form("%.0f", migrationmatrix -> GetBinContent(bindx, bindy))) + " #pm " + Form("%.0f", migrationmatrix -> GetBinError(bindx, bindy)); 
	  TString pavestring1line = TString(Form("%.2f", migrationmatrix -> GetBinContent(bindx, bindy)));// + "\\ #pm " + Form("%.0f", migrationmatrix -> GetBinError(bindx, bindy)); 
	  TString pavestring2line = TString("#pm ") + Form("%.2f", migrationmatrix -> GetBinError(bindx, bindy)); 

	  TLatex *  latex1line  = new TLatex(xaxis -> GetBinCenter(bindx), yaxis -> GetBinCenter(bindy) + 0.028*(migrationmatrix -> GetYaxis() -> GetXmax() - migrationmatrix -> GetYaxis() -> GetXmin()), pavestring1line);
	  latex1line -> SetTextAlign(22);

	  TLatex *  latex2line  = new TLatex(xaxis -> GetBinCenter(bindx), yaxis -> GetBinCenter(bindy) - 0.028*(migrationmatrix -> GetYaxis() -> GetXmax() - migrationmatrix -> GetYaxis() -> GetXmin()), pavestring2line);
	  latex2line -> SetTextAlign(22);
	  //	  latex2line -> SetTextColor(kRed);
	  // printf(" x %u y %u\n", bindx, bindy);
	  
	  // printf("x bw %f aw%f r %f\n", 
	  // 	 migrationmatrix -> GetXaxis() -> GetBinWidth(bindx), 
	  // 	 migrationmatrix -> GetXaxis() -> GetXmax() - migrationmatrix -> GetXaxis() -> GetXmin(), 
	  // 	 migrationmatrix -> GetXaxis() -> GetBinWidth(bindx)/(migrationmatrix -> GetXaxis() -> GetXmax() - migrationmatrix -> GetXaxis() -> GetXmin()));

	  // printf("y bw %f aw%f r %f\n", 
	  // 	 migrationmatrix -> GetYaxis() -> GetBinWidth(bindy), 
	  // 	 migrationmatrix -> GetYaxis() -> GetXmax() - migrationmatrix -> GetYaxis() -> GetXmin(), 
	  // 	 migrationmatrix -> GetYaxis() -> GetBinWidth(bindy)/(migrationmatrix -> GetYaxis() -> GetXmax() - migrationmatrix -> GetYaxis() -> GetXmin()));

	  if (migrationmatrix -> GetXaxis() -> GetBinWidth(bindx)/(migrationmatrix -> GetXaxis() -> GetXmax() - migrationmatrix -> GetXaxis() -> GetXmin()) > 0.1 and
	      migrationmatrix -> GetYaxis() -> GetBinWidth(bindy)/(migrationmatrix -> GetYaxis() -> GetXmax() - migrationmatrix -> GetYaxis() -> GetXmin()) > 0.1)
	    {
	      // printf("drawing x %u y %u\n", bindx, bindy);
	      latex1line -> Draw();
	      latex2line -> Draw("SAME");
	    }
	  else{}
	    //	    printf("skipping x %u y %u\n", bindx, bindy);
	  
	  // OAlatex1line -> SetTextSize(0.75 * latex1line -> GetTextSize());
	  // latex2line -> SetTextSize(0.75 * latex2line -> GetTextSize());
	  latex1line -> SetTextSize(0.6 * latex1line -> GetTextSize());
	  latex2line -> SetTextSize(0.6 * latex2line -> GetTextSize());
 	  // printf("text size %f %f\n", 


	  // TPaveText * pavetext = new TPaveText(xaxis -> GetBinLowEdge(bindx), yaxis -> GetBinLowEdge(bindy), xaxis -> GetBinUpEdge(bindx), yaxis -> GetBinUpEdge(bindy));
	  // TString pavestring1line = TString(Form("%.0f", migrationmatrix -> GetBinContent(bindx, bindy)));// + " #pm " + Form("%.0f", migrationmatrix -> GetBinError(bindx, bindy)); 
	  // pavetext -> AddText(pavestring1line);
	  // pavetext -> AddText(pavestring2line);
	  // TText * text1line = pavetext -> GetLineWith(pavestring1line.Data());
	  // TText * text2line = pavetext -> GetLineWith(pavestring2line.Data());
	  // //printf("text %p pavestring %s\n", text, pavestring.Data());
	  // text1line -> SetTextSize(0.02);
	  // text2line -> SetTextSize(0.02);
	  // pavetext -> SetBorderSize(0.0);
	  // pavetext -> SetFillStyle(0);
	  // pavetext -> Draw();
	}

    }
  const unsigned char n = 6;
  // Int_t colours[n] = {kGreen -10, kGreen -8, kGreen - 6, kGreen - 4, kGreen - 2, kGreen };
  // gStyle -> SetPalette(n, colours);
  cmigrationmatrix -> SaveAs("output/formattedmigrationmatrix.png");
  // getchar();
  delete migrationmatrix;
}

