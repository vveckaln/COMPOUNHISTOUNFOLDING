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
  printf("sys type \t%s\n", tag_sys_type[_sys_type]);
	 
  
}

SampleDescriptor::SampleDescriptor(): size(256), _xsec(0.0), _tag(nullptr), _title(nullptr), _color(nullptr)//, _colornum(0), _sample_type(0), _sys_type(0) 
{
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
