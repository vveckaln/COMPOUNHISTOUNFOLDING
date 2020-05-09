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
         instance("CompoundHistoUnfolding", ::CompoundHistoUnfolding::Class_Version(), "interface/CompoundHistoUnfolding.hh", 24,
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

namespace ROOT {
   static void *new_CompoundHistoUnfoldingcLcLLevel(void *p = 0);
   static void *newArray_CompoundHistoUnfoldingcLcLLevel(Long_t size, void *p);
   static void delete_CompoundHistoUnfoldingcLcLLevel(void *p);
   static void deleteArray_CompoundHistoUnfoldingcLcLLevel(void *p);
   static void destruct_CompoundHistoUnfoldingcLcLLevel(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::CompoundHistoUnfolding::Level*)
   {
      ::CompoundHistoUnfolding::Level *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::CompoundHistoUnfolding::Level >(0);
      static ::ROOT::TGenericClassInfo 
         instance("CompoundHistoUnfolding::Level", ::CompoundHistoUnfolding::Level::Class_Version(), "interface/CompoundHistoUnfolding.hh", 71,
                  typeid(::CompoundHistoUnfolding::Level), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::CompoundHistoUnfolding::Level::Dictionary, isa_proxy, 4,
                  sizeof(::CompoundHistoUnfolding::Level) );
      instance.SetNew(&new_CompoundHistoUnfoldingcLcLLevel);
      instance.SetNewArray(&newArray_CompoundHistoUnfoldingcLcLLevel);
      instance.SetDelete(&delete_CompoundHistoUnfoldingcLcLLevel);
      instance.SetDeleteArray(&deleteArray_CompoundHistoUnfoldingcLcLLevel);
      instance.SetDestructor(&destruct_CompoundHistoUnfoldingcLcLLevel);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::CompoundHistoUnfolding::Level*)
   {
      return GenerateInitInstanceLocal((::CompoundHistoUnfolding::Level*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::CompoundHistoUnfolding::Level*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_CompoundHistoUnfoldingcLcLLevelcLcLProjectionDeco(void *p = 0);
   static void *newArray_CompoundHistoUnfoldingcLcLLevelcLcLProjectionDeco(Long_t size, void *p);
   static void delete_CompoundHistoUnfoldingcLcLLevelcLcLProjectionDeco(void *p);
   static void deleteArray_CompoundHistoUnfoldingcLcLLevelcLcLProjectionDeco(void *p);
   static void destruct_CompoundHistoUnfoldingcLcLLevelcLcLProjectionDeco(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::CompoundHistoUnfolding::Level::ProjectionDeco*)
   {
      ::CompoundHistoUnfolding::Level::ProjectionDeco *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::CompoundHistoUnfolding::Level::ProjectionDeco >(0);
      static ::ROOT::TGenericClassInfo 
         instance("CompoundHistoUnfolding::Level::ProjectionDeco", ::CompoundHistoUnfolding::Level::ProjectionDeco::Class_Version(), "interface/CompoundHistoUnfolding.hh", 100,
                  typeid(::CompoundHistoUnfolding::Level::ProjectionDeco), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::CompoundHistoUnfolding::Level::ProjectionDeco::Dictionary, isa_proxy, 4,
                  sizeof(::CompoundHistoUnfolding::Level::ProjectionDeco) );
      instance.SetNew(&new_CompoundHistoUnfoldingcLcLLevelcLcLProjectionDeco);
      instance.SetNewArray(&newArray_CompoundHistoUnfoldingcLcLLevelcLcLProjectionDeco);
      instance.SetDelete(&delete_CompoundHistoUnfoldingcLcLLevelcLcLProjectionDeco);
      instance.SetDeleteArray(&deleteArray_CompoundHistoUnfoldingcLcLLevelcLcLProjectionDeco);
      instance.SetDestructor(&destruct_CompoundHistoUnfoldingcLcLLevelcLcLProjectionDeco);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::CompoundHistoUnfolding::Level::ProjectionDeco*)
   {
      return GenerateInitInstanceLocal((::CompoundHistoUnfolding::Level::ProjectionDeco*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::CompoundHistoUnfolding::Level::ProjectionDeco*)0x0); R__UseDummy(_R__UNIQUE_(Init));
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
atomic_TClass_ptr CompoundHistoUnfolding::Level::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *CompoundHistoUnfolding::Level::Class_Name()
{
   return "CompoundHistoUnfolding::Level";
}

//______________________________________________________________________________
const char *CompoundHistoUnfolding::Level::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::CompoundHistoUnfolding::Level*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int CompoundHistoUnfolding::Level::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::CompoundHistoUnfolding::Level*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *CompoundHistoUnfolding::Level::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::CompoundHistoUnfolding::Level*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *CompoundHistoUnfolding::Level::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::CompoundHistoUnfolding::Level*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr CompoundHistoUnfolding::Level::ProjectionDeco::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *CompoundHistoUnfolding::Level::ProjectionDeco::Class_Name()
{
   return "CompoundHistoUnfolding::Level::ProjectionDeco";
}

//______________________________________________________________________________
const char *CompoundHistoUnfolding::Level::ProjectionDeco::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::CompoundHistoUnfolding::Level::ProjectionDeco*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int CompoundHistoUnfolding::Level::ProjectionDeco::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::CompoundHistoUnfolding::Level::ProjectionDeco*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *CompoundHistoUnfolding::Level::ProjectionDeco::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::CompoundHistoUnfolding::Level::ProjectionDeco*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *CompoundHistoUnfolding::Level::ProjectionDeco::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::CompoundHistoUnfolding::Level::ProjectionDeco*)0x0)->GetClass(); }
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

//______________________________________________________________________________
void CompoundHistoUnfolding::Level::Streamer(TBuffer &R__b)
{
   // Stream an object of class CompoundHistoUnfolding::Level.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(CompoundHistoUnfolding::Level::Class(),this);
   } else {
      R__b.WriteClassBuffer(CompoundHistoUnfolding::Level::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_CompoundHistoUnfoldingcLcLLevel(void *p) {
      return  p ? new(p) ::CompoundHistoUnfolding::Level : new ::CompoundHistoUnfolding::Level;
   }
   static void *newArray_CompoundHistoUnfoldingcLcLLevel(Long_t nElements, void *p) {
      return p ? new(p) ::CompoundHistoUnfolding::Level[nElements] : new ::CompoundHistoUnfolding::Level[nElements];
   }
   // Wrapper around operator delete
   static void delete_CompoundHistoUnfoldingcLcLLevel(void *p) {
      delete ((::CompoundHistoUnfolding::Level*)p);
   }
   static void deleteArray_CompoundHistoUnfoldingcLcLLevel(void *p) {
      delete [] ((::CompoundHistoUnfolding::Level*)p);
   }
   static void destruct_CompoundHistoUnfoldingcLcLLevel(void *p) {
      typedef ::CompoundHistoUnfolding::Level current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::CompoundHistoUnfolding::Level

//______________________________________________________________________________
void CompoundHistoUnfolding::Level::ProjectionDeco::Streamer(TBuffer &R__b)
{
   // Stream an object of class CompoundHistoUnfolding::Level::ProjectionDeco.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(CompoundHistoUnfolding::Level::ProjectionDeco::Class(),this);
   } else {
      R__b.WriteClassBuffer(CompoundHistoUnfolding::Level::ProjectionDeco::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_CompoundHistoUnfoldingcLcLLevelcLcLProjectionDeco(void *p) {
      return  p ? new(p) ::CompoundHistoUnfolding::Level::ProjectionDeco : new ::CompoundHistoUnfolding::Level::ProjectionDeco;
   }
   static void *newArray_CompoundHistoUnfoldingcLcLLevelcLcLProjectionDeco(Long_t nElements, void *p) {
      return p ? new(p) ::CompoundHistoUnfolding::Level::ProjectionDeco[nElements] : new ::CompoundHistoUnfolding::Level::ProjectionDeco[nElements];
   }
   // Wrapper around operator delete
   static void delete_CompoundHistoUnfoldingcLcLLevelcLcLProjectionDeco(void *p) {
      delete ((::CompoundHistoUnfolding::Level::ProjectionDeco*)p);
   }
   static void deleteArray_CompoundHistoUnfoldingcLcLLevelcLcLProjectionDeco(void *p) {
      delete [] ((::CompoundHistoUnfolding::Level::ProjectionDeco*)p);
   }
   static void destruct_CompoundHistoUnfoldingcLcLLevelcLcLProjectionDeco(void *p) {
      typedef ::CompoundHistoUnfolding::Level::ProjectionDeco current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::CompoundHistoUnfolding::Level::ProjectionDeco

namespace ROOT {
   static TClass *vectorlEarraylEHistoUnfoldingmUcO2gRsPgR_Dictionary();
   static void vectorlEarraylEHistoUnfoldingmUcO2gRsPgR_TClassManip(TClass*);
   static void *new_vectorlEarraylEHistoUnfoldingmUcO2gRsPgR(void *p = 0);
   static void *newArray_vectorlEarraylEHistoUnfoldingmUcO2gRsPgR(Long_t size, void *p);
   static void delete_vectorlEarraylEHistoUnfoldingmUcO2gRsPgR(void *p);
   static void deleteArray_vectorlEarraylEHistoUnfoldingmUcO2gRsPgR(void *p);
   static void destruct_vectorlEarraylEHistoUnfoldingmUcO2gRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<array<HistoUnfolding*,2> >*)
   {
      vector<array<HistoUnfolding*,2> > *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<array<HistoUnfolding*,2> >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<array<HistoUnfolding*,2> >", -2, "vector", 214,
                  typeid(vector<array<HistoUnfolding*,2> >), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEarraylEHistoUnfoldingmUcO2gRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<array<HistoUnfolding*,2> >) );
      instance.SetNew(&new_vectorlEarraylEHistoUnfoldingmUcO2gRsPgR);
      instance.SetNewArray(&newArray_vectorlEarraylEHistoUnfoldingmUcO2gRsPgR);
      instance.SetDelete(&delete_vectorlEarraylEHistoUnfoldingmUcO2gRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlEarraylEHistoUnfoldingmUcO2gRsPgR);
      instance.SetDestructor(&destruct_vectorlEarraylEHistoUnfoldingmUcO2gRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<array<HistoUnfolding*,2> > >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<array<HistoUnfolding*,2> >*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEarraylEHistoUnfoldingmUcO2gRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<array<HistoUnfolding*,2> >*)0x0)->GetClass();
      vectorlEarraylEHistoUnfoldingmUcO2gRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEarraylEHistoUnfoldingmUcO2gRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEarraylEHistoUnfoldingmUcO2gRsPgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<array<HistoUnfolding*,2> > : new vector<array<HistoUnfolding*,2> >;
   }
   static void *newArray_vectorlEarraylEHistoUnfoldingmUcO2gRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<array<HistoUnfolding*,2> >[nElements] : new vector<array<HistoUnfolding*,2> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEarraylEHistoUnfoldingmUcO2gRsPgR(void *p) {
      delete ((vector<array<HistoUnfolding*,2> >*)p);
   }
   static void deleteArray_vectorlEarraylEHistoUnfoldingmUcO2gRsPgR(void *p) {
      delete [] ((vector<array<HistoUnfolding*,2> >*)p);
   }
   static void destruct_vectorlEarraylEHistoUnfoldingmUcO2gRsPgR(void *p) {
      typedef vector<array<HistoUnfolding*,2> > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<array<HistoUnfolding*,2> >

namespace ROOT {
   static TClass *vectorlETObjectmUgR_Dictionary();
   static void vectorlETObjectmUgR_TClassManip(TClass*);
   static void *new_vectorlETObjectmUgR(void *p = 0);
   static void *newArray_vectorlETObjectmUgR(Long_t size, void *p);
   static void delete_vectorlETObjectmUgR(void *p);
   static void deleteArray_vectorlETObjectmUgR(void *p);
   static void destruct_vectorlETObjectmUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<TObject*>*)
   {
      vector<TObject*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<TObject*>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<TObject*>", -2, "vector", 214,
                  typeid(vector<TObject*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlETObjectmUgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<TObject*>) );
      instance.SetNew(&new_vectorlETObjectmUgR);
      instance.SetNewArray(&newArray_vectorlETObjectmUgR);
      instance.SetDelete(&delete_vectorlETObjectmUgR);
      instance.SetDeleteArray(&deleteArray_vectorlETObjectmUgR);
      instance.SetDestructor(&destruct_vectorlETObjectmUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<TObject*> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<TObject*>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlETObjectmUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<TObject*>*)0x0)->GetClass();
      vectorlETObjectmUgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlETObjectmUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlETObjectmUgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TObject*> : new vector<TObject*>;
   }
   static void *newArray_vectorlETObjectmUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TObject*>[nElements] : new vector<TObject*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlETObjectmUgR(void *p) {
      delete ((vector<TObject*>*)p);
   }
   static void deleteArray_vectorlETObjectmUgR(void *p) {
      delete [] ((vector<TObject*>*)p);
   }
   static void destruct_vectorlETObjectmUgR(void *p) {
      typedef vector<TObject*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<TObject*>

namespace ROOT {
   static TClass *vectorlESampleDescriptormUgR_Dictionary();
   static void vectorlESampleDescriptormUgR_TClassManip(TClass*);
   static void *new_vectorlESampleDescriptormUgR(void *p = 0);
   static void *newArray_vectorlESampleDescriptormUgR(Long_t size, void *p);
   static void delete_vectorlESampleDescriptormUgR(void *p);
   static void deleteArray_vectorlESampleDescriptormUgR(void *p);
   static void destruct_vectorlESampleDescriptormUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<SampleDescriptor*>*)
   {
      vector<SampleDescriptor*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<SampleDescriptor*>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<SampleDescriptor*>", -2, "vector", 214,
                  typeid(vector<SampleDescriptor*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlESampleDescriptormUgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<SampleDescriptor*>) );
      instance.SetNew(&new_vectorlESampleDescriptormUgR);
      instance.SetNewArray(&newArray_vectorlESampleDescriptormUgR);
      instance.SetDelete(&delete_vectorlESampleDescriptormUgR);
      instance.SetDeleteArray(&deleteArray_vectorlESampleDescriptormUgR);
      instance.SetDestructor(&destruct_vectorlESampleDescriptormUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<SampleDescriptor*> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<SampleDescriptor*>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlESampleDescriptormUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<SampleDescriptor*>*)0x0)->GetClass();
      vectorlESampleDescriptormUgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlESampleDescriptormUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlESampleDescriptormUgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<SampleDescriptor*> : new vector<SampleDescriptor*>;
   }
   static void *newArray_vectorlESampleDescriptormUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<SampleDescriptor*>[nElements] : new vector<SampleDescriptor*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlESampleDescriptormUgR(void *p) {
      delete ((vector<SampleDescriptor*>*)p);
   }
   static void deleteArray_vectorlESampleDescriptormUgR(void *p) {
      delete [] ((vector<SampleDescriptor*>*)p);
   }
   static void destruct_vectorlESampleDescriptormUgR(void *p) {
      typedef vector<SampleDescriptor*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<SampleDescriptor*>

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
   static TClass *maplETStringcOvectorlEarraylEHistoUnfoldingmUcO2gRsPgRsPgR_Dictionary();
   static void maplETStringcOvectorlEarraylEHistoUnfoldingmUcO2gRsPgRsPgR_TClassManip(TClass*);
   static void *new_maplETStringcOvectorlEarraylEHistoUnfoldingmUcO2gRsPgRsPgR(void *p = 0);
   static void *newArray_maplETStringcOvectorlEarraylEHistoUnfoldingmUcO2gRsPgRsPgR(Long_t size, void *p);
   static void delete_maplETStringcOvectorlEarraylEHistoUnfoldingmUcO2gRsPgRsPgR(void *p);
   static void deleteArray_maplETStringcOvectorlEarraylEHistoUnfoldingmUcO2gRsPgRsPgR(void *p);
   static void destruct_maplETStringcOvectorlEarraylEHistoUnfoldingmUcO2gRsPgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const map<TString,vector<array<HistoUnfolding*,2> > >*)
   {
      map<TString,vector<array<HistoUnfolding*,2> > > *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(map<TString,vector<array<HistoUnfolding*,2> > >));
      static ::ROOT::TGenericClassInfo 
         instance("map<TString,vector<array<HistoUnfolding*,2> > >", -2, "map", 96,
                  typeid(map<TString,vector<array<HistoUnfolding*,2> > >), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplETStringcOvectorlEarraylEHistoUnfoldingmUcO2gRsPgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(map<TString,vector<array<HistoUnfolding*,2> > >) );
      instance.SetNew(&new_maplETStringcOvectorlEarraylEHistoUnfoldingmUcO2gRsPgRsPgR);
      instance.SetNewArray(&newArray_maplETStringcOvectorlEarraylEHistoUnfoldingmUcO2gRsPgRsPgR);
      instance.SetDelete(&delete_maplETStringcOvectorlEarraylEHistoUnfoldingmUcO2gRsPgRsPgR);
      instance.SetDeleteArray(&deleteArray_maplETStringcOvectorlEarraylEHistoUnfoldingmUcO2gRsPgRsPgR);
      instance.SetDestructor(&destruct_maplETStringcOvectorlEarraylEHistoUnfoldingmUcO2gRsPgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<TString,vector<array<HistoUnfolding*,2> > > >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const map<TString,vector<array<HistoUnfolding*,2> > >*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *maplETStringcOvectorlEarraylEHistoUnfoldingmUcO2gRsPgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const map<TString,vector<array<HistoUnfolding*,2> > >*)0x0)->GetClass();
      maplETStringcOvectorlEarraylEHistoUnfoldingmUcO2gRsPgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void maplETStringcOvectorlEarraylEHistoUnfoldingmUcO2gRsPgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_maplETStringcOvectorlEarraylEHistoUnfoldingmUcO2gRsPgRsPgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<TString,vector<array<HistoUnfolding*,2> > > : new map<TString,vector<array<HistoUnfolding*,2> > >;
   }
   static void *newArray_maplETStringcOvectorlEarraylEHistoUnfoldingmUcO2gRsPgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<TString,vector<array<HistoUnfolding*,2> > >[nElements] : new map<TString,vector<array<HistoUnfolding*,2> > >[nElements];
   }
   // Wrapper around operator delete
   static void delete_maplETStringcOvectorlEarraylEHistoUnfoldingmUcO2gRsPgRsPgR(void *p) {
      delete ((map<TString,vector<array<HistoUnfolding*,2> > >*)p);
   }
   static void deleteArray_maplETStringcOvectorlEarraylEHistoUnfoldingmUcO2gRsPgRsPgR(void *p) {
      delete [] ((map<TString,vector<array<HistoUnfolding*,2> > >*)p);
   }
   static void destruct_maplETStringcOvectorlEarraylEHistoUnfoldingmUcO2gRsPgRsPgR(void *p) {
      typedef map<TString,vector<array<HistoUnfolding*,2> > > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class map<TString,vector<array<HistoUnfolding*,2> > >

namespace ROOT {
   static TClass *maplETStringcOvectorlESampleDescriptormUgRsPgR_Dictionary();
   static void maplETStringcOvectorlESampleDescriptormUgRsPgR_TClassManip(TClass*);
   static void *new_maplETStringcOvectorlESampleDescriptormUgRsPgR(void *p = 0);
   static void *newArray_maplETStringcOvectorlESampleDescriptormUgRsPgR(Long_t size, void *p);
   static void delete_maplETStringcOvectorlESampleDescriptormUgRsPgR(void *p);
   static void deleteArray_maplETStringcOvectorlESampleDescriptormUgRsPgR(void *p);
   static void destruct_maplETStringcOvectorlESampleDescriptormUgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const map<TString,vector<SampleDescriptor*> >*)
   {
      map<TString,vector<SampleDescriptor*> > *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(map<TString,vector<SampleDescriptor*> >));
      static ::ROOT::TGenericClassInfo 
         instance("map<TString,vector<SampleDescriptor*> >", -2, "map", 96,
                  typeid(map<TString,vector<SampleDescriptor*> >), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplETStringcOvectorlESampleDescriptormUgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(map<TString,vector<SampleDescriptor*> >) );
      instance.SetNew(&new_maplETStringcOvectorlESampleDescriptormUgRsPgR);
      instance.SetNewArray(&newArray_maplETStringcOvectorlESampleDescriptormUgRsPgR);
      instance.SetDelete(&delete_maplETStringcOvectorlESampleDescriptormUgRsPgR);
      instance.SetDeleteArray(&deleteArray_maplETStringcOvectorlESampleDescriptormUgRsPgR);
      instance.SetDestructor(&destruct_maplETStringcOvectorlESampleDescriptormUgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<TString,vector<SampleDescriptor*> > >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const map<TString,vector<SampleDescriptor*> >*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *maplETStringcOvectorlESampleDescriptormUgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const map<TString,vector<SampleDescriptor*> >*)0x0)->GetClass();
      maplETStringcOvectorlESampleDescriptormUgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void maplETStringcOvectorlESampleDescriptormUgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_maplETStringcOvectorlESampleDescriptormUgRsPgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<TString,vector<SampleDescriptor*> > : new map<TString,vector<SampleDescriptor*> >;
   }
   static void *newArray_maplETStringcOvectorlESampleDescriptormUgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<TString,vector<SampleDescriptor*> >[nElements] : new map<TString,vector<SampleDescriptor*> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_maplETStringcOvectorlESampleDescriptormUgRsPgR(void *p) {
      delete ((map<TString,vector<SampleDescriptor*> >*)p);
   }
   static void deleteArray_maplETStringcOvectorlESampleDescriptormUgRsPgR(void *p) {
      delete [] ((map<TString,vector<SampleDescriptor*> >*)p);
   }
   static void destruct_maplETStringcOvectorlESampleDescriptormUgRsPgR(void *p) {
      typedef map<TString,vector<SampleDescriptor*> > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class map<TString,vector<SampleDescriptor*> >

namespace ROOT {
   static TClass *maplETStringcOvectorlEHistoUnfoldingmUgRsPgR_Dictionary();
   static void maplETStringcOvectorlEHistoUnfoldingmUgRsPgR_TClassManip(TClass*);
   static void *new_maplETStringcOvectorlEHistoUnfoldingmUgRsPgR(void *p = 0);
   static void *newArray_maplETStringcOvectorlEHistoUnfoldingmUgRsPgR(Long_t size, void *p);
   static void delete_maplETStringcOvectorlEHistoUnfoldingmUgRsPgR(void *p);
   static void deleteArray_maplETStringcOvectorlEHistoUnfoldingmUgRsPgR(void *p);
   static void destruct_maplETStringcOvectorlEHistoUnfoldingmUgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const map<TString,vector<HistoUnfolding*> >*)
   {
      map<TString,vector<HistoUnfolding*> > *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(map<TString,vector<HistoUnfolding*> >));
      static ::ROOT::TGenericClassInfo 
         instance("map<TString,vector<HistoUnfolding*> >", -2, "map", 96,
                  typeid(map<TString,vector<HistoUnfolding*> >), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplETStringcOvectorlEHistoUnfoldingmUgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(map<TString,vector<HistoUnfolding*> >) );
      instance.SetNew(&new_maplETStringcOvectorlEHistoUnfoldingmUgRsPgR);
      instance.SetNewArray(&newArray_maplETStringcOvectorlEHistoUnfoldingmUgRsPgR);
      instance.SetDelete(&delete_maplETStringcOvectorlEHistoUnfoldingmUgRsPgR);
      instance.SetDeleteArray(&deleteArray_maplETStringcOvectorlEHistoUnfoldingmUgRsPgR);
      instance.SetDestructor(&destruct_maplETStringcOvectorlEHistoUnfoldingmUgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<TString,vector<HistoUnfolding*> > >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const map<TString,vector<HistoUnfolding*> >*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *maplETStringcOvectorlEHistoUnfoldingmUgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const map<TString,vector<HistoUnfolding*> >*)0x0)->GetClass();
      maplETStringcOvectorlEHistoUnfoldingmUgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void maplETStringcOvectorlEHistoUnfoldingmUgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_maplETStringcOvectorlEHistoUnfoldingmUgRsPgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<TString,vector<HistoUnfolding*> > : new map<TString,vector<HistoUnfolding*> >;
   }
   static void *newArray_maplETStringcOvectorlEHistoUnfoldingmUgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<TString,vector<HistoUnfolding*> >[nElements] : new map<TString,vector<HistoUnfolding*> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_maplETStringcOvectorlEHistoUnfoldingmUgRsPgR(void *p) {
      delete ((map<TString,vector<HistoUnfolding*> >*)p);
   }
   static void deleteArray_maplETStringcOvectorlEHistoUnfoldingmUgRsPgR(void *p) {
      delete [] ((map<TString,vector<HistoUnfolding*> >*)p);
   }
   static void destruct_maplETStringcOvectorlEHistoUnfoldingmUgRsPgR(void *p) {
      typedef map<TString,vector<HistoUnfolding*> > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class map<TString,vector<HistoUnfolding*> >

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
"CompoundHistoUnfolding::Level", payloadCode, "@",
"CompoundHistoUnfolding::Level::ProjectionDeco", payloadCode, "@",
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
