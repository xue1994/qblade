/****************************************************************************

	BodyTransDlg Class
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

#ifndef BODYTRANSDLG_H
#define BODYTRANSDLG_H

#include <QDialog>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>

#include "../Misc/NumberEdit.h"

class BodyTransDlg : public QDialog
{
	Q_OBJECT
	friend class QMiarex;
	friend class CBody;
	friend class GL3dBodyDlg;

public:
    BodyTransDlg();
	void InitDialog();

private slots:
	void OnOK();
	void OnFrameOnly();

private:
	void keyPressEvent(QKeyEvent *event);
	void SetupLayout();

	NumberEdit *m_pctrlXTransFactor;
	NumberEdit *m_pctrlYTransFactor;
	NumberEdit *m_pctrlZTransFactor;
	NumberEdit *m_pctrlFrameID;
	QCheckBox *m_pctrlFrameOnly;
	QLabel *m_pctrlLength1;
	QLabel *m_pctrlLength2;
	QLabel *m_pctrlLength3;
	QPushButton *OKButton, *CancelButton;

	void * m_pMiarex;
	void * m_pMainFrame;

	double m_XTrans, m_YTrans, m_ZTrans;
	bool   m_bFrameOnly;
	int    m_FrameID;

};

#endif // BODYTRANSDLG_H
