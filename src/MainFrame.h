/****************************************************************************

	MainFrame Class

	Copyright (C) 2008-2012 Andre Deperrois adeperrois@xflr5.com

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

#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QMainWindow>
#include <QStackedWidget>
template <class T> class QPointer;
class QToolButton;
class QRadioButton;

#include "Misc/DisplaySettingsDlg.h"
#include "Misc/RenameDlg.h"
#include "Misc/UnitsDlg.h"
#include "StoreAssociatedComboBox_include.h"

class GLWidget;
class TwoDWidget;
class CFoil;
class CPolar;
class CVector;
class OpPoint;
class Graph;
class ScrolledDock;
class ModuleBase;
class TwoDWidgetInterface;


class MainFrame : public QMainWindow
{
	Q_OBJECT

public:
    MainFrame(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~MainFrame ();

    int LoadQBladeProject(QString PathName);
        static MainFrame* self();
        
/////////////// new NM ///////////////
signals:
		void moduleChanged ();
/////////// end new NM ///////////////
		/////////////// new JW ///////////////
		void viewChanged (int newView, int newApp);
		/////////////// end new JW ///////////////

public slots:
	void OnAFoil();
	void OnXDirect();
	void OnXInverse();
	void OnXInverseMixed();
	void OnMiarex();
        ////////////////////new code DM////////////////
        void OnBEM();
        void AboutQBlade();
        void AboutFASTAerodyn();
        //////////////////////end new code DM///////////
        ////////////////////new code JW////////////////
        void OnDMS();
		void onViewChanged (int newView, int newApp);
        void OnHAWTView();
        void OnVAWTView();
        void OnNoiseView();

        //////////////////////end new code JW//////////
	/////////////// new NM ///////////////
	void onModuleChanged ();  // will hide associated modules if no longer active
	/////////// end new NM ///////////////
	

public slots:
	void AboutQFLR5();
	void OnCurFoilStyle();
	void OnDuplicateFoil();
	void OnGuidelines();
	void OnExportCurGraph();
	void OnInsertProject();
	void OnNewProject();
	void OnLanguage();
	void OnLoadFile();
	void OnLogFile();
	void OnRenameCurFoil();
	void OnResetCurGraphScales();
	void OnResetSettings();
	void OnSaveSettings();// new JW method
	void OnRestoreToolbars();
	bool OnSaveProjectAs();
	void OnSaveViewToImageFile();
    void OnSelChangeFoil(int i);
    void OnSelChangePolar(int i);
    void OnSelChangeOpp(int i);
	void OnSelChangeUFO(int i);
	void OnSelChangeWOpp(int i);
	void OnSelChangeWPolar(int i);
	void OnSaveProject();
	void OnStyle();
	void OnUnits();
	void OnSaveUFOAsProject();
	void OnPolarProps();
	void OnWOppProps();
	void openRecentFile();
    void SetSaveStateFalse();




/*___________________________________________Methods_______________________________*/
	public:  // NM new public interface
		void setIApp (int iApp);
		void setIView (int newView, int newApp);
		void switchToTwoDWidget ();
		void switchToGlWidget () { m_centralWidget->setCurrentIndex(1); }
		void setCurrentModule (ModuleBase *newModule);
		ModuleBase* getCurrentModule () { return m_currentModule; }
		TwoDWidgetInterface* getTwoDWidgetInterface () { return m_twoDWidgetInterface; }
		GLWidget* getGlWidget () { return m_glWidget; }
		TwoDWidget* getTwoDWidget () { return m_twoDWidget; }
		QColor getBackgroundColor () { return m_BackgroundColor; }
    
public:
        ////////////new code DM//////////////
        void CreateBEMToolbar();
        void CreateBEMActions();
        void CreateBEMMenus();
        void CreateNoiseMenus();
        void CreateMainToolbar();
        //void OnBem();
        /////////end new code DM//////////

        ////////////new code JW//////////////
        void CreateDMSToolbar();
        void CreateDMSActions();
        void CreateDMSMenus();
        void ConnectBEMActions();
        void DisconnectBEMActions();
        void ConnectDMSActions();
        /////////end new codeJW//////////

	void closeEvent (QCloseEvent *event);
	void keyPressEvent (QKeyEvent *event);
	void keyReleaseEvent (QKeyEvent *event);

	void CreateDockWindows();
	void CreateToolbars();
	void CreateStatusBar();
	void CreateActions();
	void CreateMenus();
	void CreateXDirectActions();
	void CreateXDirectMenus();
	void CreateXDirectToolbar();
	void CreateXInverseActions();
	void CreateXInverseMenus();
	void CreateXInverseToolbar();
	void CreateMiarexActions();
	void CreateMiarexMenus();
	void CreateMiarexToolbar();
	void CreateAFoilActions();
	void CreateAFoilMenus();
	void CreateAFoilToolbar();
    void ConnectStores();
    void SetSaveState(bool bSave);



	void ClientToGL(QPoint const &point, CVector &real);
	void DeleteProject();
	void DeletePlane(void *pPlanePtr, bool bResultsOnly = false);
	void DeleteWing(void *pWingPtr, bool bResultsOnly = false);
	void GLToClient(CVector const &real, QPoint &point);
    void RemoveOpPoint();
	void SetCurrentFoil(CFoil* pFoil);
	void SaveSettings();
	void SelectOpPoint(OpPoint *pOpp);
	void SelectWOpp(double x);
	void SetCentralWidget();
	void SetDlgPos(QDialog &Dlg);
	void SetGraphSettings(Graph *pGraph);
	void SetProjectName(QString PathName);
	void SetMenus();
	void UpdateFoils();
	void UpdatePolars();
	void UpdateOpps();
	void UpdateUFOs();
	void UpdateWPolars();
	void UpdateWOpps();

	void UpdateView();
	void WritePolars(QDataStream &ar, CFoil *pFoil=NULL, int ProjectFormat=6);

	bool SaveProject(QString PathName="");
	bool LoadSettings();
	bool LoadPolarFileV3(QDataStream &ar, bool bIsStoring, int ArchiveFormat=0);
	bool SerializeProject(QDataStream &ar, bool bIsStoring, int ProjectFormat =6);
	bool SerializeUFOProject(QDataStream &ar, int ProjectFormat=5);
	bool DeleteFoil(CFoil *pFoil, bool bAsk=true);

	void RenameFoil(CFoil *pFoil);

	CFoil* GetFoil(QString strFoilName);
	CFoil* ReadFoilFile(QTextStream &ar);
	CFoil* ReadPolarFile(QDataStream &ar);
    CFoil* StoreFoil(CFoil* pNewFoil);
	CPolar *GetPolar(QString m_FoilName, QString PolarName);
	CPolar* AddPolar(CPolar *pPolar);
	OpPoint *GetOpp(double Alpha);
	QColor GetColor(int type);

	//recent file
	QString ShortenFileName(QString &PathName);
	void AddRecentFile(const QString &PathNAme);
	void updateRecentFileActions();

/*___________________________________________Variables_______________________________*/
public:
	bool m_bMaximized;
	QString m_StyleName;
	QString m_VersionName;
	QString m_GraphExportFilter;
	QToolBar*m_pctrlMainToolBar;

public:
        ////////////new code DM////////////////
        QAction *On360ViewAct, *OnBladeViewAct, *OnRotorViewAct, *OnTurbineViewAct, *OnCharacteristicViewAct;
        QAction *RotorGraphAct, *BladeGraphAct, *ShowAllRotorCurvesAct, *HideAllRotorCurvesAct, *IsolateCurrentBladeCurveAct, *CompareCurrentBladeCurveAct, *HideAllPolarsAct, *ShowAllPolarsAct;
        QAction *SingleGraphAct, *EditCurrentBladeAct, *EditCurrentTurbineAct, *BEMGraphDlg, *AboutBEMAct, *MainWindAct, *ParamWindAct, *MainPitchAct, *ParamNoneAct, *ParamPitchAct, *MainRotAct, *ParamRotAct;
        QAction *DeleteCurrentBladeAct, *DeleteCurrent360PolarAct, *DeleteCurrentTurbineAct, *DeleteCurrentRotorSimAct, *DeleteCurrentCharSimAct, *DeleteCurrentTurbineSimAct, *ImportPolarAct, *ExportPolarAct, *Export360PolarAct, *ExportBladeGeomAct, *ExportBladeTableAct, *LoadCylindricFoilAct, *Edit360PolarAct;
        QAction *ExportCurrentRotorAct, *ExportCurrentRotorAeroDynAct, *HAWTToolbarView, *VAWTToolbarView, *AboutFASTAct;  // new code NM
        QMenu *BEMCtxMenu, *TurbineCtxMenu, *PolarCtxMenu, *CharCtxMenu, *NoiseViewMenu, *BEMViewMenu, *BEMBladeMenu, *BEM360PolarMenu, *BEMTurbine, *currentBladeMenu, *currentBladeMenu2, *currentTurbineMenu, *current360PolarMenu, *currentBEMGraphMenu, *AboutBEMMenu;
        /////////////end new code DM////////////


        ////////////new code JW////////////////
        QAction *OnBladeViewAct2, *OnRotorViewAct2, *OnTurbineViewAct2, *OnCharacteristicViewAct2;
        QAction *AziGraphAct;
		QAction *WeibullGraphAct;
        /////////////end new code JW////////////

	//the dialog boxes are declared as member variables to enable translations... seems to be a Qt bug
	DisplaySettingsDlg m_DisplaySettingsDlg;
	RenameDlg m_RenameDlg;
	UnitsDlg m_UnitsDlg;

	void *m_pXInverse;
	void *m_pXDirect;
	void *m_pMiarex;
	void *m_pAFoil;
	void *m_pStabView;
	static QPointer<MainFrame> _self;

	QDockWidget *m_pctrlXDirectWidget, *m_pctrlMiarexWidget, *m_pctrlAFoilWidget, *m_pctrlXInverseWidget;
	QDockWidget *m_pctrl3DScalesWidget, *m_pctrlStabViewWidget;

	QToolBar *m_pctrlXDirectToolBar, *m_pctrlXInverseToolBar;
	QToolBar *m_pctrlMiarexToolBar;
	QToolBar *m_pctrlAFoilToolBar;

	//Common Menus
	QMenu * MainMenu;
	QMenu *fileMenu, *optionsMenu, *helpMenu;

	//AFoilMenus
	QMenu *AFoilViewMenu, *AFoilDesignMenu, *AFoilSplineMenu;
	QMenu *AFoilCtxMenu,*AFoilCurrentFoilMenu, *AFoilTableCtxMenu;

	//  XFoilAnalysis Menus
	QMenu * XDirectViewMenu;
	QMenu *FoilMenu, *CurGraphCtxMenu, *CurOppCtxMenu;
	QMenu *currentFoilMenu;
	QMenu *DesignMenu;
	QMenu *XFoilAnalysisMenu;
	QMenu *OpPointMenu, *CpGraphMenu, *currentOppMenu;
	QMenu *PolarMenu, *currentPolarMenu, *GraphPolarMenu, *CurPolarGraphMenu;
	QMenu *OperFoilCtxMenu, *OperPolarCtxMenu, *CurXFoilResults;

	//XInverse menu
	QMenu *XInverseViewMenu, *InverseFoilMenu, *InverseGraphMenu, *InverseContextMenu;

	//Miarex Menus
	QMenu *MiarexViewMenu;
	QMenu *MiarexAnalysisMenu;
	QMenu *UFOMenu, *currentUFOMenu, *CurWPlrMenu, *CurWOppMenu, *CurBodyMenu;
	QMenu *MiarexBodyMenu, *MiarexWPlrMenu, *MiarexWOppMenu;
	QMenu *WPlrGraphMenu,*WPlrCurGraphMenu, *WOppGraphMenu, *WOppCurGraphMenu;
	QMenu *WPlrCtxMenu, *WOppCtxMenu, *W3DCtxMenu, *WCpCtxMenu, *WTimeCtxMenu, *W3DStabCtxMenu;

	//MainFrame actions
	QAction *OnXDirectAct, *OnMiarexAct, *OnAFoilAct, *OnXInverseAct, *OnMixedInverseAct;
	QAction *openAct, *insertAct, *styleAct;
    QAction *saveAct, *saveProjectAsAct,*newProjectAct, *closeProjectAct;
	QAction *unitsAct;
	QAction *languageAct;
	QAction *exitAct;
	QAction *aboutAct, *guidelinesAct, *aboutQtAct;
	QAction *recentFileActs[MAXRECENTFILES];
	QAction *separatorAct;
	QAction *saveViewToImageFileAct, *resetSettingsAct;
	QAction *saveSettingsAct;// new code JW

	//AFoil Actions
	QAction *zoomInAct, *ResetXScaleAct, *ResetYScaleAct, *ResetXYScaleAct;
	QAction *zoomYAct, *zoomLessAct, *AFoilGridAct;
    QAction *AFoilDelete, *AFoilRename, *AFoilExport, *AFoilImport;
	QAction *AFoilSetTEGap, *AFoilSetLERadius, *AFoilSetFlap, *AFoilInterpolateFoils, *AFoilNacaFoils;
	QAction *AFoilDerotateFoil, *AFoilNormalizeFoil, *AFoilRefineLocalFoil, *AFoilRefineGlobalFoil;
	QAction *AFoilEditCoordsFoil, *AFoilScaleFoil;
	QAction *AFoilLECircle, *m_pShowLegend;
	QAction *UndoAFoilAct, *RedoAFoilAct;
	QAction *HideAllFoils, *ShowAllFoils, *ShowCurrentFoil, *HideCurrentFoil;
	QAction *SplinesAct, *storeSplineAct, *newSplinesAct, *splineControlsAct, *exportSplinesToFileAct;
	QAction *InsertSplinePt, *RemoveSplinePt;
	QAction *AFoilTableColumns, *AFoilTableColumnWidths;
	QAction *AFoilLoadImage, *AFoilClearImage;
	QToolButton *m_pctrlZoomY, *m_pctrlZoomIn;

	//Miarex Actions
	QAction *WPolarAct, *WOppAct, *W3DAct, *CpViewAct, *StabTimeAct, *RootLocusAct;
	QAction *W3DPrefsAct, *W3DLightAct, *W3DScalesAct;
	QAction *DefineWingAct, *DefinePlaneAct, *EditUFOAct, *SaveUFOAsProject;
	QAction *renameCurUFO, *deleteCurUFO, *duplicateCurUFO;
	QAction *renameCurWPolar, *editCurWPolar,*exportCurWPolar, *resetCurWPolar;
	QAction *ShowPolarProps, *ShowWOppProps;
	QAction *deleteCurWPolar, *deleteCurWOpp;
	QAction *twoGraphs, *fourGraphs;
	QAction *Graph1,*Graph2,*Graph3,*Graph4;
	QAction *MiarexGraphDlg;
	QAction *highlightWOppAct;

	QAction *ResetWingGraphScale;
	QAction *MiarexPolarFilter;
	QAction *allWPolarGraphsScalesAct, *allWingGraphsScalesAct, *allWPolarGraphsSettings, *allWingGraphsSettings;
	QAction *hideAllWPlrs, *showAllWPlrs;
	QAction *hideUFOWPlrs, *showUFOWPlrs, *deleteUFOWPlrs;
	QAction *hideUFOWOpps, *showUFOWOpps, *deleteUFOWOpps;
	QAction *resetWOppLegend, *resetWPlrLegend;
	QAction *exportCurWOpp, *showCurWOppOnly, *hideAllWOpps, *showAllWOpps, *deleteAllWOpps;
	QAction *showAllWPlrOpps, *hideAllWPlrOpps, * deleteAllWPlrOpps;
	QAction *defineWPolar, *defineStabPolar, *advancedSettings;
	QAction *defineBody, *importBody, *exportBodyDef, *exportBodyGeom, *ManageBodies, *EditCurBody;
	QAction *HalfWingAct;
	QAction *showEllipticCurve, *showXCmRefLocation, *showStabCurve, *showFinCurve, *showWing2Curve;
	QAction *exporttoAVL, *resetWingScale, *scaleWingAct;
	QAction *ManageUFOs;
	QAction *m_pImportWPolar, *m_pUFOInertia;
	QToolButton *m_pctrl3dView, *m_pctrlWPolarView, *m_pctrlWOppView, *m_pctrlCpView;
	QToolButton *m_pctrlStabilityButton, *m_pctrlRootLocusButton;//, *m_pctrlModalViewButton;


	//XDirect Actions
	QAction *PolarsAct, *OpPointsAct, *deletePolar, *definePolar, *editCurPolar, *defineBatch, *resetCurPolar;
	QAction *MultiThreadedBatchAct;
	QAction *restoreToolbarsAct;
    QAction *exportCurPolar, *exportAllPolars, *exportAllPolarsNRELAct, *exportAll360PolarsNRELAct, *hideFoilPolars, *showFoilPolars, *saveFoilPolars,*deleteFoilPolars;
	QAction *showAllPolars, *hideAllPolars, *showCurOppOnly, *showAllOpPoints, *hideAllOpPoints;
    QAction *hideFoilOpps, *showFoilOpps, *deleteFoilOpps, *OnImport360PolarAct, *OnImportBladeGeometry;
	QAction *hidePolarOpps, *showPolarOpps, *deletePolarOpps;
	QAction *exportCurOpp, *deleteCurOpp, *resetXFoil;
	QAction *viewXFoilAdvanced, *viewLogFile, *showPanels, *showNeutralLine, *resetFoilScale, *showInviscidCurve;
	QAction *exportCurFoil, *deleteCurFoil, *renameCurFoil, *setCurFoilStyle;
	QAction *DerotateFoil, *NormalizeFoil, *RefineLocalFoil, *RefineGlobalFoil , *EditCoordsFoil, *ScaleFoil;
	QAction *SetTEGap, *SetLERadius, *SetFlap, *InterpolateFoils, *NacaFoils, *DuplicateFoil;
	QAction *XDirectGraphDlg,*exportCurGraphAct, *resetCurGraphScales, *allPolarGraphsSettingsAct, *allPolarGraphsScales;
	QAction *TwoPolarGraphsAct, *AllPolarGraphsAct, *resetGraphLegend;
	QAction *PolarGraphAct[5];
	QAction *XDirectStyleAct;
	QAction *XDirectPolarFilter;
	QAction *defineCpGraphSettings, *exportCpGraphAct, *setQVarGraph, *setCpVarGraph;
	QAction *CurXFoilResExport, * CurXFoilCtPlot, *CurXFoilDbPlot, *CurXFoilDtPlot, *CurXFoilRtLPlot;
	QAction *CurXFoilRtPlot, *CurXFoilNPlot, *CurXFoilCdPlot, *CurXFoilCfPlot, *CurXFoilUePlot, *CurXFoilHPlot;
	QAction *ManageFoilsAct, *RenamePolarAct;
	QAction *m_pImportXFoilPolar;
	QAction *highlightOppAct;
	QToolButton *m_pctrlPolarView, *m_pctrlOppView;

    FoilComboBox *m_pctrlFoil;
    PolarComboBox *m_pctrlPolar;
    OpPointComboBox *m_pctrlOpPoint;

	QComboBox *m_pctrlUFO, *m_pctrlWPolar, * m_pctrlWOpp;
	QRadioButton *m_pctrlFullInverse, *m_pctrlMixedInverse;
	QLabel *m_pctrlProjectName;

	//XInverse Actions
    QAction *SaveFoil, *ExtractFoil, *InverseStyles, *InverseResetScale, *InverseInsertCtrlPt, *InverseRemoveCtrlPt;
	QAction *InvQInitial, *InvQSpec, *InvQViscous, *InvQPoints, *InvQReflected;
	QAction *InverseResetGraphScale, *XInverseGraphDlg, *InverseZoomIn, *InverseZoomX, *InverseZoomY;
	QToolButton *m_pctrlInvZoomX, *m_pctrlInvZoomY, *m_pctrlInvZoomIn;

	QStringList m_RecentFiles;

	QList <void *> m_oaPlane;
	QList <void *> m_oaWing;
	QList <void *> m_oaWPolar;
	QList <void *> m_oaWOpp;
	QList <void *> m_oaPOpp;
	QList <void *> m_oaBody;


	int m_iApp;
	int m_iView;// JW variable

	bool m_bSaved;
	bool m_bSaveOpps;
	bool m_bSaveWOpps;
	bool m_bSaveSettings;
	bool m_bReverseZoom;                    // true if the rolling forward zooms in
	bool m_bHighlightOpp, m_bHighlightWOpp;


	QString m_ProjectName, m_FileName, m_LanguageFilePath;
	QString m_LastDirName, m_ExportLastDirName, m_ImageDirName;
	QString m_UFOType;
	QColor m_crColors[30];

	QGraph m_RefGraph;//Reference setttings
	QColor m_BorderClr;

	int m_ImageFormat;

public:
 	QFont m_TextFont;
	QColor m_TextColor;
	QColor m_BackgroundColor;
	QPoint m_DlgPos;//preferred position for dialog boxes
	void *m_pGL3DScales;
	double m_mtoUnit;
	double m_mstoUnit;
	double m_m2toUnit;
	double m_kgtoUnit;
	double m_NtoUnit;
	double m_NmtoUnit;
	int m_LengthUnit;
	int m_AreaUnit;
	int m_WeightUnit;
	int m_SpeedUnit;
	int m_ForceUnit;
	int m_MomentUnit;
	int m_ExportFileType;
	bool m_bAlphaChannel;
	
	///////////////////////////new code DM////////////////////
	ScrolledDock *m_pctrlBEMWidget, *m_pctrlSimuWidget;
	QDockWidget *m_pctrlGL3dWingWidget;
	void *m_pBEM;
	void *m_pSimuWidget;
	int m_PowerUnit;
	double m_WtoUnit;
	////////////////////////////end new code DM////////////////
	
	///////////////////////////new code JW////////////////////
	//QAction *OnDMSAct;
	ScrolledDock *m_pctrlDMSWidget, *m_pctrlSimuWidgetDMS;
	void *m_pDMS;
	void *m_pQFEM;
	void *m_pSimuWidgetDMS;
    QSize BladeDockSize, VBladeDockSize, TurbineDockSize;
	QAction *HideWidgetsAct;
	////////////////////////////end new code JW////////////////
	
private:  // NM new private section. Privatize here step by step
	QStackedWidget *m_centralWidget;
	TwoDWidget *m_twoDWidget;
	GLWidget *m_glWidget;
	
	ModuleBase *m_currentModule;
	TwoDWidgetInterface *m_twoDWidgetInterface;
};

extern MainFrame *g_mainFrame;

#endif // MAINFRAME_H
