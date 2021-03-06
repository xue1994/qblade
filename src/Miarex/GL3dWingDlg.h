/****************************************************************************

	GL3dWingDlg Class
	Copyright (C) 2009 Andre Deperrois adeperrois@xflr5.com
 
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/

#ifndef GL3DWINGDLG_H
#define GL3DWINGDLG_H

#include <QDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QSlider>
#include <QSettings>
#include <QLabel>
#include <QRadioButton>
#include <QStandardItemModel>
#include <QTableView>
#include <QPushButton>
#include "ArcBall.h"
#include "WingDelegate.h"
#include "../GLWidget.h"
#include "../Misc/NumberEdit.h"
#include "../Misc/ColorButton.h"
#include "../Misc/LineButton.h"
#include "../Objects/Wing.h"



class GL3dWingDlg: public QDialog
{
	Q_OBJECT
	friend class MainFrame;
	friend class QMiarex;
	friend class GLWidget;
	friend class CWing;
	friend class GLLightDlg;
	friend class PlaneDlg;
	friend class WingDelegate;

public:
	GL3dWingDlg(void *pParent = NULL);


private slots:
	void OnAxes();
	void On3DIso();
	void On3DTop();
	void On3DLeft();
	void On3DFront();
	void On3DReset();
	void On3DPickCenter();

//	void OnSetupLight();
	void OnClipPlane(int pos);
	void OnLight();
	void OnSurfaces();
	void OnOutline();
	void OnPanels();
	void OnFoilNames();
	void OnShowMasses();

	void OnOK();
	void OnCellChanged(QWidget *pWidget);
	void OnItemClicked(const QModelIndex &index);
	void OnWingColor();
	void OnSide();
	void OnSymetric();
	void OnInsertBefore();
	void OnInsertAfter();
	void OnDeleteSection();
	void OnResetMesh();
	void OnScaleWing();
	void OnInertia();


private:
	void reject();

	void WheelEvent(QWheelEvent *event);
	void MouseDoubleClickEvent(QMouseEvent *event);
	void MouseMoveEvent(QMouseEvent *event);
	void MousePressEvent(QMouseEvent *event) ;
	void MouseReleaseEvent(QMouseEvent *event) ;
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void showEvent(QShowEvent *event);
	void contextMenuEvent(QContextMenuEvent *event);
	void resizeEvent(QResizeEvent *event);

	bool CheckWing();
	void CreateXPoints(int NXPanels, int XDist, CFoil *pFoilA, CFoil *pFoilB, double *xPointA, double *xPointB, int &NXLead, int &NXFlap);
	void ComputeGeometry();

	void SetWingData();
	void FillDataTable();
	void FillTableRow(int row);
	void ReadParams();
	void ReadSectionData(int sel);
	void SetCurrentSection(int section);
	void SetScale();
	int VLMGetPanelTotal();
	bool VLMSetAutoMesh(int total=0);

	bool InitDialog(CWing *pWing);
	void Connect();
	void SetupLayout();
	void GLCreateMesh();
	void GLRenderView();
	void GLDrawFoils();
	void GLCreateSectionHighlight();
	void GLInverseMatrix();
	void GLCallViewLists();
	void GLDraw3D();
	void Set3DRotationCenter();
	void Set3DRotationCenter(QPoint point);
	void SetWingScale();
	void UpdateView();

	bool LoadSettings(QSettings *pSettings);
	bool SaveSettings(QSettings *pSettings);


private:
	static void *s_pMiarex;
	static void *s_pMainFrame;
	static void *s_pGLLightDlg;
	static QList <void*> *s_poaWing;
	static QPoint s_WindowPos;
	static QSize  s_WindowSize;
	static bool s_bWindowMaximized;


	GLWidget *m_pGLWidget;

	QLineEdit *m_pctrlWingName;
	QTextEdit *m_pctrlWingDescription;
	QCheckBox *m_pctrlSymetric;
	QRadioButton *m_pctrlLeftSide, *m_pctrlRightSide;
	ColorButton *m_pctrlWingColor;
	QPushButton *m_pctrlResetMesh, *m_pctrlScaleWing;
	QPushButton *m_pctrlInsertBefore, *m_pctrlInsertAfter, *m_pctrlDeleteSection;
	QLabel *m_pctrlWingSpan, *m_pctrlWingArea, *m_pctrlMAC, *m_pctrlGeomChord;
	QLabel *m_pctrlMACSpanPos, *m_pctrlAspectRatio, *m_pctrlTaperRatio, *m_pctrlSweep, *m_pctrlNFlaps;
	QLabel *m_pctrlVLMPanels, *m_pctrl3DPanels;
	QLabel *m_pctrlProjectedArea, *m_pctrlProjectedSpan;

	QLabel *m_pctrlLength1, *m_pctrlLength2, *m_pctrlLength3, *m_pctrlLength4, *m_pctrlLength5;
	QLabel *m_pctrlAreaUnit1, *m_pctrlAreaUnit2, * m_pctrlVolumeUnit;
	QTableView *m_pctrlWingTable;
	QStandardItemModel *m_pWingModel;
	WingDelegate *m_pWingDelegate;

	QPushButton *OKButton, *CancelButton;
	QCheckBox *m_pctrlAxes, *m_pctrlLight, *m_pctrlSurfaces, *m_pctrlOutline, *m_pctrlPanels, *m_pctrlFoilNames;
	QCheckBox *m_pctrlShowMasses;
	QPushButton *m_pctrlX, *m_pctrlY, *m_pctrlZ, *m_pctrlIso, *m_pctrlReset, *m_pctrlPickCenter;

	QPushButton *m_pctrlInertiaButton;

	QSlider *m_pctrlClipPlanePos;

	QAction *m_pResetScales;

	QMenu *m_pContextMenu;
	QAction *m_pInsertBefore, *m_pInsertAfter, *m_pDeleteSection;

	QWidget *m_pctrlControlsWidget;

	CWing *m_pBlade;

	bool m_bAcceptName;
	bool m_bRightSide;
	bool m_bChanged;
	bool m_bTrans;
	bool m_bStored;
	bool m_bEnableName;
	bool m_bDragPoint;
	bool m_bArcball;			//true if the arcball is to be displayed
	bool m_bglLight;
	bool m_bCrossPoint;			//true if the control point on the arcball is to be displayed
	bool m_bPickCenter;			//true if the user is in the process of picking a new center for OpenGL display
	bool m_bSurfaces, m_bOutline, m_bAxes, m_bVLMPanels;
	bool m_bShowLight;			// true if the virtual light is to be displayed
	bool m_bResetglWing;
	bool m_bResetglSectionHighlight;
	bool m_bFoilNames;
	bool m_bShowMasses;

	int m_iSection;
	int m_StackPos, m_StackSize;// undo : current stack position and current stack size
	int m_GLList;
//	int m_iView;

//	double m_yMAC;
	double m_glTop, m_HorizontalSplit, m_VerticalSplit;//screen split ratio for body 3D view
	double m_glScaled;//zoom factor for UFO
	double m_ClipPlanePos;
	double MatIn[4][4], MatOut[4][4];


	QPoint m_MousePos;
	QPoint m_ptPopUp;
	QPoint m_LastPoint, m_PointDown;


	CVector m_RealPopUp;
	CVector m_UFOOffset;
	CVector m_glViewportTrans;// the translation vector in gl viewport coordinates
	CVector m_glRotCenter;    // the center of rotation in object coordinates... is also the opposite of the translation vector
	CVector m_L[(MAXBODYFRAMES+1)*(MAXSIDELINES+1)]; //temporary points to save calculation times for body NURBS surfaces
	CVector m_T[(MAXBODYFRAMES+1)*(MAXSIDELINES+1)];

	CPanel *m_pPanel;
	CVector *m_pNode;

	ArcBall m_ArcBall;


};

#endif // GL3DWINGDLG_H
