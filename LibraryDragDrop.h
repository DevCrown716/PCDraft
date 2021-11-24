// Library - PCDraft 6 - Infinisys Ltd.
#pragma once


#include "LibraryDragSource.h"
#include "LibraryDropTarget.h"


class CLibraryDragDrop:
   public CLibraryDragSource,
   public CLibraryDropTarget
{
public:


   CLibraryDragDrop();
   virtual ~CLibraryDragDrop();


   virtual DROPEFFECT StartDragging(CLibraryItem * pitem,CPoint* MousePos);


};




