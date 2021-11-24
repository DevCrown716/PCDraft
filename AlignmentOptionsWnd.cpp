// AlignmentOptionsWnd.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "AlignmentOptionsWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define kSquareSize		20

/////////////////////////////////////////////////////////////////////////////
// CAlignmentOptionsWnd

CAlignmentOptionsWnd::CAlignmentOptionsWnd()
{
	m_Bitmap.LoadBitmap(IDB_ALIGN_GRID);
	ClearMatrix();
}

CAlignmentOptionsWnd::~CAlignmentOptionsWnd()
{
}

BEGIN_MESSAGE_MAP(CAlignmentOptionsWnd, CWnd)
	//{{AFX_MSG_MAP(CAlignmentOptionsWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

bool CAlignmentOptionsWnd::ToggleMatrix(short r,short c)
{
	return m_Cells[r][c]=!m_Cells[r][c];
}

void CAlignmentOptionsWnd::ClearMatrix()
{
	for(short r=0;r<3;r++)
		for(short c=0;c<3;c++)
			m_Cells[r][c]=false;
}

void CAlignmentOptionsWnd::ClearGridExcept(short r1,short c1)
{
	for(short r=0;r<3;r++)
		for(short c=0;c<3;c++)
			if(r!=r1 || c!=c1)
				m_Cells[r][c]=false;
}

bool CAlignmentOptionsWnd::VerifyExistingMatrix(AlignMask newAlignSet)
{
	// First, use the existing matrix to create a temporary align set.
	AlignMask tempAlignSet=alignMask_None;
	BuildAlignSet(tempAlignSet);
	// Now compare the two sets, remembering to mask off the non-grid bits for the newAlignSet
	return (newAlignSet&alignMask_Grid)==tempAlignSet;
}

void CAlignmentOptionsWnd::BuildMatrix(AlignMask nAlignSet)
{
	//Because the nAlignSet's grid control (held in 4 bits) can have multiple solutions when applied
	// to a 3 by 3 grid matrix, we will try to use the existing matrix that the user last worked with.
	// So long as VerifyMatrix does not fail we can use the existing matrix. otherwise we should rebuild
	// the matrix from a pre zeroed matrix.

	if(VerifyExistingMatrix(nAlignSet))
		return;

	ClearMatrix();
	if(nAlignSet & alignMask_Grid) // Something set
	{
		 // Center
	 	if(nAlignSet==alignMask_Center)
			m_Cells[1][1]=true;
		else
		{ // Corners
			if((nAlignSet&alignMask_Top) && (nAlignSet&alignMask_Left))
				m_Cells[0][0]=true;
			else
			if((nAlignSet&alignMask_Top) && (nAlignSet&alignMask_Right))
				m_Cells[0][2]=true;
			else
			if((nAlignSet&alignMask_Bottom) && (nAlignSet&alignMask_Left))
				m_Cells[2][0]=true;
			else
			if((nAlignSet&alignMask_Bottom) && (nAlignSet&alignMask_Right))
				m_Cells[2][2]=true;
			else
			{ // Others
				if((nAlignSet&alignMask_Top) && (nAlignSet&alignMask_Bottom))
				{
					m_Cells[0][1]=true;
					m_Cells[2][1]=true;
				}
				else
				{
					if(nAlignSet&alignMask_Top)
						m_Cells[0][1]=true;
					if(nAlignSet&alignMask_Bottom)
						m_Cells[2][1]=true;
				}
				if((nAlignSet&alignMask_Left) && (nAlignSet&alignMask_Right))
				{
					m_Cells[1][0]=true;
					m_Cells[1][2]=true;
				}
				else
				{
					if(nAlignSet&alignMask_Left)
						m_Cells[1][0]=true;
					if(nAlignSet&alignMask_Right)
						m_Cells[1][2]=true;
				}
			}
		}
	}
	if(GetSafeHwnd())
	{
		Invalidate();
		UpdateWindow();
	}
}

void CAlignmentOptionsWnd::BuildAlignSet(AlignMask& nAlignSet)
{
	nAlignSet &= ~alignMask_Grid; // Clear grid bits
	if(m_Cells[1][1])
		nAlignSet=alignMask_Center;
	else
		for(short r=0;r<3;r++)
			for(short c=0;c<3;c++)
				if(m_Cells[r][c])
				{
					switch(r)
					{
						case 0 : nAlignSet|=alignMask_Top; break;
						case 2 : nAlignSet|=alignMask_Bottom; break;
					}
					switch(c)
					{
						case 0 : nAlignSet|=alignMask_Left; break;
						case 2 : nAlignSet|=alignMask_Right; break;
					}
				}
}

bool CAlignmentOptionsWnd::TrackRect(CRect& rc)
{
	CClientDC theDC(this);
	SetCapture();
	MSG msg;
	bool wasInside=true;
	theDC.InvertRect(&rc);
	while(1)
	{
		GetMessage(&msg,NULL,0,0);
		ScreenToClient(&msg.pt);
		switch(msg.message)
		{
			case WM_MOUSEMOVE:
				if(rc.PtInRect(msg.pt))
				{
					if(!wasInside)
					{
						wasInside=true;
						theDC.InvertRect(&rc);
					}
				}
				else
				{
					if(wasInside)
					{
						wasInside=false;
						theDC.InvertRect(&rc);
					}
				}
				break;
			case WM_LBUTTONUP:
				ReleaseCapture();
				return wasInside;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAlignmentOptionsWnd message handlers

void CAlignmentOptionsWnd::OnPaint() 
{
	CPaintDC dc(this);
	if(IsWindowEnabled())
	{
		CGDIBitmapDC theBmpDC(&dc,&m_Bitmap);
		dc.BitBlt(0,0,61,61,&theBmpDC,0,0,SRCCOPY);
		CRect rc(2,2,kSquareSize-1,kSquareSize-1);
		for(int r=0;r<3;r++)
		{
			for(int c=0;c<3;c++)
			{
				if(m_Cells[r][c])
					dc.InvertRect(&rc);
				rc.OffsetRect(kSquareSize,0);
			}
			rc.OffsetRect(0,kSquareSize);
			rc.left=2;
			rc.right=kSquareSize-1;
		}
	}
	else
	{
		dc.DrawState(CPoint(0,0),CSize(61,61),&m_Bitmap,DST_BITMAP|DSS_DISABLED);
	}
}

void CAlignmentOptionsWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int c=point.x/kSquareSize;
	int r=point.y/kSquareSize;
	CRect rc(2,2,kSquareSize-1,kSquareSize-1);
	rc.OffsetRect(c*kSquareSize,r*kSquareSize);
	if(TrackRect(rc))
	{
		// Just adjust the matrix for the selected item - our track proc would have got the hilighting correct.
		if(ToggleMatrix(r,c)) // set the state of the grid cell that was selected, and if turned on...
		{ //turn off non valid patterns, and check for shortcuts
			bool aSide=((r != 1 || c != 1) && ((r % 2) || (c % 2)));

			if(aSide && m_Cells[0][1] && m_Cells[1][0] && m_Cells[1][2] && m_Cells[2][1])
			{// All sides selected - make a shortcut for the center
				aSide=false;
				r=1;
				c=1;
				m_Cells[1][1]=true;
			}
			if(aSide)
			{ // clear corners & center
				m_Cells[1][1]=false;
				m_Cells[0][0]=false;
				m_Cells[0][2]=false;
				m_Cells[2][0]=false;
				m_Cells[2][2]=false;
			}
			else
				ClearGridExcept(r,c); // Clear everthing except the selected corner or center
		}
		Invalidate();
		UpdateWindow();
		UINT nID=::GetWindowLong(m_hWnd,GWL_ID);
		GetParent()->SendMessage(WM_COMMAND,MAKELONG(nID,BN_CLICKED),(LPARAM)m_hWnd);
	}
}

void CAlignmentOptionsWnd::OnEnable(BOOL bEnable) 
{
	CWnd::OnEnable(bEnable);
	Invalidate();
}
