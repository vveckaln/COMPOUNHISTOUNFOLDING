#include "CompoundHistoUnfolding.hh"
#include "TH1D.h"
#include "TRandom3.h"
#include "TMath.h"
ClassImp(CompoundHistoUnfolding)
TH1 * CompoundHistoUnfolding::GetSignalProxy(ResultLevelCode_t resultcode, RecoLevelCode_t recolevel, const char * sample, SysTypeCode_t sys, const char * tag, const bool pruned)
{
  if (resultcode == IN)
    {
      if (TString(sample) == signaltag)
	{
	  return GetLevel(resultcode) -> GetHU(SIGNALPROXY, resultcode) -> Project(recolevel, CreateName("sig"));
	}
      else
	{
	  return GetBackgroundH(sample) -> Project(recolevel);
	}
    }
  if (resultcode == OUT)
    {
      TH1 * ret = nullptr;
      if (TString(sample) == signaltag)
  	{
  	  if (sys == EXPSYS)
  	    {
	      ret = GetLevel(resultcode) -> GetHU(SIGNALPROXY, resultcode) -> Project(recolevel, CreateName("sig"));
  	    }
  	  if (sys == THEORSYS or sys == THEORSYSTEST)
  	    {
  	      // if (TString(tag).Contains("non_clos"))
  	      // 	printf("%p\n", GetSys(IN, tag, sample));
	      
  	      ret = GetSys(IN, tag, sample) -> Project(recolevel);

  	      if (recolevel == RECO)
  		{
  		  ApplyScaleFactor(ret);
  	      	}

  	      //return hsigprox;
  	    }
	  if (sys == NOMINAL)
	    {
	      ret = GetLevel(IN) -> GetHU(SIGNALPROXY, IN) -> Project(recolevel, CreateName("sig"));
	      // ret -> Print("all");
	      // getchar();
	    }
  	}
      else
  	{
  	  ret = GetLevel(resultcode) -> GetHU(SIGNALPROXY, resultcode) -> Project(recolevel, CreateName("sig"));

  	}
      if (pruned)
	{
	  GetUnfoldingHU(tag, sample, sys) -> GetTH2() -> ProjectionX() -> Print("all");
	  TH2 * matrix = GetUnfoldingHU(tag, sample, sys) -> GetTH2();
	  for (unsigned char bind = 1; bind <= ret -> GetNbinsX(); bind ++)
	    {
	      const float fact = matrix -> Integral(1, matrix -> GetNbinsX(), bind, bind)/matrix -> Integral(0, matrix -> GetNbinsX(), bind, bind);
	      //printf("GetSignalProxy %f %f ", fact, ret -> GetBinContent(bind));
	      ret -> SetBinContent(bind, ret -> GetBinContent(bind) * fact);
	      //printf(" after %f \n", ret -> GetBinContent(bind));
	    }
	}
      return ret;

    }
}
