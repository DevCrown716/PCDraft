#pragma once

#pragma pack(1)
struct MDFooter
{
   // if this is not 'RSRC' then read another footer, sizeof(MDFooter) back from this record
   long offset;		// offset from the end of file to the first resource
   long numBytes;		// the number of bytes that this "fork" contains
   long type;			// current 'RSRC' but may be adding another type in future versions
   long version;		// currently 0x00010000
   long size;			// always sizeof (MDFooter); added longs will be added at the BEGINNING of this struct
   long tag1;			// always 'RBAL'
   long tag2;			// always 'RPH '. if it is not these then it is not a MacDraft footer
};

struct MDResource
{
   long offset;			// offset to the next resource
   long length;			// size of the resource data after the resource name (see below)
   unsigned long resType;
   short resId;
   unsigned char flags;	// the standard Mac resource flags (locked, purgeable, resource, etc)
   unsigned char resourceNameLength;		// usually zero
   // if not zero, the next MDResource.resourceNameLength number of bytes is the resource name
   // now comes the resource data of size MDResource.length
};
#pragma pack()
extern unsigned long reportRowType[3];
void WriteResource(CFile* fo,long& forkLen,void* pRes,long len,unsigned long resType,short resId,const CString& resName="");
void WriteMFResource(CFile* fo,long& forkLen,CMemFile& mf,unsigned long resType,short resId,const CString& resName="");

void swapMDR(char* resFork,long len);
char* GetResource(unsigned long resType,short resId,char* resFork,long len,long* pLen=NULL);
bool GetResource(CMemFile & memFile,unsigned long resType,short resId,char* resFork,long len);