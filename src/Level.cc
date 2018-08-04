#include "CompoundHistoUnfolding.hh"
#include "TStyle.h"
#include "TColor.h"

ClassImp(CompoundHistoUnfolding::Level);

CompoundHistoUnfolding::Level::Level(): 
signal(nullptr), 
signalnominal(nullptr),
data(nullptr), 
totalMC(nullptr), 
totalMCUnc(nullptr), 
totalMCUncShape(nullptr), 
datambackground(nullptr),
cov(nullptr)
{

}

HistoUnfolding * CompoundHistoUnfolding::Level::GetHU(MOCode_t mo)
{
  switch(mo)
    {
    case DATA:
      return data;
    case SIGNALMO:
      return signal;
    case SIGNALNOMINALMO:
      return signalnominal;
    case DATAMBCKG:
      return datambackground;
    case TOTALMC:
      return totalMC;
    case TOTALMCUNC:
      return totalMCUnc;
    case TOTALMCUNCSHAPE:
      return totalMCUncShape;
    default:
      throw "HistoUnfolding *& CompoundHistoUnfolding::Level::GetHU(MOCode_t mo) - MO code incorrect\n";
    }
}
HistoUnfolding *& CompoundHistoUnfolding::Level::GetHURef(MOCode_t mo)
{
  switch(mo)
    {
    case SIGNALMO:
      return signal;
    case SIGNALNOMINALMO:
      return signalnominal;
    case DATA:
      return data;
    case DATAMBCKG:
      return datambackground;
    case TOTALMC:
      return totalMC;
    case TOTALMCUNC:
      return totalMCUnc;
    case TOTALMCUNCSHAPE:
      return totalMCUncShape;
    default:
      char e[256];
     
      sprintf(e, "%s %u\n", "HistoUnfolding *& CompoundHistoUnfolding::Level::GetHURef(MOCode_t mo) - MO code incorrect", mo);
      printf("%s\n", e);
      throw (const char *)(e);
    }
}

vector<HistoUnfolding *> * CompoundHistoUnfolding::Level::GetV(MOCode_t mo)
{
  switch(mo)
    {
    case SIGNALMO:
      return & _vsignalhistos;
    case DATA:
      return & _vdatahistos;
    case SYSMO:
      return & _vsyshistos;
    default:
      throw "HistoUnfolding *& CompoundHistoUnfolding::Level::Measurements::GetV(MOCode_t mo) - MO code incorrect\n";
      return nullptr;
    }
}

CompoundHistoUnfolding::Level::ProjectionDeco * CompoundHistoUnfolding::Level::GetProjectionDeco(RecoLevelCode_t recocode)
{
  switch(recocode)
    {
    case RECO:
      return & RECODECO;
    case GEN:
      return & GENDECO;
    }
}

void CompoundHistoUnfolding::Level::Format()
{
  printf("GetHU(SIGNALMO) -> WhoAmI() %s\n", GetHU(SIGNALMO) -> WhoAmI());
  printf("GetHU(SIGNALMO) -> GetColor() %p\n", GetHU(SIGNALMO) -> GetColor());
  printf("GetHU(SIGNALMO) -> _colornum) %u\n", GetHU(SIGNALMO) -> _colornum);
  if (TString(GetHU(SIGNALMO) -> WhoAmI()) == "HistoUnfoldingTH2")
    {
      printf("colornom IN %u\n", GetHU(SIGNALMO) -> _colornum);
      //  GetHU(SIGNALMO) -> GetTH2() -> SetFillColor(*/GetHU(SIGNALMO) -> GetColor() ? printf("probe A\n") TColor::GetColor(GetHU(SIGNALMO) -> GetColor()) : printf("probe B\n"); GetHU(SIGNALMO) -> _colornum)*/;
      GetHU(SIGNALMO) -> GetTH2() -> SetFillColor(GetHU(SIGNALMO) -> GetColor() ? TColor::GetColor(GetHU(SIGNALMO) -> GetColor()) : GetHU(SIGNALMO) -> _colornum);
    }
  if (TString(GetHU(SIGNALMO) -> WhoAmI()) == "HistoUnfoldingTH1")
    {   
      printf("GetHU(SIGNALMO) -> _colornum %u\n", GetHU(SIGNALMO) -> _colornum);
      //  GetHU(SIGNALMO) -> GetTH2() -> SetFillColor(*/GetHU(SIGNALMO) -> GetColor() ? printf("probe A\n") TColor::GetColor(GetHU(SIGNALMO) -> GetColor()) : printf("probe B\n"); GetHU(SIGNALMO) -> _colornum)*/;
      GetHU(SIGNALMO) -> GetTH1(RECO) -> SetFillColor(GetHU(SIGNALMO) -> GetColor() ? TColor::GetColor(GetHU(SIGNALMO) -> GetColor()) : GetHU(SIGNALMO) -> _colornum);
      GetHU(SIGNALMO) -> GetTH1(GEN) -> SetFillColor(GetHU(SIGNALMO) -> GetColor() ? TColor::GetColor(GetHU(SIGNALMO) -> GetColor()) : GetHU(SIGNALMO) -> _colornum);
    }
  gStyle -> SetHatchesLineWidth(1);
  if (GetHU(TOTALMCUNC) -> GetTH1(RECO))
    {
      GetHU(TOTALMCUNC) -> GetTH1(RECO) ->  SetFillColor(TColor::GetColor("#99d8c9"));
      GetHU(TOTALMCUNC) -> GetTH1(RECO) ->  SetFillStyle(3254);
    }
  if (GetHU(TOTALMCUNC) -> GetTH1(GEN))
    {
      GetHU(TOTALMCUNC) -> GetTH1(GEN) ->  SetFillColor(TColor::GetColor("#99d8c9"));
      GetHU(TOTALMCUNC) -> GetTH1(GEN) ->  SetFillStyle(3254);
    }
  if (GetHU(TOTALMCUNCSHAPE) -> GetTH1(RECO))
    {

      GetHU(TOTALMCUNCSHAPE) -> GetTH1(RECO) ->  SetFillColor(TColor::GetColor("#d73027"));
      GetHU(TOTALMCUNCSHAPE) -> GetTH1(RECO) ->  SetFillStyle(3254);
    }
  if (GetHU(TOTALMCUNCSHAPE) -> GetTH1(GEN))
    {

      GetHU(TOTALMCUNCSHAPE) -> GetTH1(GEN) ->  SetFillColor(TColor::GetColor("#d73027"));
      GetHU(TOTALMCUNCSHAPE) -> GetTH1(GEN) ->  SetFillStyle(3254);
    }
}

CompoundHistoUnfolding::Level::~Level()
{

}
