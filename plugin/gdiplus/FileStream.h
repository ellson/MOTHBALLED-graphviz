/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2008 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/

#ifndef GV_FILESTREAM_H
#define GV_FILESTREAM_H

#include <stdio.h>
#include <sys/stat.h>
#include <objbase.h>

class FileStream : public IStream
{
public:
	static IStream *Create(char* name, FILE *file);
	
	/* IUnknown methods */
	
	virtual STDMETHODCALLTYPE HRESULT QueryInterface( 
		REFIID riid,
		void **ppvObject);
    
	virtual STDMETHODCALLTYPE ULONG AddRef();
    
	virtual STDMETHODCALLTYPE ULONG Release();
            
 	/* ISequentialStream methods */
    
	virtual STDMETHODCALLTYPE HRESULT Read( 
		void *pv,
		ULONG cb,
		ULONG *pcbRead);
    
	virtual STDMETHODCALLTYPE HRESULT Write( 
		const void *pv,
		ULONG cb,
		ULONG *pcbWritten);

	/* IStream methods */
	
	virtual STDMETHODCALLTYPE HRESULT Seek( 
		LARGE_INTEGER dlibMove,
		DWORD dwOrigin,
		ULARGE_INTEGER *plibNewPosition);
    
	virtual STDMETHODCALLTYPE HRESULT SetSize( 
        	ULARGE_INTEGER libNewSize);
    
	virtual STDMETHODCALLTYPE HRESULT CopyTo( 
		IStream *pstm,
		ULARGE_INTEGER cb,
		ULARGE_INTEGER *pcbRead,
 		ULARGE_INTEGER *pcbWritten);
    
 	virtual STDMETHODCALLTYPE HRESULT Commit( 
        	DWORD grfCommitFlags);
    
 	virtual STDMETHODCALLTYPE HRESULT Revert();
    
	virtual STDMETHODCALLTYPE HRESULT LockRegion( 
        	ULARGE_INTEGER libOffset,
        	ULARGE_INTEGER cb,
        	DWORD dwLockType);
    
	virtual STDMETHODCALLTYPE HRESULT UnlockRegion( 
        	ULARGE_INTEGER libOffset,
        	ULARGE_INTEGER cb,
        	DWORD dwLockType);
    
	virtual STDMETHODCALLTYPE HRESULT Stat( 
        	STATSTG *pstatstg,
        	DWORD grfStatFlag);
    
	virtual STDMETHODCALLTYPE HRESULT Clone( 
        	IStream **ppstm);
        
private:
	FileStream(char *name, FILE *file);
	
	static void UnixTimeToFileTime(time_t unixTime, FILETIME &fileTime);
	
	ULONG _ref;
	char *_name;
	FILE *_file;

};

#endif
