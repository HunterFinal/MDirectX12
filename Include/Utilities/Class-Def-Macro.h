/*

MRenderFramework
Author : MAI ZHICONG

Description : Class definition easy macro

Update History: 2024/11/01 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/
#pragma once

#ifndef M_CLASS_DEF_MACRO
#define M_CLASS_DEF_MACRO

#include <Base-Def-Macro.h>

#define GENERATE_CONSTRUCTOR(type,...) type(__VA_ARGS__)
#define GENERATE_DESTRUCTOR(type) virtual ~type()
#define GENERATE_COPY_CONSTRUCTOR(type) type(CONST_REF(type))
#define GENERATE_COPY_ASSIGNMENT(type) L_VALUE_REF(type) operator=(CONST_REF(type)) &
#define GENERATE_MOVE_CONSTRUCTOR(type) type(R_VALUE_REF(type))
#define GENERATE_MOVE_ASSIGNMENT(type) L_VALUE_REF(type) operator=(R_VALUE_REF(type)) &

#define DELETE_FUNC(func) func = delete;
#define DEFAULT_FUNC(func) func = default;
  
#define GENERATE_CLASS_NORMAL(type) \
public:                             \
  GENERATE_CONSTRUCTOR(type);       \
  GENERATE_DESTRUCTOR(type);        \
  GENERATE_COPY_CONSTRUCTOR(type);  \
  GENERATE_COPY_ASSIGNMENT(type);   \
  GENERATE_MOVE_CONSTRUCTOR(type);  \
  GENERATE_MOVE_ASSIGNMENT(type);    

#define GENERATE_CLASS_NO_COPY(type)              \
public:                                           \
  GENERATE_CONSTRUCTOR(type);                     \
  GENERATE_DESTRUCTOR(type);                      \
  DELETE_FUNC(GENERATE_COPY_CONSTRUCTOR(type))    \
  DELETE_FUNC(GENERATE_COPY_ASSIGNMENT(type))     \
  GENERATE_MOVE_CONSTRUCTOR(type);                \
  GENERATE_MOVE_ASSIGNMENT(type);    

#endif