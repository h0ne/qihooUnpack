/*
 * MemMap.cpp
 *
 *  Created on: 2015年11月7日
 *      Author: hone
 */

#include "MemMap.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


MemMap::MemMap() :m_pData(0), m_uSize(0), m_nFile(0){
	// TODO Auto-generated constructor stub

}

//MemMap::~MemMap() {
//	// TODO Auto-generated destructor stub
//	UnMap();
//}

bool MemMap::Map(const char* szFileName){
	UnMap();
	m_nFile = open(szFileName, O_RDONLY);
	if (m_nFile < 0){
		m_nFile = 0;
		return false;
	}

	struct stat status;
	fstat(m_nFile, &status);

	m_uSize =status.st_size;
	m_pData = mmap(0, m_uSize, PROT_READ, MAP_SHARED, m_nFile, 0);
	if (MAP_FAILED != m_pData){
		return true;
	}

	close(m_nFile);
	m_pData = NULL;
	m_uSize = 0;
	m_nFile = 0;
	return false;
}

void MemMap::UnMap(){
	if (m_pData){
		munmap(m_pData, m_uSize);
		m_pData = NULL;
	}

	if (m_nFile){
		close(m_nFile);
		m_nFile = 0;
	}
	m_uSize = 0;
}
