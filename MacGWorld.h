// MacGWorld.h: interface for the CMacGWorld class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MACGWORLD_H__3547A645_0159_11D4_9953_000021D1C7AF__INCLUDED_)
#define AFX_MACGWORLD_H__3547A645_0159_11D4_9953_000021D1C7AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMacGWorld
{
public:
	CMacGWorld();
	CMacGWorld(const Rect* rcBound,int nPixelDepth=k16LE555PixelFormat,CTabHandle table=NULL);
	~CMacGWorld();
	void SetGWorld(const Rect* rcBound,int nPixelDepth=k16LE555PixelFormat,CTabHandle table=NULL);
	void BeginDrawing();
	void EndDrawing();
	const Rect& GetBoundRect() const { return m_rcBound; }
	PixMapHandle GetPixMapHandle() const { return GetGWorldPixMap(m_GWorld); }
	BitMap * GetBitMapHandle() const {return ((BitMap*)*GetGWorldPixMap(m_GWorld));}
	void CleanUp();
	CDC* GetDC() { return CDC::FromHandle((HDC)GetPortHDC((GrafPort*)m_GWorld)); }
	PicHandle GetPicture(const Rect* pRc=NULL);

private:
static void AdjustPixelDepth(int& nDepth,CTabHandle& table);
static PixMapHandle GetMaxMap();
	
	// without realization
	CMacGWorld(const CMacGWorld& g);
	CMacGWorld& operator=(const CMacGWorld& g);
protected:
	GWorldPtr m_GWorld;
	GDHandle m_OldDevice;
	CGrafPtr m_OldPort;
	bool m_bDrawing;
	Rect m_rcBound;
};

#endif // !defined(AFX_MACGWORLD_H__3547A645_0159_11D4_9953_000021D1C7AF__INCLUDED_)
