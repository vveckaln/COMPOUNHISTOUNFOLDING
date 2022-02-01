#include "CompoundHistoUnfolding.hh"
#include "TFile.h"
#include "JsonParser.hh"
#include "TCanvas.h"
#include "TMath.h"
#include "TStyle.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TROOT.h"
#include "TF1.h"
#include "TUnfoldDensity.h"
#include "TLegend.h"
#include "TKey.h"
#include "TLatex.h"
#include "TChain.h"
#include "TLine.h"
#include "TLegendEntry.h"
ClassImp(CompoundHistoUnfolding);
void CompoundHistoUnfolding::CreateTotalMCUnc(ResultLevelCode_t resultcode, RecoLevelCode_t recocode, bool shape, const char * binning, const char * unfm, bool includecflip, const char * env)
{
  using namespace TMath;
  printf("void CompoundHistoUnfolding::CreateTotalMCUnc(ResultLevelCode_t resultcode %u, RecoLevelCode_t recocode %u, bool shape %s)\n", resultcode, recocode, shape ? "shape" : "noshape");

  MOCode_t mo = shape ? TOTALMCUNCSHAPE : TOTALMCUNC;
  if (not GetLevel(resultcode) -> GetHURef(mo))
    GetLevel(resultcode) -> GetHURef(mo) = new HistoUnfoldingTH1();
  TH1 *& htotalMCUnc = GetLevel(resultcode) -> GetHU(mo) -> GetTH1Ref(recocode);
  htotalMCUnc = GetLevel(resultcode) -> GetHU(TOTALMCSIGNALPROXY, resultcode) -> Project(recocode, CreateName(shape ? "totalMCUncShape" : "totalMCUnc"), "", _YaxisTitle);
  //  RejectNonReco(htotalMCUnc);
  
  if (recocode == RECO and resultcode == IN)
    {
      ApplyScaleFactor(htotalMCUnc);
    }
  const int nbins = htotalMCUnc -> GetNbinsX();
  const unsigned char range = shape ? nbins - 1: nbins;  
  map<TString, vector<pair<SampleDescriptor*, TMatrixD>>> errors;
  
  map<TString, map<TString, pair<SampleDescriptor*, vector<TH1 **> >>> errors2D;
  // if (shape)
  //   {
  //     printf("_m2dirsyshistos.size() %lu  _m2dirsyshistos[signaltag].size() %lu _m1dirsyshistos[signaltag].size() %lu\n", 
  // 	     GetLevel(resultcode) -> _m2dirsyshistos.size(), 
  // 	     GetLevel(resultcode) -> _m2dirsyshistos[signaltag].size(),
  // 	     GetLevel(resultcode) -> _m1dirsyshistos[signaltag].size());
  //     getchar();
  //   }
  for (map<TString, vector<array<HistoUnfolding *, 2>>> :: iterator bit = GetLevel(resultcode) -> _m2dirsyshistos.begin(); bit != GetLevel(resultcode) -> _m2dirsyshistos.end(); bit ++)
    {
      TString sample = bit -> first;
      if (sample != signaltag)
	{
	  //continue;
	}
      for (vector<array<HistoUnfolding *, 2>>::iterator it =  bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  if ((*it)[0] -> GetSysType() == THEORSYSTEST)
	    continue;
	  TH1 * hsigup   = GetSignalProxy(resultcode, recocode, sample.Data(), (*it)[0] -> GetSysType(), (*it)[0] -> GetTag() );
	  hsigup -> SetName(TString(hsigup -> GetName()) + "_up");
	  TH1 * hsigdown = GetSignalProxy(resultcode, recocode, sample.Data(), (*it)[1] -> GetSysType(), (*it)[1] -> GetTag() );
	  hsigdown -> SetName(TString(hsigdown -> GetName()) + "_down");
	  if (recocode == RECO and resultcode == IN)
	    {
	      ApplyScaleFactor(hsigup);
	      ApplyScaleFactor(hsigdown);
	    }
	  const double integralnominalup   = hsigup   -> Integral();
	  const double integralnominaldown = hsigdown -> Integral();
	  
	  HistoUnfolding * up     = (*it)[0];
	  // (*it)[0] -> ls();
	  // printf("name %s\n", up -> GetTag());
	  // getchar();
	  TH1 *            hup    = up -> Project(recocode, "up");
	  hup -> SetName(TString(hup -> GetName()) + "_" + up -> GetTag());
	  hup -> SetTitle(up -> GetTitle());
	  // printf("%s\n", hup -> GetName());
	  // getchar();
	  HistoUnfolding * down   = (*it)[1];
	  TH1 *            hdown  = down -> Project(recocode, "down");
	  hdown -> SetName(TString(hdown -> GetName()) + "_" + down -> GetTag());
	  hdown -> SetTitle(down -> GetTitle());
	  TString tag = up -> GetTag();
	  tag.ReplaceAll("_up", "");
	  tag.ReplaceAll("m173v5", "topmass");
	  tag.ReplaceAll("m175v5", "topmass");
	  SampleDescriptor * sd = new SampleDescriptor(*(SampleDescriptor *) up);
	  sd -> SetTag(tag.Data());
	  sd -> SetCategory(up -> GetCategory());
	  const TString category = up -> GetCategory(); 
	  if (recocode == RECO and resultcode == IN)
	    {
	      ApplyScaleFactor(hup);
	      ApplyScaleFactor(hdown);
	    }
	  if (shape)
	    {
	      if (hup -> Integral())
		{
		  hup -> Scale(integralnominalup/hup -> Integral());
		}
	      else
		{
		  //	  printf("deleting %s\n", up -> GetTag());
		  delete   hup;
		  delete   hsigup;
		  hup    = nullptr;
		  hsigup = nullptr;
		}
	      if (hdown -> Integral())
		{
		  hdown -> Scale(integralnominaldown/hdown -> Integral());
		}
	      else
		{
		  //printf("deleting %s\n", down -> GetTag());
		  delete     hdown;
		  delete     hsigdown;
		  hdown    = nullptr;
		  hsigdown = nullptr;
		}
	    }
	  // hsig -> Print("all");
	  // hup -> Print("all");
	  // hdown -> Print("all");
	  if (not hsigdown or not hsigup)

	    continue;
	  auto cor = [&nbins] (TH1 * hsigup, TH1 * hsigdown, TH1 * hup, TH1 * hdown)
	    {
	      float cov = 0.0;
	      float sigmaup = 0.0;
	      float sigmadown = 0.0;
	      for (unsigned char bind = 1; bind < nbins + 1; bind ++)
		{
		  cov       += (hup -> GetBinContent(bind) - hsigup -> GetBinContent(bind)) * (hdown -> GetBinContent(bind) - hsigdown -> GetBinContent(bind));
		  sigmaup   += Power(hup ->GetBinContent(bind) - hsigup -> GetBinContent(bind), 2.0);
		  sigmadown += Power(hdown ->GetBinContent(bind) - hsigdown -> GetBinContent(bind), 2.0);
		}	    
	      return cov/Sqrt(sigmaup*sigmadown);
	    };

	  //printf("%s %f\n", (*it)[0] -> GetTag(), cor(hsigup, hsigdown, hup, hdown));
	  errors[sample].push_back(pair<SampleDescriptor*, TMatrixD>(sd, TMatrixD(nbins, nbins)));
	  TH1 ** herror2D = new TH1*[2];
	  herror2D[0] = (TH1* ) hup -> Clone();
	  herror2D[1] = (TH1 *) hdown -> Clone();
	  herror2D[0] -> SetDirectory(nullptr);
	  herror2D[1] -> SetDirectory(nullptr);
	  herror2D[0] -> Reset("ICE");
	  herror2D[1] -> Reset("ICE");
	 
	  if (errors2D[sample].find(category) == errors2D[sample].end())
	    {
	      errors2D[sample][category] = make_pair(sd , vector<TH1**>());
	    }
	  errors2D[sample][category].second.push_back(herror2D);
	  
	  for (unsigned char bind1 = 1; bind1 < nbins + 1; bind1 ++)
	    {
	      const double      sup1   = hup -> GetBinContent(bind1) - hsigup -> GetBinContent(bind1);
	      const double      sdown1 = hdown -> GetBinContent(bind1) - hsigdown -> GetBinContent(bind1);
	      
	      // const double      diff1   = (Power(sup, 2) + Power(sdown, 2))/2.0;
	      	      //	      printf("%u %.9f %.9f\n", bind1, sup, sdown); 
	      errors2D[sample][category] . second.back()[0] -> SetBinContent(bind1, sup1);
	      errors2D[sample][category] . second.back()[1] -> SetBinContent(bind1, sdown1);
	      for (unsigned char bind2 = 1; bind2 < nbins + 1; bind2 ++)
		{
		  const double      sup2   = hup -> GetBinContent(bind2) - hsigup -> GetBinContent(bind2);
		  const double      sdown2 = hdown -> GetBinContent(bind2) - hsigdown -> GetBinContent(bind2);
	      
		  //const double      diff2   = (Power(sup, 2) + Power(sdown, 2))/2.0;

		  errors[sample].back() . second(bind1 - 1, bind2 -1) = (sup1*sup2 + sdown1*sdown2)/2.0;
		}
	    }
	  delete hup;
	  delete hdown;
	  delete hsigdown;
	  delete hsigup;
	}
    }
  map<TString, map<TString, pair<SampleDescriptor*,  vector<TH1 *   >>>> errors1D;

  for (map<TString, vector<HistoUnfolding *>> :: iterator bit = GetLevel(resultcode) -> _m1dirsyshistos.begin(); bit != GetLevel(resultcode) -> _m1dirsyshistos.end(); bit ++)
    {
      TString sample = bit -> first;
      if (sample != signaltag)
	{
	  //continue;
	}
      for (vector<HistoUnfolding *>::iterator it =  bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  // if (TString(method) == "nominal" and TString((*it) -> GetCategory()) == "Colour flip")
	  //   continue;
	  // if (TString((*it) -> GetTag()) == "MC13TeV_TTJets2l2nu_amcatnlo")
	  //   continue;
	  if (TString((*it) -> GetTag()) == "MC13TeV_TTJets_evtgen")
	    continue;
	  if ((*it) -> GetSysType() == THEORSYSTEST)
	    continue;
	  
	  TH1 * hsig = GetSignalProxy(resultcode, recocode, sample.Data(), (*it) -> GetSysType(), (*it) -> GetTag());
	  if (recocode == RECO and resultcode == IN)
	    {
	      ApplyScaleFactor(hsig);
	    }
	  const double integralnominal = hsig -> Integral();
	  if (not integralnominal)
	    continue;
	  HistoUnfolding * hu      = *it;
	  //hu -> ls();
	  TH1 *            hsys    = hu -> Project(recocode);


	  SampleDescriptor * sd    = new SampleDescriptor(*hu);
	  // printf("created SD\n");
	  // sd -> ls();
	  if (recocode == RECO and resultcode == IN)
	    {
	      ApplyScaleFactor(hsys);
	    }
	  if (shape and TString(hu -> GetTag()).Contains("non_clos"))
	    {
	      // printf("nonclos sig unscaled\n");
	      // hsig -> Print("all");
	      // printf("nonclos sys unscaled\n");
	      // hsys -> Print("all");
	      
	    }

	  if (shape)
	    {
	      hsys -> Scale(integralnominal/hsys -> Integral());
	    }
	  // if (shape and TString(hu -> GetTag()).Contains("non_clos"))
	  //   {
	  //     printf("%p\n", hu);
	  //     printf("nonclos signal\n");
	  //     hsig -> Print("all");
	  //     printf("nonclos sys\n");
	  //     hsys -> Print("all");
	  //     for (unsigned char bind = 1; bind< hsig -> GetNbinsX() + 1; bind ++)
	  // 	printf("%u %f\n", bind, hsys -> GetBinContent(bind) - hsig -> GetBinContent(bind));
	      
	  //   }
	  TH1 * herror1D  = (TH1* ) hsys -> Clone();
	  herror1D -> SetDirectory(nullptr);
	  herror1D -> Reset("ICE");
	  herror1D -> SetTitle((*it) -> GetTitle());
	  herror1D -> SetName((TString("sys1D_") + (*it) -> GetTag()).Data());
	  const TString category = (*it) -> GetCategory();
	  if (errors1D[sample].find(category) == errors1D[sample].end())
	    {
	      errors1D[sample][category] = make_pair(sd , vector<TH1*>());
	    }
	  errors1D[sample][category].second.push_back(herror1D);
	  errors[sample].push_back(pair<SampleDescriptor*, TMatrixD>(sd, TMatrixD(nbins, nbins)));
	  for (unsigned char bind1 = 1; bind1 < nbins + 1; bind1 ++)
	    {
	      const double      diff1   = hsys -> GetBinContent(bind1) - hsig -> GetBinContent(bind1); 
	      herror1D -> SetBinContent(bind1,  diff1);
	      for (unsigned char bind2 = 1; bind2 < nbins + 1; bind2 ++)
		{
		  const double      diff2   = hsys -> GetBinContent(bind2) - hsig -> GetBinContent(bind2); 
		  errors[sample].back() . second(bind1 - 1, bind2 - 1) = diff1 * diff2;
		}
	    }
	  delete hsys;
	  delete hsig;
	}
    }
  for (map<TString, vector<pair<SampleDescriptor*, TMatrixD>>> ::iterator bit = errors.begin(); bit != errors.end(); bit ++)
    {
      TString sample = bit -> first;
      for(vector<pair<SampleDescriptor*, TMatrixD>>::iterator it = bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  //	  printf("%s %s\n", it -> first -> GetTag(), it -> first -> GetCategory());
	  for (unsigned char bind = 0; bind < nbins; bind ++)
	    {
	      //     printf("[%f] ", it -> second[bind]);
	    }
	  //printf("\n\n");
	}
    }

  // SampleDescriptor * statsd = new SampleDescriptor;
  // statsd -> SetTag("statistics");
  // statsd -> SetCategory("statistics");
  // errors[signaltag].push_back(pair<SampleDescriptor *, float *>(statsd, new float[nbins]));
  TMatrixD statistics = TMatrixD(nbins, nbins);
  for (unsigned char bind1 = 1; bind1 < nbins + 1; bind1 ++)
    {
      for (unsigned char bind2 = 1; bind2 < nbins + 1; bind2 ++)
	{
	  statistics(bind1 - 1, bind2 - 1) = htotalMCUnc -> GetBinError(bind1) * htotalMCUnc -> GetBinError(bind2);
	}

    }
  map<TString, TMatrixD> errcat;
  map<TString, map<TString, TMatrixD>> errcatmap;
  for (map<TString, vector<pair<SampleDescriptor*, TMatrixD>>> ::iterator bit = errors.begin(); bit != errors.end(); bit ++)
    {
      TString sample = bit -> first;
      for(vector<pair<SampleDescriptor*, TMatrixD>>::iterator it = bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  const char * category = it -> first -> GetCategory();
	  if (errcat.find(category) == errcat.end())
	    {
	      errcat.emplace(category, TMatrixD(nbins, nbins));
	      // for (unsigned char ind = 0; ind < nbins; ind ++)
	      // 	{
	      // 	  errcat[category][ind] = 0.0;
	      // 	}
	    }
	  if (errcatmap[sample].find(category) == errcatmap[sample].end())
	    {
	      errcatmap[sample].emplace(category, TMatrixD(nbins, nbins));
	      // for (unsigned char ind = 0; ind < nbins; ind ++)
	      // 	{
	      // 	  errcatmap[sample][category][ind] = 0.0;
	      // 	}
	    }
	  
	  // for (unsigned char bind = 0; bind < nbins; bind ++)
	  //   {
	  if (TString(category) != "Colour reconnection" or (TString(category) == "Colour reconnection" and TString(it -> first -> GetTag()).Contains("gluonMove")))
	    {
	      errcat[category] = errcat[category] + it -> second;
	    }
	  else
	    {
	      printf("skipping %s\n", it -> first -> GetTag());
	      //getchar();
	    }
	  errcatmap[sample][category] = errcatmap[sample][category] + it -> second;
	    	      
	}
    }
  // for (unsigned char bind = 0; bind < nbins; bind ++)
  //   {
      errcat["Top mass"] *= 1.0/Sqrt(3);
      errcatmap[signaltag]["Top mass"] *= 1.0/Sqrt(3);
    // }	      

  // printf("\n Categories \n");
  const unsigned char msize = errcat.size();
  pair<TString, TMatrixD> * errcatarr = (pair<TString, TMatrixD> * )::operator new (sizeof(pair<TString, TMatrixD>[msize]));
  unsigned char pind = 0;
  for (map<TString, TMatrixD>::iterator it1 = errcat.begin(); it1 != errcat.end(); it1 ++)
    {
      new (errcatarr + pind) pair<TString, TMatrixD>(it1 -> first, TMatrixD(it1 -> second));
      // printf("col 1 %u row 1 %u col 2 %u row 2 %u\n", it1 -> second.GetNcols(), it1 -> second.GetNrows(), errcatarr[pind].second.GetNcols(), errcatarr[pind].second.GetNrows());
      // getchar();
      pind ++;
    }
  auto swap = [](pair<TString, TMatrixD> * it1, pair<TString, TMatrixD> * it2)
    {
      pair<TString, TMatrixD> itswap = * it1;
      *it1 = *it2;
      *it2 = itswap;
    };
  auto sum = [&nbins] (pair<TString, TMatrixD> * it) -> float
    {
      float sumf = 0.0;
      for (unsigned char bind = 0; bind < nbins; bind ++)
	{
	  sumf += it -> second(bind, bind);
	}
      return sumf;
    };
  for (unsigned char ind1 = 0; ind1 < msize - 1; ind1 ++)
    {
      
      for (unsigned char ind2 = ind1 + 1; ind2 < msize; ind2 ++)
	{
	  if (sum(errcatarr + ind2) > sum(errcatarr + ind1))
	    swap(errcatarr + ind1 , errcatarr + ind2);
	}

    }
  TH1 * hsigtest =  GetLevel(resultcode) -> GetHU(SIGNALPROXY, resultcode) -> Project(recocode, CreateName("sig"));

  // if (shape)
  //   {
  //     for(unsigned char ind = 0; ind < msize; ind ++)
  // 	{
  // 	  pair<TString, float*> * it = errcatarr + ind;
  // 	  printf("%s\n", it -> first.Data());
  // 	  for (unsigned char bind = 0; bind < nbins; bind ++)
  // 	    {
  // 	      printf("[%.2f] ", it -> second[bind]/htotalMCUnc -> GetBinContent(bind + 1)*100.0);
  // 	    }
  // 	  printf("\n");
  // 	}
  //     printf("\n");
  //   }
  TMatrixD errf(nbins, nbins);
  // for (unsigned char bind = 0; bind < nbins; bind ++)
  //   {
  //     errf[bind] = 0.0;
  //   }

  for(map<TString, TMatrixD>::iterator it = errcat.begin(); it != errcat.end(); it ++)
    {
      // for (unsigned char bind = 0; bind < nbins; bind ++)
      // 	{
      
	  errf = errf + it -> second;
	  
	// }
    }
  printf("\n-----------------------------------------\n");
  // printf("signal\n");
  // hsigtest -> Print("all");

  // printf("Total syst errror\n");
  // for (unsigned char bind = 0; bind < nbins; bind ++)
  //   {
  //     printf("[%.2f] ", errf[bind]/htotalMCUnc -> GetBinContent(bind + 1)*100.0);
  //   }
  // printf("\n");
  // printf("Stat error\n");
 
  // for (unsigned char bind = 0; bind < nbins; bind ++)
  //   {
  //     printf("[%.2f] ", hsigtest -> GetBinError(bind + 1)*100.0/htotalMCUnc -> GetBinContent(bind + 1));
  //   }
  // printf("\n------------------------------------------\n");
  // printf("Total error\n");
  for (unsigned char bind = 0; bind < nbins; bind ++)
    {
      htotalMCUnc -> SetBinError(bind + 1, Sqrt(Power(htotalMCUnc -> GetBinError(bind + 1), 2) + errf(bind, bind)));
      // printf("[%.2f] ", htotalMCUnc -> GetBinError(bind + 1)*100.0/htotalMCUnc -> GetBinContent(bind + 1));
    }

  TMatrixD totalunc(errf);
  totalunc += statistics;

  TMatrixD cov(range, range);
  for (unsigned char indr = 0; indr < range; indr ++)
    {
      for (unsigned char indc = 0; indc < range; indc ++)
	{
	  cov(indr, indc) = totalunc(indr, indc);
	}
    }
    
  TMatrixD covinv(cov);
  covinv.Invert();
  // if (recocode == GEN and shape)
  //   {
  //     if (GetLevel(resultcode) -> cov)
  // 	{
  // 	  delete GetLevel(resultcode) -> cov;
  // 	  GetLevel(resultcode) -> cov = nullptr;
  // 	}
  //     //GetLevel(resultcode) -> cov = new TMatrixD(cov);
  //   }

  // printf("\n");
  if (shape and recocode == GEN and resultcode == OUT)
    {
      FILE * file = fopen((string(_folder) + "/systematics.txt").c_str(), "w");
      fprintf(file, "\\begin{table}[htbp]\n");
      fprintf(file, "\t\\centering\n");
      fprintf(file, "\t\\begin{tabular}{l");
      for (unsigned char bind = 1; bind < htotalMCUnc -> GetNbinsX() + 1; bind ++)
	{
	  fprintf(file, "c");
	}
      fprintf(file, "}\n");
      fprintf(file, "\t\t\\hline\n");
      fprintf(file, "\t\t \\multirow{2}{*}{Category} & \\multicolumn{%u}{c}{Bins}\\\\\n", htotalMCUnc -> GetNbinsX());
      for (unsigned char bind = 1; bind < htotalMCUnc -> GetNbinsX() + 1; bind ++)
	{
	  fprintf(file, "\t\t & %.2f \\textendash\\ %.2f", htotalMCUnc -> GetBinLowEdge(bind), htotalMCUnc -> GetXaxis() -> GetBinUpEdge(bind));
	}
      fprintf(file, "\\\\\n");
      fprintf(file, "\t\t\\hline\n");
      for(unsigned char ind = 0; ind < msize; ind ++)
	{
	  pair<TString, TMatrixD> * it = errcatarr + ind;
	  fprintf(file, "\t\t%s ", it -> first.Data());
	  for (unsigned char bind = 1; bind < nbins + 1; bind ++)
	    {
	      fprintf(file, "\t & %.2f ", TMath::Sqrt(it -> second(bind - 1, bind -1))/htotalMCUnc -> GetBinContent(bind) * 100.0);
	    }
	  fprintf(file, "\\\\\n");
	}
      fprintf(file, "\t\t\\hline\n");
      fprintf(file, "\t\tTotal syst error");
      for (unsigned char bind = 0; bind < nbins; bind ++)
	{
	  fprintf(file, " & %.2f ", TMath::Sqrt(errf(bind, bind))/htotalMCUnc -> GetBinContent(bind + 1)*100.0);
	}
      fprintf(file, "\\\\\n\t\t\\hline\n");
      fprintf(file, "\t\tStatistics");
 
      for (unsigned char bind = 0; bind < nbins; bind ++)
	{
	  fprintf(file, "\t & %.2f ", hsigtest -> GetBinError(bind + 1)*100.0/htotalMCUnc -> GetBinContent(bind + 1));
	}
      fprintf(file, "\t\t\\\\\n\t\t\\hline\n");
      fprintf(file, "\t\tTotal error");
      for (unsigned char bind = 0; bind < nbins; bind ++)
	{
	  fprintf(file, "\t & %.2f ", htotalMCUnc -> GetBinError(bind + 1)*100.0/htotalMCUnc -> GetBinContent(bind + 1));
	}
      fprintf(file, "\t\t\\\\\n\t\t\\hline\n");
      fprintf(file, "\t\\end{tabular}\n");
      fprintf(file, "\t\\caption{Bin per bin statistical uncertainties using \\protect\\unfoldingmethodtitle{%s} for the \\protect\\methodtitle{%s} method of the \\protect\\observabletitle{%s} of \\protect\\jettitle{%s} to \\protect\\countertitle{%s} including \\protect\\chargetitle{%s} using \\protect\\binningtitle{%s}%s}\n", 
	      unfm, 
	      method, 
	      observable, 
	      tag_jet_types_[jetcode], 
	      tag_jet_types_[jetcode], 
	      tag_charge_types_[chargecode], 
	      binning,
	      includecflip ? " including the colour flip sample." : ".");
      fprintf(file, "\t\\label{tab:statunc_%s_%s_%s_%s_%s_%s%s}\n", method, observable, tag_jet_types_[jetcode], tag_charge_types_[chargecode], binning, unfm, includecflip ? "_cflip" : "");
      fprintf(file, "\\end{table}\n");
      fclose(file);
    }


  delete hsigtest;


  map<TString, map<TString, pair<SampleDescriptor*, THStack *>>> stacks;
  map<TString, map<TString, pair<SampleDescriptor*, TLegend *>>> legends;
  for (map<TString, map<TString, pair<SampleDescriptor*, vector<TH1 **>>>> ::iterator bit = errors2D.begin(); bit != errors2D.end(); bit ++)
    {
      TString sample = bit -> first;
      auto swap = [](vector<TH1 **>::iterator it1, vector<TH1 **> :: iterator it2 )
	{
	  TH1 ** itswap = * it1;
	  *it1 = *it2;
	  *it2 = itswap;
	};
      auto sum = [&nbins] (vector<TH1 **> :: iterator it) -> float
	{
	  float sumf = 0.0;
	  for (unsigned char bind = 1; bind < nbins + 1; bind ++)
	    {
	      sumf += TMath::Sqrt(TMath::Power(sumf, 2) + (TMath::Power((*it)[0] -> GetBinContent(bind), 2) + TMath::Power((*it)[1] -> GetBinContent(bind), 2))/2.0);
	    }
	  return sumf;
	};
      for ( map<TString, pair<SampleDescriptor*, vector<TH1 **>>> ::iterator it = bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  for(vector<TH1 **>::iterator it1 = it -> second.second.begin(); it1 != it -> second.second.end(); it1 ++)
	    {
	      for(vector<TH1 **>::iterator it2 = it1 + 1; it2 != it -> second.second.end(); it2 ++)
		{
		  if (sum (it1) < sum(it2))
		    swap(it1, it2);
		}
	    }
	}
    }

  
  for (map<TString, map<TString, pair<SampleDescriptor*, vector<TH1 *>>>> ::iterator bit = errors1D.begin(); bit != errors1D.end(); bit ++)
    {
      TString sample = bit -> first;
      auto swap = [](vector<TH1 *>::iterator it1, vector<TH1 *> :: iterator it2 )
	{
	  TH1 * itswap = * it1;
	  *it1 = *it2;
	  *it2 = itswap;
	};
      auto sum = [&nbins] (vector<TH1 *> :: iterator it) -> float
	{
	  float sumf = 0.0;
	  for (unsigned char bind = 1; bind < nbins + 1; bind ++)
	    {
	      sumf += TMath::Sqrt(TMath::Power(sumf, 2) + TMath::Power((*it) -> GetBinContent(bind), 2));
	    }
	  return sumf;
	};
      for ( map<TString, pair<SampleDescriptor*, vector<TH1 *>>> ::iterator it = bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  for(vector<TH1 *>::iterator it1 = it -> second.second.begin(); it1 != it -> second.second.end(); it1 ++)
	    {
	      for(vector<TH1 *>::iterator it2 = it1 + 1; it2 != it -> second.second.end(); it2 ++)

		{
		  if (sum (it1) < sum(it2))
		    swap(it1, it2);
		}
	    }
	}
    }

  map<TString, map<TString, FILE *>> errfiles2D;

  auto chi2 = [& range, & covinv, this](TH1 * h) -> float
    {
      //const float width = h -> GetXaxis() -> GetXmax() - h -> GetXaxis() -> GetXmin();
      TMatrixD Diff(1, range);
      for (unsigned char bind = 1; bind < range + 1; bind ++)
	{
	  Diff(0, bind - 1) = h -> GetBinContent(bind) * h -> GetBinWidth(bind)/_orig_bin_width;
	}
      // TMatrixD DiffTranspose(nbins, 1);
      // DiffTranspose.Transpose(Diff);
      // printf("Diff\n");
      // Diff.Print();
      // printf("covinv\n");
      // covinv.Print();
      // printf("C\n");
      TMatrixD DiffTr(Diff);
      DiffTr.T();
      // TMatrixD C(Diff*covinv);
      // C.Print();
      // getchar();
      //      (TMatrixD(Diff * covinv) * Diff.T()).Print();
      //      printf("res\n");
      TMatrixD res(Diff * covinv * DiffTr);
      // res.Print();
      // getchar();
      return res(0, 0);
    };

  for (map<TString, map<TString, pair<SampleDescriptor*, vector<TH1 **>>>> ::iterator bit = errors2D.begin(); bit != errors2D.end(); bit ++)
    {
      TString sample = bit -> first;
      for(map<TString, pair<SampleDescriptor*, vector<TH1 **>>>::iterator it = bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  const char * category = it -> second .first -> GetCategory();
	  // printf("it -> first %s %s\n", it -> first.Data(), category);
	  if (stacks[sample].find(category) == stacks[sample].end())
	    {
	      stacks[sample][category] = make_pair(it -> second.first, new THStack);//make_pair(it -> second.first, vector<THStack * >());
	      TLegend * legend = new TLegend(0.3, 0.50, 0.94, 0.93);
	      legends[sample][category] = make_pair(it -> second.first, legend);
	      TString filename = TString(_folder) + "/" + sample + "_" + category;
	      if (shape)
		filename += "_shape";
	      filename += "2D.txt";
	      filename.ReplaceAll(" ", "_").ReplaceAll("-", "_");
	      // printf("opening %s\n", filename.Data());
	      FILE * file = fopen(filename.Data(), "w");
	      errfiles2D[sample][category] = file;
	      fprintf(file, "\\begin{table}[htbp]\n");
	      fprintf(file, "\t\\centering\n\\footnotesize\n");
	      fprintf(file, "\t\\begin{tabular}{p{0.1\\textwidth}");
	      const unsigned char dn = 5;
	      for (unsigned char bind = 1; bind < htotalMCUnc -> GetNbinsX() + 1 + dn; bind ++)
		{
		  fprintf(file, "r");
		}
	      fprintf(file, "}\n");
	      fprintf(file, "\t\t\\hline\n");
	      // fprintf(file, "\t\t \\makecell[c]{\\multirow{2}{*}{Process}} & \\multicolumn{%u}{c}{Bins} & \\makecell[c]{\\multirow{2}{*}{$\\rm N_{in}$ (reco $\\vee$ gen)}} & \\makecell[c]{\\multirow{2}{*}{Correlation}} & \\makecell[c]{\\multirow{2}{*}{RMS}}& \\makecell[c]{\\multirow{2}{*}{D}}& \\makecell[c]{\\multirow{2}{*}{$p\\textendash$value}}\\\\\n", nbins);
	      fprintf(file, "\t\t \\multirow{2}{*}{Process} & \\multicolumn{%u}{c}{Bins} & \\multirow{2}{*}{$\\rm N_{in}$ (reco $\\vee$ gen)} & \\multirow{2}{*}{Correlation} & \\multirow{2}{*}{RMS}}& \\multirow{2}{*}{$\\chi^{2}$} & \\multirow{2}{*}{$p\\textendash$value}\\\\\n", nbins);
	      for (unsigned char bind = 1; bind < htotalMCUnc -> GetNbinsX() + 1; bind ++)
		{
		  fprintf(file, "\t\t & %.2f \\textendash\\ %.2f", htotalMCUnc -> GetBinLowEdge(bind), htotalMCUnc -> GetXaxis() -> GetBinUpEdge(bind));
		}
	      fprintf(file, " & & & & &\\\\\n");
	      fprintf(file, "\t\t\\hline\n");
	    }
	  const static Color_t colors[7] = {kSpring, kPink, kViolet, kCyan, kMagenta, kOrange, kGreen + 2};
	  unsigned char ind = 0;
	  // printf("sample %s category %s\n", sample.Data(), category);
	  FILE *file = errfiles2D[sample][category];
	  TH1 * htotal = (TH1*) htotalMCUnc -> Clone();
	  htotal -> Reset("ICE");
	  auto RMS = [&nbins, this] (TH1 * h) -> float
	    {
	      float RMS = 0.0;
	      const float width = h -> GetXaxis() -> GetXmax() - h -> GetXaxis() -> GetXmin();
	      for (unsigned char bind = 1; bind < nbins + 1; bind ++)
		{
		  //		  printf("%u %f %f %f %f\n", bind, h -> GetBinContent(bind), h -> GetBinWidth(bind)/width, h -> GetBinContent(bind) * h -> GetBinWidth(bind)/width, RMS);
		  RMS       = Sqrt(Power(RMS, 2) + Power(h -> GetBinContent(bind) * h -> GetBinWidth(bind)/_orig_bin_width, 2));
		}
	      //	      printf("RMS %f\n", RMS);
	      return RMS;
	    };
	  for (vector<TH1 **>::iterator th1it = it -> second.second.begin(); th1it != it -> second.second.end(); th1it ++)
	    {
	      (*th1it)[0] -> SetMarkerStyle(kOpenTriangleUp);
	      (*th1it)[1] -> SetMarkerStyle(kOpenTriangleDown);
	      (*th1it)[0] -> SetMarkerSize(2);
	      (*th1it)[1] -> SetMarkerSize(2);
	      (*th1it)[0] -> SetLineColor(colors[ind]);
	      (*th1it)[1] -> SetLineColor(colors[ind]);
	      (*th1it)[0] -> SetMarkerColor(colors[ind]);
	      (*th1it)[1] -> SetMarkerColor(colors[ind]);
	      (*th1it)[0] -> SetLineWidth(2.0);
	      (*th1it)[1] -> SetLineWidth(2.0);
	      ind ++;
	      stacks[sample][category].second -> Add((*th1it)[0], "HISTO");
	      stacks[sample][category].second -> Add((*th1it)[1], "HISTO");
	      stacks[sample][category].second -> Add((*th1it)[0], "HISTOP");
	      stacks[sample][category].second -> Add((*th1it)[1], "HISTOP");
	      NormaliseToBinWidth((*th1it)[0]);
	      NormaliseToBinWidth((*th1it)[1]);
	      for (unsigned char bind = 1; bind < nbins + 1; bind ++)
		{
		  htotal -> SetBinContent(bind, 
					  TMath::Sqrt(TMath::Power(htotal -> GetBinContent(bind), 2) + 
						      (TMath::Power((*th1it)[0] -> GetBinContent(bind), 2) + TMath::Power((*th1it)[1] -> GetBinContent(bind), 2))/2.0));
		}
	      
	      legends[sample][category].second -> AddEntry((*th1it)[0], (*th1it)[0] -> GetTitle(), "lp");
	      legends[sample][category].second -> AddEntry((*th1it)[1], (*th1it)[1] -> GetTitle(), "lp");
	      auto cor = [&nbins] (TH1 * hup, TH1 * hdown) -> float
		{
		  float cov = 0.0;
		  float sigmaup = 0.0;
		  float sigmadown = 0.0;
		  const float width = hup -> GetXaxis() -> GetXmax() - hup -> GetXaxis() -> GetXmin();
		  for (unsigned char bind = 1; bind < nbins + 1; bind ++)
		    {
		      cov       += hup -> GetBinContent(bind) * TMath::Power(hup -> GetBinWidth(bind)/width, 2) * hdown -> GetBinContent(bind) ;
		      sigmaup   += Power(hup -> GetBinContent(bind) * hup -> GetBinWidth(bind)/width, 2.0);
		      sigmadown += Power(hdown ->GetBinContent(bind) * hup -> GetBinWidth(bind)/width, 2.0);
		    }	    
		  return cov/Sqrt(sigmaup*sigmadown);
		};
	      
	      // printf("name [%s] [%s]\n", (*th1it)[0] -> GetName(), (*th1it)[1] -> GetName());
	      //	      getchar();
	      fprintf(file, "\t\t $\\rm %s$", TString((*th1it)[0] -> GetTitle()).ReplaceAll("#", "\\").ReplaceAll(" ", "\\ ").ReplaceAll("_", "\\_").Data());
	      for (unsigned char bind = 1; bind < nbins + 1; bind ++)
		{
		  fprintf(file, " & %.2f ", (*th1it)[0] -> GetBinContent(bind));
		}	      
	      TString inputnameup(TString((*th1it)[0] -> GetName()).ReplaceAll(TString("up") + tag_recolevel[recocode] + "_", ""));
	      // printf("hinup %p %s %s\n", GetLevel(IN) -> GetSys(inputnameup.Data(), it -> second.first -> GetSample()), inputnameup.Data(), it -> second.first -> GetSample());
	      // getchar();
	      TH2 * hinup = GetLevel(IN) -> GetSys(inputnameup.Data(), it -> second.first -> GetSample()) -> GetTH2();
	      fprintf(file, " & %.0f", hinup -> Integral(1, hinup -> GetNbinsX(), 1, hinup -> GetNbinsY()));  
	      fprintf(file, " & \\multirow{2}{*}{%.2f}", cor((*th1it)[0], (*th1it)[1]));  
	      const float D0 = chi2((*th1it)[0]);
	      fprintf(file, " & %.1f & %.1f & %.1f\\\\\n", RMS((*th1it)[0]), D0, TMath::Prob(D0, range));  
	      fprintf(file, "\t\t $\\rm %s$", TString((*th1it)[1] -> GetTitle()).ReplaceAll("#","\\").ReplaceAll(" ","\\ ").ReplaceAll("_", "\\_").Data());
	      for (unsigned char bind = 1; bind < nbins + 1; bind ++)
		{
		  fprintf(file, " & %.2f ", (*th1it)[1] -> GetBinContent(bind));
		}	      
	      TH2 * hindown = GetLevel(IN) -> GetSys(TString((*th1it)[1] -> GetName()).ReplaceAll(TString("down") + tag_recolevel[recocode] + "_", ""), it -> second.first -> GetSample()) -> GetTH2();
	      fprintf(file, " & %.0f", hindown -> Integral(1, hindown -> GetNbinsX(), 1, hindown -> GetNbinsY()));  
	      fprintf(file, " & ");  
	      const float D1 = chi2((*th1it)[1]);
	      fprintf(file, " & %.1f & %.1f & %.1f\\\\\n\\hline\n", RMS((*th1it)[1]), D1, TMath::Prob(D1, range));  
	      fprintf(file, "\t\t Subtotal");
	      TH1 * subtotal = (TH1 *) (*th1it)[0] -> Clone();
	      
	      for (unsigned char bind = 1; bind < nbins + 1; bind ++)
		{
		  const float unc = TMath::Sqrt((TMath::Power((*th1it)[0] -> GetBinContent(bind), 2) + TMath::Power((*th1it)[1] -> GetBinContent(bind), 2))/2.0);
		  fprintf(file, " & %.2f ", unc );
		  subtotal -> SetBinContent(bind, unc);

		}	      
	      const float Dsubtotal = chi2(subtotal);
	      fprintf(file, "& & %.2f & %.2f & %.2f\\\\\n\t\t\\hline\n", RMS(subtotal), Dsubtotal, TMath::Prob(Dsubtotal, range));
	      delete subtotal;
	    }
	  
	  fprintf(file, "\t\tTotal");
	  //NormaliseToBinWidth(htotal);
	  if (TString(category) == "Top mass")
	    htotal -> Scale(1.0/Sqrt(3.0));
	  for (unsigned char bind = 1; bind < nbins + 1; bind ++)
	    {
	      fprintf(file, " & %.2f ", htotal -> GetBinContent(bind));
	    }	      
	  const float Dtotal = chi2(htotal);
	  fprintf(file, " & $\\textendash$ & %.2f & %.2f & %.2f \\\\\n\\hline\n", RMS(htotal), Dtotal, TMath::Prob(Dtotal, range));
	  fprintf(file, "\t\\end{tabular}\n");
	  fprintf(file, "\t\\caption{Bin-by-bin change in event in yield for the systematic \"%s\". The results are for \\protect\\unfoldingmethodtitle{%s} for the \\protect\\methodtitle{%s} method of the \\protect\\observabletitle{%s} of \\protect\\jettitle{%s} to \\protect\\countertitle{%s} including \\protect\\chargetitle{%s} using \\protect\\binningtitle{%s}.}\n", 
		  category, 
		  unfm,
		  method,
		  observable,
		  tag_jet_types_[jetcode],
		  tag_jet_types_[jetcode],
		  tag_charge_types_[chargecode],
		  binning);
	  fprintf(file, "\t\\label{tab:uncat_%s_%s_%s_%s%s_2D_%s_%s%s}\n", sample.Data(), TString(category).ReplaceAll(" ", "_").ReplaceAll("-", "_").Data(), tag_recolevel[recocode], tag_resultlevel[resultcode], shape ? "_shape" : "", binning, unfm, includecflip ? "_cflip" : "");
	  fprintf(file, "\\end{table}\n");
	  fclose(file);
	  delete htotal;
	      
	}
    }
  map<TString, map<TString, FILE *>> errfiles1D;

  for (map<TString, map<TString, pair<SampleDescriptor*, vector<TH1 *>>>> ::iterator bit = errors1D.begin(); bit != errors1D.end(); bit ++)
    {
      TString sample = bit -> first;
      for(map<TString, pair<SampleDescriptor*, vector<TH1 *>>>::iterator it = bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  const char * category = it -> second .first -> GetCategory();
	  map<TString, pair<SampleDescriptor*, THStack *>>:: iterator stackit = stacks[sample].find(category);
	  SampleDescriptor *sd = it -> second.first;
	  if (stackit == stacks[sample].end())
	    {
	      stacks[sample][category] = make_pair(sd, new THStack);//make_pair(it -> second.first, vector<THStack * >());
	      TLegend * legend = new TLegend(0.3, 0.50, 0.94, 0.93);
	      legends[sample][category] = make_pair(it -> second.first, legend);
	    }
	  if(errfiles1D[sample].find(category) == errfiles1D[sample].end())
	    {
	      TString filename = TString(_folder) + "/" + sample + "_" + category;
	      if (shape)
		filename += "_shape";
	      filename += "1D.txt";
	      filename.ReplaceAll(" ", "_").ReplaceAll("-", "_");
	      FILE * file = fopen(filename.Data(),"w");
	      errfiles1D[sample][category] = file;
	      fprintf(file, "\\begin{table}[htbp]\n");
	      fprintf(file, "\t\\centering\n");
	      fprintf(file, "\t\\footnotesize\n");
	      fprintf(file, "\t\\begin{tabular}{p{0.1\\textwidth}");
	      const unsigned char dn = 4;
	      for (unsigned char bind = 1; bind < htotalMCUnc -> GetNbinsX() + 1 + dn; bind ++)
		{
		  fprintf(file, "r");
		}
	      fprintf(file, "}\n");
	      fprintf(file, "\t\t\\hline\n");
	      // fprintf(file, "\t\t \\makecell[c]{\\multirow{2}{*}{Process}} & \\multicolumn{%u}{c}{Bins} &  \\makecell[c]{\\multirow{2}{*}{$\\rm N_{in}$ (reco $\\vee$ gen)}} &  \\makecell[c]{\\multirow{2}{*}{RMS}} &  \\makecell[c]{\\multirow{2}{*}{D}} &  \\makecell[c]{\\multirow{2}{*}{$p\\textendash$value}}\\\\\n", nbins);
	      fprintf(file, "\t\t \\multirow{2}{*}{Process} & \\multicolumn{%u}{c}{Bins} &  \\multirow{2}{*}{$\\rm N_{in}$ (reco $\\vee$ gen)} &  \\multirow{2}{*}{RMS} &  \\multirow{2}{*}{$\\chi^{2}$} &  \\multirow{2}{*}{$p\\textendash$value}\\\\\n", nbins);
	      fprintf(file, " \n\t\t\t");
	      for (unsigned char bind = 1; bind < htotalMCUnc -> GetNbinsX() + 1; bind ++)
		{
		  // fprintf(file, " & \\makecell[c]{%.2f \\textendash\\ %.2f} ", htotalMCUnc -> GetBinLowEdge(bind), htotalMCUnc -> GetXaxis() -> GetBinUpEdge(bind));
		  fprintf(file, " & %.2f \\textendash\\ %.2f ", htotalMCUnc -> GetBinLowEdge(bind), htotalMCUnc -> GetXaxis() -> GetBinUpEdge(bind));
		}
	      fprintf(file, " & \\\\\n");
	      fprintf(file, "\t\t\\hline\n");
	      
	    }
	  const static Color_t colors[6] = {kBlue, kGreen, kRed, kBlue + 3, kGreen + 3, kRed + 3};
	  unsigned char ind = 0;
	  TH1 * htotal = (TH1*) htotalMCUnc -> Clone();
	  htotal -> Reset("ICE");
	  FILE * file = errfiles1D[sample][category];
	  auto RMS = [&nbins] (TH1 * h) -> float
	    {
	      float RMS = 0.0;
	      const float width = h -> GetXaxis() -> GetXmax() - h -> GetXaxis() -> GetXmin();
	      for (unsigned char bind = 1; bind < nbins + 1; bind ++)
		{
		  RMS       = Sqrt(Power(RMS, 2) + Power(h -> GetBinContent(bind) * h -> GetBinWidth(bind)/width, 2));
		}
	      return RMS;
	    };
	  for (vector<TH1 *>::iterator th1it = it -> second.second.begin(); th1it != it -> second.second.end(); th1it ++)
	    {
	      (*th1it) -> SetLineColor(colors[ind]);
	      (*th1it) -> SetLineWidth(2.0);
	      ind ++;
	      NormaliseToBinWidth(*th1it);
	      if (TString(category) == "Colour reconnection")
		{
		  printf("category == colour reconnection\n");
		  // (*th1it) -> Print("all");
		  // getchar();
		}
	      stacks[sample][category] . second -> Add(*th1it, "HISTE");
	      legends[sample][category]. second -> AddEntry(*th1it, (*th1it) -> GetTitle(), "lp"); 
	      // printf("%s\n", (*th1it) -> GetName());
	      // getchar();
	      HistoUnfolding * husysin  = GetLevel(IN) -> GetSys(TString((*th1it) -> GetName()).ReplaceAll("sys1D_", ""), it -> second.first -> GetSample());
								  
	      TH2 * hsysin = husysin ? husysin -> GetTH2() : nullptr;
	      
	      fprintf(file, "\t\t $\\rm %s$", TString((*th1it) -> GetTitle()).ReplaceAll("#", "\\").ReplaceAll(" ", "\\ ").ReplaceAll("_", "\\_").Data());
	      for (unsigned char bind = 1; bind < nbins + 1; bind ++)
		{
		  fprintf(file, " & %.2f ", (*th1it) -> GetBinContent(bind));
		  htotal -> SetBinContent(bind, TMath::Sqrt(TMath::Power(htotal -> GetBinContent(bind), 2) + TMath::Power((*th1it) -> GetBinContent(bind), 2)));
		}	      
	      const float Dth1 = chi2(*th1it);
	      fprintf(file, "& %s & %.2f & %.2f & %.2f \\\\\n", hsysin ? Form("%0.f", hsysin -> Integral(1, hsysin -> GetNbinsX(), 1, hsysin -> GetNbinsY())) : "X", RMS(*th1it), Dth1, TMath::Prob(Dth1, range));  
	    }
	  fprintf(file, "\t\t\\hline\n");
	  fprintf(file, "\t\tTotal ");
	  //NormaliseToBinWidth(htotal);
	  for (unsigned char bind = 1; bind < nbins + 1; bind ++)
	    {
	      fprintf(file, " & %.2f ", htotal -> GetBinContent(bind));
	    }
	  const float Dtotal = chi2(htotal);
	  fprintf(file, "& $\\textendash$ & %.2f & %.2f & %.2f\\\\\n", RMS(htotal), Dtotal, TMath::Prob(Dtotal, range));
	  fprintf(file, "\t\t\\hline\n");
	  fprintf(file, "\t\\end{tabular}\n");
	  fprintf(file, "\t\\caption{Bin-by-bin change in event yield for the systematic \"%s\". The results are for \\protect\\unfoldingmethodtitle{%s} for the \\protect\\methodtitle{%s} method of the \\protect\\observabletitle{%s} of \\protect\\jettitle{%s} to \\protect\\countertitle{%s} including \\protect\\chargetitle{%s} using \\protect\\binningtitle{%s}.}\n", 
		  category, 
		  unfm,
		  method,
		  observable,
		  tag_jet_types_[jetcode],
		  tag_jet_types_[jetcode],
		  tag_charge_types_[chargecode],
		  binning);
	  fprintf(file, "\t\\label{tab:uncat_%s_%s_%s_%s%s_1D_%s_%s%s}\n", sample.Data(), TString(category).ReplaceAll(" ", "_").ReplaceAll("-", "_").Data(), tag_recolevel[recocode], tag_resultlevel[resultcode], shape ? "_shape" : "", binning, unfm, includecflip ? "_cflip" : "");
	  fprintf(file, "\\end{table}\n");
	  fclose(file);
	  delete htotal;
	
	}
    }
  gStyle -> SetOptStat(0);
  gStyle -> SetOptTitle(0);
  for (map<TString, map<TString, pair<SampleDescriptor*, THStack *>>>::iterator bit = stacks.begin(); bit != stacks.end(); bit ++)
    {
      const TString sample = bit -> first;
      for(map<TString, pair<SampleDescriptor*, THStack *>>::iterator it = bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  const TString category = it -> first;
	  TString name = bit -> first + "_" + it -> first + "_" + tag_recolevel[recocode] + "_" + tag_resultlevel[resultcode];
	  name.ReplaceAll(" ", "_");
	  if (shape)
	    {
	      name += "_shape";
	    }
	  TCanvas c("c", "c", 800, 800);
	  c.SetTopMargin(0.05);
	  c.SetRightMargin(0.02);
	  c.SetLeftMargin(0.2);
	  c.SetBottomMargin(0.15);
	  THStack * stack = it -> second.second;
	  TH1 * hframe = GetLevel(IN)/*level*/ -> GetHU(SIGNAL/*SIGNALPROXY*/, resultcode) -> Project(recocode, CreateName("frame"), _XaxisTitle, _YaxisTitle);
	  hframe -> Reset("ICE");
	  TMatrixD errit = errcatmap.at(sample).at(category);
	  
	  for (unsigned char bind = 1; bind < hframe -> GetNbinsX() + 1; bind ++)
	    {
	      hframe -> SetBinContent(bind, TMath::Sqrt(errit(bind -1, bind -1)));
	    }
	  hframe -> SetLineWidth(3.5);
	  hframe -> SetLineColor(kBlack);
	  TLine line(hframe -> GetXaxis() -> GetXmin(), 0.0, hframe -> GetXaxis() -> GetXmax(), 0.0);
	  line.SetLineColor(kBlack);
	  line.SetLineStyle(kDashed);
	  NormaliseToBinWidth(hframe);
	  stack -> Add(hframe);
	  hframe -> SetMinimum(stack -> GetMinimum("nostack") * 1.2);
	  const float maximum = stack -> GetMaximum("nostack") > hframe -> GetMaximum() ? stack -> GetMaximum("nostack") : hframe -> GetMaximum();
	  hframe -> SetMaximum(maximum * 4.0);
	  hframe -> GetYaxis() -> SetLabelSize(0.20*0.2/0.8);
	  hframe -> GetYaxis() -> SetTitleSize(0.21*0.2/0.8);
  
	  hframe -> GetYaxis() -> SetTitleOffset(0.45*0.8/0.2);
	  hframe -> GetXaxis() -> SetLabelSize(0.20*0.2/0.8);
	  hframe -> GetXaxis() -> SetTitleSize(0.21*0.2/0.8);
  
	  hframe -> GetXaxis() -> SetTitleOffset(0.30*0.8/0.2);

	  hframe -> Draw();
	  TLegend * legend = legends[bit -> first][it -> first].second;
	  TLegendEntry * hframeentry = new TLegendEntry(hframe, TString(category) + " (total)" , "lp");
	  // printf("%s// \n", legend -> GetListOfPrimitives() -> At(0) -> ClassName());
	  // getchar();
	  legend -> GetListOfPrimitives() -> AddAt(hframeentry, 0);
	  legend -> SetBorderSize(0);
	  legend -> SetFillStyle(0);
	  legend -> SetTextFont(42);
	  legend -> SetTextSize(0.025); 
	  stack -> Draw("NOSTACKSAME");
	  legend -> Draw("SAME");
	  line.Draw("SAME");
	  c.SaveAs(TString(_folder) + "/" + name.ReplaceAll("-", "_") + ".png");
	  delete hframe;
	}
    }

  for (map<TString, map<TString, pair<SampleDescriptor*, THStack *>>>::iterator bit = stacks.begin(); bit != stacks.end(); bit ++)
    {
      for(map<TString, pair<SampleDescriptor*, THStack *>>::iterator it = bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  delete it -> second.second;
	}
    }
  for (map<TString, map<TString, pair<SampleDescriptor*, TLegend *>>>::iterator bit = legends.begin(); bit != legends.end(); bit ++)
    {
      for(map<TString, pair<SampleDescriptor*, TLegend *>>::iterator it = bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  delete it -> second.second;
	}
    }
  
  for (map<TString, map<TString, pair<SampleDescriptor*, vector<TH1 **>>>> ::iterator bit = errors2D.begin(); bit != errors2D.end(); bit ++)
    {
      for(map<TString, pair<SampleDescriptor*, vector<TH1 **>>>::iterator it = bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  for (vector<TH1 **>::iterator th1it = it -> second.second.begin(); th1it != it -> second.second.end(); th1it ++)
	    {
	      delete (*th1it)[0];
	      delete (*th1it)[1];
	      delete [] *th1it;
	    }
	  
	}
    }


  for (map<TString, map<TString, pair<SampleDescriptor*, vector<TH1 *>>>> ::iterator bit = errors1D.begin(); bit != errors1D.end(); bit ++)
    {
      for(map<TString, pair<SampleDescriptor*, vector<TH1 *>>>::iterator it = bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  for (vector<TH1 *>::iterator th1it = it -> second.second.begin(); th1it != it -> second.second.end(); th1it ++)
	    {
	      delete *th1it;
	    }
	  
	}
    }


  for (map<TString, vector<pair<SampleDescriptor*, TMatrixD>>> ::iterator bit = errors.begin(); bit != errors.end(); bit ++)
    {
      TString sample = bit -> first;
      for(vector<pair<SampleDescriptor*, TMatrixD>>::iterator it = bit -> second.begin(); it != bit -> second.end(); it ++)
	{
	  delete    it -> first;
	  //	  delete [] it -> second;
	}
    }

  for(map<TString, TMatrixD>::iterator it = errcat.begin(); it != errcat.end(); it ++)
    {
      //delete [] it -> second; 
    }

  for(map<TString, map<TString, TMatrixD>>::iterator bit = errcatmap.begin(); bit != errcatmap.end(); bit ++)
    {
      for(map<TString, TMatrixD>::iterator it = errcatmap[bit -> first].begin(); it != errcatmap[bit -> first].end(); it ++)
	{
           
	  //delete [] it -> second; 
	}
    }
  // delete [] errf;
  delete  errcatarr;

  

  //  NormaliseToBinWidth(htotalMCUnc);
}
