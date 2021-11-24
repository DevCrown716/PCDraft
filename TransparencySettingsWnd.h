/////////////////////////////////////////////////////////////////////////////
//
// CTransparencySettingsWnd window
// Infinisys Ltd
//
#pragma once


extern UINT UM_ATTRIBSETTINGS_UPDATE;
extern UINT UM_ATTRIBSETTINGS_INFO;

class CTransparencySettingsWnd: public CWnd
{
public:


   CEdit * m_pedit;
   CString  m_str;
   

   CTransparencySettingsWnd(CEdit * pedit);
   virtual ~CTransparencySettingsWnd();

   
   afx_msg LPARAM OnAttributeSettings(WPARAM wParam,LPARAM lParam);


   DECLARE_MESSAGE_MAP()

};

