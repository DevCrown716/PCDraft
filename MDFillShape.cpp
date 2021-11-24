#include "stdafx.h"
#include "MDFillShape.h"

#include "PCDraftDoc.h"
#include "DraftContext.h"
#include "PhIOStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMDFillShape,CMDShape,1)

//////////////////////////////////////////////////////////////////////

CMDFillShape::CMDFillShape()
{
}

void CMDFillShape::Serialize(CArchive& ar)
{
	CMDShape::Serialize(ar);
	if(ar.IsLoading())
		ar >> m_FillModel;
	else
		ar << m_FillModel;
}

bool CMDFillShape::GetFillRgn(CRgn* pRgn, CDraftContext* pDC) const
{
	//if(pDC->BeginPath()) GDI+
   //pDC->path_new();
	{
		pDC->SetOutlinePen();
      pDC->m_bOutlineForFill = true;
		DrawShapeOutlineForFill(pDC);
      pDC->m_bOutlineForFill = false;
      if (IsClosed() && pDC->path())
      {
         pDC->path()->CloseFigure();
      }
		//return (pDC->EndPath(pRgn) && (pDC->path() || (pRgn && pRgn->GetSafeHandle())));
      return true;
	}
	return false;
}


void CMDFillShape::DrawFill(CDraftContext* pDC) const
{

	if(IsFilled())
	{

      UsePath usepath(pDC);

		//CRgn rgn; comment for GDI+

		//if(GetFillRgn(&rgn, pDC)) comment for GDI+

      if(GetFillRgn(nullptr, pDC)) 
		{
         
         UseRect userect(pDC, (CMDShape *) this);
			
         pDC->SetFill(m_FillModel);

			//pDC->PaintRgn(&rgn); comment for GDI+

         pDC->PaintPath();

		}

	}

}


bool CMDFillShape::PointInFill(const pointd& pt, CMapper *pMapper) const
{
	if(IsFilled())
	{
		CDC cdc;
		cdc.CreateCompatibleDC(NULL);
		CDraftContext dc(&cdc,pMapper);
		//CRgn rgn;
      UsePath usepath(&dc);
      if (GetFillRgn((CRgn*)nullptr, &dc) && dc.path()->IsVisible(__pointf(pt))) return true;
	}
	return false;
}

void CMDFillShape::MakeSelfContained(CPCDraftDoc *pDoc,bool CopyDIB)
{
	CMDShape::MakeSelfContained(pDoc,CopyDIB);
	switch(m_FillModel.m_nFillType)
	{
	case kFillColor:
		if(m_FillModel.m_crColor & PALETTEINDEX(0))
		{
			PALETTEENTRY pe;
			pDoc->m_Palette.GetPaletteEntries(m_FillModel.m_nIndex&0xFF,1,&pe);
			m_FillModel.m_crColor=RGB(pe.peRed,pe.peGreen,pe.peBlue);
		}
		break;
	case kPenPat:
		{
			// by TSM
			//if(CopyDIB && (m_FillModel.m_nIndex > 70)) {
			//	m_FillModel.m_nFillType=kFillPatEmbed;
			//	m_FillModel.m_hDib=pDoc->m_Patterns[m_FillModel.m_nIndex].CreateDIBSection();
			//}
			// end by TSM
		}
		break;
	}
}

int CMDFillShape::SaveMacDraft(PhIOContext& ctx)
{
	CMDShape::SaveMacDraft(ctx);
	ctx.ConvertFillModel(m_FillModel,ctx.pPhShape->fFillModel);
	return 0;
}

void CMDFillShape::ProcessShapesForCopy(CVersionDoc *pDoc, int* used)
{
	CMDShape::ProcessShapesForCopy(pDoc, used);
	if(m_FillModel.m_nFillType == kFillPat) used[m_FillModel.m_nIndex] = 1;
}

// Library - PCDraft 6 - Infinisys Ltd.
void CMDFillShape::ProcessShapesForLibraryArchiving(CVersionDoc *pDocNew,CVersionDoc *pDoc,int* used)
{
   CMDShape::ProcessShapesForCopy(pDoc,used);
   if(m_FillModel.m_nFillType == kPenPat)
   {

      if(used[m_FillModel.m_nIndex] < 0)
      {
         used[m_FillModel.m_nIndex] = pDocNew->m_packeditema.GetSize();
         pDocNew->m_packeditema.Add(pDoc->m_packeditema[m_FillModel.m_nIndex]);
         m_FillModel.m_nIndex = used[m_FillModel.m_nIndex];
      }
      else
      {
         m_FillModel.m_nIndex= used[m_FillModel.m_nIndex];
      }

   }
}

// Library - PCDraft 6 - Infinisys Ltd.
void CMDFillShape::ProcessShapesFromLibraryArchiving(CPCDraftDoc *pDoc,CPCDraftDoc *pDocLib)
{
   if(m_FillModel.m_nFillType == kPenPat)
   {
      if(pDocLib->m_packeditema[m_FillModel.m_nIndex].m_etype == packed_type_dib)
      {
         HBITMAP hDib = pDocLib->m_packeditema[m_FillModel.m_nIndex].m_pdib->CreateDIBSection();
         int found = -1;
         int hash = CalculateDIBHash(hDib);
         for(int k=0; k < pDoc->m_packeditema.GetCount(); k++)
         {
            if(pDoc->m_packeditema[k].m_etype == packed_type_dib)
            {
               if(hash == pDoc->m_PatternsHash[pDoc->m_packeditema[k].get_index(pDoc)])
               {
                  HBITMAP hDib2 = pDoc->m_Patterns[pDoc->m_packeditema[k].get_index(pDoc)].CreateDIBSection();
                  if(!CompareDIBs(hDib,hDib2))
                  {
                     ::DeleteObject(hDib2);
                     found = k;
                     break;
                  }
                  ::DeleteObject(hDib2);
               }
            }
         }
         if(found >= 0)
         {
            m_FillModel.m_nIndex = found;
         }
         else
         {
            CPackedItem item;
            item.m_etype = packed_type_dib;
            item.m_pdib = &pDoc->m_Patterns[pDoc->m_numDIBPatterns];
            m_FillModel.m_nIndex = pDoc->m_packeditema.GetCount();
            pDoc->m_PatternsHash[pDoc->m_numDIBPatterns] = CalculateDIBHash(hDib);
            pDoc->m_Patterns[pDoc->m_numDIBPatterns++].FromDIBSection(hDib);
            pDoc->m_packeditema.Add(item);
         }
         ::DeleteObject(hDib);
      }
      else if(pDocLib->m_packeditema[m_FillModel.m_nIndex].m_etype == packed_type_gradient)
      {
         CGradientPack  * pgrad = pDocLib->m_packeditema[m_FillModel.m_nIndex].m_pgradient;
         int found = -1;
         for(int k=0; k < pDoc->m_packeditema.GetCount(); k++)
         {
            if(pDoc->m_packeditema[k].m_etype == packed_type_gradient)
            {
               if(*pgrad == pDoc->m_Gradients[pDoc->m_packeditema[k].get_index(pDoc)])
               {
                  found = k;
                  break;
               }
            }
         }
         if(found >= 0)
         {
            m_FillModel.m_nIndex = found;
         }
         else
         {
            CPackedItem item;
            item.m_etype = packed_type_gradient;
            item.m_pgradient = &pDoc->m_Gradients[pDoc->m_numGradients];
            m_FillModel.m_nIndex =pDoc->m_packeditema.GetCount();
            pDoc->m_Gradients[pDoc->m_numGradients++] = *pgrad;
            pDoc->m_packeditema.Add(item);
         }
      }
   }
}


void CMDFillShape::ProcessShapesAfterCopy(CPCDraftDoc *pDoc, int *used)
{
	CMDShape::ProcessShapesAfterCopy(pDoc, used);
	//Check pen and fill models. If PenPatternEmbed - convert to palette entry.
	if(m_FillModel.m_nFillType == kFillPat) {
		m_FillModel.m_nIndex = used[m_FillModel.m_nIndex ];
	}
	if(m_FillModel.m_nFillType == kFillPatEmbed) {
		m_FillModel.m_nFillType = kFillPat;
		HBITMAP hDib = m_FillModel.m_hDib;
		int found = -1;
		int hash = CalculateDIBHash(hDib);
      // Infinisys Ltd
      for(int i=0; i < pDoc->m_packeditema.GetSize(); i++) {
         if(pDoc->m_packeditema[i].m_etype == packed_type_dib)
         {
            int k = pDoc->m_packeditema[i].m_pdib - pDoc->m_Patterns;
               if(hash == pDoc->m_PatternsHash[k]) {
                  HBITMAP hDib2 = pDoc->m_Patterns[k].CreateDIBSection();
                  if(!CompareDIBs(hDib,hDib2)) {
                     ::DeleteObject(hDib2);
                     found = i; break;
                  }
                  ::DeleteObject(hDib2);
               }
         }
		}
		if(found>= 0) {
			m_FillModel.m_nIndex = found;
			::DeleteObject(hDib);
		} else {
			 
			pDoc->m_PatternsHash[pDoc->m_numDIBPatterns] = CalculateDIBHash(hDib);
			pDoc->m_Patterns[pDoc->m_numDIBPatterns].FromDIBSection(hDib);
         CPackedItem item;
         item.m_etype = packed_type_dib;
         item.m_pdib = &pDoc->m_Patterns[pDoc->m_numDIBPatterns];
         pDoc->m_packeditema.Add(item);
         pDoc->m_numDIBPatterns++;
         m_FillModel.m_nIndex = pDoc->m_packeditema.GetUpperBound();
			::DeleteObject(hDib);
		}
	}
}

void CMDFillShape::ProcessShapesAfterSymLibraryCopy(CPackedItem * pitem,int *used)
{
	CMDShape::ProcessShapesAfterSymLibraryCopy(pitem, used);
	if(m_FillModel.m_nFillType == kFillPat)
	{
      if(pitem[m_FillModel.m_nIndex].m_etype == packed_type_dib)
      {
         m_FillModel.m_nFillType=kFillPatEmbed;
         m_FillModel.m_hDib=pitem[m_FillModel.m_nIndex].m_pdib->CreateDIBSection();
      }
      else if(pitem[m_FillModel.m_nIndex].m_etype == packed_type_gradient)
      {
         m_FillModel.m_nIndex = used[m_FillModel.m_nIndex];
      }
	}
}


void CMDFillShape::SetFillOpacity(int iOpacity) // Opacity/Transparency - Infinisys Ltd 
{

   m_FillModel.m_iFillOpacity = iOpacity;

}


bool CMDFillShape::GetFillOpacity(int & iOpacity) const // Opacity/Transparency - Infinisys Ltd
{
   
   iOpacity = m_FillModel.m_iFillOpacity;
   
   return true;

}

