/*
 * MemMap.h
 *
 *  Created on: 2015年11月7日
 *      Author: hone
 */

#ifndef MEMMAP_H_
#define MEMMAP_H_
#include <stdio.h>

class MemMap {
public:
	MemMap();
	//virtual ~MemMap();

	bool Map(const char* szFileName);
	void UnMap();

	const void* GetData() const { return m_pData; }
	size_t GetSize() const { return m_uSize; }

private:
	void *m_pData;
	size_t m_uSize;
	int m_nFile;
};

#endif /* MEMMAP_H_ */
