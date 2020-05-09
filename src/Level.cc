#include "CompoundHistoUnfolding.hh"
#include "TStyle.h"
#include "TColor.h"

ClassImp(CompoundHistoUnfolding::Level);

CompoundHistoUnfolding::Level::Level(): 
signal(nullptr), 
//signalnominal(nullptr),
data(nullptr), 
totalMC(nullptr), 
totalMCUnc(nullptr), 
totalMCUncShape(nullptr), 
datambackground(nullptr),
cov(nullptr)
{

}

HistoUnfolding * CompoundHistoUnfolding::Level::GetHU(MOCode_t mo , ResultLevelCode_t resultcode)
{
  switch(mo)
    {
    case DATA:
      return data;
    case SIGNALMO:
      return signal;
    // case SIGNALNOMINALMO:
    //   return signalnominal;
    case DATAMBCKG:
      return datambackground;
    case TOTALMC:
      return totalMC;
    case TOTALMCUNC:
      return totalMCUnc;
    case TOTALMCUNCSHAPE:
      return totalMCUncShape;
    case SIGNALPROXY:
      switch(resultcode)
	{
	case IN:
	  return signal;
	case OUT:
	  return datambackground;
	default:
	  printf("HistoUnfolding *& CompoundHistoUnfolding::Level::GetHU(MOCode_t mo) no resultcode provided\n"); 
	  throw "HistoUnfolding *& CompoundHistoUnfolding::Level::GetHU(MOCode_t mo) no resultcode provided\n"; 
	}
    case TOTALMCSIGNALPROXY:
      switch(resultcode)
	{
	case IN:
	  return totalMC;
	case OUT:
	  return data;
	default:
	  printf("HistoUnfolding *& CompoundHistoUnfolding::Level::GetHU(MOCode_t mo) no resultcode provided\n"); 
	  throw "HistoUnfolding *& CompoundHistoUnfolding::Level::GetHU(MOCode_t mo) no resultcode provided\n"; 
	}
    default:
      {
	printf("HistoUnfolding *& CompoundHistoUnfolding::Level::GetHU(MOCode_t mo) - MO code incorrect %u\n", mo);
	throw "HistoUnfolding *& CompoundHistoUnfolding::Level::GetHU(MOCode_t mo) - MO code incorrect\n";
      }
    }
}
HistoUnfolding *& CompoundHistoUnfolding::Level::GetHURef(MOCode_t mo, ResultLevelCode_t resultcode)
{
  switch(mo)
    {
    case SIGNALMO:
      return signal;
    // case SIGNALNOMINALMO:
    //   return signalnominal;
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
    case SIGNALPROXY:
      switch(resultcode)
	{
	case IN:
	  return signal;
	case OUT:
	  return datambackground;
	}
    case TOTALMCSIGNALPROXY:
      switch(resultcode)
	{
	case IN:
	  return totalMC;
	case OUT:
	  return data;
	}
    default:
      char e[256];
     
      sprintf(e, "%s %u\n", "HistoUnfolding *& CompoundHistoUnfolding::Level::GetHURef(MOCode_t mo) - MO code incorrect", mo);
      printf("%s\n", e);
      throw (const char *)(e);
    }
}

vector<HistoUnfolding *> * CompoundHistoUnfolding::Level::GetV(MOCode_t mo, const char * signal)
{
  switch(mo)
    {
    case SIGNALMO:
      return & _vsignalhistos;
    case DATA:
      return & _vdatahistos;
    case SYSMO:
      if (signal == nullptr)
	{
	  printf("vector<HistoUnfolding *> * CompoundHistoUnfolding::Level::GetV(MOCode_t mo, const char * signaltag): please provide signaltag when extracting systematic\n");
	  throw "vector<HistoUnfolding *> * CompoundHistoUnfolding::Level::GetV(MOCode_t mo, const char * signaltag): please provide signaltag when extracting systematic\n";
	}
      return & _msyshistos[signal];
	
    default:
      throw "HistoUnfolding *& CompoundHistoUnfolding::Level::Measurements::GetV(MOCode_t mo) - MO code incorrect\n";
      return nullptr;
    }
}

HistoUnfolding * CompoundHistoUnfolding::Level::GetInputHU(MOCode_t mo, const char * name, const char * sample)
{
  vector<HistoUnfolding *> * v = GetV(mo, sample);
  vector<HistoUnfolding *> ::iterator it = v -> begin(); 
  while( it != v -> end() and TString((*it) -> GetTag()) != name)
    {
      it ++;
    }
  if (it == v -> end())
    return nullptr;
  else return *it;
}

void CompoundHistoUnfolding::Level::lsInputHU(MOCode_t mo, const char * sample)
{
  vector<HistoUnfolding *> * v = GetV(mo, sample);
  vector<HistoUnfolding *> ::iterator it = v -> begin(); 
  while( it != v -> end() )
    {
      (*it) -> ls();
      it ++;
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
  if (GetHU(TOTALMCUNCSHAPE) and GetHU(TOTALMCUNCSHAPE) -> GetTH1(RECO))
    {

      GetHU(TOTALMCUNCSHAPE) -> GetTH1(RECO) ->  SetFillColor(TColor::GetColor("#d73027"));
      GetHU(TOTALMCUNCSHAPE) -> GetTH1(RECO) ->  SetFillStyle(3254);
    }
  if (GetHU(TOTALMCUNCSHAPE) and GetHU(TOTALMCUNCSHAPE) -> GetTH1(GEN))
    {

      GetHU(TOTALMCUNCSHAPE) -> GetTH1(GEN) ->  SetFillColor(TColor::GetColor("#d73027"));
      GetHU(TOTALMCUNCSHAPE) -> GetTH1(GEN) ->  SetFillStyle(3254);
    }
}

void CompoundHistoUnfolding::Level::SeparateSys()
{
  _m2dirsyshistos.clear();
  _m1dirsyshistos.clear();
  for (map<TString, vector<HistoUnfolding *>>:: iterator bit = _msyshistos.begin(); bit != _msyshistos.end(); bit ++)
    {
      TString sample = bit -> first;
      for (vector<HistoUnfolding *>::iterator it = GetV(SYSMO, sample.Data()) -> begin(); it != GetV(SYSMO, sample.Data()) -> end(); it ++)
	{
	  if ((*it) -> GetSysType() == THEORSYS and TString((*it) -> GetTag()).Contains("up"))
	    {
	      array<HistoUnfolding *, 2> pair;
	      pair[0] = nullptr;
	      pair[1] = nullptr;
	      pair[0] = *it;
	      TString newtag = (*it) -> GetTag();
	      if (TString((*it) -> GetCategory()) != "Background") 
		{
		  newtag.ReplaceAll("up", "dn");
		}
	      else
		{
		  newtag.ReplaceAll("up", "down");
		  if (not GetSys(newtag.Data(), sample.Data()))
		    {
		      newtag = (*it) -> GetTag();
		      newtag.ReplaceAll("up", "dn");
		    }
		}
	      //printf("%s %s\n", (*it) -> GetTag(), newtag.Data());
	      pair[1] = GetSys(newtag.Data(), sample.Data());
	      _m2dirsyshistos[sample].push_back(pair); 
	    }
	  else if ((*it) -> GetSysType() == THEORSYS and TString((*it) -> GetTag()).Contains("m173v5"))
	    {
	      array<HistoUnfolding *, 2> pair;
	      pair[0] = nullptr;
	      pair[1] = nullptr;
	      pair[0] = *it;
	      TString newtag = (*it) -> GetTag();
	      newtag.ReplaceAll("m173v5", "m171v5");
	      pair[1] = GetSys(newtag.Data(), sample.Data());
	      _m2dirsyshistos[sample].push_back(pair); 

	    }
	  else if ((*it) -> GetSysType() == THEORSYS and TString((*it) -> GetTag()).Contains("m175v5"))
	    {
	      array<HistoUnfolding *, 2> pair;
	      pair[0] = nullptr;
	      pair[1] = nullptr;
	      pair[0] = *it;
	      TString newtag = (*it) -> GetTag();
	      newtag.ReplaceAll("m175v5", "m169v5");
	      pair[1] = GetSys(newtag.Data(), sample.Data());
	      _m2dirsyshistos[sample].push_back(pair); 

	    }
	  else if ((*it) -> GetSysType() == THEORSYS and TString((*it) -> GetTag()).Contains("m178v5"))
	    {
	      array<HistoUnfolding *, 2> pair;
	      pair[0] = nullptr;
	      pair[1] = nullptr;
	      pair[0] = *it;
	      TString newtag = (*it) -> GetTag();
	      newtag.ReplaceAll("m178v5", "m166v5");
	      pair[1] = GetSys(newtag.Data(), sample.Data());
	      _m2dirsyshistos[sample].push_back(pair); 

	    }

	  else if ((*it) -> GetSysType() == EXPSYS and TString((*it) -> GetTag()).Contains("_up"))
	    {
	      array<HistoUnfolding *, 2> pair;
	      pair[0] = nullptr;
	      pair[1] = nullptr;
	      pair[0] = *it;
	      TString newtag = (*it) -> GetTag();
	      newtag.ReplaceAll("_up", "_down");
	      pair[1] = GetSys(newtag.Data(), sample.Data());
	      _m2dirsyshistos[sample].push_back(pair);
	    }
	  else
	    {
	      const TString tag = (*it) -> GetTag();                                                                      
	      if (not tag.Contains("dn") and not tag.Contains("down") and not tag.Contains("m171v5") and not tag.Contains("m169v5") and not tag.Contains("m166v5"))
		_m1dirsyshistos[sample].push_back((*it));
	    }

	}
    }
    printf("2 DIR\n");
  for (map<TString, vector<array<HistoUnfolding *, 2>>> :: iterator bit = _m2dirsyshistos.begin(); bit != _m2dirsyshistos.end(); bit ++)
    {
      TString sample = bit -> first;
      printf("sample %s\n", sample.Data());
      for (vector<array<HistoUnfolding *, 2>>::iterator it =  bit -> second.begin(); it != bit -> second.end(); it ++)
  	{
  	  printf("%s %s\n", (*it)[0] -> GetTag(), (*it)[1] -> GetTag()); 
  	}
    }
  printf("1 DIR\n");
  for (map<TString, vector<HistoUnfolding *>> :: iterator bit = _m1dirsyshistos.begin(); bit != _m1dirsyshistos.end(); bit ++)
    {
      TString sample = bit -> first;
      
      printf("sample %s\n", sample.Data());
      for (vector<HistoUnfolding *>::iterator it =  bit -> second.begin(); it != bit -> second.end(); it ++)
  	{
  	  printf("%s\n", (*it) -> GetTag()); 
  	}
      
    }
  //getchar();
}

HistoUnfolding * CompoundHistoUnfolding::Level::GetSys(const char * systag, const char * sample)
{
  
  vector<HistoUnfolding *>::iterator it = GetV(SYSMO, sample) -> begin();
  HistoUnfolding * sys = nullptr;
  while ( it != GetV(SYSMO, sample) -> end() and not sys)
    {
      //      (*it) -> ls();
      TString tag ((*it) -> GetTag());
      //      printf("%s\n", tag.Data());
      if (TString(systag) == tag)
	{
	  sys = *it;
	}
      it ++;
    }
  return sys;
}

HistoUnfolding * CompoundHistoUnfolding::Level::GetExpSys(const char * expsystag, ExpSysType_t expsyscode)
{
}

CompoundHistoUnfolding::Level::~Level()
{

}
