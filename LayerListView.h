// Layer - PCDraft 6 - Infinisys Ltd.
#pragma once

#include "LayerDoc.h"
#include "ListControl.h"

// CLayerListView view
class CLayerFooter;
class CLayerDoc;
class CLayer;

template <class TYPE,class ARG_TYPE = const TYPE& >
class CCmpArray:
   public CArray <TYPE,ARG_TYPE>
{
public:

   bool AddUnique(ARG_TYPE t)
   {

      for(int i = 0; i < GetCount(); i++)
      {

         if(ElementAt(i) == t)
         {

            return false;

         }

      }

      Add(t);

      return true;

   }


   int Remove(ARG_TYPE t)
   {

      int iCount = 0;

      for(int i = 0; i < GetCount(); )
      {

         if(ElementAt(i) == t)
         {

            iCount++;
            
            RemoveAt(i);

         }
         else
         {

            i++;

         }

      }

      return iCount;

   }

   int RemoveRenumber(ARG_TYPE t)
   {

      int iCount = 0;

      for(int i = 0; i < GetCount();)
      {

         if(ElementAt(i) == t)
         {

            iCount++;

            RemoveAt(i);

         }
         else
         {

            if(ElementAt(i) > t)
            {

               ElementAt(i)--;

            }

            i++;

         }

      }

      return iCount;

   }

   void Sort()
   {

      for(int i = 0; i < GetCount(); i++)
      {

         for(int j = i + 1; j < GetCount(); j++)
         {

            if(ElementAt(i) > ElementAt(j))
            {

               TYPE k = ElementAt(i);

               ElementAt(i) = ElementAt(j);

               ElementAt(j) = k;

            }

         }

      }

   }

};

class CLayerListView : public CScrollView
{
public:



   int                     m_iItemHeight;
   Gdiplus::Bitmap *       m_bitmapaState[2];
   Gdiplus::Bitmap *       m_bitmapaShow[3];
   Gdiplus::Bitmap *       m_pbitmapDropDown;
   
   CSize                   m_sizeMin;
   CSize                   m_sizeMax;
   CSize                   m_size;
   CCmpArray < int >       m_iaSel;
   int                     m_iLastSel;

   CLayerFooter *          m_pfooter;
   CFont                   m_font;
   int                     m_iSizeLast;
   int                     m_iSelPress;
   int                     m_iDrag;
   int                     m_bDrag;
   DWORD                   m_dwMinLast;
   DWORD                   m_dwMaxLast;
   DWORD                   m_dwLastPress;

   CListEditBox            m_edit;
   CListComboBox           m_combo;
   int                     m_iEdit;
   int                     m_iCombo;

   CLayerListView();           // protected constructor used by dynamic creation
   virtual ~CLayerListView();


   virtual BOOL PreTranslateMessage(MSG* pMsg);
   virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


   virtual void OnActivateLayer(int i);
   virtual void OnLayerNextShow(int i);

   virtual void Refresh();

   CLayerDoc * GetDocument();

   CLayer * GetActiveLayer();

   void OnUpdateItemCount();

   int get_selected_count();

   void set_selected(int iSel,bool bSel);
   void unselect_all();
   

   void get_item_rect(LPRECT lprect,int iItem);
   void get_subitem_rect(LPRECT lprect, int iItem,e_layer_subitem esubitem, bool bExternal = false);
   int get_subitem_width(e_layer_subitem esubitem);

   void rename_item();
   int get_sel();
   void library_load(CString strPath,bool bAsync,bool bRedraw);
   void finish_scale_item();
   void finish_rename_item();
   void set_size_rate(double dRate);
   void layout();
   bool is_selected(int iItem);

   void OnEditLayerName(int iItem);
   void OnDropDownLayerScale(int iItem);


   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnLButtonDown(UINT nFlags,CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags,CPoint point);
   afx_msg void OnMouseMove(UINT nFlags,CPoint point);
   afx_msg void OnSize(UINT nType,int cx,int cy);
   afx_msg void OnDestroy();
   afx_msg LRESULT OnEditKillFocus(WPARAM wparam,LPARAM lparam);
   afx_msg void OnFileRenameitem();
   afx_msg void OnUpdateFileRenameitem(CCmdUI *pCmdUI);
   afx_msg void OnKeyDown(UINT nChar,UINT nRepCnt,UINT nFlags);
   afx_msg HCURSOR OnQueryDragIcon();
   afx_msg LRESULT OnDropLayerItem(WPARAM wparam,LPARAM lparam);
   afx_msg LRESULT OnSetSizeRate(WPARAM wparam,LPARAM lparam);
   

   

   int get_selected_item();

   void OnSelchangeLayerList();
   
   bool is_add_enabled();
   bool is_delete_enabled();
   bool is_rename_enabled();
   bool is_show_enabled();
   bool is_hide_enabled();
   bool is_gray_enabled();
   bool is_merge_enabled();


   int get_selected_not_show(e_layer_show eshow);

   afx_msg void OnLvnItemchanged(NMHDR *pNMHDR,LRESULT *pResult);

   DECLARE_DYNCREATE(CLayerListView)
   DECLARE_MESSAGE_MAP()

   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   afx_msg void OnNcPaint();
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   afx_msg void OnTimer(UINT_PTR nIDEvent);
};


