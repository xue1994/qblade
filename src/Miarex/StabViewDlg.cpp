/****************************************************************************

	StabViewDlg Class
	Copyright (C) 2010 Andre Deperrois adeperrois@xflr5.com

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

#include "../Globals.h"
#include "../MainFrame.h"
#include "../Misc/NewNameDlg.h"
#include "StabViewDlg.h"
#include "Miarex.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QTimer>
#include <complex>


void *StabViewDlg::s_pMiarex;
void *StabViewDlg::s_pMainFrame;


StabViewDlg::StabViewDlg(QWidget */*parent*/)
{
	setWindowTitle(tr("Stability View Params"));
	setWindowFlags(Qt::Tool);
	m_iCurrentMode = 0;
	m_ModeAmplitude = 1.0;
	m_ModeInterval = 200;
	m_pCurve = NULL;
	for(int i=0; i<20; i++)
	{
		m_Time[i] = (double)i;
		m_Amplitude[i] = 0.0;
	}
	SetupLayout();
	Connect();
}


void StabViewDlg::showEvent(QShowEvent */*event*/)
{
//	SetControls();
}


void StabViewDlg::Connect()
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;

	connect(m_pctrlLongDynamics, SIGNAL(clicked()), pMiarex, SLOT(OnStabilityDirection()));
	connect(m_pctrlLatDynamics, SIGNAL(clicked()), pMiarex, SLOT(OnStabilityDirection()));

	connect(m_pctrlPlotStabGraph, SIGNAL(clicked()), this , SLOT(OnPlotStabilityGraph()));

	connect(m_pctrlRLMode1, SIGNAL(clicked()), this, SLOT(OnModeSelection()));
	connect(m_pctrlRLMode2, SIGNAL(clicked()), this, SLOT(OnModeSelection()));
	connect(m_pctrlRLMode3, SIGNAL(clicked()), this, SLOT(OnModeSelection()));
	connect(m_pctrlRLMode4, SIGNAL(clicked()), this, SLOT(OnModeSelection()));
	connect(m_pctrlTimeMode1, SIGNAL(clicked()), this, SLOT(OnModeSelection()));
	connect(m_pctrlTimeMode2, SIGNAL(clicked()), this, SLOT(OnModeSelection()));
	connect(m_pctrlTimeMode3, SIGNAL(clicked()), this, SLOT(OnModeSelection()));
	connect(m_pctrlTimeMode4, SIGNAL(clicked()), this, SLOT(OnModeSelection()));
	
	connect(m_pctrlAnimate ,SIGNAL(clicked()), this, SLOT(OnAnimate()));
	connect(m_pctrlAnimationSpeed ,SIGNAL(valueChanged(int)), this, SLOT(OnAnimationSpeed(int)));
	connect(m_pctrlAnimationAmplitude ,SIGNAL(valueChanged(int)), this, SLOT(OnAnimationAmplitude(int)));
	connect(m_pctrlAnimateRestart ,SIGNAL(clicked()), this, SLOT(OnAnimateRestart()));
	connect(m_pctrlDeltat, SIGNAL(editingFinished()), this, SLOT(OnReadData()));
	connect(m_pctrlModeStep, SIGNAL(editingFinished()), this, SLOT(OnReadData()));
//	connect(m_pCtrlDelegate, SIGNAL(closeEditor(QWidget *)), this, SLOT(OnCellChanged(QWidget *)));

	connect(m_pctrlInitCondResponse, SIGNAL(clicked()), this, SLOT(OnResponseType()));
	connect(m_pctrlForcedResponse, SIGNAL(clicked()), this, SLOT(OnResponseType()));
	connect(m_pctrlModalResponse, SIGNAL(clicked()), this, SLOT(OnResponseType()));
	
	connect(m_pctrlAddCurve, SIGNAL(clicked()), this, SLOT(OnAddCurve()));
	connect(m_pctrlDeleteCurve, SIGNAL(clicked()), this, SLOT(OnDeleteCurve()));
	connect(m_pctrlRenameCurve, SIGNAL(clicked()), this, SLOT(OnRenameCurve()));
	connect(m_pctrlCurveList,   SIGNAL(activated(int)), this, SLOT(OnSelChangeCurve(int)));
	
	m_pControlModel = new QStandardItemModel;
	m_pControlModel->setRowCount(20);//temporary
	m_pControlModel->setColumnCount(2);
	m_pControlModel->setHeaderData(0, Qt::Horizontal, tr("Time (s)"));
	m_pControlModel->setHeaderData(1, Qt::Horizontal, tr("Angle ")+QString::fromUtf8("(deg)"));
//	m_pControlModel->setHeaderData(2, Qt::Horizontal, tr("Ramp (s)"));


	m_pctrlControlTable->setModel(m_pControlModel);
	m_pctrlControlTable->setWindowTitle(tr("Controls"));
	m_pctrlControlTable->setColumnWidth(0,50);
	m_pctrlControlTable->setColumnWidth(1,40);
//	m_pctrlControlTable->setColumnWidth(2,60);
	QHeaderView *HorizontalHeader = m_pctrlControlTable->horizontalHeader();
	HorizontalHeader->setStretchLastSection(true);
}


void StabViewDlg::UpdateControlModelData()
{
	QModelIndex ind;
	for(int i=0; i<m_pControlModel->rowCount(); i++)
	{
		ind = m_pControlModel->index(i, 0, QModelIndex());
		m_pControlModel->setData(ind, m_Time[i]);
		ind = m_pControlModel->index(i, 1, QModelIndex());
		m_pControlModel->setData(ind, m_Amplitude[i]);
	}
}


void StabViewDlg::ReadControlModelData()
{
	for(int i=0; i<m_pControlModel->rowCount(); i++)
	{
		m_Time[i]      = m_pControlModel->index(i, 0, QModelIndex()).data().toDouble();
		m_Amplitude[i] = m_pControlModel->index(i, 1, QModelIndex()).data().toDouble();
	}
}


void StabViewDlg::FillEigenThings()
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	complex<double> c;
	double OmegaN, Omega1, Dsi, Sigma1;
	double sum, prod;
	QString strange;
	double u0, mac, span;
	complex<double> angle;

	QString ModeDescription = tr("Mode Properties:")+"\n";

	if(pMiarex->m_pCurWing && pMiarex->m_pCurWOpp && pMiarex->m_pCurWPolar->m_WPolarType==STABILITYPOLAR)
	{
		//We normalize the mode before display and only for display purposes
		u0   = pMiarex->m_pCurWOpp->m_QInf;
		mac  = pMiarex->m_pCurWing->m_MAChord;
		span = pMiarex->m_pCurWing->m_PlanformSpan;

		c = pMiarex->m_pCurWOpp->m_EigenValue[m_iCurrentMode];
		if(c.imag()>=0.0) strange = QString("%1+%2i").arg(c.real(),9,'f',4).arg(c.imag(),9,'f',4);
		else              strange = QString("%1-%2i").arg(c.real(),9,'f',4).arg(fabs(c.imag()),9,'f',4);
		m_pctrlEigenValue->setText(strange);
		ModeDescription.append("Lambda="+strange+"\n");

		sum  = c.real() * 2.0;                          // is a real number
		prod = c.real()*c.real() + c.imag()*c.imag();  // is a positive real number
		OmegaN = fabs(c.imag());
		if(OmegaN>PRECISION) Omega1 = sqrt(prod);
		else                 Omega1 = 0.0;
		Sigma1 = sum /2.0;
		if(Omega1 > PRECISION) Dsi = -Sigma1/Omega1;
		else                   Dsi = 0.0;

		m_pctrlFreqN->setValue(OmegaN/2.0/PI);
		m_pctrlFreq1->setValue(Omega1/2.0/PI);
		m_pctrlDsi->setValue(Dsi);
		strange = QString("FN=%1 Hz").arg(OmegaN/2.0/PI,6,'f',3);
		ModeDescription.append(strange+"\n");
		strange = QString("F1=%1 Hz").arg(Omega1/2.0/PI,6,'f',3);
		ModeDescription.append(strange+"\n");
		strange = QString("Xi=%1").arg(Dsi,6,'f',3);
		ModeDescription.append(strange+"\n");


		if(pMiarex->m_bLongitudinal && pMiarex->m_pCurWOpp)
		{
			angle = pMiarex->m_pCurWOpp->m_EigenVector[m_iCurrentMode][3];
			c = pMiarex->m_pCurWOpp->m_EigenVector[m_iCurrentMode][0]/u0;
			if(c.imag()>=0.0) strange = QString("%1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("%1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			m_pctrlEigenVector1->setText(strange);
			ModeDescription.append("v1="+strange+"\n");

			c = pMiarex->m_pCurWOpp->m_EigenVector[m_iCurrentMode][1]/u0;
			if(c.imag()>=0.0) strange = QString("%1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("%1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			m_pctrlEigenVector2->setText(strange);
			ModeDescription.append("v2="+strange+"\n");

			c = pMiarex->m_pCurWOpp->m_EigenVector[m_iCurrentMode][2]/(2.0*u0/mac);
			if(c.imag()>=0.0) strange = QString("%1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("%1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			m_pctrlEigenVector3->setText(strange);
			ModeDescription.append("v3="+strange+"\n");

			c = pMiarex->m_pCurWOpp->m_EigenVector[m_iCurrentMode][3]/angle;
			if(c.imag()>=0.0) strange = QString("%1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("%1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			m_pctrlEigenVector4->setText(strange);
			ModeDescription.append("v4="+strange+"\n");
		}
		else if(!pMiarex->m_bLongitudinal && pMiarex->m_pCurWOpp)
		{
			angle = pMiarex->m_pCurWOpp->m_EigenVector[m_iCurrentMode][3];

			c = pMiarex->m_pCurWOpp->m_EigenVector[m_iCurrentMode][0]/u0;
			if(c.imag()>=0.0) strange = QString("%1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("%1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			m_pctrlEigenVector1->setText(strange);
			ModeDescription.append("v1="+strange+"\n");

			c = pMiarex->m_pCurWOpp->m_EigenVector[m_iCurrentMode][1]/(2.0*u0/span);
			if(c.imag()>=0.0) strange = QString("%1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("%1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			m_pctrlEigenVector2->setText(strange);
			ModeDescription.append("v2="+strange+"\n");

			c = pMiarex->m_pCurWOpp->m_EigenVector[m_iCurrentMode][2]/(2.0*u0/span);
			if(c.imag()>=0.0) strange = QString("%1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("%1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			m_pctrlEigenVector3->setText(strange);
			ModeDescription.append("v3="+strange+"\n");

			c = pMiarex->m_pCurWOpp->m_EigenVector[m_iCurrentMode][3]/angle;
			if(c.imag()>=0.0) strange = QString("%1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("%1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			m_pctrlEigenVector4->setText(strange);
			ModeDescription.append("v4="+strange+"\n");

		}
		m_pctrlModeProperties->setText(ModeDescription);
	}
	else
	{
		m_pctrlEigenValue->clear();
		m_pctrlEigenVector1->clear();
		m_pctrlEigenVector2->clear();
		m_pctrlEigenVector3->clear();
		m_pctrlEigenVector4->clear();
		m_pctrlFreqN->clear();
		m_pctrlFreq1->clear();
		m_pctrlDsi->clear();
		m_pctrlModeProperties->clear();
	}
}


void StabViewDlg::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Return:
		{
			if(!m_pctrlPlotStabGraph->hasFocus()) m_pctrlPlotStabGraph->setFocus();
			else OnPlotStabilityGraph();
			 
			break;
		}
		case Qt::Key_Escape:
		{
			if(m_pctrlAnimate->isChecked()) m_pctrlAnimate->setChecked(false);
			OnAnimate();
			break;
		}
		default:
		{
			QMiarex * pMiarex = (QMiarex*)s_pMiarex;
			pMiarex->keyPressEvent(event);
		}
//		event->ignore();
	}
}


void StabViewDlg::OnAnimate()
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(m_pctrlAnimate->isChecked())
	{
//		pMiarex->m_iView = WSTABVIEW;
		pMiarex->m_iStabilityView=3;
		pMiarex->SetControls();
		
		pMiarex->m_Modedt = m_pctrlModeStep->getValue();
		pMiarex->m_bAnimateMode  = true;
		int speed = m_pctrlAnimationSpeed->value();
		pMiarex->m_pTimerMode->setInterval(400-speed);
		pMiarex->m_pTimerMode->start();
	}
	else
	{
		pMiarex->StopAnimate();
	}
}



void StabViewDlg::OnAnimationAmplitude(int val)
{
	m_ModeAmplitude = (double)val/500.0;
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	pMiarex->OnAnimateModeSingle(false);
}


void StabViewDlg::OnAnimationSpeed(int val)
{
	m_ModeInterval = val;
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	pMiarex->m_pTimerMode->setInterval(400-val);
}


void StabViewDlg::OnAnimateRestart()
{
	double sigma, s2, omega, o2/*, maxso*/;
	double norm1, norm2, theta_sum, psi_sum, ModeState[6];

	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	CWOpp *pWOpp = pMiarex->m_pCurWOpp;
	CWing *pWing = pMiarex->m_pCurWing;

	pMiarex->m_ModeTime = 0.0;

	if(!pWOpp || !pWing)
	{
		pMiarex->m_ModeState[0] = 0.0;
		pMiarex->m_ModeState[1] = 0.0;
		pMiarex->m_ModeState[2] = 0.0;
		pMiarex->m_ModeState[3] = 0.0;
		pMiarex->m_ModeState[4] = 0.0;
		pMiarex->m_ModeState[5] = 0.0;
		pMiarex->UpdateView();
		return;
	}

	sigma = pWOpp->m_EigenValue[m_iCurrentMode].real();
	omega = pWOpp->m_EigenValue[m_iCurrentMode].imag();
	s2 = sigma*sigma;
	o2 = omega*omega;
//	maxso  = qMax(fabs(sigma), fabs(omega));

	//calculate state at t=0 for normalization
	if(pMiarex->m_bLongitudinal)
	{
		//x, z, theta are evaluated by direct inegration of u, w, q
		ModeState[1] = 0.0;
		ModeState[3] = 0.0;
		ModeState[5] = 0.0;
		ModeState[0] = m_vabs[0]/(s2+o2) * (sigma*cos(m_phi[0])+omega*sin(m_phi[0]));
		ModeState[2] = m_vabs[1]/(s2+o2) * (sigma*cos(m_phi[1])+omega*sin(m_phi[1]));
		ModeState[4] = m_vabs[2]/(s2+o2) * (sigma*cos(m_phi[2])+omega*sin(m_phi[2]));
//		ModeState[4] = m_vabs[3]*cos(m_phi[3]);

		//add u0 x theta_sum to z component
		theta_sum      = m_vabs[3]/(s2+o2) * (sigma*cos(m_phi[3])+omega*sin(m_phi[3]));

		ModeState[2] -= theta_sum *pWOpp->m_QInf;
	}
	else
	{
		//y, phi, psi evaluation
		ModeState[0] = 0.0;
		ModeState[2] = 0.0;
		ModeState[4] = 0.0;

		// integrate (v+u0.psi.cos(theta0)) to get y
		ModeState[1] = m_vabs[0]/(s2+o2) * (sigma*cos(m_phi[0])+omega*sin(m_phi[0]));

		//integrate psi = integrate twice r (thanks Matlab !)
		psi_sum =   sigma * ( sigma * cos(m_phi[2]) + omega * sin(m_phi[2]))
				  + omega * (-omega * cos(m_phi[2]) + sigma * sin(m_phi[2]));
		psi_sum *= m_vabs[2]/ (s2+o2)/(s2+o2);

		ModeState[1] += pWOpp->m_QInf * psi_sum;

		// get directly phi from fourth eigenvector component (alternatively integrate p+r.tan(theta0));
		ModeState[3] = m_vabs[3]*cos(m_phi[3]);
//		m_ModeState[3] = m_ModeNorm*m_vabs[1]/(s2+o2) * (sigma*cos(m_phi[1])+omega*sin(m_phi[1]));

		// integrate once 'p+r.sin(theta0)' to get heading angle
		ModeState[5] = m_vabs[2]/(s2+o2) * (sigma*cos(m_phi[2])+omega*sin(m_phi[2]));
	}
	//max 10% span
	norm1 = qMax(fabs(ModeState[0]), fabs(ModeState[1]));
	norm1 = qMax(norm1, fabs(ModeState[2]));
	if(norm1>0.0)  norm1 = pWing->m_PlanformSpan *.10 / norm1;
	else           norm1 = 1.0;

	//max 10degrees
	norm2 = qMax(fabs(ModeState[3]), fabs(ModeState[4]));
	norm2 = qMax(norm2, fabs(ModeState[5]));
	if(norm2>0.0)  norm2 = PI*(10.0/180.0)/ norm2;
	else           norm2 = 1.0;

	pMiarex->m_ModeNorm = qMin(norm1, norm2);

	//set initial mode positions, i.e. t=0
	pMiarex->OnAnimateModeSingle(false);
}


void StabViewDlg::OnCellChanged(QWidget */*pWidget*/)
{
	
}


void StabViewDlg::OnPlotStabilityGraph()
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	if(!pMiarex->m_TimeGraph[0].GetCurveCount())
	{
		//we don't have a curve yet
		// so return
		return;
	}
	
	pMiarex->CreateStabilityCurves();
	pMiarex->UpdateView();
	pMiarex->setFocus();
}


void StabViewDlg::OnModeSelection()
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	if(pMiarex->m_iStabilityView==STABTIMEVIEW)
	{
		if(m_pctrlTimeMode1->isChecked())      m_iCurrentMode = 0;
		else if(m_pctrlTimeMode2->isChecked()) m_iCurrentMode = 1;
		else if(m_pctrlTimeMode3->isChecked()) m_iCurrentMode = 2;
		else if(m_pctrlTimeMode4->isChecked()) m_iCurrentMode = 3;
	}
	else if(pMiarex->m_iStabilityView==STABPOLARVIEW || pMiarex->m_iStabilityView==STAB3DVIEW)
	{
		if(m_pctrlRLMode1->isChecked())      m_iCurrentMode = 0;
		else if(m_pctrlRLMode2->isChecked()) m_iCurrentMode = 1;
		else if(m_pctrlRLMode3->isChecked()) m_iCurrentMode = 2;
		else if(m_pctrlRLMode4->isChecked()) m_iCurrentMode = 3;
	}
	if(!pMiarex->m_bLongitudinal) m_iCurrentMode +=4;
	SetMode(m_iCurrentMode);

	if(pMiarex->m_iStabilityView==STABPOLARVIEW && pMiarex->m_bHighlightOpp)
	{
		pMiarex->CreateStabRLCurves();
		pMiarex->UpdateView();
	}
}


void StabViewDlg::OnReadData()
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	pMiarex->m_Modedt = m_pctrlModeStep->getValue();
	pMiarex->m_Deltat = m_pctrlDeltat->getValue();
}



void StabViewDlg::OnResponseType()
{
	int type=0;
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	
	if(m_pctrlInitCondResponse->isChecked())    type=0;
	else if(m_pctrlForcedResponse->isChecked()) type=1;
	else if(m_pctrlModalResponse->isChecked())  type=2;
	
	if(type==pMiarex->m_StabilityResponseType) return;
	
	pMiarex->m_StabilityResponseType=type;
	SetControls();
//	pMiarex->CreateStabilityCurves();
	pMiarex->UpdateView();
	
}


void StabViewDlg::SetMode(int iMode)
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	if(iMode>=0)
	{
		m_iCurrentMode = iMode%4;
		if(!pMiarex->m_bLongitudinal) m_iCurrentMode += 4;
	}
	else if(m_iCurrentMode<0) m_iCurrentMode=0;

	m_pctrlRLMode1->setChecked(m_iCurrentMode%4==0);
	m_pctrlRLMode2->setChecked(m_iCurrentMode%4==1);
	m_pctrlRLMode3->setChecked(m_iCurrentMode%4==2);
	m_pctrlRLMode4->setChecked(m_iCurrentMode%4==3);
	FillEigenThings();
	CWOpp *pWOpp = pMiarex->m_pCurWOpp;

	if(pWOpp)
	{
		m_vabs[0] = abs(pWOpp->m_EigenVector[m_iCurrentMode][0]);
		m_vabs[1] = abs(pWOpp->m_EigenVector[m_iCurrentMode][1]);
		m_vabs[2] = abs(pWOpp->m_EigenVector[m_iCurrentMode][2]);
		m_vabs[3] = abs(pWOpp->m_EigenVector[m_iCurrentMode][3]);
		m_phi[0]  = arg(pWOpp->m_EigenVector[m_iCurrentMode][0]);
		m_phi[1]  = arg(pWOpp->m_EigenVector[m_iCurrentMode][1]);
		m_phi[2]  = arg(pWOpp->m_EigenVector[m_iCurrentMode][2]);
		m_phi[3]  = arg(pWOpp->m_EigenVector[m_iCurrentMode][3]);
	}
	else
	{
		m_vabs[0] = m_vabs[1] = m_vabs[2] = m_vabs[3] = 0.0;
		m_phi[0] = m_phi[1] = m_phi[2] = m_phi[3] = 0.0;
	}

	pMiarex->m_bResetglLegend = true;

//	if(pMiarex->m_pCurRLStabGraph && pMiarex->m_pCurWPolar) pMiarex->m_pCurRLStabGraph->DeselectPoint();

	OnAnimateRestart();
}


void StabViewDlg::SetupLayout()
{
	QSizePolicy szPolicyMinimum;
	szPolicyMinimum.setHorizontalPolicy(QSizePolicy::Minimum);
	szPolicyMinimum.setVerticalPolicy(QSizePolicy::Minimum);
	QSizePolicy szPolicyMaximum;
	szPolicyMaximum.setHorizontalPolicy(QSizePolicy::Maximum);
	szPolicyMaximum.setVerticalPolicy(QSizePolicy::Maximum);
	setSizePolicy(szPolicyMaximum);

	QFont SymbolFont("Symbol");


	//____________Stability direction__________
	QGroupBox *StabilityDirBox = new QGroupBox(tr("Stability direction"));
	{
		m_pctrlLongDynamics = new QRadioButton(tr("Longitudinal"));
		m_pctrlLatDynamics = new QRadioButton(tr("Lateral"));
		QHBoxLayout *StabilityDirLayout = new QHBoxLayout;
		StabilityDirLayout->addStretch(1);
		StabilityDirLayout->addWidget(m_pctrlLongDynamics);
		StabilityDirLayout->addStretch(1);
		StabilityDirLayout->addWidget(m_pctrlLatDynamics);
		StabilityDirLayout->addStretch(1);
		StabilityDirBox->setLayout(StabilityDirLayout);
	}

	//_______________________Time view Parameters
	QGroupBox *TimeBox = new QGroupBox(tr("Time Graph Params"));
	{
		QVBoxLayout *ResponseTypeLayout = new QVBoxLayout;
		m_pctrlModalResponse = new QRadioButton(tr("Modal Response"));
		m_pctrlModalResponse->setToolTip("Display the time response on a specific mode with normalized amplitude and random initial phase");
		m_pctrlInitCondResponse = new QRadioButton(tr("Initial Conditions Response"));
		m_pctrlInitCondResponse->setToolTip("Display the time response for specific initial conditions");
		m_pctrlForcedResponse = new QRadioButton(tr("Forced Response"));
		m_pctrlForcedResponse->setToolTip("Display the time response for a given control actuation in the form of a user-specified function of time");
		ResponseTypeLayout->addWidget(m_pctrlInitCondResponse);
		ResponseTypeLayout->addWidget(m_pctrlForcedResponse);
		ResponseTypeLayout->addWidget(m_pctrlModalResponse);

		QVBoxLayout	 *TimeParams = new QVBoxLayout;


		QGroupBox *InitCondResponse = new QGroupBox(tr("Initial conditions"));
		{
			m_pctrlStabLabel1 = new QLabel("u0__");
			m_pctrlStabLabel2 = new QLabel("w0__");
			m_pctrlStabLabel3 = new QLabel("q0__");
			m_pctrlUnit1 = new QLabel("m/s");
			m_pctrlUnit2 = new QLabel("m/s");
			m_pctrlUnit3 = new QLabel("rad/s");
			m_pctrlStabLabel1->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			m_pctrlStabLabel2->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			m_pctrlStabLabel3->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			m_pctrlUnit1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			m_pctrlUnit2->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			m_pctrlUnit3->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			m_pctrlStabVar1 = new NumberEdit(NumberEdit::Standard, 3);
			m_pctrlStabVar2 = new NumberEdit(NumberEdit::Standard, 3);
			m_pctrlStabVar3 = new NumberEdit(NumberEdit::Standard, 3);
			m_pctrlStabVar3->setValue(1);
			QGridLayout *VarParams = new QGridLayout;
			VarParams->addWidget(m_pctrlStabLabel1,1,1);
			VarParams->addWidget(m_pctrlStabLabel2,2,1);
			VarParams->addWidget(m_pctrlStabLabel3,3,1);
			VarParams->addWidget(m_pctrlStabVar1,1,2);
			VarParams->addWidget(m_pctrlStabVar2,2,2);
			VarParams->addWidget(m_pctrlStabVar3,3,2);
			VarParams->addWidget(m_pctrlUnit1,1,3);
			VarParams->addWidget(m_pctrlUnit2,2,3);
			VarParams->addWidget(m_pctrlUnit3,3,3);
			QVBoxLayout *InitCondResponseLayout = new QVBoxLayout;
			InitCondResponseLayout ->addLayout(VarParams);
			InitCondResponseLayout->addStretch(1);
			InitCondResponse->setLayout(InitCondResponseLayout);
		}

		QGroupBox *ForcedResponseBox = new QGroupBox(tr("Forced Response"));
		{
			QVBoxLayout *ForcedResponse = new QVBoxLayout;
			QLabel *ForcedText = new QLabel(tr("Control function"));
			m_pctrlControlTable = new QTableView(this);
			m_pctrlControlTable->setToolTip(tr("Enter the function of the control vs. time"));
			m_pctrlControlTable->setMinimumHeight(150);
			m_pctrlControlTable->setSelectionMode(QAbstractItemView::SingleSelection);
			m_pctrlControlTable->setSelectionBehavior(QAbstractItemView::SelectRows);
			m_pCtrlDelegate = new FloatEditDelegate;
			int  *precision = new int[3];
			precision[0]  = 3;
			precision[1]  = 3;
			precision[2]  = 3;

			m_pCtrlDelegate->SetPrecision(precision);
			m_pctrlControlTable->setItemDelegate(m_pCtrlDelegate);
//			m_pCtrlDelegate->m_pCtrlModel = m_pControlModel;

			ForcedResponse->addWidget(ForcedText);
			ForcedResponse->addWidget(m_pctrlControlTable);
			ForcedResponse->addStretch(1);
			ForcedResponseBox->setLayout(ForcedResponse);
		}

		QGroupBox *ModalTimeBox = new QGroupBox(tr("Modal response"));
		{
			QVBoxLayout *ModalTimeLayout = new QVBoxLayout;
			m_pctrlTimeMode1 = new QRadioButton("Mode 1");
			m_pctrlTimeMode2 = new QRadioButton("Mode 2");
			m_pctrlTimeMode3 = new QRadioButton("Mode 3");
			m_pctrlTimeMode4 = new QRadioButton("Mode 4");
			m_pctrlModeProperties = new QLabel("Mode Properties");
			ModalTimeLayout->addWidget(m_pctrlTimeMode1);
			ModalTimeLayout->addWidget(m_pctrlTimeMode2);
			ModalTimeLayout->addWidget(m_pctrlTimeMode3);
			ModalTimeLayout->addWidget(m_pctrlTimeMode4);
			ModalTimeLayout->addStretch(1);
			ModalTimeLayout->addWidget(m_pctrlModeProperties);
			ModalTimeBox->setLayout(ModalTimeLayout);
		}

		m_pctrlInitialConditionsWidget = new QStackedWidget;
		m_pctrlInitialConditionsWidget->addWidget(InitCondResponse);
		m_pctrlInitialConditionsWidget->addWidget(ForcedResponseBox);
		m_pctrlInitialConditionsWidget->addWidget(ModalTimeBox);
		m_pctrlInitialConditionsWidget->setCurrentIndex(0);

		QGroupBox *CurveSettings = new QGroupBox(tr("Curve Settings"));
		m_pctrlTotalTime = new NumberEdit(NumberEdit::Standard, 3);
		m_pctrlTotalTime->setValue(5);
		m_pctrlTotalTime->setToolTip(tr("Define the total time range for the graphs"));
		m_pctrlDeltat    = new NumberEdit(NumberEdit::Standard, 3);
		m_pctrlDeltat->setValue(0.01);
		m_pctrlDeltat->setToolTip(tr("Define the time step for the resolution of the differential equations"));

		QGridLayout *DtLayout  = new QGridLayout;
		QLabel *DtLabel        = new QLabel("dt=");
		QLabel *TotalTimeLabel = new QLabel(tr("Total Time")+"=");
		QLabel *TimeLab1       = new QLabel("s");
		QLabel *TimeLab2       = new QLabel("s");
		DtLayout->addWidget(DtLabel,1,1);
		DtLayout->addWidget(m_pctrlDeltat,1,2);
		DtLayout->addWidget(TimeLab1,1,3);
		DtLayout->addWidget(TotalTimeLabel,2,1);
		DtLayout->addWidget(m_pctrlTotalTime,2,2);
		DtLayout->addWidget(TimeLab2,2,3);

		QGridLayout *CurveLayout = new QGridLayout;
		m_pctrlPlotStabGraph = new QPushButton(tr("Recalc."));
		m_pctrlPlotStabGraph->setToolTip(tr("Re-calculate the currently selected curve with the user-specified input data"));
		m_pctrlAddCurve  = new QPushButton(tr("Add"));
		m_pctrlAddCurve->setToolTip(tr("Add a new curve to the graphs, using the current user-specified input"));
		m_pctrlRenameCurve  = new QPushButton(tr("Rename"));
		m_pctrlRenameCurve->setToolTip(tr("Rename the currently selected curve"));
		m_pctrlDeleteCurve  = new QPushButton(tr("Delete"));
		m_pctrlDeleteCurve->setToolTip(tr("Delete the currently selected curve"));
		m_pctrlCurveList = new QComboBox();
		CurveLayout->addWidget(m_pctrlAddCurve,1,1);
		CurveLayout->addWidget(m_pctrlPlotStabGraph,1,2);
		CurveLayout->addWidget(m_pctrlRenameCurve,2,1);
		CurveLayout->addWidget(m_pctrlDeleteCurve,2,2);

		QVBoxLayout *TimeLayout = new QVBoxLayout;

		TimeLayout->addLayout(DtLayout);
		TimeLayout->addWidget(m_pctrlCurveList);
		TimeLayout->addLayout(CurveLayout);
		CurveSettings->setLayout(TimeLayout);

	//	TimeParams->addLayout(InitialConditionsLayout);
		TimeParams->addLayout(ResponseTypeLayout);
		TimeParams->addWidget(m_pctrlInitialConditionsWidget);
		TimeParams->addWidget(CurveSettings);
		TimeParams->addStretch(5);

		TimeBox->setLayout(TimeParams);
	}

	//_______________________Root Locus View and 3D animation Parameters
	QGroupBox *ModeBox = new QGroupBox(tr("Operating point modes"));
	{
		QVBoxLayout *RLLayout = new QVBoxLayout;
		QHBoxLayout *RLModeLayout = new QHBoxLayout;
		m_pctrlRLMode1 = new QRadioButton("1");
		m_pctrlRLMode2 = new QRadioButton("2");
		m_pctrlRLMode3 = new QRadioButton("3");
		m_pctrlRLMode4 = new QRadioButton("4");
		m_pctrlRLMode1->setToolTip(tr("Press Ctrl+H to highlight the mode on the root locus plot"));
		m_pctrlRLMode2->setToolTip(tr("Press Ctrl+H to highlight the mode on the root locus plot"));
		m_pctrlRLMode3->setToolTip(tr("Press Ctrl+H to highlight the mode on the root locus plot"));
		m_pctrlRLMode4->setToolTip(tr("Press Ctrl+H to highlight the mode on the root locus plot"));
		RLModeLayout->addWidget(m_pctrlRLMode1);
		RLModeLayout->addWidget(m_pctrlRLMode2);
		RLModeLayout->addWidget(m_pctrlRLMode3);
		RLModeLayout->addWidget(m_pctrlRLMode4);
		QGroupBox *RLModeBox = new QGroupBox(tr("Mode Selection"));
		RLModeBox->setLayout(RLModeLayout);

		//_____________Mode properties _________
		QGroupBox *FreakBox = new QGroupBox(tr("Mode properties"));
		{
			QLabel *FreqNLab = new QLabel("F =");
			QLabel *Freq1Lab = new QLabel(tr("F1 ="));
			QLabel *DsiLab   = new QLabel(tr("z ="));
			FreqNLab->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
			Freq1Lab->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
			DsiLab->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
			DsiLab->setFont(SymbolFont);

			m_pctrlFreqN  = new NumberEdit(NumberEdit::Standard, 3);
			m_pctrlFreq1  = new NumberEdit(NumberEdit::Standard, 3);
			m_pctrlDsi    = new NumberEdit(NumberEdit::Standard, 3);
			m_pctrlFreqN->setEnabled(false);
			m_pctrlFreq1->setEnabled(false);
			m_pctrlDsi->setEnabled(false);
			QLabel *FreqUnit1 = new QLabel("Hz");
			QLabel *FreqUnit2 = new QLabel("Hz");
			QGridLayout *FreakLayout = new QGridLayout;
			FreakLayout->addWidget(FreqNLab,1,1);
			FreakLayout->addWidget(Freq1Lab,2,1);
			FreakLayout->addWidget(DsiLab,3,1);
			FreakLayout->addWidget(m_pctrlFreqN,1,2);
			FreakLayout->addWidget(m_pctrlFreq1,2,2);
			FreakLayout->addWidget(m_pctrlDsi,3,2);
			FreakLayout->addWidget(FreqUnit1,1,3);
			FreakLayout->addWidget(FreqUnit2,2,3);

			FreakBox->setLayout(FreakLayout);
		}

		//_____________Eigenvalue data box________________________
		QGroupBox *EigenBox = new QGroupBox(tr("Eigenvalues"));
		{
			QGridLayout *EigenLayout = new QGridLayout;
			QLabel *LabValue = new QLabel("l=");
			QFont SymbolFont("Symbol");
			LabValue->setFont(SymbolFont);
			QLabel *LabVect1 = new QLabel("v1=");
			QLabel *LabVect2 = new QLabel("v2=");
			QLabel *LabVect3 = new QLabel("v3=");
			QLabel *LabVect4 = new QLabel("v4=");
			EigenLayout->addWidget(LabValue,1,1);
			EigenLayout->addWidget(LabVect1,2,1);
			EigenLayout->addWidget(LabVect2,3,1);
			EigenLayout->addWidget(LabVect3,4,1);
			EigenLayout->addWidget(LabVect4,5,1);
			m_pctrlEigenValue = new QLineEdit("2+4i");
			m_pctrlEigenVector1 = new QLineEdit("3-7i");
			m_pctrlEigenVector2 = new QLineEdit("4-6i");
			m_pctrlEigenVector3 = new QLineEdit("2.76-1.8782i");
			m_pctrlEigenVector4 = new QLineEdit("3.4567+9.2746i");
			m_pctrlEigenValue->setAlignment(Qt::AlignRight);
			m_pctrlEigenVector1->setAlignment(Qt::AlignRight);
			m_pctrlEigenVector2->setAlignment(Qt::AlignRight);
			m_pctrlEigenVector3->setAlignment(Qt::AlignRight);
			m_pctrlEigenVector4->setAlignment(Qt::AlignRight);
			m_pctrlEigenValue->setEnabled(false);
			m_pctrlEigenVector1->setEnabled(false);
			m_pctrlEigenVector2->setEnabled(false);
			m_pctrlEigenVector3->setEnabled(false);
			m_pctrlEigenVector4->setEnabled(false);
			EigenLayout->addWidget(m_pctrlEigenValue,  1,2);
			EigenLayout->addWidget(m_pctrlEigenVector1,2,2);
			EigenLayout->addWidget(m_pctrlEigenVector2,3,2);
			EigenLayout->addWidget(m_pctrlEigenVector3,4,2);
			EigenLayout->addWidget(m_pctrlEigenVector4,5,2);
			EigenBox->setLayout(EigenLayout);
		}

		//	___________3D Animation box_________
		QGroupBox *AnimationBox = new QGroupBox(tr("Animation"));
		{
			QVBoxLayout *AnimationLayout = new QVBoxLayout;

			QGridLayout *AnimSpeedLayout = new QGridLayout;
			QLabel *LabSpeed = new QLabel(tr("Speed"));
			LabSpeed->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
			m_pctrlAnimationSpeed  = new QDial();
			m_pctrlAnimationSpeed->setMinimum(0);
			m_pctrlAnimationSpeed->setMaximum(400);
			m_pctrlAnimationSpeed->setSliderPosition(m_ModeInterval);
			m_pctrlAnimationSpeed->setNotchesVisible(true);
			m_pctrlAnimationSpeed->setSingleStep(20);

			AnimSpeedLayout->addWidget(m_pctrlAnimationSpeed,1,1);
			AnimSpeedLayout->addWidget(LabSpeed,2,1);

			QLabel *LabAmplitude = new QLabel(tr("Amplitude"));
			LabAmplitude->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
			m_pctrlAnimationAmplitude  = new QDial();
			m_pctrlAnimationAmplitude->setMinimum(0);
			m_pctrlAnimationAmplitude->setMaximum(1000);
			m_pctrlAnimationAmplitude->setSliderPosition((int)(m_ModeAmplitude*500));
			m_pctrlAnimationAmplitude->setNotchesVisible(true);
			m_pctrlAnimationAmplitude->setSingleStep(20);
			AnimSpeedLayout->addWidget(m_pctrlAnimationAmplitude,1,2);
			AnimSpeedLayout->addWidget(LabAmplitude,2,2);

			m_pctrlAnimate = new QPushButton(tr("Animate"));
			m_pctrlAnimate->setCheckable(true);
			m_pctrlAnimateRestart = new QPushButton(tr("Restart"));
			m_pctrlModeStep = new NumberEdit(NumberEdit::Standard, 3);
			m_pctrlModeStep->setValue(0.01);
			QLabel *StepLabel = new QLabel(tr("Time Step ="));
			QLabel *StepUnit  = new QLabel(tr("s"));

			QVBoxLayout *AnimationCommandsLayout = new QVBoxLayout;
			AnimationCommandsLayout->addWidget(m_pctrlAnimateRestart);
			AnimationCommandsLayout->addWidget(m_pctrlAnimate);

			QHBoxLayout *StepLayout = new  QHBoxLayout;
			StepLayout->addWidget(StepLabel);
			StepLayout->addWidget(m_pctrlModeStep);
			StepLayout->addWidget(StepUnit);
			AnimationLayout->addLayout(StepLayout);

			AnimationLayout->addLayout(AnimSpeedLayout);
			AnimationLayout->addLayout(AnimationCommandsLayout);
			AnimationBox->setLayout(AnimationLayout);
		}


		m_pctrlModeViewType= new QStackedWidget;
		m_pctrlModeViewType->addWidget(EigenBox);
		m_pctrlModeViewType->addWidget(AnimationBox);
		m_pctrlModeViewType->setCurrentIndex(0);

		RLLayout->addWidget(RLModeBox);
		RLLayout->addWidget(FreakBox);
		RLLayout->addWidget(m_pctrlModeViewType);
		RLLayout->addStretch(1);
		ModeBox->setLayout(RLLayout);
	}


	//___________________Main Layout____________
	m_pctrlStackWidget = new QStackedWidget;
	m_pctrlStackWidget->addWidget(TimeBox);
	m_pctrlStackWidget->addWidget(ModeBox);
	m_pctrlStackWidget->setCurrentIndex(0);

	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addWidget(StabilityDirBox);
	MainLayout->addWidget(m_pctrlStackWidget);
	setLayout(MainLayout);
}



void StabViewDlg::SetControls()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	QString str, strong;
	GetSpeedUnit(str, pMainFrame->m_SpeedUnit);

	m_pctrlLongDynamics->setChecked(pMiarex->m_bLongitudinal);
	m_pctrlLatDynamics->setChecked(!pMiarex->m_bLongitudinal);

	if(pMiarex->m_pCurWPolar && pMiarex->m_pCurWPolar->m_WPolarType!=STABILITYPOLAR)
	{
//		m_pControlModel->setRowCount(0);
	}

	if(pMiarex->m_iStabilityView==STABTIMEVIEW)
	{
		m_pctrlStackWidget->setCurrentIndex(0);
		m_pctrlInitialConditionsWidget->setCurrentIndex(pMiarex->m_StabilityResponseType);
		
		m_pctrlInitCondResponse->setChecked(pMiarex->m_StabilityResponseType==0);
		m_pctrlForcedResponse->setChecked(pMiarex->m_StabilityResponseType==1);
		m_pctrlModalResponse->setChecked(pMiarex->m_StabilityResponseType==2);
	}
	else if(pMiarex->m_iStabilityView==STABPOLARVIEW)
	{
		m_pctrlStackWidget->setCurrentIndex(1);
		m_pctrlModeViewType->setCurrentIndex(0);
	}
	else if(pMiarex->m_iStabilityView==STAB3DVIEW)
	{
		m_pctrlStackWidget->setCurrentIndex(1);
		m_pctrlModeViewType->setCurrentIndex(1);
	}
	SetMode(m_iCurrentMode);

	strong = QString::fromUtf8("deg/s");
	if(pMiarex->m_bLongitudinal)
	{
		m_pctrlStabLabel1->setText(tr("u0="));
		m_pctrlStabLabel2->setText(tr("w0="));
		m_pctrlStabLabel3->setText(tr("q0="));
		m_pctrlUnit1->setText(str);
		m_pctrlUnit2->setText(str);
		m_pctrlUnit3->setText(strong);
	}
	else
	{
		m_pctrlStabLabel1->setText(tr("v0="));
		m_pctrlStabLabel2->setText(tr("p0="));
		m_pctrlStabLabel3->setText(tr("r0="));
		m_pctrlUnit1->setText(str);
		m_pctrlUnit2->setText(strong);
		m_pctrlUnit3->setText(strong);
	}

	m_pctrlStabVar1->setValue(pMiarex->m_TimeInput[0]);
	m_pctrlStabVar2->setValue(pMiarex->m_TimeInput[1]);
	m_pctrlStabVar3->setValue(pMiarex->m_TimeInput[2]);
	m_pctrlTotalTime->setValue(pMiarex->m_TotalTime);
	m_pctrlDeltat->setValue(pMiarex->m_Deltat);

	m_pctrlTimeMode1->setChecked(m_iCurrentMode%4==0);
	m_pctrlTimeMode2->setChecked(m_iCurrentMode%4==1);
	m_pctrlTimeMode3->setChecked(m_iCurrentMode%4==2);
	m_pctrlTimeMode4->setChecked(m_iCurrentMode%4==3);
	m_pctrlRLMode1->setChecked(m_iCurrentMode%4==0);
	m_pctrlRLMode2->setChecked(m_iCurrentMode%4==1);
	m_pctrlRLMode3->setChecked(m_iCurrentMode%4==2);
	m_pctrlRLMode4->setChecked(m_iCurrentMode%4==3);


	m_pctrlRLMode1->setEnabled(pMiarex->m_iStabilityView>0 && pMiarex->m_pCurWOpp);
	m_pctrlRLMode2->setEnabled(pMiarex->m_iStabilityView>0 && pMiarex->m_pCurWOpp);
	m_pctrlRLMode3->setEnabled(pMiarex->m_iStabilityView>0 && pMiarex->m_pCurWOpp);
	m_pctrlRLMode4->setEnabled(pMiarex->m_iStabilityView>0 && pMiarex->m_pCurWOpp);

	// Enable the time response controls only if
	//   - the polar's type is 7
	//   - we have an active wopp
	//   - the StabilityView is0
	bool bEnableTimeCtrl = pMiarex->m_pCurWOpp && pMiarex->m_pCurWOpp->m_WPolarType==STABILITYPOLAR && pMiarex->m_iStabilityView==0;
	m_pctrlAddCurve->setEnabled(bEnableTimeCtrl);
	m_pctrlRenameCurve->setEnabled(m_pctrlCurveList->count());
	m_pctrlPlotStabGraph->setEnabled(m_pctrlCurveList->count());
	m_pctrlDeleteCurve->setEnabled(m_pctrlCurveList->count());
	m_pctrlCurveList->setEnabled(m_pctrlCurveList->count());


	m_pctrlTimeMode1->setEnabled(bEnableTimeCtrl);
	m_pctrlTimeMode2->setEnabled(bEnableTimeCtrl);
	m_pctrlTimeMode3->setEnabled(bEnableTimeCtrl);
	m_pctrlTimeMode4->setEnabled(bEnableTimeCtrl);

	m_pctrlStabVar1->setEnabled(bEnableTimeCtrl);
	m_pctrlStabVar2->setEnabled(bEnableTimeCtrl);
	m_pctrlStabVar3->setEnabled(bEnableTimeCtrl);
	m_pctrlDeltat->setEnabled(bEnableTimeCtrl);
	m_pctrlTotalTime->setEnabled(bEnableTimeCtrl);

	// Enable the 3D mode animation controls only if
	//   - the polar's type is 7
	//   - we have an active wopp
	//   - the StabilityView is 3
	bool bEnable3DAnimation = pMiarex->m_iStabilityView==3 && pMiarex->m_pCurWOpp && pMiarex->m_pCurWOpp->m_WPolarType==STABILITYPOLAR;
	m_pctrlAnimate->setEnabled(bEnable3DAnimation);
	m_pctrlAnimateRestart->setEnabled(bEnable3DAnimation);
	m_pctrlAnimationAmplitude->setEnabled(bEnable3DAnimation);
	m_pctrlAnimationSpeed->setEnabled(bEnable3DAnimation);

	m_pctrlModeStep->setValue(pMiarex->m_Modedt);

	FillEigenThings();
}





void StabViewDlg::SetTimeCurveStyle(QColor const &Color, int const&Style, int const &Width, bool const& bCurve, bool const& bPoints)
{
	if(!m_pCurve) return;
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	CCurve *pCurve;
	for (int i=0; i<pMiarex->m_TimeGraph[0].GetCurveCount(); i++)
	{
		pCurve = pMiarex->m_TimeGraph[0].GetCurve(i);
		if(pCurve == m_pCurve)
		{
			for(int ig=0; ig<4; ig++)
			{
				pCurve = pMiarex->m_TimeGraph[ig].GetCurve(i);
				pCurve->SetColor(Color);
				pCurve->SetStyle(Style);
				pCurve->SetWidth(Width);
				pCurve->SetVisible(bCurve);
				pCurve->ShowPoints(bPoints);
			}
						
			return;
		}
	}
}


void StabViewDlg::OnRenameCurve()
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	
	CCurve *pCurve;
	if(!m_pCurve) return;

	QString NewName = "Test Name";
	NewNameDlg dlg;
	dlg.move(pMainFrame->m_DlgPos);
	dlg.m_OldName = m_pCurve->GetTitle();
	dlg.InitDialog();

	if(dlg.exec() != QDialog::Accepted) return;
	pMainFrame->m_DlgPos = dlg.pos();
	NewName = dlg.m_NewName;

	for (int i=0; i<pMiarex->m_TimeGraph[0].GetCurveCount(); i++)
	{
		pCurve = pMiarex->m_TimeGraph[0].GetCurve(i);
		if(pCurve && (pCurve == m_pCurve))
		{
			for(int ig=0; ig<4; ig++)
			{
				pCurve = pMiarex->m_TimeGraph[ig].GetCurve(i);
				pCurve->SetTitle(NewName);
			}

			FillCurveList();
			OnPlotStabilityGraph();
			return;
		}
	}
}


void StabViewDlg::OnSelChangeCurve(int sel)
{
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	QString strong = m_pctrlCurveList->itemText(sel);
	m_pCurve = pMiarex->m_TimeGraph[0].GetCurve(strong);
	m_pCurve->GetTitle(strong);
	
	pMiarex->SetCurveParams();
}


void StabViewDlg::OnAddCurve()
{
	AddCurve();
	if(m_pCurve)
	{
		int pos =m_pctrlCurveList->findText(m_pCurve->GetTitle());
		m_pctrlCurveList->setCurrentIndex(pos);
	}
	OnPlotStabilityGraph();
}


void StabViewDlg::OnDeleteCurve()
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	if(!m_pCurve) return;
	QString CurveTitle = m_pCurve->GetTitle();
	for(int ig=0; ig<4; ig++)	pMiarex->m_TimeGraph[ig].DeleteCurve(CurveTitle);
	m_pCurve = NULL;

	FillCurveList();
	m_pctrlCurveList->setCurrentIndex(0);
	m_pctrlPlotStabGraph->setEnabled(pMiarex->m_pCurWOpp && m_pctrlCurveList->count());
	m_pctrlRenameCurve->setEnabled(pMiarex->m_pCurWOpp && m_pctrlCurveList->count());
	m_pctrlDeleteCurve->setEnabled(pMiarex->m_pCurWOpp && m_pctrlCurveList->count());
	m_pctrlCurveList->setEnabled(pMiarex->m_pCurWOpp && m_pctrlCurveList->count());

	if(m_pctrlCurveList->count()) m_pCurve = pMiarex->m_TimeGraph[0].GetCurve(m_pctrlCurveList->itemText(0));
	else                          m_pCurve = NULL;

	pMiarex->SetCurveParams();
	pMiarex->CreateStabilityCurves();
	pMiarex->UpdateView();
	pMiarex->setFocus();
}


void StabViewDlg::AddCurve()
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	int nCurves = pMiarex->m_TimeGraph[0].GetCurveCount();
	QString strong = tr("New curve") + QString(" %1").arg(nCurves);

	CCurve *pCurve;
	for(int ig=0; ig<4; ig++)
	{
		pCurve = pMiarex->m_TimeGraph[ig].AddCurve();
		pCurve->SetTitle(strong);
		if(ig==0) m_pCurve = pCurve;
	}

	m_pctrlCurveList->addItem(pCurve->GetTitle());
	m_pctrlPlotStabGraph->setEnabled(pMiarex->m_pCurWOpp && m_pctrlCurveList->count());
	m_pctrlRenameCurve->setEnabled(pMiarex->m_pCurWOpp && m_pctrlCurveList->count());
	m_pctrlDeleteCurve->setEnabled(pMiarex->m_pCurWOpp && m_pctrlCurveList->count());
	m_pctrlCurveList->setEnabled(pMiarex->m_pCurWOpp && m_pctrlCurveList->count());

	pMiarex->SetCurveParams();

}


void StabViewDlg::FillCurveList()
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	m_pctrlCurveList->clear();
	QString strong;
	for(int i=0; i<pMiarex->m_TimeGraph[0].GetCurveCount(); i++)
	{
		pMiarex->m_TimeGraph[0].GetCurve(i)->GetTitle(strong);
		m_pctrlCurveList->addItem(strong);
	}
	if(m_pCurve)
	{
		int sel = m_pctrlCurveList->findText(m_pCurve->GetTitle());
		m_pctrlCurveList->setCurrentIndex(sel);
	}
}


double StabViewDlg::GetControlInput(const double &time)
{
	static double t1, t2, in1, in2;
	t1 = t2 = 0.0;
	t1 = m_pControlModel->index(0, 0, QModelIndex()).data().toDouble();
	for(int i=1; i<m_pControlModel->rowCount()-1; i++)
	{
		t2 = m_pControlModel->index(i, 0, QModelIndex()).data().toDouble();
		if(t1<=time && time<t2)
		{
			in1 = m_pControlModel->index(i-1, 1, QModelIndex()).data().toDouble();
			in2 = m_pControlModel->index(i,   1, QModelIndex()).data().toDouble();
			return (in1 + (time-t1) * (in2-in1)/(t2-t1))*PI/180.0;
		}
		t1 = t2;
	}
	return 0.0;
}











