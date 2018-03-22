// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME srcdICompoundHistoUnfoldingDict

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
#include "/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/interface/CompoundHistoUnfolding.hh"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_CompoundHistoUnfolding(void *p = 0);
   static void *newArray_CompoundHistoUnfolding(Long_t size, void *p);
   static void delete_CompoundHistoUnfolding(void *p);
   static void deleteArray_CompoundHistoUnfolding(void *p);
   static void destruct_CompoundHistoUnfolding(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::CompoundHistoUnfolding*)
   {
      ::CompoundHistoUnfolding *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::CompoundHistoUnfolding >(0);
      static ::ROOT::TGenericClassInfo 
         instance("CompoundHistoUnfolding", ::CompoundHistoUnfolding::Class_Version(), "interface/CompoundHistoUnfolding.hh", 21,
                  typeid(::CompoundHistoUnfolding), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::CompoundHistoUnfolding::Dictionary, isa_proxy, 4,
                  sizeof(::CompoundHistoUnfolding) );
      instance.SetNew(&new_CompoundHistoUnfolding);
      instance.SetNewArray(&newArray_CompoundHistoUnfolding);
      instance.SetDelete(&delete_CompoundHistoUnfolding);
      instance.SetDeleteArray(&deleteArray_CompoundHistoUnfolding);
      instance.SetDestructor(&destruct_CompoundHistoUnfolding);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::CompoundHistoUnfolding*)
   {
      return GenerateInitInstanceLocal((::CompoundHistoUnfolding*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::CompoundHistoUnfolding*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr CompoundHistoUnfolding::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *CompoundHistoUnfolding::Class_Name()
{
   return "CompoundHistoUnfolding";
}

//______________________________________________________________________________
const char *CompoundHistoUnfolding::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::CompoundHistoUnfolding*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int CompoundHistoUnfolding::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::CompoundHistoUnfolding*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *CompoundHistoUnfolding::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::CompoundHistoUnfolding*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *CompoundHistoUnfolding::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::CompoundHistoUnfolding*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void CompoundHistoUnfolding::Streamer(TBuffer &R__b)
{
   // Stream an object of class CompoundHistoUnfolding.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(CompoundHistoUnfolding::Class(),this);
   } else {
      R__b.WriteClassBuffer(CompoundHistoUnfolding::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_CompoundHistoUnfolding(void *p) {
      return  p ? new(p) ::CompoundHistoUnfolding : new ::CompoundHistoUnfolding;
   }
   static void *newArray_CompoundHistoUnfolding(Long_t nElements, void *p) {
      return p ? new(p) ::CompoundHistoUnfolding[nElements] : new ::CompoundHistoUnfolding[nElements];
   }
   // Wrapper around operator delete
   static void delete_CompoundHistoUnfolding(void *p) {
      delete ((::CompoundHistoUnfolding*)p);
   }
   static void deleteArray_CompoundHistoUnfolding(void *p) {
      delete [] ((::CompoundHistoUnfolding*)p);
   }
   static void destruct_CompoundHistoUnfolding(void *p) {
      typedef ::CompoundHistoUnfolding current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::CompoundHistoUnfolding

namespace ROOT {
   static TClass *vectorlEHistoUnfoldingmUgR_Dictionary();
   static void vectorlEHistoUnfoldingmUgR_TClassManip(TClass*);
   static void *new_vectorlEHistoUnfoldingmUgR(void *p = 0);
   static void *newArray_vectorlEHistoUnfoldingmUgR(Long_t size, void *p);
   static void delete_vectorlEHistoUnfoldingmUgR(void *p);
   static void deleteArray_vectorlEHistoUnfoldingmUgR(void *p);
   static void destruct_vectorlEHistoUnfoldingmUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<HistoUnfolding*>*)
   {
      vector<HistoUnfolding*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<HistoUnfolding*>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<HistoUnfolding*>", -2, "vector", 214,
                  typeid(vector<HistoUnfolding*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEHistoUnfoldingmUgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<HistoUnfolding*>) );
      instance.SetNew(&new_vectorlEHistoUnfoldingmUgR);
      instance.SetNewArray(&newArray_vectorlEHistoUnfoldingmUgR);
      instance.SetDelete(&delete_vectorlEHistoUnfoldingmUgR);
      instance.SetDeleteArray(&deleteArray_vectorlEHistoUnfoldingmUgR);
      instance.SetDestructor(&destruct_vectorlEHistoUnfoldingmUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<HistoUnfolding*> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<HistoUnfolding*>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEHistoUnfoldingmUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<HistoUnfolding*>*)0x0)->GetClass();
      vectorlEHistoUnfoldingmUgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEHistoUnfoldingmUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEHistoUnfoldingmUgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<HistoUnfolding*> : new vector<HistoUnfolding*>;
   }
   static void *newArray_vectorlEHistoUnfoldingmUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<HistoUnfolding*>[nElements] : new vector<HistoUnfolding*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEHistoUnfoldingmUgR(void *p) {
      delete ((vector<HistoUnfolding*>*)p);
   }
   static void deleteArray_vectorlEHistoUnfoldingmUgR(void *p) {
      delete [] ((vector<HistoUnfolding*>*)p);
   }
   static void destruct_vectorlEHistoUnfoldingmUgR(void *p) {
      typedef vector<HistoUnfolding*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<HistoUnfolding*>

namespace ROOT {
   static TClass *maplETStringcOHistoUnfoldingmUgR_Dictionary();
   static void maplETStringcOHistoUnfoldingmUgR_TClassManip(TClass*);
   static void *new_maplETStringcOHistoUnfoldingmUgR(void *p = 0);
   static void *newArray_maplETStringcOHistoUnfoldingmUgR(Long_t size, void *p);
   static void delete_maplETStringcOHistoUnfoldingmUgR(void *p);
   static void deleteArray_maplETStringcOHistoUnfoldingmUgR(void *p);
   static void destruct_maplETStringcOHistoUnfoldingmUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const map<TString,HistoUnfolding*>*)
   {
      map<TString,HistoUnfolding*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(map<TString,HistoUnfolding*>));
      static ::ROOT::TGenericClassInfo 
         instance("map<TString,HistoUnfolding*>", -2, "map", 96,
                  typeid(map<TString,HistoUnfolding*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplETStringcOHistoUnfoldingmUgR_Dictionary, isa_proxy, 0,
                  sizeof(map<TString,HistoUnfolding*>) );
      instance.SetNew(&new_maplETStringcOHistoUnfoldingmUgR);
      instance.SetNewArray(&newArray_maplETStringcOHistoUnfoldingmUgR);
      instance.SetDelete(&delete_maplETStringcOHistoUnfoldingmUgR);
      instance.SetDeleteArray(&deleteArray_maplETStringcOHistoUnfoldingmUgR);
      instance.SetDestructor(&destruct_maplETStringcOHistoUnfoldingmUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<TString,HistoUnfolding*> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const map<TString,HistoUnfolding*>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *maplETStringcOHistoUnfoldingmUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const map<TString,HistoUnfolding*>*)0x0)->GetClass();
      maplETStringcOHistoUnfoldingmUgR_TClassManip(theClass);
   return theClass;
   }

   static void maplETStringcOHistoUnfoldingmUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_maplETStringcOHistoUnfoldingmUgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<TString,HistoUnfolding*> : new map<TString,HistoUnfolding*>;
   }
   static void *newArray_maplETStringcOHistoUnfoldingmUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<TString,HistoUnfolding*>[nElements] : new map<TString,HistoUnfolding*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_maplETStringcOHistoUnfoldingmUgR(void *p) {
      delete ((map<TString,HistoUnfolding*>*)p);
   }
   static void deleteArray_maplETStringcOHistoUnfoldingmUgR(void *p) {
      delete [] ((map<TString,HistoUnfolding*>*)p);
   }
   static void destruct_maplETStringcOHistoUnfoldingmUgR(void *p) {
      typedef map<TString,HistoUnfolding*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class map<TString,HistoUnfolding*>

namespace {
  void TriggerDictionaryInitialization_CompoundHistoUnfoldingDict_Impl() {
    static const char* headers[] = {
"interface/CompoundHistoUnfolding.hh",
0
    };
    static const char* includePaths[] = {
"/cvmfs/cms.cern.ch/slc6_amd64_gcc530/lcg/root/6.06.00-ikhhed6/include",
"/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "CompoundHistoUnfoldingDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$interface/CompoundHistoUnfolding.hh")))  CompoundHistoUnfolding;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "CompoundHistoUnfoldingDict dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/interface/CompoundHistoUnfolding.hh"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"CompoundHistoUnfolding", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("CompoundHistoUnfoldingDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_CompoundHistoUnfoldingDict_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_CompoundHistoUnfoldingDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_CompoundHistoUnfoldingDict() {
  TriggerDictionaryInitialization_CompoundHistoUnfoldingDict_Impl();
}
