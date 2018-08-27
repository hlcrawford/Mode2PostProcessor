// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME srcdICrystalGeoDict

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
#include "src/crystalGeometry.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_CrystalGeometry(void *p = 0);
   static void *newArray_CrystalGeometry(Long_t size, void *p);
   static void delete_CrystalGeometry(void *p);
   static void deleteArray_CrystalGeometry(void *p);
   static void destruct_CrystalGeometry(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::CrystalGeometry*)
   {
      ::CrystalGeometry *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::CrystalGeometry >(0);
      static ::ROOT::TGenericClassInfo 
         instance("CrystalGeometry", ::CrystalGeometry::Class_Version(), "src/crystalGeometry.h", 35,
                  typeid(::CrystalGeometry), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::CrystalGeometry::Dictionary, isa_proxy, 4,
                  sizeof(::CrystalGeometry) );
      instance.SetNew(&new_CrystalGeometry);
      instance.SetNewArray(&newArray_CrystalGeometry);
      instance.SetDelete(&delete_CrystalGeometry);
      instance.SetDeleteArray(&deleteArray_CrystalGeometry);
      instance.SetDestructor(&destruct_CrystalGeometry);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::CrystalGeometry*)
   {
      return GenerateInitInstanceLocal((::CrystalGeometry*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::CrystalGeometry*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr CrystalGeometry::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *CrystalGeometry::Class_Name()
{
   return "CrystalGeometry";
}

//______________________________________________________________________________
const char *CrystalGeometry::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::CrystalGeometry*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int CrystalGeometry::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::CrystalGeometry*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *CrystalGeometry::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::CrystalGeometry*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *CrystalGeometry::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::CrystalGeometry*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void CrystalGeometry::Streamer(TBuffer &R__b)
{
   // Stream an object of class CrystalGeometry.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(CrystalGeometry::Class(),this);
   } else {
      R__b.WriteClassBuffer(CrystalGeometry::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_CrystalGeometry(void *p) {
      return  p ? new(p) ::CrystalGeometry : new ::CrystalGeometry;
   }
   static void *newArray_CrystalGeometry(Long_t nElements, void *p) {
      return p ? new(p) ::CrystalGeometry[nElements] : new ::CrystalGeometry[nElements];
   }
   // Wrapper around operator delete
   static void delete_CrystalGeometry(void *p) {
      delete ((::CrystalGeometry*)p);
   }
   static void deleteArray_CrystalGeometry(void *p) {
      delete [] ((::CrystalGeometry*)p);
   }
   static void destruct_CrystalGeometry(void *p) {
      typedef ::CrystalGeometry current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::CrystalGeometry

namespace {
  void TriggerDictionaryInitialization_CrystalGeoDict_Impl() {
    static const char* headers[] = {
"src/crystalGeometry.h",
0
    };
    static const char* includePaths[] = {
"/usr/local/Cellar/root6/6.06.08/include/root",
"/Users/HCrawford/Analysis/gretina-mode2manipulate/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "CrystalGeoDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$src/crystalGeometry.h")))  CrystalGeometry;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "CrystalGeoDict dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "src/crystalGeometry.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"CrystalGeometry", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("CrystalGeoDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_CrystalGeoDict_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_CrystalGeoDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_CrystalGeoDict() {
  TriggerDictionaryInitialization_CrystalGeoDict_Impl();
}
