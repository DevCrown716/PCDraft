// Library - PCDraft 6 - Infinisys Ltd.
#pragma once


class CLibraryImageTreeView : public CTreeView
{
public:


   HTREEITEM m_hitemPictures;


   CLibraryImageTreeView();           // protected constructor used by dynamic creation
	virtual ~CLibraryImageTreeView();


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif



   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);


   DECLARE_DYNCREATE(CLibraryImageTreeView)
   DECLARE_MESSAGE_MAP()

};


