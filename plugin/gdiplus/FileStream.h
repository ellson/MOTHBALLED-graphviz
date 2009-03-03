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
	
	virtual HRESULT STDMETHODCALLTYPE  QueryInterface( 
		REFIID riid,
		void **ppvObject);
    
	virtual ULONG STDMETHODCALLTYPE  AddRef();
    
	virtual ULONG STDMETHODCALLTYPE  Release();
            
 	/* ISequentialStream methods */
    
	virtual HRESULT STDMETHODCALLTYPE  Read( 
		void *pv,
		ULONG cb,
		ULONG *pcbRead);
    
	virtual HRESULT STDMETHODCALLTYPE  Write( 
		const void *pv,
		ULONG cb,
		ULONG *pcbWritten);

	/* IStream methods */
	
	virtual HRESULT STDMETHODCALLTYPE  Seek( 
		LARGE_INTEGER dlibMove,
		DWORD dwOrigin,
		ULARGE_INTEGER *plibNewPosition);
    
	virtual HRESULT STDMETHODCALLTYPE  SetSize( 
        	ULARGE_INTEGER libNewSize);
    
	virtual HRESULT STDMETHODCALLTYPE  CopyTo( 
		IStream *pstm,
		ULARGE_INTEGER cb,
		ULARGE_INTEGER *pcbRead,
 		ULARGE_INTEGER *pcbWritten);
    
 	virtual HRESULT STDMETHODCALLTYPE  Commit( 
        	DWORD grfCommitFlags);
    
 	virtual HRESULT STDMETHODCALLTYPE  Revert();
    
	virtual HRESULT STDMETHODCALLTYPE  LockRegion( 
        	ULARGE_INTEGER libOffset,
        	ULARGE_INTEGER cb,
        	DWORD dwLockType);
    
	virtual HRESULT STDMETHODCALLTYPE  UnlockRegion( 
        	ULARGE_INTEGER libOffset,
        	ULARGE_INTEGER cb,
        	DWORD dwLockType);
    
	virtual HRESULT STDMETHODCALLTYPE  Stat( 
        	STATSTG *pstatstg,
        	DWORD grfStatFlag);
    
	virtual HRESULT STDMETHODCALLTYPE  Clone( 
        	IStream **ppstm);
        
private:
	FileStream(char *name, FILE *file);
	
	static void UnixTimeToFileTime(time_t unixTime, FILETIME &fileTime);
	
	ULONG _ref;
	char *_name;
	FILE *_file;

};

#endif
