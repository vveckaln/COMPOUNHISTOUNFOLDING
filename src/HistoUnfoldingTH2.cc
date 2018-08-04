#include "HistoUnfoldingTH2.hh"
#include "TChain.h"
#include "Definitions.hh"
ClassImp(HistoUnfoldingTH2);

HistoUnfoldingTH2::HistoUnfoldingTH2(): _th2(nullptr)
{

}

HistoUnfoldingTH2::HistoUnfoldingTH2(HistoUnfoldingTH2 *h, const char * name): _th2((TH2F*) h -> GetTH2() -> Clone(name)), HistoUnfolding((SampleDescriptor*) h)
{

}

HistoUnfoldingTH2::HistoUnfoldingTH2(const char* name, const char* title, Int_t nbinsx, const Float_t* xbins, Int_t nbinsy, const Float_t* ybins, SampleDescriptor * sd): HistoUnfolding(sd)
{
  //  printf("Calling HistoUnfoldingTH2::HistoUnfoldingTH2(const char* name, const char* title, Int_t nbinsx, const Float_t* xbins, Int_t nbinsy, const Float_t* ybins, SampleDescriptor * sd): HistoUnfolding(sd)\n");
  _th2 = new TH2F(TString(name) + "_" + GetTag(), title, nbinsx, xbins, nbinsy, ybins);
  _th2 -> SetDirectory(nullptr);
}

HistoUnfoldingTH2::HistoUnfoldingTH2(const char * name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup, SampleDescriptor * sd): HistoUnfolding(sd)
{
  _th2 = new TH2F(TString(name) + "_" + GetTag(), title, nbinsx, xlow, xup, nbinsy, ylow, yup);
  _th2 -> SetDirectory(nullptr);
}

void HistoUnfoldingTH2::FillFromTree(const char * sampletag, const char * jettag, const char *chargetag, const char * observable)
{
  TChain chain("migration");
  chain.Add(TString("$EOS/analysis_") + sampletag + ("/HADDmigration/migration_") + GetTag() + ".root/E_" + chargetag + "_" + jettag + "_" + "migration");
  chain.Add(TString("$EOS/analysis_") + sampletag + ("/HADDmigration/migration_") + GetTag() + ".root/M_" + chargetag + "_" + jettag + "_" + "migration");
  Float_t reco;
  Float_t gen;
  Float_t weight;
  chain.SetBranchAddress(TString(observable) + "_reco", & reco);
  chain.SetBranchAddress(TString(observable) + "_gen",  & gen);
  chain.SetBranchAddress("weight",          & weight);
  unsigned long coinc = 0;
  using namespace std;
  //  printf("GetTag() %s %s %s\n", GetTag(), string(GetTag()).c_str(), string(GetTag()).compare("MC13TeV_TTJets") == 0? "True" : " False");
  
  for (unsigned long event_ind = 0; event_ind < chain.GetEntries()/10; event_ind ++)
    {
      if (event_ind % 10000 == 0)
        printf("%u\r", event_ind);
      chain.GetEntry(event_ind);
      if (string(observable).compare("pull_angle") == 0)
	_th2 -> Fill(reco == -10 ? reco : TMath::Abs(reco), gen == -10.0 ? gen : TMath::Abs(gen), weight);
      /*      if (string(GetTag()).compare("MC13TeV_TTJets") == 0)
	{
	  printf("%lu %f\n", event_ind, reco);
	  getchar();
	  }*/
      if (string(observable).compare("pvmag") == 0)
	_th2 -> Fill(reco, gen, weight);
      //      _th2 -> Fill(pull_angle_reco == -10 ? pull_angle_reco : pull_angle_reco, pull_angle_gen == -10.0 ? pull_angle_gen : pull_angle_gen, weight);

    }
}

TH2F * HistoUnfoldingTH2::GetTH2()
{
  return _th2;
}

TH2F *& HistoUnfoldingTH2::GetTH2Ref()
{
  return _th2;
}

TH1 * HistoUnfoldingTH2::GetTH1(RecoLevelCode_t)
{
  throw "TH1F * HistoUnfoldingTH2::GetTH1() - no TH1F";
  return nullptr;
}

TH1 *& HistoUnfoldingTH2::GetTH1Ref(RecoLevelCode_t)
{
  static TH1 * h = nullptr;
  throw "TH1F * HistoUnfoldingTH2::GetTH1Ref() - no TH1F";
  return h;
}


TH1F * HistoUnfoldingTH2::Project(RecoLevelCode_t code, const char * name)
{
  const TString namestr = TString(name) + tag_recolevel[code];
  TH1F * proj = nullptr;
  switch (code)
    {
    case RECO:
      proj = (TH1F*) _th2 -> ProjectionX(namestr);
      proj -> SetDirectory(nullptr);
      break;
    case GEN:
      proj = (TH1F*) _th2 -> ProjectionY(namestr);
      proj -> SetDirectory(nullptr);
      break;
    }
  return proj;
}

void HistoUnfoldingTH2::lsbins(RecoLevelCode_t recocode)
{
  printf("listing bins of %s recocode %s\n", GetTitle(), tag_recolevel[recocode]);
  TH1F * h = Project(recocode, "listing");
  const unsigned short nbins = h -> GetNbinsX();
  for (unsigned char bin_ind = 0; bin_ind < nbins + 2; bin_ind ++)
    {
      printf("bin %u contents %f\n", bin_ind, h -> GetBinContent(bin_ind));
    }
  printf("end listing\n");
  delete h;
}

const char * HistoUnfoldingTH2::WhoAmI()
{
  return "HistoUnfoldingTH2";
}

HistoUnfoldingTH2::~HistoUnfoldingTH2()
{

}
