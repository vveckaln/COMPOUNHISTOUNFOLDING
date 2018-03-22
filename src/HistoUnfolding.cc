#include "HistoUnfolding.hh"
#include "TChain.h"

ClassImp(HistoUnfolding);

HistoUnfolding::HistoUnfolding(): size(0), _th2(nullptr)
{

}

HistoUnfolding::HistoUnfolding(TH2F * h, SampleDescriptor * sd): size(1), _th2(h), SampleDescriptor(*sd)
{

}

HistoUnfolding::HistoUnfolding(const char* name, const char* title, Int_t nbinsx, const Float_t* xbins, Int_t nbinsy, const Float_t* ybins, SampleDescriptor * sd): size(1), SampleDescriptor(*sd)
{
  _th2 = new TH2F(TString(name) + "_" + _tag, title, nbinsx, xbins, nbinsy, ybins);
}

HistoUnfolding::HistoUnfolding(const char * name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup, SampleDescriptor * sd): size(1), SampleDescriptor(*sd)
{
  _th2 = new TH2F(TString(name) + "_" + _tag, title, nbinsx, xlow, xup, nbinsy, ylow, yup);
}

void HistoUnfolding::FillFromTree()
{
  TChain chain("migration");
  chain.Add(TString("$EOS/analysis/migration_") + _tag + ".root/E_migration");
  chain.Add(TString("$EOS/analysis/migration_") + _tag + ".root/M_migration");
  Float_t pull_angle_reco;
  Float_t weight;
  Float_t pull_angle_gen;
  chain.SetBranchAddress("pull_angle_reco", &pull_angle_reco);
  chain.SetBranchAddress("weight",          &weight);
  chain.SetBranchAddress("pull_angle_gen",  &pull_angle_gen);
  unsigned long coinc = 0;
  for (unsigned long event_ind = 0; event_ind < chain.GetEntries(); event_ind ++)
    {
      if (event_ind % 10000 == 0)
        printf("%u\r", event_ind);
      chain.GetEntry(event_ind);
            _th2 -> Fill(pull_angle_reco == -10 ? pull_angle_reco : TMath::Abs(pull_angle_reco), pull_angle_gen == -10.0 ? pull_angle_gen : TMath::Abs(pull_angle_gen), weight);
      //      _th2 -> Fill(pull_angle_reco == -10 ? pull_angle_reco : pull_angle_reco, pull_angle_gen == -10.0 ? pull_angle_gen : pull_angle_gen, weight);

    }
}

HistoUnfolding::~HistoUnfolding()
{

}
