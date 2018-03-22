#ifndef _SampleDescriptor_hh_
#define _SampleDescriptor_hh_
#include "Definitions.hh"
#include "TNamed.h"
class SampleDescriptor: public TNamed
{
  friend class JsonParser;
  void PruneStringsFromQuotes();
public:
  int                       size; 
  float                    _xsec;
  char                   * _title; ///< [size];
  char                   * _tag; ///< [size];
  char                   * _color; ///< [size];
  unsigned char            _colornum;
  SampleTypeCode_t         _sample_type;
  SysTypeCode_t            _sys_type;
  void ls(Option_t * = "") const;
  virtual ~SampleDescriptor();
  SampleDescriptor();
  ClassDef(SampleDescriptor, 1)
};
#endif
