// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME srcdISampleDescriptorDict

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/interface/SampleDescriptor.hh"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_SampleDescriptor(void *p = 0);
   static void *newArray_SampleDescriptor(Long_t size, void *p);
   static void delete_SampleDescriptor(void *p);
   static void deleteArray_SampleDescriptor(void *p);
   static void destruct_SampleDescriptor(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SampleDescriptor*)
   {
      ::SampleDescriptor *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::SampleDescriptor >(0);
      static ::ROOT::TGenericClassInfo 
         instance("SampleDescriptor", ::SampleDescriptor::Class_Version(), "interface/SampleDescriptor.hh", 5,
                  typeid(::SampleDescriptor), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::SampleDescriptor::Dictionary, isa_proxy, 4,
                  sizeof(::SampleDescriptor) );
      instance.SetNew(&new_SampleDescriptor);
      instance.SetNewArray(&newArray_SampleDescriptor);
      instance.SetDelete(&delete_SampleDescriptor);
      instance.SetDeleteArray(&deleteArray_SampleDescriptor);
      instance.SetDestructor(&destruct_SampleDescriptor);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SampleDescriptor*)
   {
      return GenerateInitInstanceLocal((::SampleDescriptor*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::SampleDescriptor*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr SampleDescriptor::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *SampleDescriptor::Class_Name()
{
   return "SampleDescriptor";
}

//______________________________________________________________________________
const char *SampleDescriptor::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::SampleDescriptor*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int SampleDescriptor::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::SampleDescriptor*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *SampleDescriptor::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::SampleDescriptor*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *SampleDescriptor::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::SampleDescriptor*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void SampleDescriptor::Streamer(TBuffer &R__b)
{
   // Stream an object of class SampleDescriptor.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(SampleDescriptor::Class(),this);
   } else {
      R__b.WriteClassBuffer(SampleDescriptor::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_SampleDescriptor(void *p) {
      return  p ? new(p) ::SampleDescriptor : new ::SampleDescriptor;
   }
   static void *newArray_SampleDescriptor(Long_t nElements, void *p) {
      return p ? new(p) ::SampleDescriptor[nElements] : new ::SampleDescriptor[nElements];
   }
   // Wrapper around operator delete
   static void delete_SampleDescriptor(void *p) {
      delete ((::SampleDescriptor*)p);
   }
   static void deleteArray_SampleDescriptor(void *p) {
      delete [] ((::SampleDescriptor*)p);
   }
   static void destruct_SampleDescriptor(void *p) {
      typedef ::SampleDescriptor current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SampleDescriptor

namespace {
  void TriggerDictionaryInitialization_SampleDescriptorDict_Impl() {
    static const char* headers[] = {
"interface/SampleDescriptor.hh",
0
    };
    static const char* includePaths[] = {
"/cvmfs/cms.cern.ch/slc6_amd64_gcc530/lcg/root/6.06.00-ikhhed6/include",
"/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "SampleDescriptorDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$interface/SampleDescriptor.hh")))  SampleDescriptor;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "SampleDescriptorDict dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/interface/SampleDescriptor.hh"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"SampleDescriptor", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("SampleDescriptorDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_SampleDescriptorDict_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_SampleDescriptorDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_SampleDescriptorDict() {
  TriggerDictionaryInitialization_SampleDescriptorDict_Impl();
}
