#ifndef _SampleDescriptor_hh_
#define _SampleDescriptor_hh_
#include "Definitions.hh"
#include "TNamed.h"
class SampleDescriptor: public TNamed
{
  friend class JsonParser;
  void PruneStringsFromQuotes();
  int                       size; 
  float                    _xsec;
  char                   * _title; ///< [size];
  char                   * _tag; ///< [size];
  char                   * _color; ///< [size];
  SampleTypeCode_t         _sample_type;
  SysTypeCode_t            _sys_type;
public:
  unsigned char            _colornum;
  const char * GetTitle() const;
  const char * GetTag() const;
  const char * GetColor() const;
  float GetXsec() const;
  SampleTypeCode_t GetSampleType() const;
  SysTypeCode_t    GetSysType() const;
  bool IsSys() const;
  void SetXsec(float);
  void SetTitle(const char *);
  void SetTag(const char *);
  void SetColor(const char *);
  void SetSampleType(SampleTypeCode_t);
  void SetSysTypeCode(SysTypeCode_t);
  void ls(Option_t * = "") const;
  virtual ~SampleDescriptor();
  SampleDescriptor();
  SampleDescriptor(SampleDescriptor *);
  ClassDef(SampleDescriptor, 1)
};
#endif
