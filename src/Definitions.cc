#include "Definitions.hh"
const char * tag_recolevel[2]     = {"gen", "reco"};
const char * tag_resultlevel[2]   = {"in", "out"};
const char * tag_opt[2]           = {"ORIG", "OPT"};  
const char * tag_sys[2]           = {"nosys", "sys"};
const char * tag_sf[2]            = {"nosf", "sf"};
const char * tag_sample[3]        = {"data", "signal", "background"};
const char * tag_sys_type[4]      = {"nominal", "expsys", "theorsys", "theorsystest"};
const char * tag_expsys[2]        = {"up", "down"};
const char * tag_charge_types_[2] = {"allconst", "chconst"};
const char * tag_jet_types_[]             = {"leading_jet", "scnd_leading_jet", "leading_b", "scnd_leading_b"};

OptLevelCode_t opt_level_ind     = 0;
SysLevelCode_t sys_level_ind     = 0;
SFLevelCode_t  sf_level_ind      = 0; 
float          orig_bin_width    = 0;
TString        pic_folder        = "";
int            test_int          = 0;
float          luminosity        = 35874.8;
