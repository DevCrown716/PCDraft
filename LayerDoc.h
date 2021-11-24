// Layer - PCDraft 6 - Infinisys Ltd.
#pragma once


#define LIBRARY_HINT_UPDATE_TREE 20180328


class CLayerFrame;
class CLayerListView;
class CLayerHeader;



enum e_layer_subitem
{

   layer_subitem_state,
   layer_subitem_show,
   layer_subitem_name,
   layer_subitem_scale,
   layer_subitem_count,
   layer_subitem_len,

};


enum e_layer_state
{

   layer_state_active,
   layer_state_locked,

};

enum e_layer_show
{

   layer_show_visible,
   layer_show_grayed,
   layer_show_hidden,

};

class CLayerDoc : public CDocument
{
public:






   int                  m_iLayerSel;

   CPCDraftDoc *        m_pDoc;
   CLayerFrame *        m_playerframe;
   CLayerListView *     m_playerlistview;
   CLayerHeader *       m_playerheader;


	CLayerDoc();
	virtual ~CLayerDoc();

	virtual void Serialize(CArchive& ar);   // overridden for document i/o

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual BOOL OnNewDocument();

   CString GetDefaultLayerPath();
   CString GetDefaultPhotoLayerPath();
   CString GetAppDataPath();


   afx_msg void OnLayerAdd();
   afx_msg void OnLayerDelete();
   afx_msg void OnLayerRename();
   afx_msg void OnLayerShow();
   afx_msg void OnLayerHide();
   afx_msg void OnLayerGray();
   afx_msg void OnLayerMerge();
   afx_msg void OnUpdateLayerAdd(CCmdUI *pCmdUI);
   afx_msg void OnUpdateLayerDelete(CCmdUI *pCmdUI);
   afx_msg void OnUpdateLayerRename(CCmdUI *pCmdUI);
   afx_msg void OnUpdateLayerShow(CCmdUI *pCmdUI);
   afx_msg void OnUpdateLayerHide(CCmdUI *pCmdUI);
   afx_msg void OnUpdateLayerGray(CCmdUI *pCmdUI);
   afx_msg void OnUpdateLayerMerge(CCmdUI *pCmdUI);

   void Refresh();

   bool is_add_enabled();
   bool is_delete_enabled();
   bool is_rename_enabled();
   bool is_show_enabled();
   bool is_hide_enabled();
   bool is_gray_enabled();
   bool is_merge_enabled();

   INT_PTR get_item_count();
   e_layer_state get_layer_state(int iItem);
   e_layer_show get_layer_show(int iItem);
   CString get_layer_name(int iItem);
   int get_layer_scale(int iItem);
   CString get_layer_scale_text(int iItem);
   CString get_layer_shape_count_text(int iItem);

   void set_layer_show(int iItem, e_layer_show eshow);

   DECLARE_DYNCREATE(CLayerDoc)
   DECLARE_MESSAGE_MAP()

};
