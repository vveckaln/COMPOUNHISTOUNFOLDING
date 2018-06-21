// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME srcdIHistoUnfoldingTH1Dict

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
#include "/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/interface/HistoUnfoldingTH1.hh"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_HistoUnfoldingTH1(void *p = 0);
   static void *newArray_HistoUnfoldingTH1(Long_t size, void *p);
   static void delete_HistoUnfoldingTH1(void *p);
   static void deleteArray_HistoUnfoldingTH1(void *p);
   static void destruct_HistoUnfoldingTH1(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::HistoUnfoldingTH1*)
   {
      ::HistoUnfoldingTH1 *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::HistoUnfoldingTH1 >(0);
      static ::ROOT::TGenericClassInfo 
         instance("HistoUnfoldingTH1", ::HistoUnfoldingTH1::Class_Version(), "interface/HistoUnfoldingTH1.hh", 4,
                  typeid(::HistoUnfoldingTH1), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::HistoUnfoldingTH1::Dictionary, isa_proxy, 4,
                  sizeof(::HistoUnfoldingTH1) );
      instance.SetNew(&new_HistoUnfoldingTH1);
      instance.SetNewArray(&newArray_HistoUnfoldingTH1);
      instance.SetDelete(&delete_HistoUnfoldingTH1);
      instance.SetDeleteArray(&deleteArray_HistoUnfoldingTH1);
      instance.SetDestructor(&destruct_HistoUnfoldingTH1);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::HistoUnfoldingTH1*)
   {
      return GenerateInitInstanceLocal((::HistoUnfoldingTH1*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::HistoUnfoldingTH1*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr HistoUnfoldingTH1::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *HistoUnfoldingTH1::Class_Name()
{
   return "HistoUnfoldingTH1";
}

//______________________________________________________________________________
const char *HistoUnfoldingTH1::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::HistoUnfoldingTH1*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int HistoUnfoldingTH1::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::HistoUnfoldingTH1*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *HistoUnfoldingTH1::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::HistoUnfoldingTH1*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *HistoUnfoldingTH1::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::HistoUnfoldingTH1*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void HistoUnfoldingTH1::Streamer(TBuffer &R__b)
{
   // Stream an object of class HistoUnfoldingTH1.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(HistoUnfoldingTH1::Class(),this);
   } else {
      R__b.WriteClassBuffer(HistoUnfoldingTH1::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_HistoUnfoldingTH1(void *p) {
      return  p ? new(p) ::HistoUnfoldingTH1 : new ::HistoUnfoldingTH1;
   }
   static void *newArray_HistoUnfoldingTH1(Long_t nElements, void *p) {
      return p ? new(p) ::HistoUnfoldingTH1[nElements] : new ::HistoUnfoldingTH1[nElements];
   }
   // Wrapper around operator delete
   static void delete_HistoUnfoldingTH1(void *p) {
      delete ((::HistoUnfoldingTH1*)p);
   }
   static void deleteArray_HistoUnfoldingTH1(void *p) {
      delete [] ((::HistoUnfoldingTH1*)p);
   }
   static void destruct_HistoUnfoldingTH1(void *p) {
      typedef ::HistoUnfoldingTH1 current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::HistoUnfoldingTH1

namespace {
  void TriggerDictionaryInitialization_HistoUnfoldingTH1Dict_Impl() {
    static const char* headers[] = {
"interface/HistoUnfoldingTH1.hh",
0
    };
    static const char* includePaths[] = {
"/cvmfs/cms.cern.ch/slc6_amd64_gcc530/lcg/root/6.06.00-ikhhed6/include",
"/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "HistoUnfoldingTH1Dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$interface/HistoUnfoldingTH1.hh")))  HistoUnfoldingTH1;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "HistoUnfoldingTH1Dict dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/interface/HistoUnfoldingTH1.hh"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"HistoUnfoldingTH1", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("HistoUnfoldingTH1Dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_HistoUnfoldingTH1Dict_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_HistoUnfoldingTH1Dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_HistoUnfoldingTH1Dict() {
  TriggerDictionaryInitialization_HistoUnfoldingTH1Dict_Impl();
}
