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


HistoUnfolding * CompoundHistoUnfolding::GetUnfoldingHU(const char * tag, const char * sample, SysTypeCode_t code)
{
  Level * level = GetLevel(IN);
  if (code == EXPSYS)
    {
      if (TString(sample) == signaltag)
	{
	  level -> GetSys(tag, sample);
	}
      else
	return level -> GetHU(SIGNALPROXY, IN);
    }
  if (code == THEORSYS or code == NOMINAL)
    return level -> GetHU(SIGNALPROXY, IN);
}

HistoUnfolding * CompoundHistoUnfolding::GetExpSys(ResultLevelCode_t resultcode, const char * expsystag, ExpSysType_t expsyscode)
{
  vector<HistoUnfolding *>::iterator it = GetV(resultcode, SYSMO, signaltag) -> begin();
  HistoUnfolding * expsys = nullptr;
  const TString comp = expsyscode == UP ? "_up" : "_down";
  while ( it != GetV(resultcode, SYSMO, signaltag) -> end() or not expsys)
    {
      //(*it) -> ls();
      //      printf("%s\n", (*it) -> GetTag());
      TString tag ((*it) -> GetTag());
      if (TString(expsystag) + comp == tag)
	{
	  expsys = *it;
	}
      it ++;
    }
  return expsys;
}

HistoUnfolding * CompoundHistoUnfolding::GetSys(ResultLevelCode_t resultcode, const char * systag, const char * sample)
{
  
  return GetLevel(resultcode) -> GetSys(systag, sample);
}

