/*
 * DexStruct.h
 *
 *  Created on: 2015年11月7日
 *      Author: hone
 */

#ifndef DEXSTRUCT_H_
#define DEXSTRUCT_H_
#include "object.h"


typedef unsigned char byte;
typedef unsigned long ulong;

enum { kSHA1DigestLen = 20,
       kSHA1DigestOutputLen = kSHA1DigestLen*2 +1 };

struct DexHeader {
    u1  magic[8];           /* includes version number */
    u4  checksum;           /* adler32 checksum */
    u1  signature[kSHA1DigestLen]; /* SHA-1 hash */
    u4  fileSize;           /* length of entire file */
    u4  headerSize;         /* offset to start of next section */
    u4  endianTag;
    u4  linkSize;
    u4  linkOff;
    u4  mapOff;
    u4  stringIdsSize;
    u4  stringIdsOff;
    u4  typeIdsSize;
    u4  typeIdsOff;
    u4  protoIdsSize;
    u4  protoIdsOff;
    u4  fieldIdsSize;
    u4  fieldIdsOff;
    u4  methodIdsSize;
    u4  methodIdsOff;
    u4  classDefsSize;
    u4  classDefsOff;
    u4  dataSize;
    u4  dataOff;
};

struct DexEncryptInfo{
	void* database;
	void* encrypybase;
	u4 offset;
};

typedef struct s_qihoo_header
{
	byte magic[4]; // qh\x00\x01
	ulong block_size; //不包括头
}qihoo_header;

typedef struct s_apk_info_header
{
	ulong size;
} apk_info_header;

typedef struct s_rc4_state
{
	int x, y;
	byte m[256];
} rc4_state;

#pragma pack (1)
typedef struct __attribute__ ((packed)) lzma_header_s  //和LZMA SDK里的有点区别
{
	byte props[5];
	u4 uncompr_size;
	u4 compr_size;
} lzma_header;
#pragma pack ()


#endif /* DEXSTRUCT_H_ */
