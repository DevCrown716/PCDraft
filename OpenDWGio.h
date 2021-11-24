// OpenDWGio.h: interface for the COpenDWGio class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENDWGIO_H__95CE12DF_DB9B_448B_83DD_5BBABCBD9BC4__INCLUDED_)
#define AFX_OPENDWGIO_H__95CE12DF_DB9B_448B_83DD_5BBABCBD9BC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PCDraftDoc.h"
#include "matrix3x2.h"
#include "StyledText.h"
#include <math.h>

class OdDbDatabase;
class OdDbLayerTableRecord;
class OdDbEntity;
class OdDbBlockTableRecord;
class OdDbGroup;
class OdDbBlockReference;

class CMDShape;
class CMDGroupShape;

class OdGeMatrix3d;
class OdGePoint3d;
class OdGePoint2d;
class OdGeVector3d;
class OdCmColor;
class OdString;


class oda;

class COpenDWGio  
{
public:


   oda *		m_poda;


	COpenDWGio ();
	virtual ~COpenDWGio ();

public:





	void SetMode (double sUnits, bool bMetric) {m_sUnits = sUnits; m_bMetric = bMetric;}
	bool Import (CPCDraftDoc* pDoc, LPCTSTR strFileName);
	bool Export (CPCDraftDoc* pDoc, LPCTSTR strFileName);
private:
	// implementation (Set of DoXXX methods which can handle comtatible or known objects)
	void DoDocument (OdDbDatabase*);
	void DoLayer (OdDbLayerTableRecord*, int index);
	void DoEntity (OdDbEntity*);
//Group of entities
	CMDShape* DoBlockReference (OdDbEntity* pEntity, CLayer* pLayer);
	CMDShape* COpenDWGio::DoMInsertBlock (OdDbEntity* pEntity);
// Curves
	CMDShape* DoArc (OdDbEntity*);
	CMDShape* DoLine (OdDbEntity*);
	CMDShape* DoPolyline(OdDbEntity*, CLayer*);
	CMDShape* DoSpline (OdDbEntity*);
	bool CalculateCircleParams(const double radius, double& a, double& b, double& angle);
	bool CalculateCircleParams(const double radius, LVector2D& v, double& angle);
	bool CalculateCircleParams(const double radius, LVector2D& v, LVector2D& v1, double& angle);
	bool CalculateEllipseParams(const OdGeVector3d& majorAxx, const OdGeVector3d& minorAxx, double& a, double& b, double& angle) const;
	bool CalculateEllipseParams(const OdGeVector3d& majorAxx, const OdGeVector3d& minorAxx, LVector2D& v, LVector2D& v1, double& angle) const;
	bool CalculateEllipseParams(const OdGeVector3d& majorAxx, const OdGeVector3d& minorAxx, LVector2D& v, double& angle) const;
	CMDShape* DoCircle (OdDbEntity*);
	CMDShape* DoLeader (OdDbEntity*, CLayer* pLayer);
	CMDShape* Do3dPolyline (OdDbEntity*);
	CMDShape* DoRay (OdDbEntity*);
	CMDShape* Do2dPolyline (OdDbEntity*);
	CMDShape* DoXline (OdDbEntity*);
	CMDShape* DoEllipse (OdDbEntity*);
//Entities
	CMDShape* DoDimension (OdDbEntity*);
	CMDShape* DoCurve (OdDbEntity*);
	CMDShape* DoRegion (OdDbEntity*);
	CMDShape* DoMText (OdDbEntity*, CLayer* pLayer);
	CMDShape* DoHatch (OdDbEntity*);
	CMDShape* DoFace (OdDbEntity*, CLayer*);
	CMDShape* DoArcAlignedText (OdDbEntity*);
	CMDShape* DoVertex (OdDbEntity*);
	CMDShape* DoRText (OdDbEntity*);
	CMDShape* DoImage (OdDbEntity*);
	CMDShape* DoText (OdDbEntity*, CLayer* pLayer);
	CMDShape* DoPolygonMesh (OdDbEntity*);
	CMDShape* DoFaceRecord (OdDbEntity*, CLayer*);
	CMDShape* DoPolyFaceMesh (OdDbEntity*);
	CMDShape* DoShape (OdDbEntity*);
	CMDShape* DoProxyEntity (OdDbEntity*);
	CMDShape* DoTrace (OdDbEntity*, CLayer*);
	CMDShape* DoFcf (OdDbEntity*);
	CMDShape* DoSequenceEnd (OdDbEntity*);
	CMDShape* Do3dSolid (OdDbEntity*);
	CMDShape* DoFrame (OdDbEntity*);
	CMDShape* DoBody (OdDbEntity*);
	CMDShape* DoMline (OdDbEntity*);
	CMDShape* DoPoint (OdDbEntity*);
	CMDShape* DoSolid (OdDbEntity*, CLayer*);
	CMDShape* DoEntityWithGrData (OdDbEntity*);
// Fpr unsupported objects
	void BadObjectAlert(UINT nID);
	bool m_bShowBadObjectAlerts;
	CDWordArray m_BadObjects;

// Support
	void SetTransformation (OdDbBlockTableRecord* pEntityTable);
	LongPoint GetLongPoint (const OdGePoint3d& pt, bool checkBounds = true) const;
	LongPoint GetLongPoint (const OdGePoint2d& pt, bool checkBounds = true) const;
	LongPoint GetLongPoint (const OdGeVector3d& pt, bool checkBounds = true) const;
	short GetColor (const OdCmColor& color, bool zeroLayer = false) const;
	COLORREF GetRGBColor (const OdCmColor& color) const;
	CLayer* GetLayer (LPCTSTR name) const;

	CMDShape* DrawTrace(OdDbEntity*, CLayer*, OdGePoint3d&, OdGePoint3d&, OdGePoint3d&, OdGePoint3d&);
	//void SetObjectAttributes (CMDShape* pShape, const OdDbEntity* pEntity, bool toFill = false);
	void SetObjectAttributes (CMDShape* pShape, const OdDbEntity* pEntity, bool blockRef = false, bool toFill = false);
	bool AddGroupObject (CLayer *layerHdl, CMDGroupShape *groupShape);
	bool AddObject(CLayer *layer,CMDShape *shapeHdl);  //exacly original
	bool RemoveObject(CLayer *layer,CMDShape *shapeHdl);  //exacly original
	int CreateText(const char* text,CStyledText &stText,CStyledTextStyle Style);
	LongPoint getAngDimLineEndsPt (const double arcPt_x, const double arcPt_y,
								 const double line1sPt_x, const double line1sPt_y,
								 const double line1ePt_x, const double line1ePt_y,
								 const double line2sPt_x, const double line2sPt_y,
								 const double line2ePt_x, const double line2ePt_y);


private: //data
	CPCDraftDoc*		m_pDoc;
	LMatrix3x2			m_cTransform;
	CMDGroupShape*		m_pCurBlock;
	double				m_sUnits;
	bool				m_bMetric;
	double				m_gDWGScale;
	double				m_g100mmDWGScale;
	short				m_nBlockColor;


	OdDbLayerTableRecord* m_curDbLayer;
	OdDbLayerTableRecord* m_lastNon0DbLayer;
	OdDbBlockReference* m_curBlock;
	//bool firstNon0Layer;
};


#endif // !defined(AFX_OPENDWGIO_H__95CE12DF_DB9B_448B_83DD_5BBABCBD9BC4__INCLUDED_)
