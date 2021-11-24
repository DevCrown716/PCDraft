#pragma once
// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "BaseControl.h"

//#include "TemplateRecentTitle.h"
//#include "TemplateRecentButton.h"
class recent_file_list;

class TemplateRecentWnd :
   public BaseControl
{
public:

   class Title
   {
   public:

      CString     m_strTitle;
      CRect          m_rect;
      Title & operator = (const Title & title)
      {

         if (&title != this)
         {

            m_strTitle = title.m_strTitle;

            m_rect = title.m_rect;

         }

         return *this;
      }

   };

   class Item
   {
   public:

      CStringW     m_wstrPath;
      CRect          m_rect;

      Item & operator = (const Item & item)
      {

         if (&item != this)
         {

            m_wstrPath = item.m_wstrPath;

            m_rect = item.m_rect;

         }

         return *this;
      }

   };

   class Group
   {
   public:

      Title          m_title;

      CArray < Item >   m_itema;

      Group()
      {


      }

      Group(const Group & group)
      {

         m_title = group.m_title;
            
         m_itema.Append(group.m_itema);

      }

      Group & operator = (const Group & group)
      {

         if (&group != this)
         {

            m_title = group.m_title;

            m_itema.RemoveAll();

            m_itema.Append(group.m_itema);

         }

         return *this;
      }

   };

   recent_file_list *         m_precentfilelist;
   Gdiplus::Bitmap *       m_pbitmap;

   Gdiplus::Font *      m_pfont;
   Gdiplus::Font *      m_pfontLine1;
   Gdiplus::Font *      m_pfontLine2;



   int                     m_xSeparator;

   CArray < Group >    m_groupa;
   int                  m_iHoverGroup;
   int                  m_iSelGroup;
   int                  m_iHoverItem;
   int                  m_iSelItem;


   TemplateRecentWnd();
   ~TemplateRecentWnd() override;


   void OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics) override;

   void UpdateList(const CString & strFilter = "");

   bool MatchDocument(const CString & strPath, const CString & strFilter);



   DECLARE_MESSAGE_MAP()
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);

   void OnDrawGroup(int iGroup, CDC * pdc, Gdiplus::Graphics * pgraphics, Group & item);
   void OnDrawTitle(int iGroup, CDC * pdc, Gdiplus::Graphics * pgraphics, Title & title);
   void OnDrawItem(int iGroup, int iItem, CDC * pdc, Gdiplus::Graphics * pgraphics, Item & item);

   void layout();


   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnMouseLeave();
   afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

   void ensure_item_visible(int iGroup, int iItem);


   bool next_item(int & iGroup, int & iItem);

   bool previous_item(int & iGroup, int & iItem);

   void OnClick(int iGroup, int iItem);

   bool hit_test(int & iGroup, int & iItem, CPoint point);

   afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

