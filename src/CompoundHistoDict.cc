// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME srcdICompoundHistoDict

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
#include "/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/interface/CompoundHisto.hh"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_Histo(void *p = 0);
   static void *newArray_Histo(Long_t size, void *p);
   static void delete_Histo(void *p);
   static void deleteArray_Histo(void *p);
   static void destruct_Histo(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::Histo*)
   {
      ::Histo *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::Histo >(0);
      static ::ROOT::TGenericClassInfo 
         instance("Histo", ::Histo::Class_Version(), "interface/CompoundHisto.hh", 19,
                  typeid(::Histo), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::Histo::Dictionary, isa_proxy, 4,
                  sizeof(::Histo) );
      instance.SetNew(&new_Histo);
      instance.SetNewArray(&newArray_Histo);
      instance.SetDelete(&delete_Histo);
      instance.SetDeleteArray(&deleteArray_Histo);
      instance.SetDestructor(&destruct_Histo);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::Histo*)
   {
      return GenerateInitInstanceLocal((::Histo*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::Histo*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_CompoundHisto(void *p = 0);
   static void *newArray_CompoundHisto(Long_t size, void *p);
   static void delete_CompoundHisto(void *p);
   static void deleteArray_CompoundHisto(void *p);
   static void destruct_CompoundHisto(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::CompoundHisto*)
   {
      ::CompoundHisto *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::CompoundHisto >(0);
      static ::ROOT::TGenericClassInfo 
         instance("CompoundHisto", ::CompoundHisto::Class_Version(), "interface/CompoundHisto.hh", 32,
                  typeid(::CompoundHisto), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::CompoundHisto::Dictionary, isa_proxy, 4,
                  sizeof(::CompoundHisto) );
      instance.SetNew(&new_CompoundHisto);
      instance.SetNewArray(&newArray_CompoundHisto);
      instance.SetDelete(&delete_CompoundHisto);
      instance.SetDeleteArray(&deleteArray_CompoundHisto);
      instance.SetDestructor(&destruct_CompoundHisto);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::CompoundHisto*)
   {
      return GenerateInitInstanceLocal((::CompoundHisto*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::CompoundHisto*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_MinCompoundHisto(void *p = 0);
   static void *newArray_MinCompoundHisto(Long_t size, void *p);
   static void delete_MinCompoundHisto(void *p);
   static void deleteArray_MinCompoundHisto(void *p);
   static void destruct_MinCompoundHisto(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::MinCompoundHisto*)
   {
      ::MinCompoundHisto *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::MinCompoundHisto >(0);
      static ::ROOT::TGenericClassInfo 
         instance("MinCompoundHisto", ::MinCompoundHisto::Class_Version(), "interface/CompoundHisto.hh", 78,
                  typeid(::MinCompoundHisto), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::MinCompoundHisto::Dictionary, isa_proxy, 4,
                  sizeof(::MinCompoundHisto) );
      instance.SetNew(&new_MinCompoundHisto);
      instance.SetNewArray(&newArray_MinCompoundHisto);
      instance.SetDelete(&delete_MinCompoundHisto);
      instance.SetDeleteArray(&deleteArray_MinCompoundHisto);
      instance.SetDestructor(&destruct_MinCompoundHisto);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::MinCompoundHisto*)
   {
      return GenerateInitInstanceLocal((::MinCompoundHisto*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::MinCompoundHisto*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr Histo::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *Histo::Class_Name()
{
   return "Histo";
}

//______________________________________________________________________________
const char *Histo::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::Histo*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int Histo::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::Histo*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *Histo::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::Histo*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *Histo::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::Histo*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr CompoundHisto::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *CompoundHisto::Class_Name()
{
   return "CompoundHisto";
}

//______________________________________________________________________________
const char *CompoundHisto::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::CompoundHisto*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int CompoundHisto::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::CompoundHisto*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *CompoundHisto::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::CompoundHisto*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *CompoundHisto::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::CompoundHisto*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr MinCompoundHisto::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *MinCompoundHisto::Class_Name()
{
   return "MinCompoundHisto";
}

//______________________________________________________________________________
const char *MinCompoundHisto::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MinCompoundHisto*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int MinCompoundHisto::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MinCompoundHisto*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *MinCompoundHisto::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MinCompoundHisto*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *MinCompoundHisto::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MinCompoundHisto*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void Histo::Streamer(TBuffer &R__b)
{
   // Stream an object of class Histo.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(Histo::Class(),this);
   } else {
      R__b.WriteClassBuffer(Histo::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_Histo(void *p) {
      return  p ? new(p) ::Histo : new ::Histo;
   }
   static void *newArray_Histo(Long_t nElements, void *p) {
      return p ? new(p) ::Histo[nElements] : new ::Histo[nElements];
   }
   // Wrapper around operator delete
   static void delete_Histo(void *p) {
      delete ((::Histo*)p);
   }
   static void deleteArray_Histo(void *p) {
      delete [] ((::Histo*)p);
   }
   static void destruct_Histo(void *p) {
      typedef ::Histo current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::Histo

//______________________________________________________________________________
void CompoundHisto::Streamer(TBuffer &R__b)
{
   // Stream an object of class CompoundHisto.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(CompoundHisto::Class(),this);
   } else {
      R__b.WriteClassBuffer(CompoundHisto::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_CompoundHisto(void *p) {
      return  p ? new(p) ::CompoundHisto : new ::CompoundHisto;
   }
   static void *newArray_CompoundHisto(Long_t nElements, void *p) {
      return p ? new(p) ::CompoundHisto[nElements] : new ::CompoundHisto[nElements];
   }
   // Wrapper around operator delete
   static void delete_CompoundHisto(void *p) {
      delete ((::CompoundHisto*)p);
   }
   static void deleteArray_CompoundHisto(void *p) {
      delete [] ((::CompoundHisto*)p);
   }
   static void destruct_CompoundHisto(void *p) {
      typedef ::CompoundHisto current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::CompoundHisto

//______________________________________________________________________________
void MinCompoundHisto::Streamer(TBuffer &R__b)
{
   // Stream an object of class MinCompoundHisto.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(MinCompoundHisto::Class(),this);
   } else {
      R__b.WriteClassBuffer(MinCompoundHisto::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_MinCompoundHisto(void *p) {
      return  p ? new(p) ::MinCompoundHisto : new ::MinCompoundHisto;
   }
   static void *newArray_MinCompoundHisto(Long_t nElements, void *p) {
      return p ? new(p) ::MinCompoundHisto[nElements] : new ::MinCompoundHisto[nElements];
   }
   // Wrapper around operator delete
   static void delete_MinCompoundHisto(void *p) {
      delete ((::MinCompoundHisto*)p);
   }
   static void deleteArray_MinCompoundHisto(void *p) {
      delete [] ((::MinCompoundHisto*)p);
   }
   static void destruct_MinCompoundHisto(void *p) {
      typedef ::MinCompoundHisto current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::MinCompoundHisto

namespace ROOT {
   static TClass *vectorlEHistomUgR_Dictionary();
   static void vectorlEHistomUgR_TClassManip(TClass*);
   static void *new_vectorlEHistomUgR(void *p = 0);
   static void *newArray_vectorlEHistomUgR(Long_t size, void *p);
   static void delete_vectorlEHistomUgR(void *p);
   static void deleteArray_vectorlEHistomUgR(void *p);
   static void destruct_vectorlEHistomUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<Histo*>*)
   {
      vector<Histo*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<Histo*>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<Histo*>", -2, "vector", 214,
                  typeid(vector<Histo*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEHistomUgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<Histo*>) );
      instance.SetNew(&new_vectorlEHistomUgR);
      instance.SetNewArray(&newArray_vectorlEHistomUgR);
      instance.SetDelete(&delete_vectorlEHistomUgR);
      instance.SetDeleteArray(&deleteArray_vectorlEHistomUgR);
      instance.SetDestructor(&destruct_vectorlEHistomUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<Histo*> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<Histo*>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEHistomUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<Histo*>*)0x0)->GetClass();
      vectorlEHistomUgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEHistomUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEHistomUgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<Histo*> : new vector<Histo*>;
   }
   static void *newArray_vectorlEHistomUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<Histo*>[nElements] : new vector<Histo*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEHistomUgR(void *p) {
      delete ((vector<Histo*>*)p);
   }
   static void deleteArray_vectorlEHistomUgR(void *p) {
      delete [] ((vector<Histo*>*)p);
   }
   static void destruct_vectorlEHistomUgR(void *p) {
      typedef vector<Histo*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<Histo*>

namespace ROOT {
   static TClass *maplETStringcOHistomUgR_Dictionary();
   static void maplETStringcOHistomUgR_TClassManip(TClass*);
   static void *new_maplETStringcOHistomUgR(void *p = 0);
   static void *newArray_maplETStringcOHistomUgR(Long_t size, void *p);
   static void delete_maplETStringcOHistomUgR(void *p);
   static void deleteArray_maplETStringcOHistomUgR(void *p);
   static void destruct_maplETStringcOHistomUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const map<TString,Histo*>*)
   {
      map<TString,Histo*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(map<TString,Histo*>));
      static ::ROOT::TGenericClassInfo 
         instance("map<TString,Histo*>", -2, "map", 96,
                  typeid(map<TString,Histo*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplETStringcOHistomUgR_Dictionary, isa_proxy, 0,
                  sizeof(map<TString,Histo*>) );
      instance.SetNew(&new_maplETStringcOHistomUgR);
      instance.SetNewArray(&newArray_maplETStringcOHistomUgR);
      instance.SetDelete(&delete_maplETStringcOHistomUgR);
      instance.SetDeleteArray(&deleteArray_maplETStringcOHistomUgR);
      instance.SetDestructor(&destruct_maplETStringcOHistomUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<TString,Histo*> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const map<TString,Histo*>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *maplETStringcOHistomUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const map<TString,Histo*>*)0x0)->GetClass();
      maplETStringcOHistomUgR_TClassManip(theClass);
   return theClass;
   }

   static void maplETStringcOHistomUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_maplETStringcOHistomUgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<TString,Histo*> : new map<TString,Histo*>;
   }
   static void *newArray_maplETStringcOHistomUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<TString,Histo*>[nElements] : new map<TString,Histo*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_maplETStringcOHistomUgR(void *p) {
      delete ((map<TString,Histo*>*)p);
   }
   static void deleteArray_maplETStringcOHistomUgR(void *p) {
      delete [] ((map<TString,Histo*>*)p);
   }
   static void destruct_maplETStringcOHistomUgR(void *p) {
      typedef map<TString,Histo*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class map<TString,Histo*>

namespace {
  void TriggerDictionaryInitialization_CompoundHistoDict_Impl() {
    static const char* headers[] = {
"interface/CompoundHisto.hh",
0
    };
    static const char* includePaths[] = {
"/cvmfs/cms.cern.ch/slc6_amd64_gcc530/lcg/root/6.06.00-ikhhed6/include",
"/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "CompoundHistoDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$interface/CompoundHisto.hh")))  Histo;
class __attribute__((annotate("$clingAutoload$interface/CompoundHisto.hh")))  CompoundHisto;
class __attribute__((annotate("$clingAutoload$interface/CompoundHisto.hh")))  MinCompoundHisto;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "CompoundHistoDict dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "/afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib/interface/CompoundHisto.hh"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"CompoundHisto", payloadCode, "@",
"Histo", payloadCode, "@",
"MinCompoundHisto", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("CompoundHistoDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_CompoundHistoDict_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_CompoundHistoDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_CompoundHistoDict() {
  TriggerDictionaryInitialization_CompoundHistoDict_Impl();
}
