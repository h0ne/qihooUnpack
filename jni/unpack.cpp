/*
 * unpack.cpp
 *
 *  Created on: 2015年11月7日
 *      Author: hone
 */
//qihoo 1.5 工行手机银行

#include <jni.h>
#include <unistd.h>
#include <fcntl.h>
#include <android/log.h>

#include "MemMap.h"
#include "DexStruct.h"

#include "lzma/Alloc.h"
#include "lzma/7zFile.h"
#include "lzma/7zVersion.h"
#include "lzma/Lzma2Dec.h"
#include "lzma/LzmaEnc.h"

#define JNI_METHOD(name) Java_com_example_qihoounpack_MainActivity_##name
#define TAG "qihoo"

DexEncryptInfo dealTargetFile(const char* dexName){

	int fd = 0;
	int ret = -1;
	DexEncryptInfo dexencrypt;
	memset(&dexencrypt, 0x0, sizeof(dexencrypt));
	if (access(dexName, F_OK) == -1){
		__android_log_print(ANDROID_LOG_INFO, TAG, "not find dexfile");
		return dexencrypt;
	}

	MemMap mmfile;
	bool bFailed = !mmfile.Map(dexName);
	if (bFailed){
		return dexencrypt;
	}
	size_t uFileSize = mmfile.GetSize();
	const char* dexBase = (const char *)mmfile.GetData();
	if (uFileSize <= 0 || dexBase == NULL){
		return dexencrypt;
	}

	DexHeader *header = (DexHeader *)dexBase;
	int injectedSize= header->fileSize - (header->dataOff + header->dataSize);
	ret = memcmp((const void *)(dexBase + *(u4 *)(dexBase + 0x68) + *(u4 *)(dexBase + 0x6C)), "qh", 2u);
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "file_size=%d", uFileSize);
	if (!ret){
		dexencrypt.encrypybase = (void *)(dexBase + *(u4 *)(dexBase + 0x68) + *(u4 *)(dexBase + 0x6C));
		dexencrypt.database = (void *)dexBase;
		dexencrypt.offset = injectedSize;

		return dexencrypt;
	}

	else {
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "no find qh");
	}
	close(fd);
	return dexencrypt;
}
#define RC4_KEY_LEN 0x10
#define kSHA1DigestLen 20

void gen_key(const byte* encrypy_bytes, ulong len, byte* result, ulong& seed)
{
	ulong i = 0;
	int temp_key;
	int key;
	while (i != len){
		key = seed;
		temp_key = (*(byte *)(encrypy_bytes + i) ^ i) & 0xFF ^ key;
		*(byte *)(result + (i & 0xF)) = *(byte *)(encrypy_bytes + i) ^ i ^ key;
		++i;
		seed = temp_key;
	}
}

void init_key(rc4_state *s, unsigned char *key, int length)
{
	int i, j, k, a;

	byte *m;

	s->x = 0;
	s->y = 0;
	m = s->m;

	for (i = 0; i < 256; i++)
	{
		m[i] = i;
	}

	j = k = 0;

	for (i = 0; i < 256; i++)
	{
		a = m[i];
		j = (unsigned char)(j + a + key[k]);
		m[i] = m[j]; m[j] = a;
		if (++k >= length) k = 0;
	}
}

void rc4_crypt(rc4_state *s, byte *data, int length)
{
	int i, x, y, a, b;

	byte *m;

	x = s->x;
	y = s->y;
	m = s->m;

	for (i = 0; i < length; i++)
	{
		x = (unsigned char)(x + 1); a = m[x];
		y = (unsigned char)(y + a);
		m[x] = b = m[y];
		m[y] = a;
		data[i] ^= m[(unsigned char)(a + b)];
	}

	s->x = x;
	s->y = y;
}

static void *SzAlloc(void *p, size_t size) { return MyAlloc(size); }
static void SzFree(void *p, void *address) { MyFree(address); }
static ISzAlloc g_Alloc = { SzAlloc, SzFree };

static SRes lzma_uncompress(byte* dst_buff,  byte* src_buff, const lzma_header& header)
{


	int i;
	SRes res = 0;

	CLzmaDec state;

	LzmaDec_Construct(&state);
	RINOK(LzmaDec_Allocate(&state, header.props, LZMA_PROPS_SIZE, &g_Alloc));

	UInt64 unpackSize = header.uncompr_size;
	int thereIsSize = (unpackSize != (UInt64)(Int64)-1);
	size_t inPos = 0, inSize = 0, outPos = 0;
	LzmaDec_Init(&state);
	while (true)
	{
		if (inPos == inSize)
		{
			inSize = header.compr_size;
			inPos = 0;
		}
		SRes res;
		SizeT inProcessed = inSize - inPos;
		SizeT outProcessed = header.uncompr_size - outPos;
		ELzmaFinishMode finishMode = LZMA_FINISH_ANY;
		ELzmaStatus status;
		if (thereIsSize && outProcessed >= unpackSize)
		{
			outProcessed = (SizeT)unpackSize;
			finishMode = LZMA_FINISH_END;
		}

		res = LzmaDec_DecodeToBuf(&state, dst_buff + outPos, &outProcessed,
								  src_buff + inPos, &inProcessed, finishMode, &status);
		inPos += inProcessed;
		outPos += outProcessed;
		unpackSize -= outProcessed;

		outPos = 0;

		if (res != SZ_OK || thereIsSize && unpackSize == 0)
		{
			LzmaDec_Free(&state, &g_Alloc);
			return res;
		}

		if (inProcessed == 0 && outProcessed == 0)
		{
			if (thereIsSize || status != LZMA_STATUS_FINISHED_WITH_MARK)
			{
				LzmaDec_Free(&state, &g_Alloc);
				return SZ_ERROR_DATA;
			}
			LzmaDec_Free(&state, &g_Alloc);
			return res;
		}
	}
}
void decrypt_dex_header(byte* buff, ulong len, byte key)
{
	for (ulong i = 0; i != len; ++i)
	{
		buff[i] ^= key;
	}
}


extern "C" jint JNI_METHOD(unpackEntry)(JNIEnv* env, jclass *clazz, jstring target){

	const char *tar_path = env->GetStringUTFChars(target, 0);
	__android_log_print(ANDROID_LOG_INFO, TAG, "dexfile: %s", tar_path);
//	//单个dex解密
	DexEncryptInfo encrtptinfo =dealTargetFile(tar_path);
	byte *block = (byte *)encrtptinfo.encrypybase;
	if (block != NULL )
	{
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "encrypt_addr=%p, offset = %d", encrtptinfo.database, encrtptinfo.offset);
		qihoo_header *qh_header = (qihoo_header *)block;
		block += sizeof(qihoo_header);

		apk_info_header *info_header = (apk_info_header*)block;
		block += sizeof(apk_info_header);

		byte *info_buff = block;
		ulong seed = 0x7A;
		static byte key[RC4_KEY_LEN] = {0};
		gen_key(info_buff, info_header->size, key, seed);//size = 0x9B

		//解密 rc4初始化key
		byte* cipher_buff = block + info_header->size;//被加密的ori 数据
		ulong cipher_size = qh_header->block_size - sizeof(apk_info_header) - info_header->size;
		// rc4 解密
		rc4_state state;
		memset(&state, 0, sizeof(rc4_state));
		init_key(&state, key, RC4_KEY_LEN);
		byte* rc4_data = new byte[cipher_size];
		memcpy(rc4_data, cipher_buff, cipher_size);

		rc4_crypt(&state, rc4_data, cipher_size);

		lzma_header lz_header;
		memset(&lz_header, 0x00, sizeof(lzma_header));
		memcpy(&lz_header, rc4_data, sizeof(lzma_header));

		byte* compr_buff = rc4_data + sizeof(lzma_header);
		byte* uncompr_buff = new byte[lz_header.uncompr_size];
		__android_log_print(ANDROID_LOG_INFO, TAG, "uncompr_size = %x, compr_size=%x", lz_header.uncompr_size, lz_header.compr_size);
		if (lzma_uncompress(uncompr_buff, compr_buff, lz_header)){
			return -1;
		}

		decrypt_dex_header(uncompr_buff, sizeof(DexHeader), seed);

		FILE *fout = fopen("dec_classes.dex", "wb");
		if (!fout)
			return -1;
		fwrite(uncompr_buff, lz_header.uncompr_size, 1, fout);
		fclose(fout);

		delete[] uncompr_buff;
		delete[] rc4_data;

	} else{
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "no find dexEncryptInfo");
	}

	return 0;
}
