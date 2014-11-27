/********************************************************************
 * (C) Copyright 2014 by Autodesk, Inc. All Rights Reserved. By using
 * this code,  you  are  agreeing  to the terms and conditions of the
 * License  Agreement  included  in  the documentation for this code.
 * AUTODESK  MAKES  NO  WARRANTIES,  EXPRESS  OR  IMPLIED,  AS TO THE
 * CORRECTNESS OF THIS CODE OR ANY DERIVATIVE WORKS WHICH INCORPORATE
 * IT.  AUTODESK PROVIDES THE CODE ON AN 'AS-IS' BASIS AND EXPLICITLY
 * DISCLAIMS  ANY  LIABILITY,  INCLUDING CONSEQUENTIAL AND INCIDENTAL
 * DAMAGES  FOR ERRORS, OMISSIONS, AND  OTHER  PROBLEMS IN THE  CODE.
 *
 * Use, duplication,  or disclosure by the U.S. Government is subject
 * to  restrictions  set forth  in FAR 52.227-19 (Commercial Computer
 * Software Restricted Rights) as well as DFAR 252.227-7013(c)(1)(ii)
 * (Rights  in Technical Data and Computer Software),  as applicable.
 *******************************************************************/
#ifndef __LG_BINARYSERIALIZER_H__
#define __LG_BINARYSERIALIZER_H__

#include <vector>
#include <string>
#include <lgConfig.h>

// [RMS] VS2008 requires different handling for std::vector<bool>
//#define LG_BUILDING_WITH_VS2008

namespace rms {


template<typename T>
struct gcc_Identity_hack { typedef T type; };


#ifdef WIN32
#define BINSER_INLINE __declspec(noinline)
#else
#define BINSER_INLINE
#endif




template<typename T>
void BSerializer_append(BinarySerializer * pSerializer, const std::vector<T> & vec);

#ifdef LG_BUILDING_WITH_VS2008
template<>
static void BSerializer_append<bool>(BinarySerializer * pSerializer, const std::vector<bool> & b);
#endif // LG_BUILDING_WITH_VS2008

template<typename T>
bool BSerializer_restore(BinarySerializer * pSerializer, const std::vector<T> & vec);

#ifdef LG_BUILDING_WITH_VS2008
template<>
static bool BSerializer_restore<bool>(BinarySerializer * pSerializer, const std::vector<bool> & b);
#endif // LG_BUILDING_WITH_VS2008



class BinarySerializer
{
public:
	BinarySerializer() { beginRestore(); }
	BinarySerializer( const std::vector<unsigned char> & buf ) { m_vBuffer = buf; beginRestore(); }

	void reserveTotal( size_t nBytes ) {
		m_vBuffer.reserve(nBytes);
	}
	void reserveAppend( size_t nBytes ) {
		m_vBuffer.reserve( m_vBuffer.capacity() + nBytes );
	}

	template<typename T>
	BINSER_INLINE void append(T value) {
		unsigned int nCur = (unsigned int)m_vBuffer.size();
		m_vBuffer.resize(nCur + sizeof(T));
		memcpy_s(&m_vBuffer[nCur], sizeof(T), &value, sizeof(T));
	}

	BINSER_INLINE void append(const std::string & str) {
		unsigned int nStrLen = (unsigned int)str.length() + 1;
		append(nStrLen);
		unsigned int nCur = (unsigned int)m_vBuffer.size();
		m_vBuffer.resize(nCur + nStrLen);
		memcpy_s(&m_vBuffer[nCur], nStrLen, str.c_str(), nStrLen);
	}

	template<typename T>
	BINSER_INLINE void append(const T * pBuffer, unsigned int nSize) {
		unsigned int nStorageSize = nSize * sizeof(T);
		append(nStorageSize);
		unsigned int nCur = (unsigned int)m_vBuffer.size();
		m_vBuffer.resize(nCur + nStorageSize);
		memcpy_s(&m_vBuffer[nCur], nStorageSize, pBuffer, nStorageSize);
	}

#if 0
	template<typename T>
	BINSER_INLINE void append(const std::vector<T> & vec) {
		unsigned int nSize = (unsigned int)vec.size();
		append( &vec[0], nSize);
	}

	// [RMS] Microsoft STL defines a template class specialization for vector<bool> that only
	//  uses one bit per element. So we need a special case to serialize this class.
	template<>
	BINSER_INLINE void append<bool>(const std::vector<bool> & b) {
		unsigned int nSize = (unsigned int)b.size();
		const std::_Vbase * pStart = b.begin()._Myptr;
		const std::_Vbase * pEnd = b.end()._Myptr;
		unsigned int nElements = (unsigned int)(pEnd - pStart);
		if ( nSize % std::_VBITS != 0 )
			nElements++;
		append( nSize );
		append( pStart, nElements );
	}
#else
	template<typename T>
	BINSER_INLINE void append(const std::vector<T> & vec) {
		BSerializer_append(this, vec );
	}
#endif


	void beginRestore() {
		m_nCursor = 0;
	}

	template<typename T>
	BINSER_INLINE bool restore(T & value) {
		lgRASSERT( m_nCursor + sizeof(T) <= m_vBuffer.size() );
		memcpy_s(&value, sizeof(T), &m_vBuffer[m_nCursor], sizeof(T));
		m_nCursor += sizeof(T);
		return true;
	}
	template<typename T>
	BINSER_INLINE bool peek(T & value) {
		lgRASSERT( m_nCursor + sizeof(T) <= m_vBuffer.size() );
		memcpy_s(&value, sizeof(T), &m_vBuffer[m_nCursor], sizeof(T));
		return true;
	}



	BINSER_INLINE bool restore(std::string & str) {
		unsigned int nStrLen;
		const unsigned char * pBlock = restoreBlock(nStrLen);
		str = std::string(reinterpret_cast<const char *>(pBlock));
		return true;
	}

	BINSER_INLINE bool test_restore(const std::string & testStr) {
		unsigned int nStrLen = (unsigned int)testStr.length();
		const unsigned char * pBlock = peekBlock(nStrLen);
		std::string str = std::string(reinterpret_cast<const char *>(pBlock));
		return ( str.compare(testStr) == 0 );
		//return (str == testStr);		// [RMS] inexplicably this line gives errors in some cases...
	}


	template<typename T>
	BINSER_INLINE bool restore(T ** pBuffer, unsigned int & nSize) {
		unsigned int nBlockSize;
		const unsigned char * pBlock = restoreBlock(nBlockSize);
		nSize = nBlockSize/sizeof(T);
		*pBuffer = new T[nSize];
		memcpy_s(*pBuffer, nBlockSize, pBlock, nBlockSize);
		return true;
	}
	template<typename T>
	BINSER_INLINE bool restore(T * pBuffer, unsigned int nSize) {
		unsigned int nBlockSize;
		const unsigned char * pBlock = restoreBlock(nBlockSize);
		unsigned int nMySize = nBlockSize/sizeof(T);
		lgRASSERT(nMySize == nSize);
		memcpy_s(pBuffer, nBlockSize, pBlock, nBlockSize);
		return true;
	}


#if 0
	template<typename T>
	bool restore(std::vector<T> & vec) {
		unsigned int nSize;
		const unsigned char * pBlock = restoreBlock(nSize);
		vec.resize(nSize/sizeof(T));
		memcpy_s(&vec[0], nSize, pBlock, nSize);
		return true;
	}


	// [RMS] Microsoft STL defines a template class specialization for vector<bool> that only
	//  uses one bit per element. So we need a special case to serialize this class.
	// [WARNING] note that (at least in VS2008) this specialization *will not* be called if
	//   you try to pass a subclass of a bool vector, ie "class BoolVector : public std::vector<bool>"
	//   In that case you will get ::restore(T & value) instead. I don't know if this is a bug or not...
	template<>
	bool restore<bool>(std::vector<bool> & vec ) {
		unsigned int nSize;
		restore(nSize);
		vec.resize(nSize);
		std::_Vbase * pStart = const_cast<std::_Vbase *>( vec.begin()._Myptr );
		const std::_Vbase * pEnd = vec.end()._Myptr;
		unsigned int nElements = (unsigned int)(pEnd - pStart);
		if ( nSize % std::_VBITS != 0 )
			nElements++;
		return restore( pStart, nElements);
	}
#else
	template<typename T>
	bool restore(std::vector<T> & vec) {
		return BSerializer_restore(this, vec);
	}

#endif




#ifdef _WIN64
	/*
	 * [RMS] cannot store these types because they are incompatible between 32 and 64-bit, so catch with specializations
	 */
	template<>
	BINSER_INLINE void append<size_t>(size_t value) {
		lgDevelBreak();
	}
	template<>
	BINSER_INLINE bool restore<size_t>(size_t & value) {
		lgDevelBreak();
	}

#endif



	//!  this function has the same effect as restoring a string/vector/pointer, but
	//!  just returns the raw buffer pointer/size instead of allocating new memory
	BINSER_INLINE const unsigned char * restoreBlock(unsigned int & nBlockSize) {
		restore(nBlockSize);
		if ( nBlockSize > 0 ) {
			const unsigned char * pCur = &m_vBuffer[m_nCursor];
			m_nCursor += nBlockSize;
			return pCur;
		} else
			return NULL;
	}


	BINSER_INLINE const unsigned char * peekBlock(unsigned int & nBlockSize) {
		//peek(nBlockSize);
		const unsigned char * pCur = &m_vBuffer[m_nCursor+sizeof(unsigned int)];
		return pCur;
	}

	std::vector<unsigned char> & buffer() { return m_vBuffer; }
	const std::vector<unsigned char> & buffer() const { return m_vBuffer; }

	void setBuffer(std::vector<unsigned char> & buf) { m_vBuffer = buf; }

protected:
	std::vector<unsigned char> m_vBuffer;
	unsigned int m_nCursor;

};




template<typename T>
BINSER_INLINE void BSerializer_append(BinarySerializer * pSerializer, const std::vector<T> & vec) {
	unsigned int nSize = (unsigned int)vec.size();
	pSerializer->append( &vec[0], nSize);
}

#ifdef LG_BUILDING_WITH_VS2008

// [RMS] STL defines a template class specialization for vector<bool> that only
//  uses one bit per element. So we need a special case to serialize this class.
template<>
static BINSER_INLINE void BSerializer_append<bool>(BinarySerializer * pSerializer, const std::vector<bool> & b)
{
	unsigned int nSize = (unsigned int)b.size();
#ifdef WIN32
	const std::_Vbase * pStart = b.begin()._Myptr;
	const std::_Vbase * pEnd = b.end()._Myptr;
	unsigned int nElements = (unsigned int)(pEnd - pStart);
	if ( nSize % std::_VBITS != 0 )
		nElements++;
#else
	const std::_Bit_type * pStart = b.begin()._M_p;
	const std::_Bit_type * pEnd = b.end()._M_p;
	unsigned int nElements = (unsigned int)(pEnd - pStart);
	if ( nSize % std::_S_word_bit != 0 )
		nElements++;
#endif
	pSerializer->append( nSize );
	pSerializer->append( pStart, nElements );
}

#endif // LG_BUILDING_WITH_VS2008


template<typename T>
BINSER_INLINE bool BSerializer_restore(BinarySerializer * pSerializer, std::vector<T> & vec) {
	unsigned int nSize;
	const unsigned char * pBlock = pSerializer->restoreBlock(nSize);
	vec.resize(nSize/sizeof(T));
	if ( nSize > 0 )
		memcpy_s(&vec[0], nSize, pBlock, nSize);
	return true;
}


#ifdef LG_BUILDING_WITH_VS2008

// [RMS]  STL defines a template class specialization for vector<bool> that only
//  uses one bit per element. So we need a special case to serialize this class.
// [WARNING] note that (at least in VS2008) this specialization *will not* be called if
//   you try to pass a subclass of a bool vector, ie "class BoolVector : public std::vector<bool>"
//   In that case you will get ::restore(T & value) instead. I don't know if this is a bug or not...
//   [TODO] does this still happen now that we have moved to file-scope specializations? member-function template
//     specialization is not in C++ standard, only supported in visual studio, so maybe it was buggy after all...
template<>
static BINSER_INLINE bool BSerializer_restore(BinarySerializer * pSerializer, std::vector<bool> & vec ) {
	unsigned int nSize;
	pSerializer->restore(nSize);
	vec.resize(nSize);
#ifdef WIN32
	std::_Vbase * pStart = const_cast<std::_Vbase *>( vec.begin()._Myptr );
	const std::_Vbase * pEnd = vec.end()._Myptr;
	unsigned int nElements = (unsigned int)(pEnd - pStart);
	if ( nSize % std::_VBITS != 0 )
		nElements++;
#else
	std::_Bit_type * pStart = const_cast<std::_Bit_type *>( vec.begin()._M_p );
	const std::_Bit_type * pEnd = vec.end()._M_p;
	unsigned int nElements = (unsigned int)(pEnd - pStart);
	if ( nSize % std::_S_word_bit != 0 )
		nElements++;
#endif
	return pSerializer->restore( pStart, nElements);
}

#endif // LG_BUILDING_WITH_VS2008


} // end namespace rms

#endif // __BD_CACHE_BINARYSERIALIZER_H__
