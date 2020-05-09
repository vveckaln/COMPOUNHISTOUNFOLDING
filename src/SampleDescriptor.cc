#include "SampleDescriptor.hh"
ClassImp(SampleDescriptor);

void SampleDescriptor::ls(Option_t * opt) const
{
  if (_tag)
    printf("\ntag \t\t[%s]\n", _tag);
  else
    printf("tag null\n");
  printf("xsec \t\t %f\n", _xsec);
  if (_category)
    printf("category \t\t[%s]\n", _category);
  else
    printf("category null\n");

  if (_title)
    printf("title \t\t[%s]\n", _title);
  else
    printf("title: null\n");

  if (_sample)
    printf("referring to sample \t\t[%s]\n", _sample);
  else
    printf("sample null\n");


  if (_color)
    printf("color \t\t[%s]\n", _color);
  else
    printf("color \t\t%u\n", _colornum);
  printf("sample type \t%s\n", tag_sample[_sample_type]);
  printf("sys type %u \t%s\n\n", _sys_type, tag_sys_type[_sys_type]);
}

SampleDescriptor::SampleDescriptor(): size(0), size_color(0), _xsec(0.0), _tag(nullptr), _title(nullptr), _sample(nullptr), _category(nullptr), _color(nullptr), _colornum(0), _sample_type(0), _sys_type(0) 
{
  //printf("calling SampleDescriptor::SampleDescriptor()\n");
}

SampleDescriptor::SampleDescriptor(SampleDescriptor & sd): SampleDescriptor()
{
  SetXsec(sd . GetXsec());
  SetSampleType(sd . GetSampleType());
  SetSysType(sd . GetSysType());
  SetCategory(sd. GetCategory());
  SetTitle(sd . GetTitle());
  SetTag(sd . GetTag());
  SetSample(sd.GetSample());
  if (sd._color)
    SetColor(sd . GetColor());
  else
    _colornum = sd . _colornum;
}

/*SampleDescriptor::SampleDescriptor(SampleDescriptor *sd): SampleDescriptor() 
{
  printf("calling SampleDescriptor::SampleDescriptor(SampleDescriptor *sd)\n");
  SetXsec(sd -> GetXsec());
  SetSampleType(sd -> GetSampleType());
  SetSysType(sd -> GetSysType());
  SetTitle(sd -> GetTitle());
  SetTag(sd -> GetTag());
  SetColor(sd -> GetColor());
  _colornum = sd -> _colornum;
}
*/

float SampleDescriptor::GetXsec() const
{
  return _xsec;
}

const char * SampleDescriptor::GetCategory() const
{
  return _category;
}


const char * SampleDescriptor::GetTitle() const
{
  return _title;
}

const char * SampleDescriptor::GetTag() const
{
  return _tag;
}

const char * SampleDescriptor::GetSample() const
{
  return _sample;
}


const char * SampleDescriptor::GetColor() const
{
  return _color;
}

SampleTypeCode_t SampleDescriptor::GetSampleType() const
{
  return _sample_type;
}

SysTypeCode_t    SampleDescriptor::GetSysType() const
{
  return _sys_type;
}

bool SampleDescriptor::IsSys() const
{
  return _sys_type == NOMINAL ? false: true;
}

void SampleDescriptor::SetXsec(float xsec)
{
  _xsec = xsec;
}

void SampleDescriptor::SetCategory(const char * category)
{
  if (not _category)
    {
      size = 256;
      _category = new char[size];
    }
  sprintf(_category, "%s", category);
}


void SampleDescriptor::SetTitle(const char * title)
{
  if (not _title)
    {
      size = 256;
      _title = new char[size];
    }
  sprintf(_title, "%s", title);
}

void SampleDescriptor::SetTag(const char * tag)
{
  if (not _tag)
    {
      size = 256;
      _tag = new char[size];
    }
  sprintf(_tag, "%s", tag);
}

void SampleDescriptor::SetSample(const char * sampletag)
{
  //  printf("_sample %p %p %s\n", _sample, sampletag, sampletag);
  if (not _sample)
    {
      size = 256;
      _sample = new char[size];
    }
  sprintf(_sample, "%s", sampletag);
}

void SampleDescriptor::SetColor(const char * color)
{
  if (not _color)
    {
      size_color = 256;
      _color = new char[size];
    }
  sprintf(_color, "%s", color);
}
void SampleDescriptor::SetSampleType(SampleTypeCode_t sample_type)
{
  _sample_type = sample_type;
}
void SampleDescriptor::SetSysType(SysTypeCode_t sys_type)
{
  _sys_type = sys_type;
}

void SampleDescriptor::Unset()
{
  _category = nullptr;
  _sample   = nullptr;
  _tag      = nullptr;
  _sample   = nullptr;
  _title    = nullptr;
  _color    = nullptr;

} 

SampleDescriptor::~SampleDescriptor()
{
  if (_tag)
    delete [] _tag;
  if (_category)
    delete [] _category;
  if (_sample)
    delete [] _sample;

  if (_title)
    delete [] _title;
  if (_color)
    delete[] _color;
} 

void SampleDescriptor::PruneStringsFromQuotes()
{
  const unsigned char Nstrings = 6;
  char * str[Nstrings] = {_tag, _title, _tag, _color, _category, _sample};
  for (unsigned char str_ind = 0; str_ind < Nstrings; str_ind ++)
    {
      // if (str_ind == 0)
      // 	printf("tag before pruning from quotes [%s]\n", str[str_ind]);
      char *strp = str[str_ind];
      if (not strp)
	continue;
      char *p = strp;
      char *wp = strp;
      unsigned char qind = 0;
      while (*p != '\0' and qind < 2)
	{
	  if (*p != '"')
	    {
	      *wp = *p;
	      wp ++;
	    }
	  else
	    qind ++;
	  p ++;
	}
      *wp = '\0';
      // if (str_ind == 0)
      // 	printf("tag pruned from quotes [%s]\n", str[str_ind]);
    }
}
