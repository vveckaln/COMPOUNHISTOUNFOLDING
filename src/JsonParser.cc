#include "JsonParser.hh"
#include <stdio.h>
#include <stdlib.h>

void JsonParser::Load(const char * json, SysTypeCode_t sys)
{
  FILE * pFile;
  long lSize;
  char * buffer;
  size_t result;

  pFile = fopen ( json , "rb" );
  if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

  // obtain file size:
  fseek (pFile , 0 , SEEK_END);
  lSize = ftell (pFile);
  rewind (pFile);

  // allocate memory to contain the whole file:
  buffer = (char*) malloc (sizeof(char)*lSize);
  if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

  // copy the file into the buffer:
  result = fread (buffer,1,lSize,pFile);
  if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
  /* the whole file is now loaded in the memory buffer. */

  // terminate
  fclose (pFile);
  bool delim1 = false;
  bool delim2 = false;
  
  bool fillbuffer = false;
  vector<const char * > samplebuffers;
  char * samplebuffer = 0;
  unsigned int samplecharind = 0;
  char *p = buffer;
  while(*p != '{')  
    {
      p ++;
    };
  p ++;
  samplebuffer = new char[4 * 256];
  samplebuffers.push_back(samplebuffer);
  for (; not (delim1 and *p == '}'); p ++)
    {
      //	  char array[2] = {*p, '\0' };
      //  printf("%s", array);


      if (*p == ',' and delim1)
	{
	  delim2 = true;
	 
	}
      if (*p == ']')
	{
	  delim1 = true;
	}	
      if (delim1 and delim2)
	{
	   if (samplebuffer)
	    {
	      samplebuffer[samplecharind] = '\0';
	      samplecharind ++;

	      samplebuffer = 0;
	      samplecharind = 0;
	    }
	  samplebuffer = new char[4 * 256];
	  samplebuffers.push_back(samplebuffer);
	  delim1 = false;
	  delim2 = false;
	  continue;
	}


      if (samplebuffer)
	{
	  samplebuffer[samplecharind] = *p;
	  samplecharind ++;
	}

     
    
    }
  if (samplebuffer)
    {
      samplebuffer[samplecharind] = '\0';
      samplebuffer = 0;
      samplecharind = 0;
    }
  free (buffer);
  for (unsigned long ind = 0; ind < samplebuffers.size(); ind ++)
    {
      //      printf("ind %u sample %s\n", ind, samplebuffers[ind]);
      SampleDescriptor *sd = new SampleDescriptor() ;
      ParseSample(samplebuffers[ind], *sd, sys);
      _samples.push_back(sd);;
    }

}

void JsonParser::ParseSample(const char * sample, SampleDescriptor & sd, SysTypeCode_t sys)
{
  
  const char *p = sample;
  
  
  char * tag = new char[256];
  unsigned char tagind = 0;
  for( ; *p != ':'; p ++)
    {
      tag[tagind] = *p;
      tagind ++;
    }
  tag[tagind] = '\0';
  {
    char *p = tag;
    char *wp = tag;
    bool startreached = false;
    for(; *p != '\0'; p++)
      {
        if (*p != ' ' and *p != '\n')
	  startreached = true;
	if (startreached)
	  {
	    *wp = *p;
	    wp ++;
	  }
      }				       
    *wp = '\0';
    for( ; *p <=32 or *p >= 127; p --)
      {
	  
      }
    p ++;
    *p = '\0';
  }
  //printf("tag [%s]\n", tag);
  char * field = 0;
  vector<char *> fields;
  unsigned char fieldcharind = 0;
  for ( ; *p != ']'; p ++)
    {
      if (*p == '[' or *p == ',')
	{
	  if (field)
	    {
	      field[fieldcharind] = '\0';

	    }
	  
	  field = new char[256];
	  fields.push_back(field);
	  fieldcharind = 0;
	  continue;
	}
      if (field)
	{
	  field[fieldcharind] = *p;
	  fieldcharind ++;
	} 
    }
  if (field)
    field[fieldcharind] = '\0';
  /*  for (unsigned char ind = 0; ind < fields.size(); ind ++)
    {
      printf("before ind %u field [%s]\n", ind, fields[ind]);
    }
  */
  //printf("title before [%s]\n", fields[3]);
  for (unsigned char ind = 0; ind < fields.size(); ind ++)
    {
      char *p = fields[ind];
      char *wp = fields[ind];
      bool startreached = false;
      for(; *p != '\0'; p++)
	{
	  /*if (ind == 3)
	    {
	      char arr[2] = {*p, '\0'};
	      printf("[%u][%s]", *p, arr);
	    }*/
	  if (*p > 32 and *p < 127)
	    startreached = true;
	  if (startreached)
	    {
	      *wp = *p;
	      wp ++;
	    }
	}				       
      *wp = '\0';
      p = wp; p--;
      /*      char *t = p;
      t--;
      char array[2] = {*t, '\0'};
      printf("t [%s] %s \n", array, *t == ' ' or *t);*/
      for( ; *p == ' ' or *p == '\n'; p --)
	{
	  //	  char array[2] = {*p, '\0'};
	  //printf("[%s]\n", array);
	}
      p ++;
      *p = '\0';
    }
  for (unsigned char ind = 0; ind < fields.size(); ind ++)
    {
      //printf("ind %u field [%s]\n", ind, fields[ind]);
    }
  //printf("title test [%s]\n", fields[3]);
  sd._tag = tag;
  sd._xsec = (stof(fields[0], 0));
  sd._title = fields[3];
  if (fields[4][0] == '"')
    {
      sd._color = fields[4];
    }
  else
    {
      sd._colornum = stoi(fields[4]);
    }
  sd._sys_type = sys;
  //sd.ls();
  sd.PruneStringsFromQuotes();
  if (TString(fields[1]) == '1')
    {
      sd._sample_type = DATA;
    }
  else if (TString(sd._tag) == _signal_tag)
    {
      sd._sample_type = SIGNAL;
    }
  else
    {
      sd._sample_type = BACKGROUND;
    }
  delete[] fields[0];
  delete[] fields[1];
  delete[] fields[2];
  delete[] fields[5];
  delete[] fields[6];
  delete[] fields[7];
  


}

void JsonParser::ls()
{
  printf("Listing parsed samples\n");
  for (unsigned long ind = 0; ind < _samples.size(); ind ++)
    {
      printf("sample %lu\n", ind);
      _samples[ind] -> ls();
    }
  printf("End listing \n");
}
unsigned long JsonParser::GetSize()
{
  return _samples.size(); 
}

const char * JsonParser::GetTag(unsigned long ind)
{
  return _samples[ind] -> _tag;
}


const char * JsonParser::GetTitle(unsigned long ind)
{
  return _samples[ind] -> _title;
}

float JsonParser::GetXsec(unsigned long ind)
{
  return _samples[ind] -> _xsec;
}
SampleTypeCode_t   JsonParser::GetSampleType(unsigned long ind)
{
  return _samples[ind] -> _sample_type;
}
SysTypeCode_t JsonParser::GetSysType(unsigned long ind)
{
  return _samples[ind] -> _sys_type;
}

void JsonParser::Test()
 {
  Load("/afs/cern.ch/work/v/vveckaln/private/CMSSW_8_0_26_patch1/src/TopLJets2015/TopAnalysis/data/era2016/samples.json");
  ls();
  SampleDescriptor sd;

  //   ParseSample("\"MC13TeV_SingleTbar_t\" : [80.95,\n 0,\n \"/ST_t-channel_antitop_4f_inclusiveDecays_TuneCUETP8M2T4_13TeV-powhegV2-madspin/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM\",\n  \"\"  ,\n \"#91 bfdb\",\nfalse,\n false,\n 1]", sd);

 }

void JsonParser::UnsetSample(unsigned long ind)
{
  _samples[ind] -> _tag    = nullptr;
  _samples[ind] -> _title  = nullptr;
  _samples[ind] -> _color  = nullptr;
} 


SampleDescriptor * JsonParser::GetSample(unsigned long ind)
{
  return _samples[ind];
} 


void JsonParser::SetSignalTag(const char * tag)
{
  sprintf(_signal_tag, "%s", tag);
}


JsonParser::~JsonParser()
{
  printf("Deconstructing json parser\n");
  for(vector<SampleDescriptor *> :: iterator it = _samples.begin(); it != _samples.end(); it ++)
    {
      delete *it;
    }
}
