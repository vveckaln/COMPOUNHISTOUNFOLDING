#include "SampleDescriptor.hh"
ClassImp(SampleDescriptor);

void SampleDescriptor::ls(Option_t * opt) const
{
  if (_tag)
    printf("tag \t\t[%s]\n", _tag);
  else
    printf("null\n");
  printf("xsec \t\t %f\n", _xsec);
  if (_title)
    printf("title \t\t[%s]\n", _title);
  else
    printf("null\n");
  if (_color)
    printf("color \t\t[%s]\n", _color);
  else
    printf("color \t\t%u\n", _colornum);
  printf("sample type \t%s\n", tag_sample[_sample_type]);
  printf("sys type %u\n", _sys_type);
  printf("sys type \t%s\n", tag_sys_type[_sys_type]);
	 
  
}

SampleDescriptor::SampleDescriptor(): size(0), size_color(0), _xsec(0.0), _tag(nullptr), _title(nullptr), _color(nullptr), _colornum(0), _sample_type(0), _sys_type(0) 
{
  //printf("calling SampleDescriptor::SampleDescriptor()\n");
}

SampleDescriptor::SampleDescriptor(SampleDescriptor & sd): SampleDescriptor()
{
  SetXsec(sd . GetXsec());
  SetSampleType(sd . GetSampleType());
  SetSysType(sd . GetSysType());
  SetTitle(sd . GetTitle());
  SetTag(sd . GetTag());
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

const char * SampleDescriptor::GetTitle() const
{
  return _title;
}

const char * SampleDescriptor::GetTag() const
{
  return _tag;
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
  _tag   = nullptr;
  _title = nullptr;
  _color = nullptr;
} 

SampleDescriptor::~SampleDescriptor()
{
  if (_tag)
    delete [] _tag;
  if (_title)
    delete [] _title;
  if (_color)
    delete[] _color;
} 

void SampleDescriptor::PruneStringsFromQuotes()
{
  char * str[4] = {_tag, _title, _tag, _color};
  for (unsigned char str_ind = 0; str_ind < 4; str_ind ++)
    {
      char *strp = str[str_ind];
      if (not strp)
	continue;
      char *p = strp;
      char *wp = strp;
      while (*p != '\0')
	{
	  if (*p != '"')
	    {
	      *wp = *p;
	      wp ++;
	    }
	  p ++;
	}
      *wp = '\0';
    }
}
