#include "TStyle.h"
#include "TColor.h"
#include "TROOT.h"
void palettegreen()
{

  const unsigned char Number = 8;                                                                                                                                               
  
  Int_t colours[4] = {kGreen -10, kGreen -6, kGreen + 1, kGreen - 2}; 
  Double_t Red[Number];//   =  {0.7,  0.7,   0.6,       0.6,   0.4,     0.4,   0.3,       0.3};                                                                                     
  Double_t Green[Number];// =  {1.0,  1.0,   0.9,       0.9,   0.7,     0.7,   0.6,       0.6};                                                                                     
  Double_t Blue[Number];//  =  {0.7,  0.7,   0.6,       0.6,   0.4,     0.4,   0.3,       0.3};                                                                                     
  float red[4], green[4], blue[4];
  for (unsigned char cind = 0; cind < 4; cind ++)
    {
      TColor *color = gROOT -> GetColor(colours[cind]);
      color -> GetRGB(red[cind], green[cind], blue[cind]);
      Red[2 * cind] = red[cind];
      Red[2 * cind + 1] = red[cind];
      Blue[2*cind] = blue[cind];
      Blue[2*cind + 1] = blue[cind];
      Green[2 * cind] = green[cind];
      Green[2 * cind + 1] = green[cind];

    }
   
 
  Double_t Stops[Number] =  {0.0 , 0.250, 0.250001,  0.500, 0.50001,  0.5800,   0.580001,  1.000};     
  //  gStyle -> SetPalette(n, colours);
  TColor::CreateGradientColorTable(Number, Stops, Red, Green, Blue, 2*Number-1);

}

void palettedefault()
{
  gStyle -> SetPalette();
}
