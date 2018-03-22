#include "Definitions.hh"
const char * tag_recolevel[2] = {"gen", "reco"};
const char * tag_resultlevel[2] = {"in", "out"};
const char * tag_opt[2]     = {"orig", "opt"};  
const char * tag_sys[2]    = {"nosys", "sys"};
const char * tag_sf[2]      = {"nosf", "sf"};
const char * tag_sample[3]      = {"data", "signal", "background"};
const char * tag_sys_type[2]      = {"nominal", "sys"};

OptLevelCode_t opt_level_ind     = 0;
SysLevelCode_t sys_level_ind     = 0;
SFLevelCode_t  sf_level_ind      = 0; 
float          orig_bin_width    = 0;
TString        pic_folder        = "";
int            test_int          = 0;
float          luminosity        = 35874.8;
