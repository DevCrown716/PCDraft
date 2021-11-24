#pragma once
#include <commdlg.h>

class CFileDialogW : public CCommonDialog
{
   DECLARE_DYNAMIC(CFileDialogW)

public:
   // Attributes
   __declspec(property(get = GetOFN)) OPENFILENAMEW m_ofn;
   const OPENFILENAMEW& GetOFN() const;
   OPENFILENAMEW& GetOFN();
   LPOPENFILENAMEW m_pOFN;

   // Constructors
   explicit CFileDialogW(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
      LPCWSTR lpszDefExt = NULL,
      LPCWSTR lpszFileName = NULL,
      DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
      LPCWSTR lpszFilter = NULL,
      CWnd* pParentWnd = NULL,
      DWORD dwSize = 0,
      BOOL bVistaStyle = TRUE);
   virtual ~CFileDialogW();

   // Operations
   virtual INT_PTR DoModal();

   /// <summary>
   /// Determines if the current dialog in folder picker mode.</summary>
   /// <returns>
   /// If the dialog in the folder picker mode, the return value is TRUE. Otherwise - FALSE</returns>
   BOOL IsPickFoldersMode() const { return m_bPickFoldersMode; }

   /// <summary>
   /// Determines if the current dialog in non-file system folder picker mode.</summary>
   /// <returns>
   /// If the dialog in the non-file system folder picker mode, the return value is TRUE. Otherwise - FALSE</returns>
   BOOL IsPickNonFileSysFoldersMode() const { return m_bPickNonFileSysFoldersMode; }
   friend UINT_PTR CALLBACK _AfxCommDlgProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
   // Helpers for parsing file name after successful return
   // or during Overridable callbacks if OFN_EXPLORER is set
   CStringW GetPathName() const;  // return full path and filename
   CStringW GetFileName() const;  // return only filename
   CStringW GetFileExt() const;   // return only ext
   CStringW GetFileTitle() const; // return file title
   BOOL GetReadOnlyPref() const; // return TRUE if readonly checked

   // Enumerating multiple file selections
   POSITION GetStartPosition() const;
   CStringW GetNextPathName(POSITION& pos) const;

   // Helpers for custom templates
   void SetTemplate(UINT nWin3ID, UINT nWin4ID);
   void SetTemplate(LPCWSTR lpWin3ID, LPCWSTR lpWin4ID);

   // Other operations available while the dialog is visible
   CStringW GetFolderPath() const; // return full path
   void SetControlText(int nID, LPCWSTR lpsz);
   void HideControl(int nID);
   void SetDefExt(LPCWSTR lpsz);

   virtual void UpdateOFNFromShellDialog();
   void ApplyOFNToShellDialog();
   IFileOpenDialog* GetIFileOpenDialog() throw();
   IFileSaveDialog* GetIFileSaveDialog() throw();
   IFileDialogCustomize* GetIFileDialogCustomize() throw();

#if NTDDI_VERSION >= NTDDI_VISTA
   /// <summary>
   /// Adds a folder to the list of places available for the user to open or save items.</summary>
   /// <param name="lpszFolder">A path to the folder to be made available to the user. This can only be a folder.</param>
   /// <param name="fdap">Specifies where the folder is placed within the list.</param>
   void AddPlace(LPCWSTR lpszFolder, FDAP fdap = FDAP_TOP) throw();

   /// <summary>
   /// Adds a folder to the list of places available for the user to open or save items.</summary>
   /// <param name="psi">A pointer to an IShellItem that represents the folder to be made available to the user. This can only be a folder.</param>
   /// <param name="fdap">Specifies where the folder is placed within the list.</param>
   void AddPlace(IShellItem* psi, FDAP fdap = FDAP_TOP) throw();

   /// <summary>
   /// Gets the choice that the user made in the dialog.</summary>
   /// <returns>
   /// A pointer to an IShellItem that represents the user's choice</returns>
   IShellItem* GetResult() throw();

   /// <summary>
   /// Gets the user's choices in a dialog that allows multiple selection.</summary>
   /// <returns>
   /// A pointer to an IShellItemArray through which the items selected in the dialog can be accessed</returns>
   IShellItemArray* GetResults() throw();

   /// <summary>
   /// Provides a property store that defines the default values to be used for the item being saved.</summary>
   /// <returns>
   /// TRUE if successful, FALSE - otherwise</returns>
   /// <param name="lpszPropList">A list of predefined properties divided by ";"</param>
   BOOL SetProperties(LPCWSTR lpszPropList);

   // IFileDialogCustomize wrappers:

   /// <summary>
   /// Enables a drop-down list on the Open or Save button in the dialog.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the drop-down list</param>
   HRESULT EnableOpenDropDown(DWORD dwIDCtl);

   /// <summary>
   /// Adds a menu to the dialog.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the menu to add</param>
   /// <param name="strLabel">The menu name</param>
   HRESULT AddMenu(DWORD dwIDCtl, const CStringW& strLabel);

   /// <summary>
   /// Adds a button to the dialog.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the button to add</param>
   /// <param name="strLabel">The button name</param>
   HRESULT AddPushButton(DWORD dwIDCtl, const CStringW& strLabel);

   /// <summary>
   /// Adds a combo box to the dialog.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the combo box to add</param>
   HRESULT AddComboBox(DWORD dwIDCtl);

   /// <summary>
   /// Adds an option button (also known as radio button) group to the dialog.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the option button group to add</param>
   HRESULT AddRadioButtonList(DWORD dwIDCtl);

   /// <summary>
   /// Adds a check button to the dialog.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the check button to add</param>
   /// <param name="strLabel">The check button name</param>
   /// <param name="bChecked">A BOOL indicating the current state of the check button. TRUE if checked; FALSE otherwise</param>
   HRESULT AddCheckButton(DWORD dwIDCtl, const CStringW& strLabel, BOOL bChecked);

   /// <summary>
   /// Adds an edit box to the dialog.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the edit box to add</param>
   /// <param name="strText">The edit box name</param>
   HRESULT AddEditBox(DWORD dwIDCtl, const CStringW& strText);

   /// <summary>
   /// Adds a separator to the dialog.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the separator add</param>
   HRESULT AddSeparator(DWORD dwIDCtl);

   /// <summary>
   /// Adds text content to the dialog.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the text to add</param>
   /// <param name="strText">The text name</param>
   HRESULT AddText(DWORD dwIDCtl, const CStringW& strText);

   /// <summary>
   /// Sets the text associated with a control, such as button text or an edit box label.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the control</param>
   /// <param name="strLabel">The control name</param>
   HRESULT SetControlLabel(DWORD dwIDCtl, const CStringW& strLabel);

   /// <summary>
   /// Gets the current visibility and enabled states of a given control.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the control</param>
   /// <param name="dwState">A reference to a variable that receives one or more values from the CDCONTROLSTATE enumeration that indicate the current state of the control.</param>
   HRESULT GetControlState(DWORD dwIDCtl, CDCONTROLSTATEF& dwState);

   /// <summary>
   /// Sets the current visibility and enabled states of a given control.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the control</param>
   /// <param name="dwState"> One or more values from the CDCONTROLSTATE enumeration that indicate the current state of the control.</param>
   HRESULT SetControlState(DWORD dwIDCtl, CDCONTROLSTATEF dwState);

   /// <summary>
   /// Gets the current text in an edit box control.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the edit box</param>
   /// <param name="strText">The text value</param>
   HRESULT GetEditBoxText(DWORD dwIDCtl, CStringW& strText);

   /// <summary>
   /// Sets the current text in an edit box control.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the edit box</param>
   /// <param name="strText">The text value</param>
   HRESULT SetEditBoxText(DWORD dwIDCtl, const CStringW& strText);

   /// <summary>
   /// Gets the current state of a check button (check box) in the dialog.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the check box</param>
   /// <param name="bChecked">check box state: TRUE means checked; FALSE, unchecked</param>
   HRESULT GetCheckButtonState(DWORD dwIDCtl, BOOL& bChecked);

   /// <summary>
   /// Sets the current state of a check button (check box) in the dialog.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the check box</param>
   /// <param name="bChecked">check box state: TRUE means checked; FALSE, unchecked</param>
   HRESULT SetCheckButtonState(DWORD dwIDCtl, BOOL bChecked);

   /// <summary>
   /// Adds an item to a container control in the dialog.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the container control to which the item is to be added</param>
   /// <param name="dwIDItem">The ID of the item</param>
   /// <param name="strLabel">Item's text</param>
   HRESULT AddControlItem(DWORD dwIDCtl, DWORD dwIDItem, const CStringW& strLabel);

   /// <summary>
   /// Removes an item from a container control in the dialog.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the container control from which the item is to be removed</param>
   /// <param name="dwIDItem">The ID of the item</param>
   HRESULT RemoveControlItem(DWORD dwIDCtl, DWORD dwIDItem);

   /// <summary>
   /// Gets the current state of an item in a container control found in the dialog.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the container control</param>
   /// <param name="dwIDItem">The ID of the item</param>
   /// <param name="dwState">A reference to a variable that receives one of more values from the CDCONTROLSTATE enumeration that indicate the current state of the control</param>
   HRESULT GetControlItemState(DWORD dwIDCtl, DWORD dwIDItem, CDCONTROLSTATEF& dwState);

   /// <summary>
   /// Sets the current state of an item in a container control found in the dialog.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the container control</param>
   /// <param name="dwIDItem">The ID of the item</param>
   /// <param name="dwState">One or more values from the CDCONTROLSTATE enumeration that indicate the new state of the control</param>
   HRESULT SetControlItemState(DWORD dwIDCtl, DWORD dwIDItem, CDCONTROLSTATEF dwState);

   /// <summary>
   /// Gets a particular item from specified container controls in the dialog.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the container control</param>
   /// <param name="dwIDItem">The ID of the item that the user selected in the control</param>
   HRESULT GetSelectedControlItem(DWORD dwIDCtl, DWORD& dwIDItem);

   /// <summary>
   /// Sets the selected state of a particular item in an option button group or a combo box found in the dialog.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the container control</param>
   /// <param name="dwIDItem">The ID of the item that the user selected in the control</param>
   HRESULT SetSelectedControlItem(DWORD dwIDCtl, DWORD dwIDItem);

   /// <summary>
   /// Declares a visual group in the dialog. Subsequent calls to any "add" method add those elements to this group.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the visual group</param>
   /// <param name="strLabel">The group name</param>
   HRESULT StartVisualGroup(DWORD dwIDCtl, const CStringW& strLabel);

   /// <summary>
   /// Stops the addition of elements to a visual group in the dialog.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   HRESULT EndVisualGroup();

   /// <summary>
   /// Places a control in the dialog so that it stands out compared to other added controls.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the control</param>
   HRESULT MakeProminent(DWORD dwIDCtl);

   /// <summary>
   /// Sets the text of a control item. For example, the text that accompanies a radio button or an item in a menu.</summary>
   /// <returns>
   /// Returns S_OK if successful, or an error value otherwise.</returns>
   /// <param name="dwIDCtl">The ID of the container control</param>
   /// <param name="dwIDItem">The ID of the item</param>
   /// <param name="strLabel">Item's text</param>
   HRESULT SetControlItemText(DWORD dwIDCtl, DWORD dwIDItem, const CStringW& strLabel);
#endif // NTDDI_VERSION >= NTDDI_VISTA

   // Overridable callbacks
protected:

#if NTDDI_VERSION >= NTDDI_VISTA
   HRESULT CDialogEventHandler_CreateInstance(REFIID riid, void **ppv);
#endif // NTDDI_VERSION >= NTDDI_VISTA

   friend UINT_PTR CALLBACK _AfxCommDlgProc(HWND, UINT, WPARAM, LPARAM);
   virtual UINT OnShareViolation(LPCWSTR lpszPathName);
   virtual BOOL OnFileNameOK();
   virtual void OnLBSelChangedNotify(UINT nIDBox, UINT iCurSel, UINT nCode);

   // only called back if OFN_EXPLORER is set
   virtual void OnInitDone();
   virtual void OnFileNameChange();
   virtual void OnFolderChange();
   virtual void OnTypeChange();

   /// <summary>
   /// Called when the container item is being selected.</summary>
   /// <param name="dwIDCtl">The ID of the container control</param>
   /// <param name="dwIDItem">The ID of the item</param>
   virtual void OnItemSelected(DWORD dwIDCtl, DWORD dwIDItem);

   /// <summary>
   /// Called when the button is clicked.</summary>
   /// <param name="dwIDCtl">The ID of the button</param>
   virtual void OnButtonClicked(DWORD dwIDCtl);

   /// <summary>
   /// Called when the check box is checked/unchecked.</summary>
   /// <param name="dwIDCtl">The ID of the check box</param>
   /// <param name="bChecked">checked/unchecked</param>
   virtual void OnCheckButtonToggled(DWORD dwIDCtl, BOOL bChecked);

   /// <summary>
   /// Called when the control is being active.</summary>
   /// <param name="dwIDCtl">The ID of the control</param>
   virtual void OnControlActivating(DWORD dwIDCtl);

   // Implementation
#ifdef _DEBUG
public:
   virtual void Dump(CDumpContext& dc) const;
#endif

protected:

   BOOL m_bVistaStyle;
   BOOL m_bPickFoldersMode;
   BOOL m_bPickNonFileSysFoldersMode;
   DWORD m_dwCookie;
   void* m_pIFileDialog;
   void* m_pIFileDialogCustomize;

   BOOL m_bOpenFileDialog;       // TRUE for file open, FALSE for file save
   CStringW m_strFilter;          // filter string
                  // separate fields with '|', terminate with '||\0'
   WCHAR m_szFileTitle[_MAX_FNAME];       // contains file title after return
   WCHAR m_szFileName[_MAX_PATH]; // contains full path name after return

   OPENFILENAMEW*  m_pofnTemp;

   virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

private:
   BOOL m_bFileTypesSet;   // have file types already been set for Vista style dialog?

protected:
   DECLARE_INTERFACE_MAP()

#if NTDDI_VERSION >= NTDDI_VISTA
   BEGIN_INTERFACE_PART(FileDialogEvents, IFileDialogEvents)
      STDMETHOD(OnFileOk)(IFileDialog *);
   STDMETHOD(OnFolderChange)(IFileDialog *);
   STDMETHOD(OnFolderChanging)(IFileDialog *, IShellItem *);
   STDMETHOD(OnHelp)(IFileDialog *);
   STDMETHOD(OnSelectionChange)(IFileDialog *);
   STDMETHOD(OnShareViolation)(IFileDialog *pfd, IShellItem *psi, FDE_SHAREVIOLATION_RESPONSE *pResponse);
   STDMETHOD(OnTypeChange)(IFileDialog *);
   STDMETHOD(OnOverwrite)(IFileDialog *, IShellItem *, FDE_OVERWRITE_RESPONSE *);
   END_INTERFACE_PART_OPTIONAL(FileDialogEvents)

   BEGIN_INTERFACE_PART(FileDialogControlEvents, IFileDialogControlEvents)
      STDMETHOD(OnItemSelected)(IFileDialogCustomize *, DWORD, DWORD);
   STDMETHOD(OnButtonClicked)(IFileDialogCustomize *, DWORD);
   STDMETHOD(OnCheckButtonToggled)(IFileDialogCustomize *, DWORD, BOOL);
   STDMETHOD(OnControlActivating)(IFileDialogCustomize *, DWORD);
   END_INTERFACE_PART_OPTIONAL(FileDialogControlEvents)
#else
   BEGIN_INTERFACE_PART(FileDialogEvents, IUnknown)
   END_INTERFACE_PART_OPTIONAL(FileDialogEvents)

   BEGIN_INTERFACE_PART(FileDialogControlEvents, IUnknown)
   END_INTERFACE_PART_OPTIONAL(FileDialogControlEvents)
#endif // NTDDI_VERSION >= NTDDI_VISTA
};
