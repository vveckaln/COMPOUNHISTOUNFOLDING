// some C++ header definition
#ifdef __MAKECINT__
// turns off dictionary generation for all
#pragma link off all class;
#pragma link off all function;
#pragma link off all global;
#pragma link off all typedef;
#pragma link C++ class SampleDescriptor+;
#pragma link C++ class Histo+;
#pragma link C++ class CompoundHisto+;

#endif
