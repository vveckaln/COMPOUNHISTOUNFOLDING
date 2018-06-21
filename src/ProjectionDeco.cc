#include "CompoundHistoUnfolding.hh"
#include "TColor.h"

ClassImp(CompoundHistoUnfolding::Level::ProjectionDeco)

CompoundHistoUnfolding::Level::ProjectionDeco::ProjectionDeco():
_ratiogr(nullptr), 
  _grtotalMC(nullptr),
  _datagr(nullptr),
  _ratioframe(nullptr), 
  _ratioframeshape(nullptr), 
  legend(nullptr)
{

}

void CompoundHistoUnfolding::Level::ProjectionDeco::Format()
{
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
      _ratiogr -> SetMarkerStyle(20);
      _ratiogr -> SetMarkerSize(1.0);
      _ratiogr -> SetMarkerColor(kBlack);
      _ratiogr -> SetLineColor(kBlack);
      _ratiogr -> SetLineWidth(2);
    }

  if (_datagr)
    {
      _datagr -> SetMarkerStyle(20);
      _datagr -> SetMarkerSize(1.0);
      _datagr -> SetMarkerColor(kBlack);
      _datagr -> SetLineColor(kBlack);
      _datagr -> SetLineWidth(2);  
    }

}

CompoundHistoUnfolding::Level::ProjectionDeco::~ProjectionDeco()
{

}
