#include "HistoUnfoldingTH1.hh"
#include "Definitions.hh"
ClassImp(HistoUnfoldingTH1);

HistoUnfoldingTH1::HistoUnfoldingTH1():  _th1RECO(nullptr), _th1GEN(nullptr)
{

}

HistoUnfoldingTH1::HistoUnfoldingTH1(SampleDescriptor * sd):  _th1RECO(nullptr), _th1GEN(nullptr), HistoUnfolding(sd)
{
  //  printf("calling HistoUnfoldingTH1::HistoUnfoldingTH1(SampleDescriptor * sd)\n");
}

HistoUnfoldingTH1::HistoUnfoldingTH1(HistoUnfoldingTH1 *h, const char * name): 
_th1RECO((TH1F*) h -> GetTH1(RECO) -> Clone(TString(name) + "_reco")), 
_th1GEN((TH1F*) h -> GetTH1(GEN) -> Clone(TString(name) + "_gen")), 
HistoUnfolding((SampleDescriptor*) h)
{

}


/*
HistoUnfoldingTH1::HistoUnfoldingTH1(const char* name, const char* title, Int_t nbins, const Float_t* xbins, SampleDescriptor * sd): SampleDescriptor(*sd)
{
  _th1 = new TH1F(TString(name) + "_" + _tag, title, nbins, xbins);
}

HistoUnfoldingTH1::HistoUnfoldingTH1(const char * name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, SampleDescriptor * sd): SampleDescriptor(*sd)
{
  _th1 = new TH1F(TString(name) + "_" + _tag, title, nbinsx, xlow, xup);
}
*/
TH1F *HistoUnfoldingTH1::Project(RecoLevelCode_t code, const char * name, const char * XaxisTitle, const char * YaxisTitle)
{
  const TString namestr = TString(name) + tag_recolevel[code];
  TH1F * proj = nullptr;
  switch (code)
    {
    case RECO:
      proj = (TH1F*) _th1RECO -> Clone(namestr);
      proj -> SetDirectory(nullptr);
      break;
    case GEN:
      proj = (TH1F*) _th1GEN -> Clone(namestr);
      proj -> SetDirectory(nullptr);
      break;
    }
  proj -> GetXaxis() -> SetTitle(XaxisTitle);
  proj -> GetYaxis() -> SetTitle(YaxisTitle);
  return proj;
}

TH1 *& HistoUnfoldingTH1::GetTH1Ref(RecoLevelCode_t code)
{
  switch (code)
    {
    case RECO:
      return _th1RECO;
    case GEN:
      return _th1GEN;
    }
}

TH1 * HistoUnfoldingTH1::GetTH1(RecoLevelCode_t code)
{
  switch (code)
    {
    case RECO:
      return _th1RECO;
    case GEN:
      return _th1GEN;
    }
}

TH2F * HistoUnfoldingTH1::GetTH2()
{
  throw "TH2F *& HistoUnfoldingTH1::GetTH2(RecoLevelCode_t code) - no TH2F";
  return nullptr;
}

TH2F *& HistoUnfoldingTH1::GetTH2Ref()
{
  static TH2F * h = nullptr;
  throw "TH2F *& HistoUnfoldingTH1::GetTH2Ref(RecoLevelCode_t code) - no TH2F";
  return h;
}

void HistoUnfoldingTH1::lsbins(RecoLevelCode_t recocode)
{
  printf("listing bins of %s recocode %s\n", GetTitle(), tag_recolevel[recocode]);
  TH1 * h = GetTH1(recocode);
  const unsigned short nbins = h -> GetNbinsX();
  for (unsigned char bin_ind = 0; bin_ind < nbins + 2; bin_ind ++)
    {
      printf("bin %u contents %f\n", bin_ind, h -> GetBinContent(bin_ind));
    }
  printf("End listing\n");
}

const char * HistoUnfoldingTH1::WhoAmI()
{
  return "HistoUnfoldingTH1";
}

HistoUnfoldingTH1::~HistoUnfoldingTH1()
{

}
