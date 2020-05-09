#ifndef _SampleDescriptor_hh_
#define _SampleDescriptor_hh_
#include "Definitions.hh"
#include "TNamed.h"
class SampleDescriptor: public TNamed
{
  friend class JsonParser;
  friend class CompoundHistoUnfolding;
  void PruneStringsFromQuotes();
  void PruneTrailingSpace();

  int                       size; 
  int                       size_color;
  float                    _xsec;
  char                   * _category; ///< [size];
  char                   * _title; ///< [size];
  char                   * _tag; ///< [size];
  char                   * _color; ///< [size_color];
  SampleTypeCode_t         _sample_type;
  char                   * _sample;
  SysTypeCode_t            _sys_type;
public:
  void Unset();
  unsigned char            _colornum;
  const char * GetCategory() const;
  const char * GetTitle() const;
  const char * GetTag() const;
  const char * GetSample() const;
  const char * GetColor() const;
  float GetXsec() const;
  SampleTypeCode_t GetSampleType() const;
  SysTypeCode_t    GetSysType() const;
  bool IsSys() const;
  void SetXsec(float);
  void SetCategory(const char *);
  void SetTitle(const char *);
  void SetTag(const char *);
  void SetColor(const char *);
  void SetSample(const char *);
  void SetSampleType(SampleTypeCode_t);
  void SetSysType(SysTypeCode_t);
  void ls(Option_t * = "") const;
  virtual ~SampleDescriptor();
  SampleDescriptor();
  //  SampleDescriptor(SampleDescriptor *);
  SampleDescriptor(SampleDescriptor &);
  ClassDef(SampleDescriptor, 1)
};
#endif
