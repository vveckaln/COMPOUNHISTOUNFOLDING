#ifndef _Definitions_hh_
#define _Definitions_hh_
#include "TString.h"
enum SampleType {DATA, SIGNAL, BACKGROUND};
enum MO {DATAMO, SIGNALMO, BACKGROUNDMO, SIGNALNOMINALMO, SYSMO, DATAMBCKG, TOTALMC, TOTALMCUNC, TOTALMCUNCSHAPE};
typedef unsigned char MOCode_t;
enum SystematicsType {NOMINAL, EXPSYS, THEORSYS};
typedef unsigned char SampleTypeCode_t;
typedef unsigned char SysTypeCode_t;
typedef unsigned char ResultLevelCode_t;
typedef unsigned char ExpSysType_t;
typedef unsigned char ChargeCode_t;
typedef unsigned char VectorCode_t;
enum optlevel     {ORIG, OPT};
enum recolevel    {GEN, RECO};
enum systlevel    {NOSYS, SYS};  
enum sflevel      {NOSF, SF};
enum resultlevel  {IN, OUT};
enum expsystype   {UP, DOWN};
typedef unsigned char  RecoLevelCode_t;
typedef unsigned char  OptLevelCode_t;
typedef unsigned char  SysLevelCode_t;
typedef unsigned char  SFLevelCode_t;
typedef unsigned char  OptCode_t;
extern const char    * tag_resultlevel[2];
extern const char    * tag_recolevel[2];
extern const char    * tag_sample[3];
extern const char    * tag_sys_type[3];
extern const char    * tag_opt[2];  
extern const char    * tag_sys[2];
extern const char    * tag_sf[2];
extern const char    * tag_expsys[2];
extern const char          * tag_charge_types_[2];  
extern const char          * tag_jet_types_[];
extern OptLevelCode_t  opt_level_ind;
extern SysLevelCode_t  sys_level_ind;
extern SFLevelCode_t   sf_level_ind;
extern float           orig_bin_width;
extern TString         pic_folder;
extern int             test_int;
extern float           luminosity; 
#endif
