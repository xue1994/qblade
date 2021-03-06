/****************************************************************************

	InertiaDlg Class
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

#ifndef INERTIADLG_H
#define INERTIADLG_H

#include <QDialog>
#include <QLabel>
#include <QStackedWidget>
#include <QRadioButton>
#include <QPushButton>
#include <QTableView>
#include <QStandardItemModel>

#include "../Params.h"
#include "../Misc/FloatEditDelegate.h"
#include "../Misc/NumberEdit.h"
#include "../Objects/CVector.h"
#include "../Objects/Plane.h"
//#include "../Objects/Body.h"

class InertiaDlg : public QDialog
{
	Q_OBJECT
	friend class GL3dWingDlg;
	friend class GL3dBodyDlg;
	friend class PlaneDlg;
	friend class QMiarex;
	friend class MainFrame;

public:
    InertiaDlg();
	void InitDialog();

private slots:
	void OnOK();
	void OnBodyInertia();
	void OnWingInertia();
	void OnWing2Inertia();
	void OnStabInertia();
	void OnFinInertia();
	void OnCellChanged(QWidget *pWidget=NULL);
	void OnExportToAVL();
	void OnInsertMassRow();
	void OnDeleteMassRow();

private:
	void contextMenuEvent(QContextMenuEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void resizeEvent(QResizeEvent *event);
	void showEvent(QShowEvent *event);
	void FillMassModel();
	void ComputeInertia();
//	void ComputeBodyInertia(double &Ixx, double &Iyy, double &Izz, double &Ixz);
	void CleanEmptyRows();
	void ComputeBodyAxisInertia();
	void SetupLayout();
	void ReadData();

	CBody *m_pBody;
	CWing *m_pBlade;
	CPlane *m_pPlane;
	static void *s_pMainFrame;

	QStackedWidget *m_pctrlTopStack;
	QPushButton *m_pctrlWingInertia, *m_pctrlWing2Inertia, *m_pctrlStabInertia, *m_pctrlFinInertia, *m_pctrlBodyInertia;
	QPushButton *OKButton;
	QLabel *m_pctrlMassUnit, *m_pctrlMassUnit2, *m_pctrlLengthUnit, *m_pctrlLengthUnit2;
	QLabel *m_pctrlVolumeMassLabel, *m_pctrlTotalMassLabel;
	QLabel *m_pctrlInertiaUnit1, *m_pctrlInertiaUnit2;
	QTableView *m_pctrlMassView;
	QStandardItemModel *m_pMassModel;
	FloatEditDelegate *m_pFloatDelegate;
	NumberEdit *m_pctrlCoGIxx, *m_pctrlCoGIyy, *m_pctrlCoGIzz, *m_pctrlCoGIxz;
	NumberEdit *m_pctrlXCoG,*m_pctrlYCoG,*m_pctrlZCoG;
	NumberEdit *m_pctrlVolumeMass;
	
	NumberEdit *m_pctrlTotalIxx, *m_pctrlTotalIyy, *m_pctrlTotalIzz, *m_pctrlTotalIxz;
	NumberEdit *m_pctrlXTotalCoG,*m_pctrlYTotalCoG,*m_pctrlZTotalCoG;
	NumberEdit *m_pctrlTotalMass;

	QMenu *m_pContextMenu;
	QAction *m_pInsertMassRow, *m_pDeleteMassRow;

	double m_VolumeMass;
	CVector m_VolumeCoG;

	CVector m_PtRef;
	double m_CoGIxx, m_CoGIyy, m_CoGIzz, m_CoGIxz;
	double m_MassValue[MAXMASSES];
	CVector m_MassPosition[MAXMASSES];
	QString m_MassTag[MAXMASSES];
	int m_NMass;

	bool m_bChanged;
};

#endif // INERTIADLG_H
