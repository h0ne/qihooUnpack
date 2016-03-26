/*
 * object.h
 *
 *  Created on: 2015年11月7日
 *      Author: hone
 */

#ifndef OBJECT_H_
#define OBJECT_H_


#ifdef HAVE_STDINT_H
# include <stdint.h>    /* C99 */


typedef uint8_t u1;
typedef uint16_t u2;
typedef uint32_t u4;
typedef uint64_t u8;
typedef int8_t s1;
typedef int16_t s2;
typedef int32_t s4;
typedef int64_t s8;
#else
typedef unsigned char u1;
typedef unsigned short u2;
typedef unsigned int u4;
typedef unsigned long long u8;
typedef signed char s1;
typedef signed short s2;
typedef signed int s4;
typedef signed long long s8;
#endif

/* fwd decl */
struct DataObject;
struct InitiatingLoaderList;
struct ClassObject;
struct StringObject;
struct ArrayObject;
struct Method;
struct ExceptionEntry;
struct LineNumEntry;
struct StaticField;
struct InstField;
struct Field;
struct RegisterMap;
struct DvmDex;
struct DexFile;
struct Object;
struct Thread;

union JValue {
	u1 z;
	s1 b;
	u2 c;
	s2 s;
	s4 i;
	s8 j;
	float f;
	double d;
	void* l;
};

#endif /* OBJECT_H_ */
