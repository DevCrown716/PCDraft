#include "stdafx.h"
#include "PCDraftRibbon.h"
#include "MainFrm.h"
#include "ribbon.h"
#include <Propvarutil.h>
#include "resource.h"
using namespace Gdiplus;
Gdiplus::Bitmap * LoadPNG(LPCTSTR pName,LPCTSTR pType,HMODULE hInst);
void copy(Gdiplus::Rect & r,LPCRECT lpcrect);

#ifdef _PERSONAL_EDITION

#define RIBBON_TAB_PROPERTIES 8

#else

#define RIBBON_TAB_PROPERTIES 9

#endif

void copy(Gdiplus::RectF & r,LPCRECT lpcrect);
#undef new
CRibbonBar::CItem::CItem()
{

   m_bPreferTwoLines = false;
   m_pimage1 = NULL;
   m_pimageLarge = NULL;
   m_pimageSmall = NULL;
   m_bPopup = false;
   m_bAlwaysEnabledPopup = false;

}

CRibbonBar::CItem::CItem(const CItem & i)
{
   m_pimage1 = NULL;
   m_pimageLarge = NULL;
   m_pimageSmall = NULL;
   operator = (i);
}

CRibbonBar::CItem::~CItem()
{
   if(m_pimageLarge != NULL)
   {
      delete m_pimageLarge;
      m_pimageLarge = NULL;
   }
   if(m_pimageSmall != NULL)
   {
      delete m_pimageSmall;
      m_pimageSmall = NULL;
   }

}


CRibbonBar::CItem & CRibbonBar::CItem::operator =(const CItem & i)
{

   if(this != &i)
   {

      m_iId = i.m_iId;
      m_bEnabled = i.m_bEnabled;
      m_bAlwaysEnabledPopup = i.m_bAlwaysEnabledPopup;
      m_bPreferTwoLines = i.m_bPreferTwoLines;
      m_bPopup = i.m_bPopup;
      if(m_pimageLarge != NULL)
      {
         delete m_pimageLarge;
         m_pimageLarge = NULL;
      }
      if(i.m_pimageLarge != NULL)
      {
         m_pimageLarge = i.m_pimageLarge->Clone();
      }
      if(m_pimageSmall != NULL)
      {
         delete m_pimageSmall;
         m_pimageSmall = NULL;
      }
      if(i.m_pimageSmall != NULL)
      {
         m_pimageSmall = i.m_pimageSmall->Clone();
      }
      if(i.m_pimage1 == i.m_pimageLarge)
      {
         m_pimage1 = m_pimageLarge;
      }
      else if(i.m_pimage1 == i.m_pimageSmall)
      {
         m_pimage1 = m_pimageSmall;
      }
      else
      {
         m_pimage1 = NULL;
      }
      m_wstrName = i.m_wstrName;

   }

   return *this;

}


void CRibbonBar::CItem::ClearLayout()
{
   
   m_iRow = -1;
   m_x = -1;
   m_y = -1;
   m_iLastMaxW = -1;
   m_rItem.SetRectEmpty();
   m_rImage.SetRectEmpty();
   m_rText.SetRectEmpty();

}


CStringW CRibbonBar::CItem::get_draw_name()
{

   if(m_bPopup)
   {

      return m_wstrName + L" ▾";

   }

   return m_wstrName;

}


bool CRibbonBar::CItem::get_prefer_two_lines()
{

   return (m_bPopup && (m_pimageLarge == NULL || m_pimageLarge->GetWidth() < 20)) || m_bPreferTwoLines;

}


CRibbonBar::CRibbonBar():
m_properties(this),
m_font2(L"Segoe UI", 13.0, 0, Gdiplus::UnitPixel)
{

   m_iFullHeight = 140;

   m_iScroll = 0;

   m_iScrollMax = 0;
   LOGFONTW lfw;

   {

      HDC hdc = ::GetDC(NULL);

      {

         Gdiplus::Graphics g(hdc);

         m_font2.GetLogFontW(&g,&lfw);

         m_font.Attach(::CreateFontIndirectW(&lfw));

      }

      ::ReleaseDC(NULL,hdc);

   }

   m_bExpanded = true;
   m_bFocus = false;
   m_iMargin = 0;
   m_hiconHelp = (HICON)LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_RIBBON_HELP),IMAGE_ICON,24,24,0);;
   m_hiconExpand = (HICON)LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_RIBBON_EXPAND),IMAGE_ICON,24,24,0);;
   m_hiconCollapse = (HICON)LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_RIBBON_COLLAPSE),IMAGE_ICON,24,24,0);;

   m_format.SetAlignment(Gdiplus::StringAlignmentCenter);
   m_format.SetLineAlignment(Gdiplus::StringAlignmentNear);
   m_format.SetTrimming(Gdiplus::StringTrimmingWord);
   m_format.SetFormatFlags(0);

   m_formatTab.SetAlignment(Gdiplus::StringAlignmentCenter);
   m_formatTab.SetLineAlignment(Gdiplus::StringAlignmentCenter);
   m_formatTab.SetTrimming(Gdiplus::StringTrimmingWord);
   m_formatTab.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);



   //m_font.CreatePointFont(95,"Segoe UI");

   m_ulRibbonHeight = m_iFullHeight;
   m_rIconMargin.left = 0;
   m_rIconMargin.right = 0;
   m_rIconMargin.top = 15;
   m_rIconMargin.bottom = 5;

   m_rIconBorder.left = 8;
   m_rIconBorder.right = 8;
   m_rIconBorder.top = 5;
   m_rIconBorder.bottom = 1;
   m_rTabBorder.left =20;
   m_rTabBorder.right =20;
   m_rTabBorder.top =5;
   m_rTabBorder.bottom =5;

   m_iTabSel = 0;

   CItem item;

   item.m_bPreferTwoLines = true;

   {

      CTab tab;

      {

         CGroup group;

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_FILE_NEW_DRAWING_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_FILE_NEW_DRAWING_LabelTitle_RESID);

         item.m_iId = ID_FILE_WELCOME_NEW;

         item.m_bPopup = true;

         item.m_bAlwaysEnabledPopup = true;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_FILE_OPEN_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_FILE_OPEN_LabelTitle_RESID);

         item.m_iId = ID_FILE_OPEN;

         item.m_bPopup = false;

         group.m_itema.Add(item);

#ifndef _PERSONAL_EDITION

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_FILE_LOAD_LAYER_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_FILE_LOAD_LAYER_LabelTitle_RESID);

         item.m_iId = ID_FILE_LOAD_LAYER;

         group.m_itema.Add(item);

#endif
         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_FILE_RECENT_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_FILE_RECENT_LabelTitle_RESID);

         item.m_iId = ID_FILE_MRU_FILE1;

         item.m_bPopup = true;

         group.m_itema.Add(item);

         
         item.m_bPopup = false;
         tab.m_groupa.Add(group);

      }


      {

         CGroup group;

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_FILE_SAVE_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_FILE_SAVE_LabelTitle_RESID);

         item.m_iId = ID_FILE_SAVE;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_FILE_SAVE_AS_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_FILE_SAVE_AS_LabelTitle_RESID);

         item.m_iId = ID_FILE_SAVE_AS;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_FILE_REVERT_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_FILE_REVERT_LabelTitle_RESID);

         item.m_iId = ID_FILE_REVERT;

         group.m_itema.Add(item);

         tab.m_groupa.Add(group);

      }

      {

         CGroup group;

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_FILE_IMPORT_IMAGE_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_FILE_IMPORT_IMAGE_LabelTitle_RESID);

         item.m_iId = ID_FILE_IMPORT_IMAGE;

         group.m_itema.Add(item);

#ifndef _PERSONAL_EDITION

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_FILE_EXPORT_REPORT_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_FILE_EXPORT_REPORT_LabelTitle_RESID);

         item.m_iId = ID_FILE_EXPORT_REPORT;

         group.m_itema.Add(item);

#endif

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_FILE_EXPORT_FILE_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_FILE_EXPORT_FILE_LabelTitle_RESID);

         item.m_iId = ID_FILE_EXPORT_FILE;

         group.m_itema.Add(item);


         tab.m_groupa.Add(group);

      }

#ifndef _PERSONAL_EDITION

      {

         CGroup group;

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_FILE_SET_DEFAULTS_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_FILE_SET_DEFAULTS_LabelTitle_RESID);

         item.m_iId = ID_FILE_SET_DEFAULTS;

         group.m_itema.Add(item);

         tab.m_groupa.Add(group);

      }

#endif

      {

         CGroup group;

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_FILE_PAGE_SETUP_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_FILE_PAGE_SETUP_LabelTitle_RESID);

         item.m_iId = ID_FILE_PAGE_SETUP;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_FILE_PRINT_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_FILE_PRINT_LabelTitle_RESID);

         item.m_iId = ID_FILE_PRINT;

         group.m_itema.Add(item);

#ifndef _PERSONAL_EDITION

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_FILE_PRINT_SPECIAL_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_FILE_PRINT_SPECIAL_LabelTitle_RESID);

         item.m_iId = ID_FILE_PRINT_ACTIVE_LAYER;

         item.m_bPopup = true;

         group.m_itema.Add(item);

#endif

         tab.m_groupa.Add(group);

         item.m_bPopup = false;

      }

      {

         CGroup group;

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_FILE_QUIT_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_FILE_QUIT_LabelTitle_RESID);

         item.m_iId = ID_APP_EXIT;

         group.m_itema.Add(item);
         
         tab.m_groupa.Add(group);

         item.m_bPopup = false;

      }

      tab.m_wstrName.LoadString(IDC_CMD_FILE_LabelTitle_RESID);

      m_taba.Add(tab);

   }

   {

      CTab tab;

      tab.m_wstrName.LoadStringA(IDC_CMD_EDIT_LabelTitle_RESID);

      {

         CGroup group;

#ifndef _PERSONAL_EDITION

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_EDIT_PASTE_OPTIONS_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_EDIT_PASTE_OPTIONS_LabelTitle_RESID);

         item.m_iId = ID_EDIT_PASTE_UNSCALED;

         item.m_bPopup = true;

         group.m_itema.Add(item);

#endif

         item.m_bPopup = false;

         item.m_wstrName.Empty();

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_EDIT_CUT_SmallImages_RESID),"IMAGE",NULL);

         item.m_iId = ID_EDIT_CUT;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_EDIT_COPY_SmallImages_RESID),"IMAGE",NULL);

         item.m_iId = ID_EDIT_COPY;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_EDIT_PASTE_SmallImages_RESID),"IMAGE",NULL);

         item.m_iId = ID_EDIT_PASTE;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_EDIT_CLEAR_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_EDIT_CLEAR_LabelTitle_RESID);

         item.m_iId = ID_EDIT_CLEAR;

         group.m_itema.Add(item);

         tab.m_groupa.Add(group);

      }

            {

               CGroup group;

               item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_EDIT_COPY_ATTRIBUTES_LargeImages_RESID),"IMAGE",NULL);

               item.m_wstrName.LoadStringA(ID_CMD_EDIT_COPY_ATTRIBUTES_LabelTitle_RESID);

               item.m_iId = ID_EDIT_COPY_ATTRIB;

               group.m_itema.Add(item);

               item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_EDIT_APPLY_ATTRIBUTES_LargeImages_RESID),"IMAGE",NULL);

               item.m_wstrName.LoadStringA(ID_CMD_EDIT_APPLY_ATTRIBUTES_LabelTitle_RESID);

               item.m_iId = ID_EDIT_APPLY_ATTRIB;

               group.m_itema.Add(item);

               item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_EDIT_ATTRIBUTE_OPTIONS_LargeImages_RESID),"IMAGE",NULL);

               item.m_wstrName.LoadStringA(ID_CMD_EDIT_ATTRIBUTE_OPTIONS_LabelTitle_RESID);

               item.m_iId = ID_EDIT_ATTRIB_OPTIONS;

               group.m_itema.Add(item);

               tab.m_groupa.Add(group);

            }

            item.m_bPreferTwoLines = false;

            {

               CGroup group;

               item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_EDIT_DUPLICATE_LargeImages_RESID),"IMAGE",NULL);

               item.m_wstrName.LoadStringA(ID_CMD_EDIT_DUPLICATE_LabelTitle_RESID);

               item.m_iId = ID_EDIT_DUPLUCATE;

               group.m_itema.Add(item);

#ifndef _PERSONAL_EDITION

               item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_EDIT_DUPLICATE_LINEAR_LargeImages_RESID),"IMAGE",NULL);

               item.m_wstrName.LoadStringA(ID_CMD_EDIT_DUPLICATE_LINEAR_LabelTitle_RESID);

               item.m_iId = ID_EDIT_DUP_LINEAR;

               group.m_itema.Add(item);

               item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_EDIT_DUPLICATE_CIRCULAR_LargeImages_RESID),"IMAGE",NULL);

               item.m_wstrName.LoadStringA(ID_CMD_EDIT_DUPLICATE_CIRCULAR_LabelTitle_RESID);

               item.m_iId = ID_EDIT_DUP_CIRCULAR;

               group.m_itema.Add(item);

#endif

               tab.m_groupa.Add(group);

            }

            item.m_bPreferTwoLines = true;

            {

               CGroup group;

               item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_EDIT_SELECT_ALL_LargeImages_RESID),"IMAGE",NULL);

               item.m_wstrName.LoadStringA(ID_CMD_EDIT_SELECT_ALL_LabelTitle_RESID);

               item.m_iId = ID_EDIT_SELECT_ALL;

               group.m_itema.Add(item);

               item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_EDIT_SELECT_NONE_LargeImages_RESID),"IMAGE",NULL);

               item.m_wstrName.LoadStringA(ID_CMD_EDIT_SELECT_NONE_LabelTitle_RESID);

               item.m_iId = ID_EDIT_SELECT_NONE;

               group.m_itema.Add(item);

#ifndef _PERSONAL_EDITION

               item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_EDIT_SELECT_SPECIAL_LargeImages_RESID),"IMAGE",NULL);

               item.m_wstrName.LoadStringA(ID_CMD_EDIT_SELECT_SPECIAL_LabelTitle_RESID);

               item.m_iId = ID_EDIT_SELECT_SPECIAL;

               group.m_itema.Add(item);

#endif

               tab.m_groupa.Add(group);

            }

            item.m_bPreferTwoLines = false;

            {

               CGroup group;

               item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_EDIT_MOVE_LargeImages_RESID),"IMAGE",NULL);

               item.m_wstrName.LoadStringA(ID_CMD_EDIT_MOVE_LabelTitle_RESID);

               item.m_iId = ID_EDIT_MOVE;

               group.m_itema.Add(item);

               item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_EDIT_EXPAND_LargeImages_RESID),"IMAGE",NULL);

               item.m_wstrName.LoadStringA(ID_CMD_EDIT_EXPAND_LabelTitle_RESID);

               item.m_iId = ID_EDIT_EXPAND;

               group.m_itema.Add(item);

               item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_EDIT_CONTRACT_LargeImages_RESID),"IMAGE",NULL);

               item.m_wstrName.LoadStringA(ID_CMD_EDIT_CONTRACT_LabelTitle_RESID);

               item.m_iId = ID_EDIT_CONTRACT;

               group.m_itema.Add(item);

               item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_EDIT_RESHAPE_LargeImages_RESID),"IMAGE",NULL);

               item.m_wstrName.LoadStringA(ID_CMD_EDIT_RESHAPE_LabelTitle_RESID);

               item.m_iId = ID_EDIT_RESHAPE_SMOOTH;

               item.m_bPopup = true;

               group.m_itema.Add(item);

               tab.m_groupa.Add(group);

               item.m_bPopup = false;

            }

            item.m_bPreferTwoLines = true;

#ifndef _PERSONAL_EDITION

            {

               CGroup group;

               item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_EDIT_DATUM_LargeImages_RESID),"IMAGE",NULL);

               item.m_wstrName.LoadStringA(ID_CMD_EDIT_DATUM_LabelTitle_RESID);

               item.m_iId = ID_EDIT_DATUM;

               group.m_itema.Add(item);

               tab.m_groupa.Add(group);

            }

#endif
            m_taba.Add(tab);
   }

   {

      CTab tab;
      tab.m_wstrName.LoadStringA(IDC_CMD_TEXT_LabelTitle_RESID);

      {

         CGroup group;

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_TEXT_FONT_SELECT_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_TEXT_FONT_SELECT_LabelTitle_RESID);

         item.m_iId = ID_TEXT_FONT;

         group.m_itema.Add(item);

         tab.m_groupa.Add(group);

      }

      item.m_bPreferTwoLines = false;
      {

         CGroup group;

         group.m_wstrName.LoadStringA(cmdGroupJustification_LabelTitle_RESID);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_TEXT_JUSTIFICATION_LEFT_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_TEXT_JUSTIFICATION_LEFT_LabelTitle_RESID);

         item.m_iId = ID_TEXT_JUSTIFY_LEFT;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_TEXT_JUSTIFICATION_CENTER_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_TEXT_JUSTIFICATION_CENTER_LabelTitle_RESID);

         item.m_iId = ID_TEXT_JUSTIFY_CENTER;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_TEXT_JUSTIFICATION_RIGHT_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_TEXT_JUSTIFICATION_RIGHT_LabelTitle_RESID);

         item.m_iId = ID_TEXT_JUSTIFY_RIGHT;

         group.m_itema.Add(item);

         tab.m_groupa.Add(group);

      }

      {

         CGroup group;

         group.m_wstrName.LoadStringA(cmdGroupVerticalPosition_LabelTitle_RESID);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_TEXT_VERTICAL_POSITION_TOP_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_TEXT_VERTICAL_POSITION_TOP_LabelTitle_RESID);

         item.m_iId = ID_TEXT_JUSTIFY_TOP;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_TEXT_VERTICAL_POSITION_MIDDLE_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_TEXT_VERTICAL_POSITION_MIDDLE_LabelTitle_RESID);

         item.m_iId = ID_TEXT_JUSTIFY_MIDDLE;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_TEXT_VERTICAL_POSITION_BOTTOM_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_TEXT_VERTICAL_POSITION_BOTTOM_LabelTitle_RESID);

         item.m_iId = ID_TEXT_JUSTIFY_BOTTOM;

         group.m_itema.Add(item);

         tab.m_groupa.Add(group);

      }


      {

         CGroup group;

         group.m_wstrName.LoadStringA(cmdGroupLineSpacing_LabelTitle_RESID);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_TEXT_LINE_SPACING_SINGLE_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_TEXT_LINE_SPACING_SINGLE_LabelTitle_RESID);

         item.m_iId = ID_TEXT_LINESPACING_SINGLE;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_TEXT_LINE_SPACING_ONE_AND_A_HALF_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_TEXT_LINE_SPACING_ONE_AND_A_HALF_LabelTitle_RESID);

         item.m_iId = ID_TEXT_LINESPACING_1_5SPACE;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_TEXT_LINE_SPACING_DOUBLE_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_TEXT_LINE_SPACING_DOUBLE_LabelTitle_RESID);

         item.m_iId = ID_TEXT_LINESPACING_DOUBLE;

         group.m_itema.Add(item);

         tab.m_groupa.Add(group);

      }

      {

         CGroup group;

         group.m_wstrName.LoadStringA(cmdGroupCase_LabelTitle_RESID);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_TEXT_CASE_UPPER_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_TEXT_CASE_UPPER_LabelTitle_RESID);

         item.m_iId = ID_TEXT_UPPERCASE;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_TEXT_CASE_LOWER_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_TEXT_CASE_LOWER_LabelTitle_RESID);

         item.m_iId = ID_TEXT_LOWERCASE;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_TEXT_CASE_TITLE_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_TEXT_CASE_TITLE_LabelTitle_RESID);

         item.m_iId = ID_TEXT_TITLE;

         group.m_itema.Add(item);

         tab.m_groupa.Add(group);

      }

      {

         CGroup group;

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_TEXT_SELECT_ALL_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_TEXT_SELECT_ALL_LabelTitle_RESID);

         item.m_iId = ID_TEXT_SELECTALLTEXT;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_TEXT_HIDE_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_TEXT_HIDE_LabelTitle_RESID);

         item.m_iId = ID_TEXT_HIDETEXT;

         group.m_itema.Add(item);

         tab.m_groupa.Add(group);

      }


      //{

      //   CGroup group;

      //   item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_TEXT_COLOR_LargeImages_RESID),"IMAGE",NULL);

      //   item.m_wstrName.LoadStringA(ID_CMD_TEXT_COLOR_LabelTitle_RESID);

      //   //item.m_iId = ID_EDIT_COPY_ATTRIBUTES;

      //   item.m_bPopup = true;

      //   group.m_itema.Add(item);

      //   tab.m_groupa.Add(group);

      //   item.m_bPopup = false;

      //}

      m_taba.Add(tab);

   }

   {

      CTab tab;

      tab.m_wstrName .LoadStringA(IDC_CMD_OPTIONS_LabelTitle_RESID);;

      {

         CGroup group;

         group.m_wstrName.LoadStringA(cmdGroupBorderPosition_LabelTitle_RESID);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_OPTIONS_BORDER_POSITION_INSIDE_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_OPTIONS_BORDER_POSITION_INSIDE_LabelTitle_RESID);

         item.m_iId = ID_PREF_BORDER_LINE_INSIDE;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_OPTIONS_BORDER_POSITION_CENTER_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_OPTIONS_BORDER_POSITION_CENTER_LabelTitle_RESID);

         item.m_iId = ID_PREF_BORDER_LINE_CENTERED;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_OPTIONS_BORDER_POSITION_OUTSIDE_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_OPTIONS_BORDER_POSITION_OUTSIDE_LabelTitle_RESID);

         item.m_iId = ID_PREF_BORDER_LINE_OUTSIDE;

         group.m_itema.Add(item);

         tab.m_groupa.Add(group);

      }

#ifndef _PERSONAL_EDITION

      {

         CGroup group;

         group.m_wstrName .LoadStringA(cmdGroupPenStyle_LabelTitle_RESID);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_OPTIONS_PEN_STYLE_BLADE_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_OPTIONS_PEN_STYLE_BLADE_LabelTitle_RESID);

         item.m_iId = ID_PREF_PENSTYLE_BLADE;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_OPTIONS_PEN_STYLE_ROUND_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_OPTIONS_PEN_STYLE_ROUND_LabelTitle_RESID);

         item.m_iId = ID_PREF_PENSTYLE_ROUND;

         group.m_itema.Add(item);

         tab.m_groupa.Add(group);

      }

#endif

#ifndef _PERSONAL_EDITION

     {

        CGroup group;

        item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_OPTIONS_ZOOM_LINE_WEIGHTS_LargeImages_RESID),"IMAGE",NULL);

        item.m_wstrName .LoadStringA(ID_CMD_OPTIONS_ZOOM_LINE_WEIGHTS_LabelTitle_RESID);

        item.m_iId = ID_PREF_ZOOM_LINE_WEIGHTS;

        group.m_itema.Add(item);

        item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_OPTIONS_HIDE_SHOW_LINE_STYLES_LargeImages_RESID),"IMAGE",NULL);

        item.m_wstrName .LoadStringA(ID_CMD_OPTIONS_HIDE_SHOW_LINE_STYLES_LabelTitle_RESID);

        item.m_iId = ID_PREF_HIDE_LINE_STYLE;

        group.m_itema.Add(item);

        tab.m_groupa.Add(group);

     }

      {

         CGroup group;

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_OPTIONS_EDIT_LINE_STYLES_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_OPTIONS_EDIT_LINE_STYLES_LabelTitle_RESID);

         item.m_bPopup = true;

         item.m_iId = ID_EDIT_STYLES_BASE;

         group.m_itema.Add(item);

         tab.m_groupa.Add(group);

         item.m_bPopup = false;

      }

#endif



      {

         CGroup group;

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_OPTIONS_NEW_COLOR_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_OPTIONS_NEW_COLOR_LabelTitle_RESID);

         item.m_iId = ID_NEW_COLOR;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_OPTIONS_NEW_GRADIENT_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_OPTIONS_NEW_GRADIENT_LabelTitle_RESID);

         item.m_iId = ID_NEW_GRADIENT;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_OPTIONS_NEW_PATTERN_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_OPTIONS_NEW_PATTERN_LabelTitle_RESID);

         item.m_iId = ID_NEW_PATTERN;

         group.m_itema.Add(item);

         tab.m_groupa.Add(group);

      }


      {

         CGroup group;

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_OPTIONS_EDIT_COLORS_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_OPTIONS_EDIT_COLORS_LabelTitle_RESID);

         item.m_bPopup = true;

         item.m_iId = ID_COLORS_BASE;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_OPTIONS_EDIT_PATTERN_GRADIENT_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_OPTIONS_EDIT_PATTERN_GRADIENT_LabelTitle_RESID);

         item.m_iId = ID_PATTERNS_BASE;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_OPTIONS_DUPLICATE_PATTERN_GRADIENT_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_OPTIONS_DUPLICATE_PATTERN_GRADIENT_LabelTitle_RESID);

         item.m_iId = ID_DUP_PATTERNS_BASE;

         group.m_itema.Add(item);

         item.m_bPopup = false;

#ifndef _PERSONAL_EDITION

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_OPTIONS_HIDE_SHOW_FILLS_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName.LoadStringA(ID_CMD_OPTIONS_HIDE_SHOW_FILLS_LabelTitle_RESID);

         item.m_iId = ID_PREF_HIDE_FILLS;

         group.m_itema.Add(item);

#endif

         tab.m_groupa.Add(group);

      }

      {

         CGroup group;

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_OPTIONS_CROSS_CURSOR_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_OPTIONS_CROSS_CURSOR_LabelTitle_RESID);

         item.m_iId = ID_PREF_CROSS_CURSOR;

         group.m_itema.Add(item);


         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_OPTIONS_TOOL_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_OPTIONS_TOOL_LabelTitle_RESID);

         item.m_iId = ID_PREF_TOOL_OPTIONS;

         group.m_itema.Add(item);


		   item.m_pimageLarge = LoadPNG(MAKEINTRESOURCE(ID_CMD_OPTIONS_AUTO_SAVE_LargeImages_RESID), "IMAGE", NULL);

		   item.m_wstrName.LoadStringA(ID_CMD_OPTIONS_AUTO_SAVE_LabelTitle_RESID);

   		item.m_iId = ID_OPTIONS_AUTO_SAVE;

	   	group.m_itema.Add(item);


         item.m_pimageLarge = LoadPNG(MAKEINTRESOURCE(ID_CMD_OPTIONS_GUIDES_LargeImages_RESID), "IMAGE", NULL);

         item.m_wstrName.LoadStringA(ID_CMD_OPTIONS_GUIDES_LabelTitle_RESID);

         item.m_iId = ID_GUIDES_ADDHORIZONTALGUIDE;

         item.m_bPopup = true;

         group.m_itema.Add(item);


         tab.m_groupa.Add(group);

         item.m_bPopup = false;


      }


      m_taba.Add(tab);

   }


   item.m_bPreferTwoLines = true;
   {

      CTab tab;

      tab.m_wstrName .LoadStringA(IDC_CMD_ARRANGE_LabelTitle_RESID);

      {

         CGroup group;

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_BRING_TO_FRONT_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_ARRANGE_BRING_TO_FRONT_LabelTitle_RESID);

         item.m_iId = ID_ARRANGE_TO_FRONT;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_SEND_TO_BACK_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_ARRANGE_SEND_TO_BACK_LabelTitle_RESID);

         item.m_iId = ID_ARRANGE_TO_BACK;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_MOVE_FORWARD_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_ARRANGE_MOVE_FORWARD_LabelTitle_RESID);

         item.m_iId = ID_ARRANGE_FORWARD;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_MOVE_BACK_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_ARRANGE_MOVE_BACK_LabelTitle_RESID);

         item.m_iId = ID_ARRANGE_BACKWARD;

         group.m_itema.Add(item);

         tab.m_groupa.Add(group);

      }

      item.m_bPreferTwoLines = false;
      {

         CGroup group;

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_GROUP_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_ARRANGE_GROUP_LabelTitle_RESID);

         item.m_iId = ID_ARRANGE_GROUP;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_UNGROUP_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_ARRANGE_UNGROUP_LabelTitle_RESID);

         item.m_iId = ID_ARRANGE_UNGROUP;

         group.m_itema.Add(item);


         tab.m_groupa.Add(group);

      }

      item.m_bPreferTwoLines = true;
      {

         CGroup group;

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_ROTATE_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_ARRANGE_ROTATE_LabelTitle_RESID);

         item.m_iId = ID_ARRANGE_ROTATE;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_ROTATE_TO_ZERO_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_ARRANGE_ROTATE_TO_ZERO_LabelTitle_RESID);

         item.m_iId = ID_ARRANGE_ROTATE_ZERO;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_ROTATE_OPTIONS_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_ARRANGE_ROTATE_OPTIONS_LabelTitle_RESID);

         item.m_iId = ID_ARRANGE_ROTATE_OPT;

         group.m_itema.Add(item);

         tab.m_groupa.Add(group);

      }


                  {

                     CGroup group;

                     item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_FLIP_HORIZONTAL_LargeImages_RESID),"IMAGE",NULL);

                     item.m_wstrName .LoadStringA(ID_CMD_ARRANGE_FLIP_HORIZONTAL_LabelTitle_RESID);

                     item.m_iId = ID_ARRANGE_FLIP_HORIZ;

                     group.m_itema.Add(item);

                     item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_FLIP_VERTICAL_LargeImages_RESID),"IMAGE",NULL);

                     item.m_wstrName .LoadStringA(ID_CMD_ARRANGE_FLIP_VERTICAL_LabelTitle_RESID);

                     item.m_iId = ID_ARRANGE_FLIP_VERT;

                     group.m_itema.Add(item);


                     tab.m_groupa.Add(group);

                  }



                        {

                           CGroup group;

                           group.m_iMaxRowCount = 2;

                           group.m_wstrName .LoadStringA(cmdGroupAlignAndDistribute_LabelTitle_RESID);

                           item.m_wstrName.Empty();

                           item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_ALIGN_BOTTOM_LargeImages_RESID),"IMAGE",NULL);

                           item.m_iId = ID_ALIGN_BOTTOM;

                           group.m_itema.Add(item);

                           item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_ALIGN_LEFT_LargeImages_RESID),"IMAGE",NULL);

                           item.m_iId = ID_ALIGN_LEFT;

                           group.m_itema.Add(item);

                           item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_ALIGN_TOP_LargeImages_RESID),"IMAGE",NULL);

                           item.m_iId = ID_ALIGN_TOP;

                           group.m_itema.Add(item);

                           item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_ALIGN_RIGHT_LargeImages_RESID),"IMAGE",NULL);

                           item.m_iId = ID_ALIGN_RIGHT;

                           group.m_itema.Add(item);

                           item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_ALIGN_VERTICAL_CENTER_LargeImages_RESID),"IMAGE",NULL);

                           item.m_iId = ID_ALIGN_VCENTER;

                           group.m_itema.Add(item);

                           item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_ALIGN_HORIZONTAL_CENTER_LargeImages_RESID),"IMAGE",NULL);

                           item.m_iId = ID_ALIGN_HCENTER;

                           group.m_itema.Add(item);

                           item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_DISTRIBUTE_HORIZONTAL_CENTER_LargeImages_RESID),"IMAGE",NULL);

                           item.m_iId = ID_ALIGN_DISTH_CENTER;

                           group.m_itema.Add(item);

                           item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_DISTRIBUTE_VERTICAL_CENTER_LargeImages_RESID),"IMAGE",NULL);

                           item.m_iId = ID_ALIGN_DISTV_CENTER;

                           group.m_itema.Add(item);

                           item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_DISTRIBUTE_HORIZONTAL_DATUM_LargeImages_RESID),"IMAGE",NULL);

                           item.m_iId = ID_ALIGN_DISTH_DATUM;

                           group.m_itema.Add(item);

                           item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_DISTRIBUTE_VERTICAL_DATUM_LargeImages_RESID),"IMAGE",NULL);

                           item.m_iId = ID_ALIGN_DISTV_DATUM;

                           group.m_itema.Add(item);

                           tab.m_groupa.Add(group);

                        }


            {

               CGroup group;

#ifndef _PERSONAL_EDITION

               item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_ALIGN_AGAIN_LargeImages_RESID),"IMAGE",NULL);

               item.m_wstrName .LoadStringA(ID_CMD_ARRANGE_ALIGN_AGAIN_LabelTitle_RESID);

               item.m_iId = ID_ARRANGE_ALIGN_AGAIN;

               group.m_itema.Add(item);

               item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_ALIGN_OPTIONS_LargeImages_RESID),"IMAGE",NULL);

               item.m_wstrName .LoadStringA(ID_CMD_ARRANGE_ALIGN_OPTIONS_LabelTitle_RESID);

               item.m_iId = ID_ARRANGE_ALIGN_OPT;

               group.m_itema.Add(item);

#endif

               item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_DISTRIBUTE_ON_LINE_LargeImages_RESID),"IMAGE",NULL);

               item.m_wstrName .LoadStringA(ID_CMD_ARRANGE_DISTRIBUTE_ON_LINE_LabelTitle_RESID);

               item.m_iId = ID_ARRANGE_DISTRIBUTE;

               group.m_itema.Add(item);

               tab.m_groupa.Add(group);

            }
            item.m_bPreferTwoLines = false;
            {

               CGroup group;

               item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_LOCK_LargeImages_RESID),"IMAGE",NULL);

               item.m_wstrName .LoadStringA(ID_CMD_ARRANGE_LOCK_LabelTitle_RESID);

               item.m_iId = ID_ARRANGE_LOCK;

               group.m_itema.Add(item);

               item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_ARRANGE_UNLOCK_LargeImages_RESID),"IMAGE",NULL);

               item.m_wstrName .LoadStringA(ID_CMD_ARRANGE_UNLOCK_LabelTitle_RESID);

               item.m_iId = ID_ARRANGE_UNLOCK;

               group.m_itema.Add(item);


               tab.m_groupa.Add(group);

            }

            m_taba.Add(tab);

   }

#ifndef _PERSONAL_EDITION

   item.m_bPreferTwoLines = true;
   {

      CTab tab;

      tab.m_wstrName.LoadStringA(IDC_CMD_DATA_LabelTitle_RESID);
      {

         CGroup group;

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_DATA_EDIT_FIELD_NAMES_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_DATA_EDIT_FIELD_NAMES_LabelTitle_RESID);

         item.m_iId = ID_DATA_FIELD_NAMES;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_DATA_EDIT_OBJECT_INFO_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_DATA_EDIT_OBJECT_INFO_LabelTitle_RESID);

         item.m_iId = ID_DATA_OBJECT_INFO;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_DATA_FIND_AND_REPLACE_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_DATA_FIND_AND_REPLACE_LabelTitle_RESID);

         item.m_iId = ID_DATA_FIND_REPLACE;

         group.m_itema.Add(item);

         tab.m_groupa.Add(group);

      }
   {

      CGroup group;

      item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_DATA_REPORT_FORMATS_LargeImages_RESID),"IMAGE",NULL);

      item.m_wstrName .LoadStringA(ID_CMD_DATA_REPORT_FORMATS_LabelTitle_RESID);

      item.m_iId = ID_DATA_REPORT_FORMATS;

      group.m_itema.Add(item);

      item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_DATA_PRINT_REPORT_LargeImages_RESID),"IMAGE",NULL);

      item.m_wstrName .LoadStringA(ID_CMD_DATA_PRINT_REPORT_LabelTitle_RESID);

      item.m_iId = ID_DATA_PRINTREPORT;

      group.m_itema.Add(item);


      tab.m_groupa.Add(group);

   }

   {

      CGroup group;

      item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_DATA_UPDATE_LINK_LargeImages_RESID),"IMAGE",NULL);

      item.m_wstrName .LoadStringA(ID_CMD_DATA_UPDATE_LINK_LabelTitle_RESID);

      item.m_iId = ID_DATA_UPDATELINK;

      group.m_itema.Add(item);

      item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_DATA_BREAK_LINK_LargeImages_RESID),"IMAGE",NULL);

      item.m_wstrName .LoadStringA(ID_CMD_DATA_BREAK_LINK_LabelTitle_RESID);

      item.m_iId = ID_DATA_BREAKLINK;

      group.m_itema.Add(item);


      tab.m_groupa.Add(group);

   }


   m_taba.Add(tab);

   }

#endif

   {

      CTab tab;

      tab.m_wstrName.LoadStringA(IDC_CMD_LAYOUT_LabelTitle_RESID);
      {

         CGroup group;

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_LAYOUT_SET_SCALE_AND_UNITS_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_LAYOUT_SET_SCALE_AND_UNITS_LabelTitle_RESID);

         item.m_iId = ID_LAYOUT_SET_SCALE_UNITS;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_LAYOUT_SET_GRID_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_LAYOUT_SET_GRID_LabelTitle_RESID);

         item.m_bPopup = true;

         item.m_iId = ID_GRID_BASE;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_LAYOUT_SET_ANGLE_SNAP_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_LAYOUT_SET_ANGLE_SNAP_LabelTitle_RESID);

         item.m_iId = ID_ANGLE_SNAP_BASE;

         group.m_itema.Add(item);

         tab.m_groupa.Add(group);

         item.m_bPopup = false;

      }



   {

      CGroup group;

      item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_LAYOUT_HIDE_SHOW_RULERS_LargeImages_RESID),"IMAGE",NULL);

      item.m_wstrName .LoadStringA(ID_CMD_LAYOUT_HIDE_SHOW_RULERS_LabelTitle_RESID);

      item.m_iId = ID_LAYOUT_SHOW_RULERS;

      group.m_itema.Add(item);

      item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_LAYOUT_SCALE_RULERS_LargeImages_RESID),"IMAGE",NULL);

      item.m_wstrName .LoadStringA(ID_CMD_LAYOUT_SCALE_RULERS_LabelTitle_RESID);

      item.m_iId = ID_LAYOUT_SCALE_RULERS;

      group.m_itema.Add(item);

      item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_LAYOUT_STANDARD_RULERS_LargeImages_RESID),"IMAGE",NULL);

      item.m_wstrName .LoadStringA(ID_CMD_LAYOUT_STANDARD_RULERS_LabelTitle_RESID);

      item.m_iId = ID_LAYOUT_STANDARD_RULERS;

      group.m_itema.Add(item);

      tab.m_groupa.Add(group);

   }

   {

      CGroup group;

      item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_LAYOUT_SNAP_TO_OBJECT_LargeImages_RESID),"IMAGE",NULL);

      item.m_wstrName .LoadStringA(ID_CMD_LAYOUT_SNAP_TO_OBJECT_LabelTitle_RESID);

      item.m_iId = ID_LAYOUT_SNAP_TO_OBJECT;

      group.m_itema.Add(item);


      item.m_pimageLarge = LoadPNG(MAKEINTRESOURCE(ID_CMD_LAYOUT_SNAP_TO_GUIDES_LargeImages_RESID), "IMAGE", NULL);

      item.m_wstrName.LoadStringA(ID_CMD_LAYOUT_SNAP_TO_GUIDES_LabelTitle_RESID);

      item.m_iId = ID_LAYOUT_SNAP_TO_GUIDES;

      group.m_itema.Add(item);

      
      item.m_pimageLarge = LoadPNG(MAKEINTRESOURCE(ID_CMD_LAYOUT_SHOW_GUIDE_LINES_LargeImages_RESID), "IMAGE", NULL);

      item.m_wstrName.LoadStringA(ID_CMD_LAYOUT_SHOW_GUIDE_LINES_LabelTitle_RESID);

      item.m_iId = ID_LAYOUT_SHOW_GUIDE_LINES;

      group.m_itema.Add(item);

      tab.m_groupa.Add(group);

   }




   {

      CGroup group;

      item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_LAYOUT_HIDE_SHOW_GRID_LINES_LargeImages_RESID),"IMAGE",NULL);

      item.m_wstrName .LoadStringA(ID_CMD_LAYOUT_HIDE_SHOW_GRID_LINES_LabelTitle_RESID);

      item.m_iId = ID_LAYOUT_HIDE_GRID_LINES;

      group.m_itema.Add(item);

      item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_LAYOUT_HIDE_SHOW_PAGE_BREAKS_LargeImages_RESID),"IMAGE",NULL);

      item.m_wstrName .LoadStringA(ID_CMD_LAYOUT_HIDE_SHOW_PAGE_BREAKS_LabelTitle_RESID);

      item.m_iId = ID_LAYOUT_HIDE_PAGE_BREAKS;

      group.m_itema.Add(item);


      tab.m_groupa.Add(group);

   }




   {

      CGroup group;

      item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_LAYOUT_DRAWING_SIZE_LargeImages_RESID),"IMAGE",NULL);

      item.m_wstrName .LoadStringA(ID_CMD_LAYOUT_DRAWING_SIZE_LabelTitle_RESID);

      item.m_iId = ID_LAYOUT_DRAWING_SIZE;

      group.m_itema.Add(item);

      item.m_bPreferTwoLines = false;

#ifndef _PERSONAL_EDITION

      item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_LAYOUT_POSITION_LargeImages_RESID),"IMAGE",NULL);

      item.m_wstrName .LoadStringA(ID_CMD_LAYOUT_POSITION_LabelTitle_RESID);

      item.m_iId = ID_LAYOUT_POSITION;

      group.m_itema.Add(item);

#endif

      tab.m_groupa.Add(group);

   }

   m_taba.Add(tab);

   }

   {

      CTab tab;


      tab.m_wstrName.LoadStringA(IDC_CMD_VIEW_LabelTitle_RESID);

      {

         CGroup group;

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_VIEW_ZOOM_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_VIEW_ZOOM_LabelTitle_RESID);

         item.m_iId = ID_VIEW_ZOOM_IN_4X;

         item.m_bPopup = true;

         group.m_itema.Add(item);

         item.m_bPreferTwoLines = true;

         item.m_bPopup = false;

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_VIEW_RESTORE_PREVIOUS_ZOOM_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_VIEW_RESTORE_PREVIOUS_ZOOM_LabelTitle_RESID);

         item.m_iId = ID_VIEW_RESTORE_PREV_ZOOM;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_VIEW_HOME_VIEW_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_VIEW_HOME_VIEW_LabelTitle_RESID);

         item.m_iId = ID_VIEW_HOME_VIEW;

         group.m_itema.Add(item);

         tab.m_groupa.Add(group);

      }



   {

      CGroup group;

      item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_VIEW_HIDE_SHOW_CURSOR_POSITION_LargeImages_RESID),"IMAGE",NULL);

      item.m_wstrName .LoadStringA(ID_CMD_VIEW_HIDE_SHOW_CURSOR_POSITION_LabelTitle_RESID);

      item.m_iId = ID_VIEW_CURSOR_POS;

      group.m_itema.Add(item);


      tab.m_groupa.Add(group);

   }



   {

      CGroup group;

      item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_VIEW_SHOW_HIDE_AREA_LargeImages_RESID),"IMAGE",NULL);

      item.m_wstrName .LoadStringA(ID_CMD_VIEW_SHOW_HIDE_AREA_LabelTitle_RESID);

      item.m_iId = ID_VIEW_HIDE_SHOW_AREA;

      group.m_itema.Add(item);


      tab.m_groupa.Add(group);

   }


   item.m_bPreferTwoLines = false;


#ifndef _PERSONAL_EDITION

   {

      CGroup group;

      item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_VIEW_SLIDE_SHOW_LargeImages_RESID),"IMAGE",NULL);

      item.m_wstrName .LoadStringA(ID_CMD_VIEW_SLIDE_SHOW_LabelTitle_RESID);

      item.m_iId = ID_VIEW_SLIDE_SHOW;

      group.m_itema.Add(item);


      tab.m_groupa.Add(group);

   }

#endif

   {

      CGroup group;

      group.m_wstrName .LoadStringA(cmdGroupPalettes_LabelTitle_RESID);

      item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_VIEW_PALETTE_TOOL_LargeImages_RESID),"IMAGE",NULL);

      item.m_wstrName .LoadStringA(ID_CMD_VIEW_PALETTE_TOOL_LabelTitle_RESID);

      item.m_iId = ID_VIEW_PALETTE_TOOL;

      group.m_itema.Add(item);

      item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_VIEW_PALETTE_DIMENSIONS_LargeImages_RESID),"IMAGE",NULL);

      item.m_wstrName .LoadStringA(ID_CMD_VIEW_PALETTE_DIMENSIONS_LabelTitle_RESID);

      item.m_iId = ID_VIEW_PALETTE_DIM;

      group.m_itema.Add(item);

#ifndef _PERSONAL_EDITION

      item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_VIEW_PALETTE_ACCESSORY_LargeImages_RESID),"IMAGE",NULL);

      item.m_wstrName .LoadStringA(ID_CMD_VIEW_PALETTE_ACCESSORY_LabelTitle_RESID);

      item.m_iId = ID_VIEW_PALETTE_ACC;

      group.m_itema.Add(item);

#endif

      item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_VIEW_PALETTE_SHOW_SIZE_LargeImages_RESID),"IMAGE",NULL);

      item.m_wstrName .LoadStringA(ID_CMD_VIEW_PALETTE_SHOW_SIZE_LabelTitle_RESID);

      item.m_iId = ID_VIEW_PALETTE_SHOWSIZE;

      group.m_itema.Add(item);

      item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_VIEW_PALETTE_LIBRARY_LargeImages_RESID),"IMAGE",NULL);

      item.m_wstrName .LoadStringA(ID_CMD_VIEW_PALETTE_LIBRARY_LabelTitle_RESID);

      item.m_iId = ID_VIEW_LIBRARY;

      group.m_itema.Add(item);


#ifndef _PERSONAL_EDITION

      item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_VIEW_PALETTE_LAYERS_LargeImages_RESID),"IMAGE",NULL);

      item.m_wstrName .LoadStringA(ID_CMD_VIEW_PALETTE_LAYERS_LabelTitle_RESID);

      item.m_iId = ID_VIEW_LAYER;

      group.m_itema.Add(item);

#endif

      tab.m_groupa.Add(group);

   }
   m_taba.Add(tab);

   }

   item.m_bPreferTwoLines = true;
   {

      CTab tab;

      tab.m_wstrName.LoadStringA(IDC_CMD_WINDOW_LabelTitle_RESID);

      {

         CGroup group;

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_WINDOW_SHOW_HIDE_ALL_PALETTES_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_WINDOW_SHOW_HIDE_ALL_PALETTES_LabelTitle_RESID);

         item.m_iId = ID_VIEW_PALETTE_ALL;

         group.m_itema.Add(item);


         tab.m_groupa.Add(group);

      }


      item.m_bPreferTwoLines = false;


      {

         CGroup group;

         group.m_wstrName .LoadStringA(cmdGroupPalettes_LabelTitle_RESID);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_VIEW_PALETTE_TOOL_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_VIEW_PALETTE_TOOL_LabelTitle_RESID);

         item.m_iId = ID_VIEW_PALETTE_TOOL;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_VIEW_PALETTE_DIMENSIONS_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_VIEW_PALETTE_DIMENSIONS_LabelTitle_RESID);

         item.m_iId = ID_VIEW_PALETTE_DIM;

         group.m_itema.Add(item);

#ifndef _PERSONAL_EDITION

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_VIEW_PALETTE_ACCESSORY_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_VIEW_PALETTE_ACCESSORY_LabelTitle_RESID);

         item.m_iId = ID_VIEW_PALETTE_ACC;

         group.m_itema.Add(item);

#endif

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_VIEW_PALETTE_SHOW_SIZE_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_VIEW_PALETTE_SHOW_SIZE_LabelTitle_RESID);

         item.m_iId = ID_VIEW_PALETTE_SHOWSIZE;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_VIEW_PALETTE_LIBRARY_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_VIEW_PALETTE_LIBRARY_LabelTitle_RESID);

         item.m_iId = ID_VIEW_LIBRARY;

         group.m_itema.Add(item);


#ifndef _PERSONAL_EDITION

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_VIEW_PALETTE_LAYERS_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_VIEW_PALETTE_LAYERS_LabelTitle_RESID);

         item.m_iId = ID_VIEW_LAYER;

         group.m_itema.Add(item);

#endif

         tab.m_groupa.Add(group);
      }

      {

         CGroup group;

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_WINDOW_CASCADE_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_WINDOW_CASCADE_LabelTitle_RESID);

         item.m_iId = ID_WINDOW_CASCADE;

         group.m_itema.Add(item);

         item.m_bPreferTwoLines = true;


         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_WINDOW_TILE_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_WINDOW_TILE_LabelTitle_RESID);

         item.m_iId = ID_WINDOW_TILE_HORZ;

         group.m_itema.Add(item);

         item.m_pimageLarge= LoadPNG(MAKEINTRESOURCE(ID_CMD_WINDOW_ARRANGE_ICONS_LargeImages_RESID),"IMAGE",NULL);

         item.m_wstrName .LoadStringA(ID_CMD_WINDOW_ARRANGE_ICONS_LabelTitle_RESID);

         item.m_iId = ID_WINDOW_ARRANGE;

         group.m_itema.Add(item);

         tab.m_groupa.Add(group);

      }

      {

         CGroup group;

         item.m_pimageLarge = LoadPNG(MAKEINTRESOURCE(ID_CMD_WELCOME_SCREEN_LargeImages_RESID), "IMAGE", NULL);

         item.m_wstrName.LoadStringA(ID_CMD_WELCOME_SCREEN_LabelTitle_RESID);

         item.m_iId = ID_WELCOME_SCREEN;

         group.m_itema.Add(item);

         item.m_pimageLarge = LoadPNG(MAKEINTRESOURCE(IDB_PNG_WELCOME_TEMPLATE), "PNG", AfxGetApp()->m_hInstance);

         item.m_wstrName.LoadStringA(IDS_TEMPLATE_CHOOSER);

         item.m_iId = ID_FILE_NEW_FROM_TEMPLATE;

         group.m_itema.Add(item);

         tab.m_groupa.Add(group);

      }




      m_taba.Add(tab);

   }

   {

      CTab tab;

      tab.m_wstrName.LoadStringA(IDC_CMD_PROPERTIES_LabelTitle_RESID);



      m_taba.Add(tab);

   }

   // initialize state
   m_pStringMap = NULL;
   m_hRsrcImageWell = NULL;
   m_hInstImageWell = NULL;
   m_hbmImageWell = NULL;
   m_bDelayedButtonLayout = TRUE;

   // default image sizes
   m_sizeImage.cx = 16;
   m_sizeImage.cy = 15;

   // default button sizes
   m_sizeButton.cx = 23;
   m_sizeButton.cy = 22;

   // top and bottom borders are 1 larger than default for ease of grabbing
   m_cyTopBorder = 3;
   m_cyBottomBorder = 3;
   EnableActiveAccessibility();

}


CPCDraftRibbon::CPCDraftRibbon()
   : m_l(1)
   ,m_pframe(NULL)
{

}

CPCDraftRibbon::~CPCDraftRibbon()
{
}

BOOL CPCDraftRibbon::GetStatus(UINT32 nCmdID)
{
   //CHTMLEdView *pView = (CHTMLEdView*)m_pMainFrame->GetActiveView();
   //if(!pView)
   //   return 0;

   //return pView->GetCommandStatus(nCmdID);
   return FALSE;
}


// Static method to create an instance of the object.
__checkReturn HRESULT CPCDraftRibbon::CreateInstance(__in CMainFrame* pFrame,__deref_out_opt CPCDraftRibbon **ppApplication)
{
   if(!pFrame || !ppApplication)
   {
      return E_POINTER;
   }

   *ppApplication = NULL;
   HRESULT hr = S_OK;

   CPCDraftRibbon* pribbon = new CPCDraftRibbon();

   if(pribbon != NULL)
   {
      pribbon->m_pframe = pFrame;
      // pApplication->m_cRef is 1.
      *ppApplication = pribbon;
   }
   else
   {
      hr = E_OUTOFMEMORY;
   }

   return hr;
}

// IUnknown methods.
STDMETHODIMP_(ULONG) CPCDraftRibbon::AddRef()
{
   return InterlockedIncrement(&m_l);
}

STDMETHODIMP_(ULONG) CPCDraftRibbon::Release()
{
   LONG cRef = InterlockedDecrement(&m_l);
   if(cRef == 0)
   {
      delete this;
   }
   return cRef;
}

STDMETHODIMP CPCDraftRibbon::QueryInterface(REFIID iid,void** ppv)
{
   if(!ppv)
   {
      return E_POINTER;
   }

   if(iid == __uuidof(IUnknown))
   {
      *ppv = static_cast<IUnknown*>(static_cast<IUIApplication*>(this));
   }
   else if(iid == __uuidof(IUIApplication))
   {
      *ppv = static_cast<IUIApplication*>(this);
   }
   else if(iid == __uuidof(IUICommandHandler))
   {
      *ppv = static_cast<IUICommandHandler*>(this);
   }
   else
   {
      *ppv = NULL;
      return E_NOINTERFACE;
   }

   AddRef();
   return S_OK;
}

// IUIApplication methods.
STDMETHODIMP CPCDraftRibbon::OnViewChanged(UINT32 /*nViewID*/,
   __in UI_VIEWTYPE /*typeID*/,
   __in IUnknown* pView,
   UI_VIEWVERB verb,
   INT32 /*uReasonCode*/)
{
   HRESULT hr;

   // The Ribbon size has changed.
   if(verb == UI_VIEWVERB_SIZE)
   {
      IUIRibbon* pRibbon;
      pView->QueryInterface(IID_PPV_ARGS(&pRibbon));
      if(!pRibbon)
      {
         return E_FAIL;
      }

      UINT ulRibbonHeight;
      // Get the Ribbon height.
      hr = pRibbon->GetHeight(&ulRibbonHeight);

      pRibbon->Release();
      if(FAILED(hr))
      {
         return hr;
      }

      // Update the Ribbon bar so that the main frame can recalculate the child layout.
      m_pframe->m_RibbonBar.SetRibbonHeight(ulRibbonHeight);
      m_pframe->RecalcLayout();
   }

   return S_OK;
}


void CRibbonBar::SetRibbonHeight(UINT ui)
{
   m_ulRibbonHeight = ui;
}

STDMETHODIMP CPCDraftRibbon::OnCreateUICommand(UINT32 /*nCmdID*/,
   __in UI_COMMANDTYPE /*typeID*/,
   __deref_out IUICommandHandler** ppCommandHandler)
{
   // This application uses one command handler for all ribbon commands.
   return QueryInterface(IID_PPV_ARGS(ppCommandHandler));
}

STDMETHODIMP CPCDraftRibbon::OnDestroyUICommand(UINT32 /*commandId*/,
   __in UI_COMMANDTYPE /*typeID*/,
   __in_opt  IUICommandHandler* /*commandHandler*/)
{
   return E_NOTIMPL;
}

// IUICommandHandler methods.

// User action callback, with transient execution parameters.
STDMETHODIMP CPCDraftRibbon::Execute(UINT nCmdID,
   UI_EXECUTIONVERB /*verb*/,
   __in_opt const PROPERTYKEY* key,
   __in_opt const PROPVARIANT* ppropvarValue,
   __in_opt IUISimplePropertySet* /*pCommandExecutionProperties*/)
{
   m_pframe->SendMessage(WM_COMMAND,nCmdID,0);
   HRESULT hr = S_OK;
   //switch(nCmdID)
   //{
   //case IDC_FONT:
   //{
   //   if(key != NULL && ppropvarValue != NULL)
   //   {
   //      IPropertyStore* pPropertyStore = NULL;
   //      UIPropertyToInterface(*key,*ppropvarValue,&pPropertyStore);
   //      if(pPropertyStore)
   //      {
   //         // Set font to the HTML editor.
   //         hr = SetFont(m_pMainFrame,pPropertyStore);
   //         pPropertyStore->Release();
   //      }
   //   }
   //   break;
   //}
   //case IDC_MRULIST:
   //{
   //   if(key != NULL && UI_PKEY_SelectedItem == *key)
   //   {
   //      UINT uSelectedMRUItem = 0xffffffff;
   //      if(ppropvarValue != NULL && SUCCEEDED(UIPropertyToUInt32(*key,*ppropvarValue,&uSelectedMRUItem)))
   //      {
   //         ASSERT(uSelectedMRUItem < RECENT_FILE_COUNT);
   //         ::SendMessage(*m_pMainFrame,WM_COMMAND,uSelectedMRUItem + ID_FILE_MRU_FILE1,0);
   //      }
   //   }
   //   break;
   //}
   //default:
   //   ::SendMessage(*m_pMainFrame,WM_COMMAND,nCmdID,0);
   //}
   return hr;
}

STDMETHODIMP CPCDraftRibbon::UpdateProperty(UINT32 nCmdID,
   __in REFPROPERTYKEY key,
   __in_opt  const PROPVARIANT *currentValue,
   __out PROPVARIANT *newValue)
{
   HRESULT hr = E_NOTIMPL;
   //if(UI_PKEY_Enabled == key)
   //{
   //   return UIInitPropertyFromBoolean(UI_PKEY_Enabled,GetStatus(nCmdID),newValue);
   //}

   //switch(nCmdID)
   //{
   //case IDC_MRULIST:
   //   //if(UI_PKEY_Label == key)
   //   //{
   //   //   WCHAR label[MAX_PATH];
   //   //   LoadStringW(AfxGetResourceHandle(),IDS_RECENTFILES,label,_countof(label));
   //   //   hr = UIInitPropertyFromString(UI_PKEY_Label,label,newValue);
   //   //}
   //   //else if(UI_PKEY_RecentItems == key)
   //   //{
   //   //   hr = PopulateRibbonRecentItems(theApp.GetRecentFileList(),newValue);
   //   //}
   //   break;
   //case IDC_FONT:
   //   //if(UI_PKEY_FontProperties == key && currentValue != NULL)
   //   //{
   //   //   IPropertyStore* pPropertyStore;
   //   //   UIPropertyToInterface(UI_PKEY_FontProperties,*currentValue,&pPropertyStore);
   //   //   if(pPropertyStore)
   //   //   {
   //   //      // Update the Ribbon font control.
   //   //      hr = UpdateFont(m_pMainFrame,pPropertyStore);
   //   //      pPropertyStore->Release();
   //   //   }
   //   //}
   //   break;
   //case IDC_VIEWSOURCE:
   //   //if(UI_PKEY_BooleanValue == key)
   //   //{
   //   //   hr = UIInitPropertyFromBoolean(UI_PKEY_BooleanValue,!(GetStatus(nCmdID) & OLECMDF_LATCHED),newValue);
   //   //}
   //   break;
   //default:
   //   //if(UI_PKEY_BooleanValue == key)
   //   //{
   //   //   hr = UIInitPropertyFromBoolean(UI_PKEY_BooleanValue,GetStatus(nCmdID) & OLECMDF_LATCHED,newValue);
   //   //}
   //   break;
   //}
   return hr;
}


__checkReturn HRESULT InitRibbon(__in CMainFrame* pMainFrame,__deref_out_opt IUnknown** ppFramework)
{
   if(ppFramework == NULL)
   {
      return E_POINTER;
   }

   CComPtr<IPropertyStore> spPropertyStore;


   // Create the IUIFramework instance.
   IUIFramework* pFramework = NULL;
   HRESULT hr = ::CoCreateInstance(CLSID_UIRibbonFramework,NULL,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&pFramework));
   if(FAILED(hr))
   {
      return hr;
   }

   // Instantiate the CPCDraftRibbon object.
   CPCDraftRibbon* pAppObject = NULL;
   IUIApplication* pApplication = NULL;
   hr = CPCDraftRibbon::CreateInstance(pMainFrame,&pAppObject);

   if(FAILED(hr))
   {
      goto done;
   }

#pragma warning( disable : 6011)    // pAppObject cannot be NULL.
   hr = pAppObject->QueryInterface(IID_PPV_ARGS(&pApplication));
#pragma warning( default : 6011)    

   if(FAILED(hr))
   {
      goto done;
   }

   hr = pFramework->Initialize(*pMainFrame,pApplication);
   if(FAILED(hr))
   {
      goto done;
   }


   //// _spFramework is a pointer to the IUIFramework interface that is assigned 
   //// when the Ribbon is initialized.
   //if(SUCCEEDED(pFramework->QueryInterface(&spPropertyStore)))
   //{
   //   PROPVARIANT propvarBackground;
   //   PROPVARIANT propvarHighlight;
   //   PROPVARIANT propvarText;

   //   // UI_HSBCOLOR is a type defined in UIRibbon.h that is composed of 
   //   // three component values: hue, saturation and brightness, respectively.
   //   UI_HSBCOLOR BackgroundColor = UI_HSB(0x00,0x00,0x00);
   //   UI_HSBCOLOR HighlightColor = UI_HSB(0x00,0x00,0x87);
   //   UI_HSBCOLOR TextColor = UI_HSB(0x00,0x00,0xff);

   //   InitPropVariantFromUInt32(BackgroundColor,&propvarBackground);
   //   InitPropVariantFromUInt32(HighlightColor,&propvarHighlight);
   //   InitPropVariantFromUInt32(TextColor,&propvarText);

   //   spPropertyStore->SetValue(UI_PKEY_GlobalBackgroundColor,propvarBackground);
   //   spPropertyStore->SetValue(UI_PKEY_GlobalTextColor,propvarText);

   //   spPropertyStore->Commit();
   //}

   hr = pFramework->LoadUI(GetModuleHandle(NULL),L"APPLICATION_RIBBON");
   if(FAILED(hr))
   {
      goto done;
   }

   // Return IUIFramework interface to the caller.
   hr = pFramework->QueryInterface(ppFramework);

done:

   if(pApplication)
   {
      pApplication->Release();
   }

   if(pAppObject)
   {
      pAppObject->Release();
   }

   if(pFramework)
   {
      pFramework->Release();
   }

   return hr;
}

void DestroyRibbon(__in IUnknown* pFramework)
{
   IUIFramework* pUIFramework;
   pFramework->QueryInterface(IID_PPV_ARGS(&pUIFramework));

   ASSERT(pFramework != NULL);

   // Destroy the Ribbon.
   pUIFramework->Destroy();
   pUIFramework->Release();
}

HRESULT SetModes(__in IUnknown* pFramework,UINT modes)
{
   IUIFramework* pUIFramework;
   pFramework->QueryInterface(IID_PPV_ARGS(&pUIFramework));
   if(pUIFramework == NULL)
   {
      return E_FAIL;
   }

   // Change the modes.
   HRESULT hr = pUIFramework->SetModes(modes);
   pUIFramework->Release();

   return hr;
}

HRESULT RibbonInvalidate(__in IUnknown* pFramework)
{
   IUIFramework* pUIFramework;
   pFramework->QueryInterface(IID_PPV_ARGS(&pUIFramework));
   if(pUIFramework == NULL)
   {
      return E_FAIL;
   }

   //// Invalidate the Font control properties.
   //HRESULT hr = pUIFramework->InvalidateUICommand(IDC_FONT,UI_INVALIDATIONS_PROPERTY,&UI_PKEY_FontProperties);

   HRESULT hr;

   // Invalidate the control values, for example, the checked value of toggle buttons and enabled state.
   hr = pUIFramework->InvalidateUICommand(0,UI_INVALIDATIONS_STATE | UI_INVALIDATIONS_VALUE,NULL);

   UNREFERENCED_PARAMETER(hr);

   pUIFramework->Release();
   return S_OK;
}



// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "RibbonBar.h"



#define new DEBUG_NEW

// Forward declaration
HRESULT GetCommCtrlVersion(
   _Out_ LPDWORD pdwMajor,
   _Out_ LPDWORD pdwMinor);

#define STRING_REFRESH_OFFSET 1000000
/////////////////////////////////////////////////////////////////////////////
// CRibbonBar creation etc


/*
DIBs use RGBQUAD format:
0xbb 0xgg 0xrr 0x00

Reasonably efficient code to convert a COLORREF into an
RGBQUAD is byte-order-dependent, so we need different
code depending on the byte order we're targeting.
*/
#define AFX_RGB_TO_RGBQUAD(r,g,b)   (RGB(b,g,r))
#define AFX_CLR_TO_RGBQUAD(clr)     (RGB(GetBValue(clr), GetGValue(clr), GetRValue(clr)))

struct AFX_COLORMAP
{
   // use DWORD instead of RGBQUAD so we can compare two RGBQUADs easily
   DWORD rgbqFrom;
   int iSysColorTo;
};

AFX_STATIC_DATA const AFX_COLORMAP _afxSysColorMap[] =
{
   // mapping from color in DIB to system color
   {AFX_RGB_TO_RGBQUAD(0x00,0x00,0x00),COLOR_BTNTEXT},       // black
   {AFX_RGB_TO_RGBQUAD(0x80,0x80,0x80),COLOR_BTNSHADOW},     // dark gray
   {AFX_RGB_TO_RGBQUAD(0xC0,0xC0,0xC0),COLOR_BTNFACE},       // bright gray
   {AFX_RGB_TO_RGBQUAD(0xFF,0xFF,0xFF),COLOR_BTNHIGHLIGHT}   // white
};


CRibbonBar::~CRibbonBar()
{
   if(m_hbmImageWell)
   {
      ::DeleteObject(m_hbmImageWell);
      m_hbmImageWell = NULL;
   }
   delete m_pStringMap;

   m_nCount = 0;
}

BOOL CRibbonBar::Create(CWnd* pParentWnd,DWORD dwStyle,UINT nID)
{
   return CreateEx(pParentWnd,0,dwStyle,
      CRect(m_cxLeftBorder,m_cyTopBorder,m_cxRightBorder,m_cyBottomBorder),nID);
}

BOOL CRibbonBar::CreateEx(CWnd* pParentWnd,DWORD dwCtrlStyle,DWORD dwStyle,CRect rcBorders,UINT nID)
{
   ASSERT_VALID(pParentWnd);   // must have a parent
   ASSERT(!((dwStyle & CBRS_SIZE_FIXED) && (dwStyle & CBRS_SIZE_DYNAMIC)));

   SetBorders(rcBorders);

   // save the style
   m_dwStyle = (dwStyle & CBRS_ALL);
   if(nID == AFX_IDW_TOOLBAR)
      m_dwStyle |= CBRS_HIDE_INPLACE;

   dwStyle &= ~CBRS_ALL;
   dwStyle |= CCS_NOPARENTALIGN | CCS_NOMOVEY | CCS_NODIVIDER | CCS_NORESIZE;
   dwStyle |= dwCtrlStyle;
   m_bNeedLayout = true;
   // create the HWND
   CRect rect; rect.SetRectEmpty();
   if(!CWnd::Create(NULL,NULL,dwStyle,rect,pParentWnd,nID))
      return FALSE;

   // sync up the sizes
   SetSizes(m_sizeButton,m_sizeImage);

   if(!m_properties.Create(this))
   {

      return FALSE;

   }

   char sz[2];

   sz[0] = 231;
   sz[1] = '\0';
   m_buttonTabLeft.Create(NULL,WS_CHILD | BS_OWNERDRAW,CRect(0,0,16,100),this,ID_TAB_LEFT_ARROW);
   m_buttonTabLeft.EnableWindow();
   m_buttonTabLeft.m_etype = CArrowButton::type_left;

   m_buttonTabRight.Create(NULL, WS_CHILD | BS_OWNERDRAW,CRect(64,0,64 + 16,100),this,ID_TAB_RIGHT_ARROW);
   m_buttonTabRight.EnableWindow();
   m_buttonTabRight.m_etype = CArrowButton::type_right;

   m_buttonLeft.Create(NULL,WS_CHILD | BS_OWNERDRAW,CRect(0,0,16,100),this,ID_RIBBON_LEFT_ARROW);
   m_buttonLeft.EnableWindow();
   m_buttonLeft.m_etype = CArrowButton::type_left;

   m_buttonRight.Create(NULL,WS_CHILD | BS_OWNERDRAW,CRect(64,0,64 + 16,100),this,ID_RIBBON_RIGHT_ARROW);
   m_buttonRight.EnableWindow();
   m_buttonRight.m_etype = CArrowButton::type_right;

   // Note: Parent must resize itself for control bar to be resized
   SetTimer(1000,100,NULL);
   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CRibbonBar

BOOL CRibbonBar::OnNcCreate(LPCREATESTRUCT lpCreateStruct)
{
   if(!CControlBar::OnNcCreate(lpCreateStruct))
      return FALSE;

   // if the owner was set before the toolbar was created, set it now
   if(m_hWndOwner != NULL)
      DefWindowProc(TB_SETPARENT,(WPARAM)m_hWndOwner,0);

   DefWindowProc(TB_BUTTONSTRUCTSIZE,(WPARAM)sizeof(TBBUTTON),0);
   return TRUE;
}

void CRibbonBar::SetOwner(CWnd* pOwnerWnd)
{
   ASSERT_VALID(this);
   if(m_hWnd != NULL)
   {
      ASSERT(::IsWindow(m_hWnd));
      DefWindowProc(TB_SETPARENT,(WPARAM)pOwnerWnd->GetSafeHwnd(),0);
   }
   CControlBar::SetOwner(pOwnerWnd);
}

void CRibbonBar::SetSizes(SIZE sizeButton,SIZE sizeImage)
{
   ASSERT_VALID(this);

   // sizes must be non-zero and positive
   ASSERT(sizeButton.cx > 0 && sizeButton.cy > 0);
   ASSERT(sizeImage.cx > 0 && sizeImage.cy > 0);

   // button must be big enough to hold image
   //   + 7 pixels on x
   //   + 6 pixels on y
   ASSERT(sizeButton.cx >= sizeImage.cx + 7);
   ASSERT(sizeButton.cy >= sizeImage.cy + 6);

   if(::IsWindow(m_hWnd))
   {
      // set the sizes via TB_SETBITMAPSIZE and TB_SETBUTTONSIZE
      //VERIFY(SendMessage(TB_SETBITMAPSIZE,0,MAKELONG(sizeImage.cx,sizeImage.cy)));
      //VERIFY(SendMessage(TB_SETBUTTONSIZE,0,MAKELONG(sizeButton.cx,sizeButton.cy)));
      //if(_afxComCtlVersion >= VERSION_6)
      //{
      //   DWORD dwSize = (DWORD)SendMessage(TB_GETBUTTONSIZE,0,0);
      //   m_sizeButton.cx = LOWORD(dwSize);
      //   m_sizeButton.cy = HIWORD(dwSize);
      //}
      Invalidate();   // just to be nice if called when toolbar is visible
   }
   else
   {
      // just set our internal values for later
      m_sizeButton = sizeButton;
      m_sizeImage = sizeImage;
   }
}

void CRibbonBar::SetHeight(int cyHeight)
{
   ASSERT_VALID(this);

   int nHeight = cyHeight;
   //   if(m_dwStyle & CBRS_BORDER_TOP)
   //    cyHeight -= afxData.cyBorder2;
   // if(m_dwStyle & CBRS_BORDER_BOTTOM)
   //  cyHeight -= afxData.cyBorder2;
   m_cyBottomBorder = (cyHeight - m_sizeButton.cy) / 2;
   // if there is an extra pixel, m_cyTopBorder will get it
   m_cyTopBorder = cyHeight - m_sizeButton.cy - m_cyBottomBorder;
   if(m_cyTopBorder < 0)
   {
      TRACE(traceAppMsg,0,"Warning: CRibbonBar::SetHeight(%d) is smaller than button.\n",
         nHeight);
      m_cyBottomBorder += m_cyTopBorder;
      m_cyTopBorder = 0;  // will clip at bottom
   }

   // recalculate the non-client region
   SetWindowPos(NULL,0,0,0,0,
      SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
   Invalidate();   // just to be nice if called when toolbar is visible
}

struct CRibbonBarData
{
   WORD wVersion;
   WORD wWidth;
   WORD wHeight;
   WORD wItemCount;
   //WORD aItems[wItemCount]

   WORD* items()
   {
      return (WORD*)(this + 1);
   }
};

BOOL CRibbonBar::LoadToolBar(LPCTSTR lpszResourceName)
{
   ASSERT_VALID(this);
   ASSERT(lpszResourceName != NULL);

   // determine location of the bitmap in resource fork
   HINSTANCE hInst = AfxFindResourceHandle(lpszResourceName,RT_TOOLBAR);
   HRSRC hRsrc = ::FindResource(hInst,lpszResourceName,RT_TOOLBAR);
   if(hRsrc == NULL)
      return FALSE;

   HGLOBAL hGlobal = LoadResource(hInst,hRsrc);
   if(hGlobal == NULL)
      return FALSE;

   CRibbonBarData* pData = (CRibbonBarData*)LockResource(hGlobal);
   if(pData == NULL)
      return FALSE;
   ASSERT(pData->wVersion == 1);

   UINT* pItems = new UINT[pData->wItemCount];
   for(int i = 0; i < pData->wItemCount; i++)
      pItems[i] = pData->items()[i];
   BOOL bResult = SetButtons(pItems,pData->wItemCount);
   delete[] pItems;

   if(bResult)
   {
      // set new sizes of the buttons
      CSize sizeImage(pData->wWidth,pData->wHeight);
      CSize sizeButton(pData->wWidth + 7,pData->wHeight + 7);
      SetSizes(sizeButton,sizeImage);

      // load bitmap now that sizes are known by the toolbar control
      bResult = LoadBitmap(lpszResourceName);
   }

   UnlockResource(hGlobal);
   FreeResource(hGlobal);

   return bResult;
}

BOOL CRibbonBar::LoadBitmap(LPCTSTR lpszResourceName)
{
   ASSERT_VALID(this);
   ASSERT(lpszResourceName != NULL);

   // determine location of the bitmap in resource fork
   HINSTANCE hInstImageWell = AfxFindResourceHandle(lpszResourceName,RT_BITMAP);
   HRSRC hRsrcImageWell = ::FindResource(hInstImageWell,lpszResourceName,RT_BITMAP);
   if(hRsrcImageWell == NULL)
      return FALSE;

   // load the bitmap
   HBITMAP hbmImageWell;
   hbmImageWell = AfxLoadSysColorBitmap(hInstImageWell,hRsrcImageWell);

   // tell common control toolbar about the new bitmap
   if(!AddReplaceBitmap(hbmImageWell))
      return FALSE;

   // remember the resource handles so the bitmap can be recolored if necessary
   m_hInstImageWell = hInstImageWell;
   m_hRsrcImageWell = hRsrcImageWell;
   return TRUE;
}

BOOL CRibbonBar::SetBitmap(HBITMAP hbmImageWell)
{
   ASSERT_VALID(this);
   ASSERT(hbmImageWell != NULL);

   // the caller must manage changing system colors
   m_hInstImageWell = NULL;
   m_hRsrcImageWell = NULL;

   // tell common control toolbar about the new bitmap
   return AddReplaceBitmap(hbmImageWell);
}

BOOL CRibbonBar::AddReplaceBitmap(HBITMAP hbmImageWell)
{
   // need complete bitmap size to determine number of images
   BITMAP bitmap;
   VERIFY(::GetObject(hbmImageWell,sizeof(BITMAP),&bitmap));

   // add the bitmap to the common control toolbar
   BOOL bResult;
   if(m_hbmImageWell == NULL)
   {
      TBADDBITMAP addBitmap;
      addBitmap.hInst = NULL; // makes TBADDBITMAP::nID behave a HBITMAP
      addBitmap.nID = (UINT_PTR)hbmImageWell;
      bResult =  DefWindowProc(TB_ADDBITMAP,
         bitmap.bmWidth / m_sizeImage.cx,(LPARAM)&addBitmap) == 0;
   }
   else
   {
      TBREPLACEBITMAP replaceBitmap;
      replaceBitmap.hInstOld = NULL;
      replaceBitmap.nIDOld = (UINT_PTR)m_hbmImageWell;
      replaceBitmap.hInstNew = NULL;
      replaceBitmap.nIDNew = (UINT_PTR)hbmImageWell;
      replaceBitmap.nButtons = bitmap.bmWidth / m_sizeImage.cx;
      bResult = (BOOL)DefWindowProc(TB_REPLACEBITMAP,0,
         (LPARAM)&replaceBitmap);
   }
   // remove old bitmap, if present
   if(bResult)
   {
      if(m_hbmImageWell != NULL)
      {
         ::DeleteObject(m_hbmImageWell);
      }
      m_hbmImageWell = hbmImageWell;
   }

   return bResult;
}

BOOL CRibbonBar::SetButtons(const UINT* lpIDArray,int nIDCount)
{
   ASSERT_VALID(this);
   ASSERT(nIDCount >= 1);  // must be at least one of them
   ASSERT(lpIDArray == NULL ||
      AfxIsValidAddress(lpIDArray,sizeof(UINT) * nIDCount,FALSE));

   // delete all existing buttons
   int nCount = (int)DefWindowProc(TB_BUTTONCOUNT,0,0);
   while(nCount--)
      VERIFY(DefWindowProc(TB_DELETEBUTTON,0,0));

   TBBUTTON button; memset(&button,0,sizeof(TBBUTTON));
   button.iString = -1;
   if(lpIDArray != NULL)
   {
      // add new buttons to the common control
      int iImage = 0;
      for(int i = 0; i < nIDCount; i++)
      {
         button.fsState = TBSTATE_ENABLED;
         if((button.idCommand = *lpIDArray++) == 0)
         {
            // separator
            button.fsStyle = TBSTYLE_SEP;
            // width of separator includes 8 pixel overlap
            if((GetStyle() & TBSTYLE_FLAT))
               button.iBitmap = 6;
            else
               button.iBitmap = 8;
         }
         else
         {
            // a command button with image
            button.fsStyle = TBSTYLE_BUTTON;
            button.iBitmap = iImage++;
         }
         if(!DefWindowProc(TB_ADDBUTTONS,1,(LPARAM)&button))
            return FALSE;
      }
   }
   else
   {
      // add 'blank' buttons
      button.fsState = TBSTATE_ENABLED;
      for(int i = 0; i < nIDCount; i++)
      {
         ASSERT(button.fsStyle == TBSTYLE_BUTTON);
         if(!DefWindowProc(TB_ADDBUTTONS,1,(LPARAM)&button))
            return FALSE;
      }
   }
   m_nCount = (int)DefWindowProc(TB_BUTTONCOUNT,0,0);
   m_bDelayedButtonLayout = TRUE;

   return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CRibbonBar attribute access

void CRibbonBar::_GetButton(int nIndex,TBBUTTON* pButton) const
{
   CRibbonBar* pBar = (CRibbonBar*)this;
   VERIFY(pBar->DefWindowProc(TB_GETBUTTON,nIndex,(LPARAM)pButton));
   // TBSTATE_ENABLED == TBBS_DISABLED so invert it
   pButton->fsState ^= TBSTATE_ENABLED;
}

void CRibbonBar::_SetButton(int nIndex,TBBUTTON* pButton)
{
   // get original button state
   TBBUTTON button;
   VERIFY(DefWindowProc(TB_GETBUTTON,nIndex,(LPARAM)&button));

   // prepare for old/new button comparsion
   button.bReserved[0] = 0;
   button.bReserved[1] = 0;
   // TBSTATE_ENABLED == TBBS_DISABLED so invert it
   pButton->fsState ^= TBSTATE_ENABLED;
   pButton->bReserved[0] = 0;
   pButton->bReserved[1] = 0;

   // nothing to do if they are the same
   if(memcmp(pButton,&button,sizeof(TBBUTTON)) != 0)
   {
      // don't redraw everything while setting the button
      DWORD dwStyle = GetStyle();
      ModifyStyle(WS_VISIBLE,0);
      VERIFY(DefWindowProc(TB_DELETEBUTTON,nIndex,0));

      // Force a recalc of the toolbar's layout to work around a comctl bug
      if(pButton->iString < -1){
         int iTextRows;
         iTextRows = (int)::SendMessage(m_hWnd,TB_GETTEXTROWS,0,0);
         ::SendMessage(m_hWnd,WM_SETREDRAW,FALSE,0);
         ::SendMessage(m_hWnd,TB_SETMAXTEXTROWS,iTextRows + 1,0);
         ::SendMessage(m_hWnd,TB_SETMAXTEXTROWS,iTextRows,0);
         ::SendMessage(m_hWnd,WM_SETREDRAW,TRUE,0);
         pButton->iString+=STRING_REFRESH_OFFSET;
      }
      VERIFY(DefWindowProc(TB_INSERTBUTTON,nIndex,(LPARAM)pButton));
      ModifyStyle(0,dwStyle & WS_VISIBLE);

      // invalidate appropriate parts
      if(((pButton->fsStyle ^ button.fsStyle) & TBSTYLE_SEP) ||
         ((pButton->fsStyle & TBSTYLE_SEP) && pButton->iBitmap != button.iBitmap))
      {
         // changing a separator
         Invalidate();
      }
      else
      {
         // invalidate just the button
         CRect rect;
         if(DefWindowProc(TB_GETITEMRECT,nIndex,(LPARAM)&rect))
            InvalidateRect(rect);
      }
   }
}

int CRibbonBar::CommandToIndex(UINT nIDFind) const
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));

   CRibbonBar* pBar = (CRibbonBar*)this;
   return (int)pBar->DefWindowProc(TB_COMMANDTOINDEX,nIDFind,0);
}

UINT CRibbonBar::GetItemID(int nIndex) const
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));

   TBBUTTON button;
   _GetButton(nIndex,&button);
   return button.idCommand;
}

void CRibbonBar::GetItemRect(int nIndex,LPRECT lpRect) const
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));

   // handle any delayed layout
   if(m_bDelayedButtonLayout)
      ((CRibbonBar*)this)->Layout();

   // now it is safe to get the item rectangle
   CRibbonBar* pBar = (CRibbonBar*)this;
   if(!pBar->DefWindowProc(TB_GETITEMRECT,nIndex,(LPARAM)lpRect))
      SetRectEmpty(lpRect);
}

void CRibbonBar::Layout()
{
   ASSERT(m_bDelayedButtonLayout);

   m_bDelayedButtonLayout = FALSE;

   BOOL bHorz = (m_dwStyle & CBRS_ORIENT_HORZ) != 0;
   if((m_dwStyle & CBRS_FLOATING) && (m_dwStyle & CBRS_SIZE_DYNAMIC))
      ((CRibbonBar*)this)->CalcDynamicLayout(0,LM_HORZ | LM_MRUWIDTH | LM_COMMIT);
   else if(bHorz)
      ((CRibbonBar*)this)->CalcDynamicLayout(0,LM_HORZ | LM_HORZDOCK | LM_COMMIT);
   else
      ((CRibbonBar*)this)->CalcDynamicLayout(0,LM_VERTDOCK | LM_COMMIT);
}

UINT CRibbonBar::GetButtonStyle(int nIndex) const
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));

   TBBUTTON button;
   _GetButton(nIndex,&button);
   return MAKELONG(button.fsStyle,button.fsState);
}

void CRibbonBar::SetButtonStyle(int nIndex,UINT nStyle)
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));

   TBBUTTON button;
   _GetButton(nIndex,&button);
   if(button.fsStyle != (BYTE)LOWORD(nStyle) || button.fsState != (BYTE)HIWORD(nStyle))
   {
      button.fsStyle = (BYTE)LOWORD(nStyle);
      button.fsState = (BYTE)HIWORD(nStyle);
      _SetButton(nIndex,&button);
      m_bDelayedButtonLayout = TRUE;
   }
}

#define CX_OVERLAP  0

CSize CRibbonBar::CalcSize(TBBUTTON* pData,int nCount)
{
   ASSERT(pData != NULL && nCount > 0);

   CPoint cur(0,0);
   CSize sizeResult(0,0);

   DWORD dwExtendedStyle = DWORD(DefWindowProc(TB_GETEXTENDEDSTYLE,0,0));

   for(int i = 0; i < nCount; i++)
   {
      //  The IE4 version of COMCTL32.DLL calculates the separation
      //	on a TBSTYLE_WRAP button as 100% of the value in iBitmap compared
      //	to the other versions which calculate it at 2/3 of that value.
      //	This is actually a bug which should be fixed in IE 4.01, so we
      //	only do the 100% calculation specifically for IE4.
      int cySep = pData[i].iBitmap;
      if(!(GetStyle() & TBSTYLE_FLAT))
         cySep = cySep * 2 / 3;

      if(pData[i].fsState & TBSTATE_HIDDEN)
         continue;

      int cx = m_sizeButton.cx;
      if(pData[i].fsStyle & TBSTYLE_SEP)
      {
         // a separator represents either a height or width
         if(pData[i].fsState & TBSTATE_WRAP)
            sizeResult.cy = max(cur.y + m_sizeButton.cy + cySep,sizeResult.cy);
         else
            sizeResult.cx = max(cur.x + pData[i].iBitmap,sizeResult.cx);
      }
      else
      {
         // check for dropdown style, but only if the buttons are being drawn
         if((pData[i].fsStyle & TBSTYLE_DROPDOWN) &&
            (dwExtendedStyle & TBSTYLE_EX_DRAWDDARROWS))
         {
         }
         sizeResult.cx = max(cur.x + cx,sizeResult.cx);
         sizeResult.cy = max(cur.y + m_sizeButton.cy,sizeResult.cy);
      }

      if(pData[i].fsStyle & TBSTYLE_SEP)
         cur.x += pData[i].iBitmap;
      else
         cur.x += cx - CX_OVERLAP;

      if(pData[i].fsState & TBSTATE_WRAP)
      {
         cur.x = 0;
         cur.y += m_sizeButton.cy;
         if(pData[i].fsStyle & TBSTYLE_SEP)
            cur.y += cySep;
      }
   }
   return sizeResult;
}

int CRibbonBar::WrapToolBar(TBBUTTON* pData,int nCount,int nWidth)
{
   ASSERT(pData != NULL && nCount > 0);

   int nResult = 0;
   int x = 0;
   for(int i = 0; i < nCount; i++)
   {
      pData[i].fsState &= ~TBSTATE_WRAP;

      if(pData[i].fsState & TBSTATE_HIDDEN)
         continue;

      int dx,dxNext;
      if(pData[i].fsStyle & TBSTYLE_SEP)
      {
         dx = pData[i].iBitmap;
         dxNext = dx;
      }
      else
      {
         dx = m_sizeButton.cx;
         dxNext = dx - CX_OVERLAP;
      }

      if(x + dx > nWidth)
      {
         BOOL bFound = FALSE;
         for(int j = i; j >= 0 && !(pData[j].fsState & TBSTATE_WRAP); j--)
         {
            // Find last separator that isn't hidden
            // a separator that has a command ID is not
            // a separator, but a custom control.
            if((pData[j].fsStyle & TBSTYLE_SEP) &&
               (pData[j].idCommand == 0) &&
               !(pData[j].fsState & TBSTATE_HIDDEN))
            {
               bFound = TRUE; i = j; x = 0;
               pData[j].fsState |= TBSTATE_WRAP;
               nResult++;
               break;
            }
         }
         if(!bFound)
         {
            for(int j = i - 1; j >= 0 && !(pData[j].fsState & TBSTATE_WRAP); j--)
            {
               // Never wrap anything that is hidden,
               // or any custom controls
               if((pData[j].fsState & TBSTATE_HIDDEN) ||
                  ((pData[j].fsStyle & TBSTYLE_SEP) &&
                  (pData[j].idCommand != 0)))
                  continue;

               bFound = TRUE; i = j; x = 0;
               pData[j].fsState |= TBSTATE_WRAP;
               nResult++;
               break;
            }
            if(!bFound)
               x += dxNext;
         }
      }
      else
         x += dxNext;
   }
   return nResult + 1;
}

void  CRibbonBar::SizeToolBar(TBBUTTON* pData,int nCount,int nLength,BOOL bVert)
{
   ASSERT(pData != NULL && nCount > 0);

   if(!bVert)
   {
      int nMin,nMax,nTarget,nCurrent,nMid;

      // Wrap ToolBar as specified
      nMax = nLength;
      nTarget = WrapToolBar(pData,nCount,nMax);

      // Wrap ToolBar vertically
      nMin = 0;
      nCurrent = WrapToolBar(pData,nCount,nMin);

      if(nCurrent != nTarget)
      {
         while(nMin < nMax)
         {
            nMid = (nMin + nMax) / 2;
            nCurrent = WrapToolBar(pData,nCount,nMid);

            if(nCurrent == nTarget)
               nMax = nMid;
            else
            {
               if(nMin == nMid)
               {
                  WrapToolBar(pData,nCount,nMax);
                  break;
               }
               nMin = nMid;
            }
         }
      }
      CSize size = CalcSize(pData,nCount);
      WrapToolBar(pData,nCount,size.cx);
   }
   else
   {
      CSize sizeMax,sizeMin,sizeMid;

      // Wrap ToolBar vertically
      WrapToolBar(pData,nCount,0);
      sizeMin = CalcSize(pData,nCount);

      // Wrap ToolBar horizontally
      WrapToolBar(pData,nCount,32767);
      sizeMax = CalcSize(pData,nCount);

      while(sizeMin.cx < sizeMax.cx)
      {
         sizeMid.cx = (sizeMin.cx + sizeMax.cx) / 2;
         WrapToolBar(pData,nCount,sizeMid.cx);
         sizeMid = CalcSize(pData,nCount);

         if(nLength < sizeMid.cy)
         {
            if(sizeMin == sizeMid)
            {
               WrapToolBar(pData,nCount,sizeMax.cx);
               return;
            }
            sizeMin = sizeMid;
         }
         else if(nLength > sizeMid.cy)
         {
            if(sizeMax == sizeMid)
            {
               WrapToolBar(pData,nCount,sizeMin.cx);
               return;
            }
            sizeMax = sizeMid;
         }
         else
            return;
      }
   }
}

struct _AFX_CONTROLPOS
{
   int nIndex,nID;
   CRect rectOldPos;
};

CSize CRibbonBar::CalcLayout(DWORD dwMode,int nLength)
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));
   if(dwMode & LM_HORZDOCK)
      ASSERT(dwMode & LM_HORZ);

   int nCount;
   TBBUTTON* pData = NULL;
   CSize sizeResult(0,0);

   //BLOCK: Load Buttons
   {
      nCount = int(DefWindowProc(TB_BUTTONCOUNT,0,0));
      if(nCount != 0)
      {
         int i;
         pData = new TBBUTTON[nCount];
         for(i = 0; i < nCount; i++)
            _GetButton(i,&pData[i]);
      }
   }

   if(nCount > 0)
   {
      if(!(m_dwStyle & CBRS_SIZE_FIXED))
      {
         BOOL bDynamic = m_dwStyle & CBRS_SIZE_DYNAMIC;

         if(bDynamic && (dwMode & LM_MRUWIDTH))
            SizeToolBar(pData,nCount,m_nMRUWidth);
         else if(bDynamic && (dwMode & LM_HORZDOCK))
            SizeToolBar(pData,nCount,32767);
         else if(bDynamic && (dwMode & LM_VERTDOCK))
            SizeToolBar(pData,nCount,0);
         else if(bDynamic && (nLength != -1))
         {
            CRect rect; rect.SetRectEmpty();
            CalcInsideRect(rect,(dwMode & LM_HORZ));
            BOOL bVert = (dwMode & LM_LENGTHY);
            int nLen = nLength + (bVert ? rect.Height() : rect.Width());

            SizeToolBar(pData,nCount,nLen,bVert);
         }
         else if(bDynamic && (m_dwStyle & CBRS_FLOATING))
            SizeToolBar(pData,nCount,m_nMRUWidth);
         else
            SizeToolBar(pData,nCount,(dwMode & LM_HORZ) ? 32767 : 0);
      }

      sizeResult = CalcSize(pData,nCount);

      if(dwMode & LM_COMMIT)
      {
         _AFX_CONTROLPOS* pControl = NULL;
         int nControlCount = 0;
         BOOL bIsDelayed = m_bDelayedButtonLayout;
         m_bDelayedButtonLayout = FALSE;
         int i;

         for(i = 0; i < nCount; i++)
            if((pData[i].fsStyle & TBSTYLE_SEP) && (pData[i].idCommand != 0))
               nControlCount++;

         if(nControlCount > 0)
         {
            pControl = new _AFX_CONTROLPOS[nControlCount];
            int nControlAlloc=nControlCount;
            nControlCount = 0;

            for(i = 0; i < nCount && nControlCount < nControlAlloc; i++)
            {
               if((pData[i].fsStyle & TBSTYLE_SEP) && (pData[i].idCommand != 0))
               {
                  pControl[nControlCount].nIndex = i;
                  pControl[nControlCount].nID = pData[i].idCommand;

                  CRect rect;
                  GetItemRect(i,&rect);
                  ClientToScreen(&rect);
                  pControl[nControlCount].rectOldPos = rect;

                  nControlCount++;
               }
            }
         }

         if((m_dwStyle & CBRS_FLOATING) && (m_dwStyle & CBRS_SIZE_DYNAMIC))
            m_nMRUWidth = sizeResult.cx;
         for(i = 0; i < nCount; i++)
            _SetButton(i,&pData[i]);

         if(nControlCount > 0)
         {
            for(i = 0; i < nControlCount; i++)
            {
               CWnd* pWnd = GetDlgItem(pControl[i].nID);
               if(pWnd != NULL)
               {
                  CRect rect;
                  pWnd->GetWindowRect(&rect);
                  CPoint pt = rect.TopLeft() - pControl[i].rectOldPos.TopLeft();
                  GetItemRect(pControl[i].nIndex,&rect);
                  pt = rect.TopLeft() + pt;
                  pWnd->SetWindowPos(NULL,pt.x,pt.y,0,0,SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
               }
            }
            delete[] pControl;
         }
         m_bDelayedButtonLayout = bIsDelayed;
      }
      delete[] pData;
   }

   //BLOCK: Adjust Margins
   {
      CRect rect; rect.SetRectEmpty();
      CalcInsideRect(rect,(dwMode & LM_HORZ));
      sizeResult.cy -= rect.Height();
      sizeResult.cx -= rect.Width();

      CSize size = CControlBar::CalcFixedLayout((dwMode & LM_STRETCH),(dwMode & LM_HORZ));
      sizeResult.cx = max(sizeResult.cx,size.cx);
      sizeResult.cy = max(sizeResult.cy,size.cy);
   }
   return sizeResult;
}

CSize CRibbonBar::CalcFixedLayout(BOOL bStretch,BOOL bHorz)
{
   DWORD dwMode = bStretch ? LM_STRETCH : 0;
   dwMode |= bHorz ? LM_HORZ : 0;
   CSize size = CalcLayout(dwMode);
   size.cy = m_ulRibbonHeight;
   return size;
}

CSize CRibbonBar::CalcDynamicLayout(int nLength,DWORD dwMode)
{
   if((nLength == -1) && !(dwMode & LM_MRUWIDTH) && !(dwMode & LM_COMMIT) &&
      ((dwMode & LM_HORZDOCK) || (dwMode & LM_VERTDOCK)))
   {
      return CalcFixedLayout(dwMode & LM_STRETCH,dwMode & LM_HORZDOCK);
   }
   return CalcLayout(dwMode,nLength);
}

void CRibbonBar::GetButtonInfo(int nIndex,UINT& nID,UINT& nStyle,int& iImage) const
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));

   TBBUTTON button;
   _GetButton(nIndex,&button);
   nID = button.idCommand;
   nStyle = MAKELONG(button.fsStyle,button.fsState);
   iImage = button.iBitmap;
}

void CRibbonBar::SetButtonInfo(int nIndex,UINT nID,UINT nStyle,int iImage)
{
   ASSERT_VALID(this);

   TBBUTTON button;
   _GetButton(nIndex,&button);
   TBBUTTON save;

   Checked::memcpy_s(&save,sizeof(TBBUTTON),&button,sizeof(TBBUTTON));
   button.idCommand = nID;
   button.iBitmap = iImage;
   button.fsStyle = (BYTE)LOWORD(nStyle);
   button.fsState = (BYTE)HIWORD(nStyle);
   if(memcmp(&save,&button,sizeof(save)) != 0)
   {
      _SetButton(nIndex,&button);
      m_bDelayedButtonLayout = TRUE;
   }
}

INT_PTR CRibbonBar::OnToolHitTest(CPoint point,TOOLINFO* pTI) const
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));

   // check child windows first by calling CControlBar
   INT_PTR nHit = CControlBar::OnToolHitTest(point,pTI);
   if(nHit != -1)
      return nHit;

   // now hit test against CRibbonBar buttons
   CRibbonBar* pBar = (CRibbonBar*)this;
   int nButtons = (int)pBar->DefWindowProc(TB_BUTTONCOUNT,0,0);
   for(int i = 0; i < nButtons; i++)
   {
      CRect rect;
      TBBUTTON button;
      if(pBar->DefWindowProc(TB_GETITEMRECT,i,(LPARAM)&rect))
      {
         ++rect.bottom;
         ++rect.right;
         if(rect.PtInRect(point) &&
            pBar->DefWindowProc(TB_GETBUTTON,i,(LPARAM)&button) &&
            !(button.fsStyle & TBSTYLE_SEP))
         {
            UINT_PTR uHit = GetItemID(i);
            if(pTI != NULL)
            {
               pTI->hwnd = m_hWnd;
               pTI->rect = rect;
               pTI->uId = uHit;
               pTI->lpszText = LPSTR_TEXTCALLBACK;
            }
            // found matching rect, return the ID of the button
            return uHit != 0 ? static_cast<INT_PTR>(uHit) : static_cast<INT_PTR>(-1);
         }
      }
   }
   return -1;
}

BOOL CRibbonBar::SetButtonText(int nIndex,LPCTSTR lpszText)
{
   // attempt to lookup string index in map
   INT_PTR nString = -1;
   void* p;
   if(m_pStringMap != NULL && m_pStringMap->Lookup(lpszText,p))
      nString = (INT_PTR)p;

   // add new string if not already in map
   if(nString == -1)
   {
      // initialize map if necessary
      if(m_pStringMap == NULL)
         m_pStringMap = new CMapStringToPtr;

      // add new string to toolbar list
      CString strTemp(lpszText,AtlStrLen(lpszText) + 1);
      nString = (INT_PTR)DefWindowProc(TB_ADDSTRING,0,(LPARAM)(LPCTSTR)strTemp);
      if(nString == -1)
         return FALSE;

      // cache string away in string map
      m_pStringMap->SetAt(lpszText,(void*)nString);
      ASSERT(m_pStringMap->Lookup(lpszText,p));
   }

   // change the toolbar button description
   TBBUTTON button;
   _GetButton(nIndex,&button);
   button.iString = nString - STRING_REFRESH_OFFSET;
   _SetButton(nIndex,&button);

   return TRUE;
}

CString CRibbonBar::GetButtonText(int nIndex) const
{
   CString strResult;
   GetButtonText(nIndex,strResult);
   return strResult;
}

void CRibbonBar::GetButtonText(int nIndex,CString& rString) const
{
   if(m_pStringMap != NULL)
   {
      // get button information (need button.iString)
      TBBUTTON button;
      _GetButton(nIndex,&button);

      // look in map for matching iString
      POSITION pos = m_pStringMap->GetStartPosition();
      CString str; void* p;
      while(pos)
      {
         m_pStringMap->GetNextAssoc(pos,str,p);
         if((INT_PTR)p == button.iString)
         {
            rString = str;
            return;
         }
      }
   }
   rString.Empty();
}

/////////////////////////////////////////////////////////////////////////////
// CRibbonBar message handlers

BEGIN_MESSAGE_MAP(CRibbonBar,CControlBar)
   ON_WM_NCHITTEST()
   ON_WM_NCPAINT()
   ON_WM_PAINT()
   ON_WM_ERASEBKGND()
   ON_WM_NCCALCSIZE()
   ON_WM_WINDOWPOSCHANGING()
   ON_WM_NCCREATE()
   ON_MESSAGE(TB_SETBITMAPSIZE,&CRibbonBar::OnSetBitmapSize)
   ON_MESSAGE(TB_SETBUTTONSIZE,&CRibbonBar::OnSetButtonSize)
   ON_MESSAGE(WM_SETTINGCHANGE,&CRibbonBar::OnPreserveZeroBorderHelper)
   ON_MESSAGE(WM_SETFONT,&CRibbonBar::OnPreserveZeroBorderHelper)
   ON_MESSAGE(TB_SETDISABLEDIMAGELIST,&CRibbonBar::OnPreserveSizingPolicyHelper)
   ON_MESSAGE(TB_SETHOTIMAGELIST,&CRibbonBar::OnPreserveSizingPolicyHelper)
   ON_MESSAGE(TB_SETIMAGELIST,&CRibbonBar::OnPreserveSizingPolicyHelper)
   ON_WM_SYSCOLORCHANGE()
   ON_WM_LBUTTONDOWN()
   ON_WM_MOUSEMOVE()
   ON_WM_MOUSELEAVE()
   ON_WM_LBUTTONUP()
   ON_WM_TIMER()
   ON_WM_SIZE()
   ON_WM_KILLFOCUS()
   ON_COMMAND(ID_TAB_LEFT_ARROW, &CRibbonBar::OnTabLeftArrow)
   ON_COMMAND(ID_TAB_RIGHT_ARROW,&CRibbonBar::OnTabRightArrow)
   ON_COMMAND(ID_RIBBON_LEFT_ARROW,&CRibbonBar::OnRibbonLeftArrow)
   ON_COMMAND(ID_RIBBON_RIGHT_ARROW,&CRibbonBar::OnRibbonRightArrow)

END_MESSAGE_MAP()

BOOL CRibbonBar::OnEraseBkgnd(CDC*)
{
   return TRUE;
}

LRESULT CRibbonBar::OnNcHitTest(CPoint)
{
   return HTCLIENT;
}

void CRibbonBar::OnNcCalcSize(BOOL /*bCalcValidRects*/,NCCALCSIZE_PARAMS* lpncsp)
{
   // calculate border space (will add to top/bottom, subtract from right/bottom)
   CRect rect; rect.SetRectEmpty();
   //BOOL bHorz = (m_dwStyle & CBRS_ORIENT_HORZ) != 0;
   //CControlBar::CalcInsideRect(rect,bHorz);

   // adjust non-client area for border space
   lpncsp->rgrc[0].left += rect.left;
   lpncsp->rgrc[0].top += rect.top;
   // previous versions of COMCTL32.DLL had a built-in 2 pixel border
   lpncsp->rgrc[0].right += rect.right;
   lpncsp->rgrc[0].bottom += rect.bottom;
}

void CRibbonBar::OnBarStyleChange(DWORD dwOldStyle,DWORD dwNewStyle)
{
   // a dynamically resizeable toolbar can not have the CBRS_FLOAT_MULTI
   ASSERT(!((dwNewStyle & CBRS_SIZE_DYNAMIC) &&
      (m_dwDockStyle & CBRS_FLOAT_MULTI)));

   // a toolbar can not be both dynamic and fixed in size
   ASSERT(!((dwNewStyle & CBRS_SIZE_FIXED) &&
      (dwNewStyle & CBRS_SIZE_DYNAMIC)));

   // CBRS_SIZE_DYNAMIC can not be disabled once it has been enabled
   ASSERT(((dwOldStyle & CBRS_SIZE_DYNAMIC) == 0) ||
      ((dwNewStyle & CBRS_SIZE_DYNAMIC) != 0));

   if(m_hWnd != NULL &&
      ((dwOldStyle & CBRS_BORDER_ANY) != (dwNewStyle & CBRS_BORDER_ANY)))
   {
      // recalc non-client area when border styles change
      SetWindowPos(NULL,0,0,0,0,
         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_DRAWFRAME);
   }
   m_bDelayedButtonLayout = TRUE;
}

void CRibbonBar::OnNcPaint()
{
   //EraseNonClient();
}

void CRibbonBar::OnWindowPosChanging(LPWINDOWPOS lpWndPos)
{
   // not necessary to invalidate the borders
   DWORD dwStyle = m_dwStyle;
   m_dwStyle &= ~(CBRS_BORDER_ANY);
   CControlBar::OnWindowPosChanging(lpWndPos);
   m_dwStyle = dwStyle;

   // If we can resize while floating
   if(dwStyle & CBRS_SIZE_DYNAMIC)
   {
      // And we are resizing
      if(lpWndPos->flags & SWP_NOSIZE)
         return;

      // Then redraw the buttons
      Invalidate();
   }
}

void CRibbonBar::get_tab_rect(LPRECT lprect,int i)
{

   CDC dc;

   dc.CreateCompatibleDC(NULL);

   Graphics g(dc.GetSafeHdc());

   CRect rectClient;

   GetClientRect(rectClient);

   CRect r = rectClient;

   int j = 0;
   int x = 0;
   int iTabHeight = 0;
   CArray<RectF> boxa;
   boxa.SetSize(m_taba.GetCount());
   int iTotalW = 0;
   int iFullTotalW = 0;
   CArray < int > ia;
   for(;j < m_taba.GetCount(); j++)
   {
      g.MeasureString(m_taba[j].m_wstrName,m_taba[j].m_wstrName.GetLength(),&m_font2,PointF(0,0),&boxa[j]);
      float f = boxa[j].Width;
      boxa[j].Width += 2;
      f = boxa[j].Width;
      boxa[j].Width = ceil(f);
      f = boxa[j].Width;
      ia.Add(f);
      iTotalW += ceil(boxa[j].Width);
      iFullTotalW += ceil(boxa[j].Width);
      iFullTotalW +=m_rTabBorder.left + m_rTabBorder.right;
      iTabHeight = max(ceil(boxa[j].Height),iTabHeight);
   }

   for(int i = 0; i < GetCount(); i++)
   {

      for(int j = i + 1; j < GetCount(); j++)
      {

         if(ia.ElementAt(i) > ia.ElementAt(j))
         {

            int k = ia.ElementAt(i);

            ia.ElementAt(i) = ia.ElementAt(j);

            ia.ElementAt(j) = k;

         }

      }

   }

   for(int i = 0; i < ia.GetUpperBound();)
   {

      if(ia[i] == ia[i + 1])
      {

         ia.RemoveAt(i);

      }
      else
      {

         i++;

      }

   }
   
   int iMax3 = ia.ElementAt(min(2,ia.GetUpperBound()));
   
   int iMinW = min(iMax3 * m_taba.GetCount(), iTotalW);
   
   iMax3 = iMinW / m_taba.GetCount();

   m_iMargin = (m_rTabBorder.left + m_rTabBorder.right) / 2;

   int iFixed = 32 + 32 + 10 + 10; // two right buttons plus two scroll bars

   bool bScroll = false;

   if(rectClient.Width() - iFixed < iFullTotalW)
   {

      m_iMargin = (rectClient.Width() - iFixed - iTotalW) / (m_taba.GetCount() * 2);

   }

   if(m_iMargin <= 1)
   {

      m_iMargin = (rectClient.Width() - iFixed - iMinW) / (m_taba.GetCount() * 2);

      if(m_iMargin <= 1)
      {

         m_iTabScrollMax = iFixed + iMinW - rectClient.Width() -10;

         bScroll = true;

      }

   }
   else
   {

      iMax3 = 0;

   }

   if(bScroll)
   {

      if(rectClient.Width() < 300)
      {

         m_iTabScroll = 0;

         m_iMargin = 0;

         if(i == 0)
         {

            m_buttonTabLeft.ShowWindow(SW_HIDE);

            m_buttonTabRight.ShowWindow(SW_HIDE);

         }

      }
      else
      {
         
         m_iMargin = 1;

         if(i == 0)
         {

            if(m_iTabScroll <= 0)
            {

               m_buttonTabLeft.ShowWindow(SW_HIDE);
               
            }
            else
            {

               x += 10;

               m_buttonTabLeft.SetWindowPos(&CWnd::wndTop,0,0,10,m_iTabHeight,SWP_SHOWWINDOW);

            }

            if(m_iTabScroll >= m_iTabScrollMax)
            {

               m_buttonTabRight.ShowWindow(SW_HIDE);

            }
            else
            {

               m_buttonTabRight.SetWindowPos(&CWnd::wndTop,rectClient.right - 10,0,10,m_iTabHeight,SWP_SHOWWINDOW);

            }

         }

      }

   }
   else
   {
      
      m_iTabScroll = 0;

      if(i == 0)
      {

         m_buttonTabLeft.ShowWindow(SW_HIDE);

         m_buttonTabRight.ShowWindow(SW_HIDE);

      }

   }

   x -=m_iTabScroll;
   for(j=0;j < m_taba.GetCount(); j++)
   {
      if(j < i)
      {
         if(iMax3 > 0)
         {

            x += min(iMax3,ceil(boxa[j].Width)) ;
            x +=m_iMargin * 2;

         }
         else
         {
            x += ceil(boxa[j].Width);
            x +=m_iMargin * 2;
         }
      }
   }
   iTabHeight += m_rTabBorder.top + m_rTabBorder.bottom;
   r.bottom = r.top + iTabHeight;
   m_iTabHeight = iTabHeight;
   if(i < m_taba.GetCount())
   {
      lprect->left = x;
      if(iMax3 > 0)
      {
         lprect->right = x + min(iMax3,ceil(boxa[i].Width)) + m_iMargin * 2;
      }
      else
      {
         lprect->right = x + ceil(boxa[i].Width) + m_iMargin * 2;
      }
      lprect->top = r.top;
      lprect->bottom = r.bottom;
   }

   if(i >= m_taba.GetCount())
   {
      lprect->top = 4;
      lprect->bottom = lprect->top + 24;
      lprect->left = rectClient.right - 24 - 8;
      lprect->right = lprect->left + 24;
      if(i == m_taba.GetCount() + 1)
      {
         return;
      }
      lprect->right = lprect->left - 8;
      lprect->left = lprect->right - 24;
      if(i == m_taba.GetCount())
      {
         return;
      }
   }

}

bool CRibbonBar::hit_test(CPoint pt,int & iGroup,int & iItem)
{
   CRect rItem;
   CRect rImage;
   CRect rText;
   for(int i = 0; i < m_taba[m_iTabSel].m_groupa.GetCount(); i++)
   {

      CGroup & group = m_taba[m_iTabSel].m_groupa[i];

      for(int j = 0; j < group.m_itema.GetCount(); j++)
      {

         get_item_rect(rItem,rImage,rText,i,j);

         if(rItem.PtInRect(pt))
         {

            iGroup = i;
            iItem = j;

            return true;

         }



      }

   }
   iGroup = -1;
   iItem = -1;
   return false;
}

void CRibbonBar::get_item_rect(LPRECT lprectItem,LPRECT lprectImage,LPRECT lprectText,int iGroup,int iItem)
{

   *lprectItem = m_taba[m_iTabSel].m_groupa[iGroup].m_itema[iItem].m_rItem;
   *lprectImage = m_taba[m_iTabSel].m_groupa[iGroup].m_itema[iItem].m_rImage;
   *lprectText = m_taba[m_iTabSel].m_groupa[iGroup].m_itema[iItem].m_rText;

}


void CRibbonBar::calc_item_rect(Gdiplus::Graphics & g,LPRECT lprectItem,LPRECT lprectImage,LPRECT lprectText,int iGroup,int iItem)
{

   CRect rectClient;

   GetClientRect(rectClient);

   Gdiplus::Rect r2;

   m_xStart =rectClient.left + 10;

   int x = m_xStart;

   int iRow = 0;

   int y = 0;

   CRect rItem;
   CRect rImage;
   CRect rText;

   int iRowH;

   int iLastMaxW;



   Gdiplus::RectF box;

   for(int i = 0; i < m_taba[m_iTabSel].m_groupa.GetCount(); i++)
   {

      CGroup & group = m_taba[m_iTabSel].m_groupa[i];

      iRow = 0;

      iLastMaxW = 0;

      y = 0;

      for(int j = 0; j < group.m_itema.GetCount(); j++)
      {

         CItem & item = group.m_itema[j];

         if(item.m_pimageLarge != NULL && item.m_pimageSmall == NULL)
         {

            int iW = item.m_pimageLarge->GetWidth();

            int iH = item.m_pimageLarge->GetHeight();

            if(iW <= 20 && iH <= 20)
            {

               item.m_pimageSmall = item.m_pimageLarge->Clone();

            }
            else
            {
#undef new
               item.m_pimageSmall = new Gdiplus::Bitmap(20,20,&g);

               Gdiplus::Graphics gSmall(item.m_pimageSmall);

               gSmall.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

               gSmall.DrawImage(item.m_pimageLarge,Gdiplus::Rect(0,0,20,20));

            }

         }

         if(i < m_iGroupSmall)
         {

            item.m_pimage1 = item.m_pimageLarge;

         }
         else
         {

            item.m_pimage1 = item.m_pimageSmall;

         }
            
         if(item.m_x < 0)
         {

            int iW = item.m_pimage1 == NULL ? 0 : item.m_pimage1->GetWidth();

            int iH = item.m_pimage1 == NULL ? 0 : item.m_pimage1->GetHeight();

            int iFullH = max(iH,32) + m_rIconBorder.bottom + m_rIconBorder.top + m_rIconBorder.bottom + m_fMaxTextHeight;

            iRowH = iFullH;

            if(iW <= 20 && iH <= 20)
            {

               iRowH /= group.m_iMaxRowCount;

            }
            else
            {

               iRow = 0;

               y = 0;

               iLastMaxW = 0;

            }

            int w = iW + m_rIconBorder.left + m_rIconBorder.right;

            if(!item.m_wstrName.IsEmpty() && i < m_iGroupSmall)
            {

               CStringW wstr;

               wstr = item.get_draw_name();

               Gdiplus::RectF r2;
               rItem.left = x + m_rIconMargin.left;
               rItem.right = rItem.left + w ;
               rItem.top = y + m_iTabHeight + m_rIconMargin.top;
               rItem.bottom = rItem.top + iRowH;
               rImage.left = rItem.left + m_rIconBorder.left;
               rImage.right = rImage.right - m_rIconBorder.right;
               rImage.top = rImage.top + m_rIconBorder.top;
               rImage.bottom = rImage.top + iH;
               rText.left = rItem.left;
               rText.right = rItem.right;
               rText.top = rItem.top + m_rIconBorder.top + iH + m_rIconBorder.bottom;
               rText.bottom = rItem.bottom - m_rIconBorder.bottom;

               copy(r2,rText);

               int h = -1;

               float fWidth = -1.f;

               for(int i = 0; i < 35; i+= 5)
               {

                  Gdiplus::Status s = g.MeasureString(wstr,wstr.GetLength(),&m_font2,r2,&m_format,&box);

                  if(fWidth < 0.f)
                  {
                     fWidth = box.Width;
                  }
                  //else if(fWidth >= box.Width)
                  //{

                  //   break;

                  //}


                  if(h < 0)
                  {

                     h  = box.Height;

                  }
                  else if(item.get_prefer_two_lines() && (int)box.Height < (int)h)
                  {

                     break;

                  }

                  if(ceil(box.Width) > w)
                  {

                     w = ceil(box.Width);

                  }

                  r2.Width += 5.f;

               }

            }
            else
            {

               box.Height = 0;

            }

            iLastMaxW = max(iLastMaxW,w + m_rIconMargin.left + m_rIconMargin.right);


            if(i == iGroup && iItem == j)
            {

               lprectItem->left = x + m_rIconMargin.left;
               lprectItem->right = lprectItem->left + w;
               lprectItem->top = y + m_iTabHeight + m_rIconMargin.top;
               lprectItem->bottom = lprectItem->top + iRowH;
               lprectImage->left = (lprectItem->left + lprectItem->right) / 2 - iW / 2;
               lprectImage->right = lprectImage->left + iW;
               lprectImage->top = lprectItem->top + m_rIconBorder.top;
               lprectImage->bottom = lprectImage->top + iH;
               lprectText->left = lprectItem->left;
               lprectText->right = lprectItem->right;
               lprectText->top = lprectItem->top + m_rIconBorder.top + iH + m_rIconBorder.bottom;
               lprectText->bottom = lprectItem->bottom - m_rIconBorder.bottom;

            }


            if(iW <= 20 && iH <= 20)
            {

               iRow++;

               y += iH + m_rIconBorder.top + m_rIconBorder.bottom + 4;

               if(iRow >= group.m_iMaxRowCount || iItem == group.m_itema.GetUpperBound())
               {

                  iRow = 0;

                  y = 0;

                  x += iLastMaxW;

                  iLastMaxW = 0;

               }

            }
            else
            {

               x += iLastMaxW;

               iLastMaxW = 0;

            }

            item.m_x = x;

            item.m_y = y;

            item.m_iLastMaxW = iLastMaxW;

            item.m_iRow = iRow;

            if(i == iGroup && iItem == j)
            {

               if(iItem == group.m_itema.GetUpperBound())
               {

                  group.m_iSep = x + 10;

               }

               return;

            }
         }
         else
         {


            if(i == iGroup && iItem == j)
            {

               *lprectItem = item.m_rItem;
               *lprectImage = item.m_rImage;
               *lprectText = item.m_rText;

               if(iItem == group.m_itema.GetUpperBound())
               {

                  group.m_iSep = x + 10;

               }

               return;

            }

            x = item.m_x;

            y = item.m_y;

            iLastMaxW = item.m_iLastMaxW;

            iRow = item.m_iRow;

         }

      }

      x += 10;

      group.m_iSep = x;

      x += 10;

   }

}


void CRibbonBar::OnPaint()
{

   CPaintDC dcPaint(this);

   if(m_bNeedLayout)
   {

      m_bNeedLayout = false;

      layout();

   }

   if(&m_font2 == NULL)
   {

      return;

   }

   CMemDC dcMem(dcPaint,this);

   CDC &dc = dcMem.GetDC();

   CRect rectClient;

   GetClientRect(rectClient);

   CRect r = rectClient;

   CRect rTab;

   get_tab_rect(rTab,0);

   r.bottom = m_iTabHeight;

   dc.FillSolidRect(r,RGB(0,0,0));

   dc.SetBkMode(TRANSPARENT);

   if(m_iMargin <= 0)
   {

      return;

   }

   Gdiplus::Graphics g(dc.GetSafeHdc());

   Gdiplus::SolidBrush brTabTextSel(Gdiplus::Color(255,0,0,0));

   Gdiplus::SolidBrush brTabBackSel(Gdiplus::Color(255,250,250,250));

   Gdiplus::SolidBrush brTabText(Gdiplus::Color(255,250,250,250));

   Gdiplus::SolidBrush brTabBack(Gdiplus::Color(255,0,0,0));

   Gdiplus::RectF r2;

   CStringW wstr;

   g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
   g.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
   g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
   g.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
   g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);


   for(int i = 0; i < m_taba.GetCount(); i++)
   {

      get_tab_rect(rTab,i);

      copy(r2,rTab);

      wstr = m_taba[i].m_wstrName;

      if(i == m_iTabSel)
      {

         g.FillRectangle(&brTabBackSel,r2);
         g.DrawString(wstr,wstr.GetLength(),&m_font2,r2,&m_formatTab,&brTabTextSel);

      }
      else
      {

         g.FillRectangle(&brTabBack,r2);
         g.DrawString(wstr,wstr.GetLength(),&m_font2,r2,&m_formatTab,&brTabText);

      }

   }

   get_tab_rect(rTab,m_taba.GetCount());

   {
      CRect rBack(rectClient);
      rBack.bottom = m_iTabHeight;
      rBack.left = rTab.left;
      Gdiplus::Rect r;
      copy(r,rBack);
      Gdiplus ::SolidBrush br(Gdiplus::Color(255, 0, 0, 0));
      
      g.FillRectangle(&br,r);
   }

   Bitmap image(m_hiconHelp);

   get_tab_rect(rTab,m_taba.GetCount() + 1);

   copy(r2,rTab);

   g.DrawImage(&image,r2);

   if(m_ulRibbonHeight == m_iFullHeight)
   {
    
      Bitmap image(m_hiconCollapse);

      get_tab_rect(rTab,m_taba.GetCount());

      copy(r2,rTab);

      g.DrawImage(&image,r2);

   }
   else
   {

      Bitmap image(m_hiconExpand);

      get_tab_rect(rTab,m_taba.GetCount());

      copy(r2,rTab);

      g.DrawImage(&image,r2);

   }

   r2.X = rectClient.left;
   r2.Y = m_iTabHeight;
   r2.Width = rectClient.Width();
   r2.Height = rectClient.bottom - r2.Y;
   Gdiplus::SolidBrush brBk(Gdiplus::Color(255,250,250,250));
   g.FillRectangle(&brBk,r2);
   Gdiplus::Pen penBorder(Gdiplus::Color(255,0,0,0));
   g.DrawLine(&penBorder, r2.GetLeft(),r2.GetBottom() - 1,r2.GetRight(),r2.GetBottom() - 1);

   CRect rItem;
   CRect rImage;
   CRect rText;

   if(!m_bExpanded && !m_bFocus)
   {

      return;

   }

   g.TranslateTransform(-m_iScroll,0);


   Gdiplus::SolidBrush brText(Gdiplus::Color(255,0,0,0));

   Gdiplus::SolidBrush brChecked(Gdiplus::Color(255,190,190,190));

   Gdiplus::Pen penChecked(Gdiplus::Color(255,190,190,190));

   Gdiplus::SolidBrush brCheckedHover(Gdiplus::Color(255,190,190,190));

   Gdiplus::Pen penCheckedHover(Gdiplus::Color(255,120,120,120));

   Gdiplus::SolidBrush brHover(Gdiplus::Color(15,40,120,230));

   Gdiplus::Pen penHover(Gdiplus::Color(80,40,120,230));

   Gdiplus::SolidBrush brPress(Gdiplus::Color(60,40,120,230));

   Gdiplus::Pen penPress(Gdiplus::Color(180,40,120,230));

   Gdiplus::Pen penSep(Gdiplus::Color(80,128,128,128));

   for(int i = 0; i < m_taba[m_iTabSel].m_groupa.GetCount(); i++)
   {

      CGroup & group = m_taba[m_iTabSel].m_groupa[i];

      for(int j = 0; j < group.m_itema.GetCount(); j++)
      {
         CItem & item = group.m_itema[j];

         get_item_rect(rItem,rImage,rText,i,j);

         if(item.m_bEnabled && item.m_bChecked && (item.m_bAlwaysEnabledPopup || !item.m_bPopup))
         {
            Gdiplus::Rect r;

            copy(r,rItem);


            if(m_iGroupHover == i && m_iItemHover == j)
            {

               g.FillRectangle(&brCheckedHover, r);

               g.DrawRectangle(&penCheckedHover,r);

            }
            else
            {
               
               g.FillRectangle(&brChecked,r);

               g.DrawRectangle(&penChecked,r);

            }

         }
         else if(m_iGroupHover == i && m_iItemHover == j && (item.m_bEnabled || item.m_bAlwaysEnabledPopup))
         {

            Gdiplus::Rect r;

            copy(r,rItem);

            if(m_iGroupLButtonDown == i && m_iItemLButtonDown == j)
            {

               g.FillRectangle(&brPress,r);

               g.DrawRectangle(&penPress,r);

            }
            else
            {

               g.FillRectangle(&brHover,r);

               g.DrawRectangle(&penHover,r);

            }

         }

         Gdiplus::Rect r;

         copy(r,rImage);

         if(!item.m_bEnabled && !item.m_bAlwaysEnabledPopup)
         {
            Gdiplus::ColorMatrix m={
               0.33,0.33,0.33,0.0,0.0,
               0.33,0.33,0.33,0.0,0.0,
               0.33,0.33,0.33,0.0,0.0,
               0.0,0.0,0.0,1.0,0.0,
               0.0,0.0,0.0,0.0,1.0};

            

            Gdiplus::ImageAttributes attr;

            attr.SetColorMatrix(&m);
            g.DrawImage(item.m_pimage1,r,0, 0, item.m_pimage1->GetWidth(),
               item.m_pimage1->GetHeight(), Gdiplus::UnitPixel, &attr);

         }
         else
         {
            g.DrawImage(item.m_pimage1,r);
         }

         if(i < m_iGroupSmall)
         {

            CStringW wstr;

            wstr = item.get_draw_name();

            Gdiplus::RectF r2;

            copy(r2,rText);

            g.DrawString(wstr,wstr.GetLength(),&m_font2,r2,&m_format,&brText);

         }

      }

      if(!group.m_wstrName.IsEmpty() && i < m_iGroupSmall)
      {

         if(i > 0)
         {

            rText.left = m_taba[m_iTabSel].m_groupa[i - 1].m_iSep;

         }
         else
         {

            rText.left = m_xStart;

         }

         rText.right = group.m_iSep;

         rText.top = m_ulRibbonHeight - 32;

         rText.bottom = m_ulRibbonHeight;

         Gdiplus::RectF r2;

         copy(r2,rText);

         CStringW wstr;

         wstr = group.m_wstrName;

         Gdiplus::SolidBrush brGroup(Gdiplus::Color(255,128,128,128));

         Gdiplus::RectF box;

         Gdiplus::Status s = g.MeasureString(wstr,wstr.GetLength(),&m_font2,r2,&m_format,&box);

         rText.top = m_ulRibbonHeight - box.Height;

         rText.bottom = m_ulRibbonHeight;

         copy(r2,rText);

         g.DrawString(wstr,wstr.GetLength(),&m_font2,r2,&m_format,&brGroup);

      }

      if(i < m_taba[m_iTabSel].m_groupa.GetUpperBound())
      {

         g.DrawLine(&penSep,Gdiplus::Point(group.m_iSep,m_iTabHeight + 16),Gdiplus::Point(group.m_iSep,m_ulRibbonHeight - 16));

      }

   }


}

LRESULT CRibbonBar::OnSetButtonSize(WPARAM,LPARAM lParam)
{
   return OnSetSizeHelper(m_sizeButton,lParam);
}

LRESULT CRibbonBar::OnSetBitmapSize(WPARAM,LPARAM lParam)
{
   return OnSetSizeHelper(m_sizeImage,lParam);
}

LRESULT CRibbonBar::OnSetSizeHelper(CSize& size,LPARAM lParam)
{
   //  The IE4 version of COMCTL32.DLL supports a zero border, but 
   //	only if TBSTYLE_TRANSPARENT is on during the the TB_SETBITMAPSIZE
   //	and/or TB_SETBUTTONSIZE messages.  In order to enable this feature
   //	all the time (so we get consistent border behavior, dependent only
   //	on the version of COMCTL32.DLL) we turn on TBSTYLE_TRANSPARENT 
   //	whenever these messages go through.  It would be nice that in a
   //	future version, the system toolbar would just allow you to set
   //	the top and left borders to anything you please.

   //  In comctl32 version 6.00 when using XP Look combined with TBSTYLE_EX_DRAWDDARROWS 
   //	style minimal sizing policy was changed to be
   //			button.height >= image.height + 13
   //			button.width >= image.width + 16
   //	this force buttons to be bigger then usual
   //	To override this behavior we should remove TBSTYLE_EX_DRAWDDARROWS prior to sizing operations

   BOOL bModify = FALSE;
   DWORD dwStyle = 0;
   DWORD dwStyleEx = 0;
   //if(_afxComCtlVersion >= VERSION_IE4)
   //{
   //   dwStyle = GetStyle();
   //   bModify = ModifyStyle(0,TBSTYLE_TRANSPARENT | TBSTYLE_FLAT);
   //   if((_afxComCtlVersion >= VERSION_6) && ::IsWindow(GetToolBarCtrl()))
   //   {
   //      dwStyleEx = GetToolBarCtrl().SetExtendedStyle(GetToolBarCtrl().GetExtendedStyle() &~ TBSTYLE_EX_DRAWDDARROWS);
   //   }
   //}

   LRESULT lResult = Default();
   if(lResult)
   {
      size = DWORD(lParam);
   }

   if(bModify)
   {
      SetWindowLong(m_hWnd,GWL_STYLE,dwStyle);
   }

   return lResult;
}

LRESULT CRibbonBar::OnPreserveZeroBorderHelper(WPARAM,LPARAM)
{
   BOOL bModify = FALSE;
   DWORD dwStyle = 0;
   DWORD dwStyleEx = 0;
   //if(_afxComCtlVersion >= VERSION_IE4)
   //{
   //   dwStyle = GetStyle();
   //   bModify = ModifyStyle(0,TBSTYLE_TRANSPARENT | TBSTYLE_FLAT);
   //   if((_afxComCtlVersion >= VERSION_6) && ::IsWindow(GetToolBarCtrl()))
   //   {
   //      dwStyleEx = GetToolBarCtrl().SetExtendedStyle(GetToolBarCtrl().GetExtendedStyle() &~ TBSTYLE_EX_DRAWDDARROWS);
   //   }
   //}

   LRESULT lResult = Default();

   if(bModify)
   {
      SetWindowLong(m_hWnd,GWL_STYLE,dwStyle);
   }

   return lResult;
}

LRESULT CRibbonBar::OnPreserveSizingPolicyHelper(WPARAM,LPARAM)
{
   //	NOTE: in comctl32 version 6.00 when using XP Look combined with TBSTYLE_EX_DRAWDDARROWS 
   //	style minimal sizing policy was changed to be
   //			button.height >= image.height + 13
   //			button.width >= image.width + 16
   //	this force buttons to be bigger then usual
   //	To override this behavior we should remove TBSTYLE_DROPDOWN from all buttons prior to setimagelist 
   //  operations

   UINT uiButtonNum = 0;
   DWORD* pdwStyles = NULL;
   //if(_afxComCtlVersion >= VERSION_6)
   //{
   //   uiButtonNum = GetToolBarCtrl().GetButtonCount();
   //   pdwStyles = new DWORD[uiButtonNum];
   //   ASSERT(pdwStyles);
   //   for(UINT i =0; i < uiButtonNum; i++)
   //   {
   //      pdwStyles[i] = GetButtonStyle(i);
   //      SetButtonStyle(i,pdwStyles[i] & ~TBSTYLE_DROPDOWN);
   //   }
   //}

   LRESULT lResult = Default();

   if(pdwStyles)
   {
      for(UINT i =0; i < uiButtonNum; i++)
      {
         SetButtonStyle(i,pdwStyles[i]);
      }
      delete[] pdwStyles;
   }

   return lResult;
}

void CRibbonBar::OnSysColorChange()
{
   // re-color bitmap for toolbar
   if(m_hInstImageWell != NULL && m_hbmImageWell != NULL)
   {
      HBITMAP hbmNew;
      hbmNew = AfxLoadSysColorBitmap(m_hInstImageWell,m_hRsrcImageWell);
      if(hbmNew != NULL)
         AddReplaceBitmap(hbmNew);
   }
}

// Active Accessibility support
HRESULT CRibbonBar::get_accName(VARIANT varChild,BSTR *pszName)
{
   if(varChild.lVal == CHILDID_SELF)
   {
      CString strText;
      GetWindowText(strText);

      *pszName = strText.AllocSysString();
      return S_OK;
   }
   else if(varChild.lVal != 0)
   {
      CString szFullText;
      CString strTipText;

      UINT nID;
      UINT nStyle;
      int nImage;
      GetButtonInfo(varChild.lVal - 1,nID,nStyle,nImage);
      // don't handle the message if no string resource found
      if(szFullText.LoadString((UINT)nID) != 0)
      {
         // this is the command id, not the button index
         AfxExtractSubString(strTipText,szFullText,1,'\n');
         *pszName = strTipText.AllocSysString();
         return S_OK;
      }
   }
   return CControlBar::get_accName(varChild,pszName);
}

/////////////////////////////////////////////////////////////////////////////
// CRibbonBar idle update through CRibbonCmdUI class

class CRibbonCmdUI: public CCmdUI        // class private to this file !
{
public: // re-implementations only
   int m_iGroup;
   virtual void Enable(BOOL bOn);
   virtual void SetCheck(int nCheck);
   virtual void SetText(LPCTSTR lpszText);
};

void CRibbonCmdUI::Enable(BOOL bOn)
{
   m_bEnableChanged = TRUE;
   CRibbonBar* pToolBar = (CRibbonBar*)m_pOther;
   ASSERT(pToolBar != NULL);
   ASSERT_KINDOF(CRibbonBar,pToolBar);
   ASSERT(m_iGroup < pToolBar->m_taba[pToolBar->m_iTabSel].m_groupa.GetCount());
   ASSERT(m_nIndex < pToolBar->m_taba[pToolBar->m_iTabSel].m_groupa[m_iGroup].m_itema.GetCount());

   CRibbonBar::CItem & item = pToolBar->m_taba[pToolBar->m_iTabSel].m_groupa[m_iGroup].m_itema[m_nIndex];

   if(!bOn  ^ !item.m_bEnabled)
   {
      item.m_bEnabled = bOn != FALSE;

      pToolBar->Invalidate();

   }
   

}

void CRibbonCmdUI::SetCheck(int nCheck)
{
   ASSERT(nCheck >= 0 && nCheck <= 2); // 0=>off, 1=>on, 2=>indeterminate
   CRibbonBar* pToolBar = (CRibbonBar*)m_pOther;
   ASSERT(pToolBar != NULL);
   ASSERT_KINDOF(CRibbonBar,pToolBar);
   ASSERT(m_nIndex < m_nIndexMax);

   CRibbonBar::CItem & item = pToolBar->m_taba[pToolBar->m_iTabSel].m_groupa[m_iGroup].m_itema[m_nIndex];


   if(!nCheck  ^ !item.m_bChecked)
   {
      item.m_bChecked = nCheck != 0;

      pToolBar->Invalidate();

   }

}

void CRibbonCmdUI::SetText(LPCTSTR)
{
   // ignore it
}

void CRibbonBar::OnUpdateCmdUI(CFrameWnd* pTarget,BOOL bDisableIfNoHndler)
{
   CRibbonCmdUI state;
   state.m_pOther = this;


   for(int i = 0; i < m_taba[m_iTabSel].m_groupa.GetCount(); i++)
   {

      CGroup & group = m_taba[m_iTabSel].m_groupa[i];
      state.m_nIndexMax = group.m_itema.GetCount();
      state.m_iGroup = i;
      for(int j = 0; j < group.m_itema.GetCount(); j++)
      {
         CItem & item = group.m_itema[j];

         state.m_nID = item.m_iId;
         state.m_nIndex = j;


         if(item.m_iId > 0)
         {
            // allow reflections
            if(CWnd::OnCmdMsg(0,
               MAKELONG(CN_UPDATE_COMMAND_UI & 0xffff,WM_COMMAND + WM_REFLECT_BASE),
               &state,NULL))
               continue;

            // allow the toolbar itself to have update handlers
            if(CWnd::OnCmdMsg(state.m_nID,CN_UPDATE_COMMAND_UI,&state,NULL))
               continue;

            // allow the owner to process the update
            state.DoUpdate(pTarget,bDisableIfNoHndler);
         }
      }

   }

   // update the dialog controls added to the toolbar
   //UpdateDialogControls(pTarget,bDisableIfNoHndler);
}

/////////////////////////////////////////////////////////////////////////////
// CRibbonBar diagnostics

#ifdef _DEBUG
void CRibbonBar::AssertValid() const
{
   // Note: CControlBar::AssertValid is not called because it checks for
   //  m_nCount and m_pData to be in sync, which they are not in CRibbonBar.

   ASSERT(m_hbmImageWell == NULL || ::GetObjectType(m_hbmImageWell) == OBJ_BITMAP);

   if(m_hInstImageWell != NULL && m_hbmImageWell != NULL)
      ASSERT(m_hRsrcImageWell != NULL);
}

void CRibbonBar::Dump(CDumpContext& dc) const
{
   CControlBar::Dump(dc);

   dc << "m_hbmImageWell = " << (void*)m_hbmImageWell;
   dc << "\nm_hInstImageWell = " << (void*)m_hInstImageWell;
   dc << "\nm_hRsrcImageWell = " << (void*)m_hRsrcImageWell;
   dc << "\nm_sizeButton = " << m_sizeButton;
   dc << "\nm_sizeImage = " << m_sizeImage;

   if(dc.GetDepth() > 0)
   {
      CRibbonBar* pBar = (CRibbonBar*)this;
      int nCount = int(pBar->DefWindowProc(TB_BUTTONCOUNT,0,0));
      for(int i = 0; i < nCount; i++)
      {
         TBBUTTON button;
         _GetButton(i,&button);
         dc << "\ntoolbar button[" << i << "] = {";
         dc << "\n\tnID = " << button.idCommand;
         dc << "\n\tnStyle = " << MAKELONG(button.fsStyle,button.fsState);
         if(button.fsStyle & TBSTYLE_SEP)
            dc << "\n\tiImage (separator width) = " << button.iBitmap;
         else
            dc << "\n\tiImage (bitmap image index) = " << button.iBitmap;
         dc << "\n}";
      }
   }

   dc << "\n";
}
#endif


IMPLEMENT_DYNAMIC(CRibbonBar,CControlBar)

/////////////////////////////////////////////////////////////////////////////


void CRibbonBar::OnLButtonDown(UINT nFlags,CPoint point)
{
   
   CRect rTab;

   for(int i = 0; i < m_taba.GetCount() + 2; i++)
   {
      get_tab_rect(rTab,i);
      if(rTab.PtInRect(point))
      {

         if(i == m_taba.GetCount())
         {
            if(m_ulRibbonHeight == m_iFullHeight)
            {
               m_bExpanded = false;
            }
            else if(m_bExpanded)
            {
               m_bExpanded = false;
            }
            else
            {
               m_bExpanded = true;
            }
            if(m_bExpanded)
            {
               m_ulRibbonHeight =m_iFullHeight;
            }
            else
            {
               m_ulRibbonHeight = rTab.bottom;
            }
            GetParentFrame()->RecalcLayout();
         }
         else if(i == m_taba.GetCount() + 1)
         {
            
            CMainFrame * pframe = (CMainFrame *)GetParentFrame();

            pframe->TrackPopupMenu(ID_HELP_MANUAL);

         }
         else
         {
            set_cur_tab(i);
            if(!m_bExpanded)
            {
               m_ulRibbonHeight = m_iFullHeight;
               m_bExpanded = true;
               GetParentFrame()->RecalcLayout();
            /*   if(!m_bFocus || m_ulRibbonHeight != m_iFullHeight)
               {
                  m_bFocus = true;
                  CRect r;
                  GetWindowRect(r);
                  m_ulRibbonHeight = m_iFullHeight;
                  r.bottom = r.top + m_ulRibbonHeight;
                  GetParent()->ScreenToClient(r);
                  SetWindowPos(&CWnd::wndTop,r.left,r.top,r.Width(),r.Height(),SWP_SHOWWINDOW);
                  if(m_iTabSel == RIBBON_TAB_PROPERTIES)
                  {
                     ShowProperties(true);
                  }
                  m_bNeedLayout = true;
                  SetFocus();
               }*/
            }
         }
      }
   }
   Invalidate();
   SetCapture();
   hit_test(point,m_iGroupLButtonDown,m_iItemLButtonDown);
   CControlBar::OnLButtonDown(nFlags,point);
}


void CRibbonBar::OnMouseMove(UINT nFlags,CPoint point)
{
   // TODO: Add your message handler code here and/or call default
   TRACKMOUSEEVENT tme ={};
   tme.cbSize = sizeof(tme);
   tme.dwFlags = TME_LEAVE;
   TrackMouseEvent(&tme);

   int iGroupHover;
   int iItemHover;
   hit_test(point,iGroupHover,iItemHover);
   if(iGroupHover != m_iGroupHover || iItemHover != m_iItemHover)
   {
      m_iGroupHover = iGroupHover;
      m_iItemHover = iItemHover;

      if(iItemHover >= 0 && iGroupHover >= 0)
      {

         CItem & item = m_taba[m_iTabSel].m_groupa[iGroupHover].m_itema[iItemHover];

         if(item.m_bPopup)
         {

            GetParentFrame()->SendMessage(WM_SETMESSAGESTRING,(WPARAM)0,0L);

         }
         else
         {

            GetParentFrame()->SendMessage(WM_SETMESSAGESTRING,(WPARAM)item.m_iId,0L);

         }

      }

      Invalidate();
   }

   CControlBar::OnMouseMove(nFlags,point);

}


void CRibbonBar::OnMouseLeave()
{
   // TODO: Add your message handler code here and/or call default
   m_iGroupHover = -1;
   m_iItemHover = -1;
   Invalidate();
   CControlBar::OnMouseLeave();
}


void CRibbonBar::OnLButtonUp(UINT nFlags,CPoint point)
{
   // TODO: Add your message handler code here and/or call default
   ReleaseCapture();
   int iGroupHover;
   int iItemHover;
   if(hit_test(point,iGroupHover,iItemHover))
   {
      int iGroupLButtonDown = m_iGroupLButtonDown;
      int iItemLButtonDown = m_iItemLButtonDown;
      m_iGroupLButtonDown = -1;
      m_iItemLButtonDown = -1;
      Invalidate();
      if(iGroupHover == iGroupLButtonDown && iItemHover == iItemLButtonDown)
      {
         CItem & item = m_taba[m_iTabSel].m_groupa[iGroupHover].m_itema[iItemHover];
         // allow the toolbar itself to have update handlers
         if(GetParentFrame() != NULL)
         {

            if(item.m_bPopup && item.m_iId > 0)
            {

               CMainFrame * pframe = (CMainFrame *) GetParentFrame();
               
               pframe->TrackPopupMenu(item.m_iId);

            }
            else if(GetParentFrame()->GetActiveView() != NULL)
            {

               GetParentFrame()->GetActiveView()->PostMessage(WM_COMMAND,item.m_iId);

            }
            else
            {

               GetParentFrame()->PostMessage(WM_COMMAND,item.m_iId);

            }

         }

      }

   }

   CControlBar::OnLButtonUp(nFlags,point);
   Invalidate();
}





void CRibbonBar::OnTimer(UINT_PTR nIDEvent)
{
   // TODO: Add your message handler code here and/or call default
   if(nIDEvent == 1000)
   {
      CPoint point;
      ::GetCursorPos(&point);
      ScreenToClient(&point);
      OnMouseMove(0,point);
   }
   CControlBar::OnTimer(nIDEvent);
}


void CRibbonBar::ShowProperties(bool bShowProperties)
{

   if(bShowProperties)
   {
      CRect rectClient;
      GetClientRect(rectClient);
      CRect rTab;
      get_tab_rect(rTab,RIBBON_TAB_PROPERTIES);
      m_properties.SetWindowPos(&CWnd::wndTop,-m_iScroll,rTab.bottom,rectClient.Width(),m_ulRibbonHeight - rTab.bottom -1,SWP_SHOWWINDOW);
   }
   else
   {
      m_properties.ShowWindow(SW_HIDE);
   }

}


void CRibbonBar::set_cur_tab(int iTab)
{

   if(iTab < 0 || iTab >= m_taba.GetCount())
   {

      return;

   }

   if(iTab != m_iTabSel)
   {

      m_iTabSel = iTab;

      m_iScroll = 0;

      m_iTabScroll = 0;

   }

   CRect rTab;
   get_tab_rect(rTab,iTab);

   ShowProperties(iTab == RIBBON_TAB_PROPERTIES);

   m_bNeedLayout = true;

   Invalidate();

}


void CRibbonBar::OnSize(UINT nType,int cx,int cy)
{
   CControlBar::OnSize(nType,cx,cy);

   m_bNeedLayout = true;


}
#undef new


void CRibbonBar::layout()
{

   CRect rectClient;
   GetClientRect(rectClient);
   if(rectClient.IsRectEmpty())
      return;
   CDC dc;
   dc.CreateCompatibleDC(NULL);



   CRect rItem;
   CRect rImage;
   CRect rText;
   m_taba[m_iTabSel].ClearLayout();

   CRect r = rectClient;

   CRect rTab;
   get_tab_rect(rTab,0);

   if(m_iMargin < 1)
   {
      if(m_ulRibbonHeight > 1)
      {
         m_ulRibbonHeight = 1;
         GetParentFrame()->RecalcLayout();
      }
   }
   else
   {
      if(m_ulRibbonHeight == 1)
      {
         if(m_bExpanded || m_bFocus)
         {
            m_ulRibbonHeight = m_iFullHeight;
         }
         else
         {
            m_ulRibbonHeight = 32;
          

         }
         GetParentFrame()->RecalcLayout();
      }
   }

   r.bottom = m_iTabHeight;



   Gdiplus::Graphics g(dc.GetSafeHdc());

   CStringW wstr;

   wstr = L"WApyg";

   Gdiplus::RectF r2;

   r2.X = 0.0f;
   r2.Y = 0.0f;
   r2.Width = 200.0f;
   r2.Height = 200.0f;

   Gdiplus::RectF box;

   Gdiplus::Status s = g.MeasureString(wstr,wstr.GetLength(),&m_font2,r2,&m_format,&box);

   m_fMaxTextHeight = box.Height * 2.f;

   m_iGroupSmall = m_taba[m_iTabSel].m_groupa.GetCount();
   CRect rTotal(0,0,0,0);
   while(true)
   {
      m_taba[m_iTabSel].ClearLayout();

      rTotal.SetRect(0,0,0,0);

      if(&m_font2 == NULL)
      {
      }
      for(int i = 0; i < m_taba[m_iTabSel].m_groupa.GetCount(); i++)
      {

         CGroup & group = m_taba[m_iTabSel].m_groupa[i];

         for(int j = 0; j < group.m_itema.GetCount(); j++)
         {

            CItem & item = group.m_itema[j];

            calc_item_rect(g,item.m_rItem,item.m_rImage,item.m_rText,i,j);

            rTotal.UnionRect(rTotal,item.m_rItem);

         }

      }

      if(rTotal.Width() < rectClient.Width())
      {
         break;
      }
      m_iGroupSmall--;
      if(m_iGroupSmall < 0)
      {
         break;
      }
   }

   if(m_iTabSel == RIBBON_TAB_PROPERTIES)
   {

      CRect rectProps;

      m_properties.GetWindowRect(rectProps);

      m_iScrollMax = rectProps.Width() - rectClient.Width() + 10;

   }
   else
   {
      
      m_iScrollMax = rTotal.Width() - rectClient.Width() + 10;

   }

   layout_scroll();

}


BOOL CRibbonBar::PreCreateWindow(CREATESTRUCT& cs)
{
   
   if(!CControlBar::PreCreateWindow(cs))
   {

      return FALSE;

   }

   cs.dwExStyle |= WS_EX_COMPOSITED;

   return true;

}


void CRibbonBar::OnKillFocus(CWnd* pNewWnd)
{
   CControlBar::OnKillFocus(pNewWnd);

   //if(!m_bExpanded)
   //{

   //   if(m_bFocus)
   //   {

   //      m_bFocus = false;
   //      CRect r;
   //      GetWindowRect(r);
   //      m_ulRibbonHeight = m_iTabHeight;
   //      r.bottom = r.top + m_ulRibbonHeight;
   //      GetParent()->ScreenToClient(r);
   //      SetWindowPos(&CWnd::wndTop,r.left,r.top,r.Width(),r.Height(),SWP_SHOWWINDOW);
   //      m_bNeedLayout = true;
   //      SetFocus();
   //      GetTopLevelFrame()->Invalidate();


   //   }


   //}
   //else
   //{
   //   if(m_bFocus)
   //   {

   //      m_bFocus = false;

   //   }
   //}
   Invalidate();
}


void CRibbonBar::OnTabLeftArrow()
{
   m_iTabScroll -= 3;
   if(m_iTabScroll < 0)
   {
      m_iTabScroll =  0;
   }
   Invalidate();
}


void CRibbonBar::OnTabRightArrow()
{
   
   m_iTabScroll += 3;

   if(m_iTabScroll > m_iTabScrollMax)
   {

      m_iTabScroll = m_iTabScrollMax;

   }

   Invalidate();

}


void CRibbonBar::OnRibbonLeftArrow()
{
   m_iScroll -= 3;
   if(m_iScroll < 0)
   {
      m_iScroll =  0;
   }
   layout_scroll();
   Invalidate();
}


void CRibbonBar::OnRibbonRightArrow()
{
   
   m_iScroll += 3;

   if(m_iScroll > m_iScrollMax)
   {

      m_iScroll = m_iScrollMax;

   }
   
   layout_scroll();
   
   Invalidate();

}


void CRibbonBar::layout_scroll()
{

   if(m_iTabSel == RIBBON_TAB_PROPERTIES)
   {

      ShowProperties(true);
   }

   CRect rectClient;

   GetClientRect(rectClient);

   if(m_iScrollMax > 0)
   {

      if(m_iScroll > 0)
      {

         m_buttonLeft.SetWindowPos(&CWnd::wndTop,0,m_iTabHeight,10,m_ulRibbonHeight - m_iTabHeight,SWP_SHOWWINDOW);

      }
      else
      {

         m_buttonLeft.ShowWindow(SW_HIDE);

      }

      if(m_iScroll < m_iScrollMax)
      {

         m_buttonRight.SetWindowPos(&CWnd::wndTop,rectClient.Width() - 10,m_iTabHeight,10,m_ulRibbonHeight - m_iTabHeight,SWP_SHOWWINDOW);

      }
      else
      {

         m_buttonRight.ShowWindow(SW_HIDE);

      }

   }
   else
   {
      m_iScroll = 0;
      m_buttonLeft.ShowWindow(SW_HIDE);
      m_buttonRight.ShowWindow(SW_HIDE);

   }

}




