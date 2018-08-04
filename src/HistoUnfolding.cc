#include "HistoUnfolding.hh"
ClassImp(HistoUnfolding);

HistoUnfolding::HistoUnfolding(): SampleDescriptor()
{
}

HistoUnfolding::HistoUnfolding(SampleDescriptor * sd): SampleDescriptor(*sd)
{
  //  printf("Callin HistoUnfolding::HistoUnfolding(SampleDescriptor * sd): SampleDescriptor(*sd)\n");
}
/*
TH1F * HistoUnfolding::Project(RecoLevelCode_t code)
{
  return nullptr;
}

const char * HistoUnfolding::WhoAmI()
{
  return "HistoUnfolding";
}

TH2F * HistoUnfolding::GetTH2()
{
  return nullptr;
}
TH1F * HistoUnfolding::GetTH1(RecoLevelCode_t)
{
  return nullptr;
}

TH2F *& HistoUnfolding::GetTH2Ref()
{
  static TH2F *h = nullptr;
  return h;
}

TH1F *& HistoUnfolding::GetTH1Ref(RecoLevelCode_t)
{
  static TH1F *h = nullptr;
  return h;
}
*/
HistoUnfolding::~HistoUnfolding()
{

}
