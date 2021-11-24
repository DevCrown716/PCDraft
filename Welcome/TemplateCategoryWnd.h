#pragma once
// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "BaseControl.h"
#include "../DocMapper.h"

//#include "TemplateRecentTitle.h"
//#include "TemplateRecentButton.h"
class TemplateWnd;
class CPCDraftDoc;

class TemplateCategoryWnd :
   public BaseControl
{
public:

   class Item
   {
   public:

      CDocMapper m_Mapper;

      CStringW     m_wstrPath;
      CRect          m_rect;

      CPCDraftDoc * m_pdoc;
      Gdiplus::Bitmap *       m_pbitmap;

      Item();
      ~Item();

      Item & operator = (const Item & item)
      {

         if (&item != this)
         {

            m_wstrPath = item.m_wstrPath;

            m_rect = item.m_rect;

         }

         return *this;
      }

      void load_bk();

      void generate_thumbnail(CDC * pDC);

   };


   TemplateWnd *           m_ptemplatewnd;

   Gdiplus::Font *      m_pfont;
   Gdiplus::Font *      m_pfontLine1;
   Gdiplus::Font *      m_pfontLine2;

   int                     m_iThumbnailSize;

   int                     m_xSeparator;

   int                     m_iMaxColumnCount;

   CArray < Item >         m_itema;
   int                     m_iHover;
   int                     m_iSel;


   TemplateCategoryWnd();
   ~TemplateCategoryWnd() override;



   void ListCategories(CArray < CStringW > & stra);
   void ListCategoryTemplates(CArray < CStringW > & stra, CStringW strCategory, CStringW strFilter);
   void ListAllTemplates(CArray < CStringW > & stra, CStringW strFilter);

   void OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics) override;

   virtual int hit_test(const CPoint & point);

   void UpdateList(const CStringW & strFilter = "");

   bool MatchDocument(const CString & strPath, const CString & strFilter);

   void OnClick(int iItem);

   DECLARE_MESSAGE_MAP()
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

   afx_msg void OnSize(UINT nType, int cx, int cy);

   void OnDrawItem(int iItem, CDC * pdc, Gdiplus::Graphics * pgraphics, Item & item);

   void layout();

   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnMouseLeave();
   afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

   void ensure_item_visible(int iItem);

};

