// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME srcdIHistoUnfoldingTH2Dict

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
#include "/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/interface/HistoUnfoldingTH2.hh"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_HistoUnfoldingTH2(void *p = 0);
   static void *newArray_HistoUnfoldingTH2(Long_t size, void *p);
   static void delete_HistoUnfoldingTH2(void *p);
   static void deleteArray_HistoUnfoldingTH2(void *p);
   static void destruct_HistoUnfoldingTH2(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::HistoUnfoldingTH2*)
   {
      ::HistoUnfoldingTH2 *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::HistoUnfoldingTH2 >(0);
      static ::ROOT::TGenericClassInfo 
         instance("HistoUnfoldingTH2", ::HistoUnfoldingTH2::Class_Version(), "interface/HistoUnfoldingTH2.hh", 4,
                  typeid(::HistoUnfoldingTH2), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::HistoUnfoldingTH2::Dictionary, isa_proxy, 4,
                  sizeof(::HistoUnfoldingTH2) );
      instance.SetNew(&new_HistoUnfoldingTH2);
      instance.SetNewArray(&newArray_HistoUnfoldingTH2);
      instance.SetDelete(&delete_HistoUnfoldingTH2);
      instance.SetDeleteArray(&deleteArray_HistoUnfoldingTH2);
      instance.SetDestructor(&destruct_HistoUnfoldingTH2);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::HistoUnfoldingTH2*)
   {
      return GenerateInitInstanceLocal((::HistoUnfoldingTH2*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::HistoUnfoldingTH2*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr HistoUnfoldingTH2::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *HistoUnfoldingTH2::Class_Name()
{
   return "HistoUnfoldingTH2";
}

//______________________________________________________________________________
const char *HistoUnfoldingTH2::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::HistoUnfoldingTH2*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int HistoUnfoldingTH2::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::HistoUnfoldingTH2*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *HistoUnfoldingTH2::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::HistoUnfoldingTH2*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *HistoUnfoldingTH2::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::HistoUnfoldingTH2*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void HistoUnfoldingTH2::Streamer(TBuffer &R__b)
{
   // Stream an object of class HistoUnfoldingTH2.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(HistoUnfoldingTH2::Class(),this);
   } else {
      R__b.WriteClassBuffer(HistoUnfoldingTH2::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_HistoUnfoldingTH2(void *p) {
      return  p ? new(p) ::HistoUnfoldingTH2 : new ::HistoUnfoldingTH2;
   }
   static void *newArray_HistoUnfoldingTH2(Long_t nElements, void *p) {
      return p ? new(p) ::HistoUnfoldingTH2[nElements] : new ::HistoUnfoldingTH2[nElements];
   }
   // Wrapper around operator delete
   static void delete_HistoUnfoldingTH2(void *p) {
      delete ((::HistoUnfoldingTH2*)p);
   }
   static void deleteArray_HistoUnfoldingTH2(void *p) {
      delete [] ((::HistoUnfoldingTH2*)p);
   }
   static void destruct_HistoUnfoldingTH2(void *p) {
      typedef ::HistoUnfoldingTH2 current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::HistoUnfoldingTH2

namespace {
  void TriggerDictionaryInitialization_HistoUnfoldingTH2Dict_Impl() {
    static const char* headers[] = {
"interface/HistoUnfoldingTH2.hh",
0
    };
    static const char* includePaths[] = {
"/cvmfs/cms.cern.ch/slc6_amd64_gcc530/lcg/root/6.06.00-ikhhed6/include",
"/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "HistoUnfoldingTH2Dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$interface/HistoUnfoldingTH2.hh")))  HistoUnfoldingTH2;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "HistoUnfoldingTH2Dict dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/interface/HistoUnfoldingTH2.hh"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"HistoUnfoldingTH2", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("HistoUnfoldingTH2Dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_HistoUnfoldingTH2Dict_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_HistoUnfoldingTH2Dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_HistoUnfoldingTH2Dict() {
  TriggerDictionaryInitialization_HistoUnfoldingTH2Dict_Impl();
}
