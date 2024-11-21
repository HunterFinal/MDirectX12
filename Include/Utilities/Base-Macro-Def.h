/*

MRenderFramework
Author : MAI ZHICONG

Description : Base definition macro

Update History: 2024/11/01 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/
#pragma once

#ifndef M_BASE_MACRO_DEF
#define M_BASE_MACRO_DEF


#if defined(__clang__) || defined(__GNUC__)
    #define CPP_STANDARD __cpluspluc
#elif defined(_MSC_VER)
    #define CPP_STANDARD _MSVC_LANG
#endif

#define CONST_VAR(prop) const prop

#define L_VALUE_REF(prop) prop&
#define R_VALUE_REF(prop) prop&&
#define CONST_REF(prop) CONST_VAR(L_VALUE_REF(prop))

#define HAS_CPP_11 CPP_STANDARD >= 201103L

#if HAS_CPP_11
#define ALIAS(type, typeAlias) using typeAlias = type;
#define REF_ALIAS(type) using Ref = L_VALUE_REF(type);
#define CONST_REF_ALIAS(type) using Const_Ref = CONST_VAR(L_VALUE_REF(type));
#else
#define ALIAS(type, typeAlias) typedef typeAlias type;
#define REF_ALIAS(type) typedef type& Ref;
#define CONST_REF_ALIAS(type) typedef CONST(type&) Const_Ref;
#endif

#endif


