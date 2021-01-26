// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME srcdIHistoUnfoldingDict

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
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
#include "/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/interface/HistoUnfolding.hh"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void delete_HistoUnfolding(void *p);
   static void deleteArray_HistoUnfolding(void *p);
   static void destruct_HistoUnfolding(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::HistoUnfolding*)
   {
      ::HistoUnfolding *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::HistoUnfolding >(0);
      static ::ROOT::TGenericClassInfo 
         instance("HistoUnfolding", ::HistoUnfolding::Class_Version(), "interface/HistoUnfolding.hh", 6,
                  typeid(::HistoUnfolding), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::HistoUnfolding::Dictionary, isa_proxy, 4,
                  sizeof(::HistoUnfolding) );
      instance.SetDelete(&delete_HistoUnfolding);
      instance.SetDeleteArray(&deleteArray_HistoUnfolding);
      instance.SetDestructor(&destruct_HistoUnfolding);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::HistoUnfolding*)
   {
      return GenerateInitInstanceLocal((::HistoUnfolding*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::HistoUnfolding*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr HistoUnfolding::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *HistoUnfolding::Class_Name()
{
   return "HistoUnfolding";
}

//______________________________________________________________________________
const char *HistoUnfolding::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::HistoUnfolding*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int HistoUnfolding::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::HistoUnfolding*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *HistoUnfolding::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::HistoUnfolding*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *HistoUnfolding::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::HistoUnfolding*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void HistoUnfolding::Streamer(TBuffer &R__b)
{
   // Stream an object of class HistoUnfolding.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(HistoUnfolding::Class(),this);
   } else {
      R__b.WriteClassBuffer(HistoUnfolding::Class(),this);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_HistoUnfolding(void *p) {
      delete ((::HistoUnfolding*)p);
   }
   static void deleteArray_HistoUnfolding(void *p) {
      delete [] ((::HistoUnfolding*)p);
   }
   static void destruct_HistoUnfolding(void *p) {
      typedef ::HistoUnfolding current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::HistoUnfolding

namespace {
  void TriggerDictionaryInitialization_HistoUnfoldingDict_Impl() {
    static const char* headers[] = {
"interface/HistoUnfolding.hh",
0
    };
    static const char* includePaths[] = {
"/cvmfs/cms.cern.ch/slc7_amd64_gcc630/lcg/root/6.10.08-fmblme2/include",
"/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "HistoUnfoldingDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$interface/HistoUnfolding.hh")))  HistoUnfolding;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "HistoUnfoldingDict dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/interface/HistoUnfolding.hh"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"HistoUnfolding", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("HistoUnfoldingDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_HistoUnfoldingDict_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_HistoUnfoldingDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_HistoUnfoldingDict() {
  TriggerDictionaryInitialization_HistoUnfoldingDict_Impl();
}
