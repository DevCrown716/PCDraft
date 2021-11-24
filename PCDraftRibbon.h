#pragma once

#include <windows.h>
#include <ObjBase.h>
#include <initguid.h>
#include <Uiribbon.h>

class CMainFrame;

// This is the Ribbon implementation that is done by an application.
// Applications have to implement IUIApplication and IUICommandHandler to set up communication with the Windows Ribbon.
class CPCDraftRibbon : 
   public IUIApplication,
   public IUICommandHandler
{
public:

   CMainFrame *               m_pframe;
   LONG                       m_l;
   CComPtr < IUIFramework  >  m_pframework;

   CPCDraftRibbon();
   virtual ~CPCDraftRibbon();

   // Static method to create an instance of the object.
   __checkReturn static HRESULT CreateInstance(__in CMainFrame* pFrame,__deref_out_opt CPCDraftRibbon **ppApplication);

   // IUnknown methods.
   STDMETHODIMP_(ULONG) AddRef();
   STDMETHODIMP_(ULONG) Release();

   STDMETHODIMP QueryInterface(REFIID iid,void** ppv);
   // IUIApplication methods.
   STDMETHODIMP OnViewChanged(UINT32 /*nViewID*/,
      __in UI_VIEWTYPE /*typeID*/,
      __in IUnknown* pView,
      UI_VIEWVERB verb,
      INT32 /*uReasonCode*/);
   
   STDMETHODIMP OnCreateUICommand(UINT32 /*nCmdID*/,
      __in UI_COMMANDTYPE /*typeID*/,
      __deref_out IUICommandHandler** ppCommandHandler);
   
   STDMETHODIMP OnDestroyUICommand(UINT32 /*commandId*/,
      __in UI_COMMANDTYPE /*typeID*/,
      __in_opt  IUICommandHandler* /*commandHandler*/);
   

   STDMETHODIMP Execute(UINT nCmdID,
      UI_EXECUTIONVERB /*verb*/,
      __in_opt const PROPERTYKEY* key,
      __in_opt const PROPVARIANT* ppropvarValue,
      __in_opt IUISimplePropertySet* /*pCommandExecutionProperties*/);
   STDMETHODIMP UpdateProperty(UINT32 nCmdID,
      __in REFPROPERTYKEY key,
      __in_opt  const PROPVARIANT *currentValue,
      __out PROPVARIANT *newValue);
      
   BOOL GetStatus(UINT32 nCmdID);

};


__checkReturn HRESULT InitRibbon(__in CMainFrame* pMainFrame,__deref_out_opt IUnknown** ppFramework);

void DestroyRibbon(__in IUnknown* pFramework);

HRESULT SetModes(__in IUnknown* pFramework,UINT modes);

HRESULT RibbonInvalidate(__in IUnknown* pFramework);


