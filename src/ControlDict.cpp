// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME srcdIControlDict

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
#include "src/ControlParameters.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_controlParameters(void *p = 0);
   static void *newArray_controlParameters(Long_t size, void *p);
   static void delete_controlParameters(void *p);
   static void deleteArray_controlParameters(void *p);
   static void destruct_controlParameters(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::controlParameters*)
   {
      ::controlParameters *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::controlParameters >(0);
      static ::ROOT::TGenericClassInfo 
         instance("controlParameters", ::controlParameters::Class_Version(), "src/ControlParameters.h", 11,
                  typeid(::controlParameters), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::controlParameters::Dictionary, isa_proxy, 4,
                  sizeof(::controlParameters) );
      instance.SetNew(&new_controlParameters);
      instance.SetNewArray(&newArray_controlParameters);
      instance.SetDelete(&delete_controlParameters);
      instance.SetDeleteArray(&deleteArray_controlParameters);
      instance.SetDestructor(&destruct_controlParameters);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::controlParameters*)
   {
      return GenerateInitInstanceLocal((::controlParameters*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::controlParameters*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr controlParameters::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *controlParameters::Class_Name()
{
   return "controlParameters";
}

//______________________________________________________________________________
const char *controlParameters::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::controlParameters*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int controlParameters::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::controlParameters*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *controlParameters::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::controlParameters*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *controlParameters::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::controlParameters*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void controlParameters::Streamer(TBuffer &R__b)
{
   // Stream an object of class controlParameters.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(controlParameters::Class(),this);
   } else {
      R__b.WriteClassBuffer(controlParameters::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_controlParameters(void *p) {
      return  p ? new(p) ::controlParameters : new ::controlParameters;
   }
   static void *newArray_controlParameters(Long_t nElements, void *p) {
      return p ? new(p) ::controlParameters[nElements] : new ::controlParameters[nElements];
   }
   // Wrapper around operator delete
   static void delete_controlParameters(void *p) {
      delete ((::controlParameters*)p);
   }
   static void deleteArray_controlParameters(void *p) {
      delete [] ((::controlParameters*)p);
   }
   static void destruct_controlParameters(void *p) {
      typedef ::controlParameters current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::controlParameters

namespace {
  void TriggerDictionaryInitialization_ControlDict_Impl() {
    static const char* headers[] = {
"src/ControlParameters.h",
0
    };
    static const char* includePaths[] = {
"/usr/local/Cellar/root6/6.06.08/include/root",
"/Users/HCrawford/Analysis/gretina-mode2manipulate/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "ControlDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$src/ControlParameters.h")))  controlParameters;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "ControlDict dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "src/ControlParameters.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"controlParameters", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("ControlDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_ControlDict_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_ControlDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_ControlDict() {
  TriggerDictionaryInitialization_ControlDict_Impl();
}
