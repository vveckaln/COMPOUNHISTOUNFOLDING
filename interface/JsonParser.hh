#ifndef _JsonParser_hh_
#define _JsonParser_hh_

#include <vector>
#include "SampleDescriptor.hh"
using namespace std;
class JsonParser
{
  vector<SampleDescriptor *> _samples;
  void ParseSample(const char*, SampleDescriptor &, SysTypeCode_t = NOMINAL);
public:
  void Load(const char *, SysTypeCode_t = NOMINAL);
  void ls();
  unsigned long GetSize();
  const char * GetTag(unsigned long);
  const char * GetTitle(unsigned long);
  float  GetXsec(unsigned long);
  SampleTypeCode_t   GetSampleType(unsigned long);
  SysTypeCode_t      GetSysType(unsigned long);
  void Test();
  void UnsetSample(unsigned long);
  SampleDescriptor * GetSample(unsigned long);
  ~JsonParser();
};

#endif
