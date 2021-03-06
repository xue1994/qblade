/****************************************************************************

    CPanel Class
	Copyright (C) 2006-2008 Andre Deperrois adeperrois@xflr5.com

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



#include <QtCore>
#include "Panel.h"
#include <math.h>

double CPanel::s_VortexPos = 0.25;
double CPanel::s_CtrlPos   = 0.25;
double CPanel::mat[9];
double CPanel::det;
CVector CPanel::smp, CPanel::smq, CPanel::MidA, CPanel::MidB;
CVector *CPanel::s_pNode;
CVector CPanel::ILA, CPanel::ILB, CPanel::ITA, CPanel::ITB, CPanel::T, CPanel::V, CPanel::W;
CVector CPanel::P;
CVector CPanel::LATB, CPanel::TALB;


CPanel::CPanel()
{
	Reset();
}

void CPanel::Reset()
{
	dl     = 0.0;
	Size   = 0.0;
	SMP    = 0.0;
	SMQ    = 0.0;
	Area   = 0.0;
    m_Gamma = 0.0;
    angularPos = 0.0;

    pTB = NULL;
    pTA = NULL;

	m_bIsLeading     = false;
	m_bIsTrailing    = false;
	m_bIsInSymPlane  = false;
	m_bIsLeftPanel   = false;
	m_bIsWakePanel   = false;
    isHub            = false;
    isTip            = false;


	m_Pos         =  MIDSURFACE;

	m_iElement    = -1;
	m_iSym        = -1;
	m_iLA         =  0;
	m_iLB         =  0;
	m_iTA         =  0;
	m_iTB         =  0;
	m_iPL         = -1;
	m_iPR         = -1;
	m_iPU         = -1;
	m_iPD         = -1;
	m_iWake       = -1;
	m_iWakeColumn = -1;

	memset(lij, 0, sizeof(lij));
}



void CPanel::SetFrame()
{
	//set the boundary conditions from existing nodes
	SetFrame(s_pNode[m_iLA], s_pNode[m_iLB], s_pNode[m_iTA], s_pNode[m_iTB]);
}



void CPanel::SetFrame(CVector const &LA, CVector const &LB, CVector const &TA, CVector const &TB)
{
    Old_CtrlPt = CtrlPt;

	LATB.x = TB.x - LA.x;
	LATB.y = TB.y - LA.y;
	LATB.z = TB.z - LA.z;
	TALB.x = LB.x - TA.x;
	TALB.y = LB.y - TA.y;
	TALB.z = LB.z - TA.z;

	Normal = LATB * TALB;
	Area = Normal.VAbs()/2.0;
	Normal.Normalize();

	VA.x = LA.x*(1.0-s_VortexPos)+TA.x*s_VortexPos;
	VA.y = LA.y*(1.0-s_VortexPos)+TA.y*s_VortexPos;
	VA.z = LA.z*(1.0-s_VortexPos)+TA.z*s_VortexPos;

	VB.x = LB.x*(1.0-s_VortexPos)+TB.x*s_VortexPos;
	VB.y = LB.y*(1.0-s_VortexPos)+TB.y*s_VortexPos;
	VB.z = LB.z*(1.0-s_VortexPos)+TB.z*s_VortexPos;

	Vortex.x = VB.x - VA.x;
	Vortex.y = VB.y - VA.y;
	Vortex.z = VB.z - VA.z;

	dl = Vortex.VAbs();

	VortexPos.x = (VA.x+VB.x)/2.0;
	VortexPos.y = (VA.y+VB.y)/2.0;
	VortexPos.z = (VA.z+VB.z)/2.0;

    if(fabs(LA.y)<1.e-5 && fabs(TA.y)<1.e-5 && fabs(LB.y)<1.e-5 && fabs(TB.y)<1.e-5) m_bIsInSymPlane = true;
	else m_bIsInSymPlane = false;

	MidA.x = LA.x*(1.0-s_CtrlPos)+TA.x*s_CtrlPos;
	MidA.y = LA.y*(1.0-s_CtrlPos)+TA.y*s_CtrlPos;
	MidA.z = LA.z*(1.0-s_CtrlPos)+TA.z*s_CtrlPos;

	MidB.x = LB.x*(1.0-s_CtrlPos)+TB.x*s_CtrlPos;
	MidB.y = LB.y*(1.0-s_CtrlPos)+TB.y*s_CtrlPos;
	MidB.z = LB.z*(1.0-s_CtrlPos)+TB.z*s_CtrlPos;
//Implemented new computation of CtrlPt
//    CtrlPt.x = (MidA.x+MidB.x)/4.0;
//    CtrlPt.y = (MidA.y+MidB.y)/4.0;
//    CtrlPt.z = (MidA.z+MidB.z)/4.0;

	CollPt.x = (LA.x + LB.x + TA.x + TB.x)/4.0;
	CollPt.y = (LA.y + LB.y + TA.y + TB.y)/4.0;
	CollPt.z = (LA.z + LB.z + TA.z + TB.z)/4.0;

	//Use VSAERO figure 8. p23
//	if(m_iPos==THINSURFACE || m_Pos==TOPSURFACE || m_Pos==BODYSURFACE)
//	{
		m.x = (LB.x + TB.x) *0.5 - CollPt.x;
		m.y = (LB.y + TB.y) *0.5 - CollPt.y;
		m.z = (LB.z + TB.z) *0.5 - CollPt.z;
/*	}
	else						
	{
		m.x = (LA.x + TA.x) *0.5 - CollPt.x;
		m.y = (LA.y + TA.y) *0.5 - CollPt.y;
		m.z = (LA.z + TA.z) *0.5 - CollPt.z;
	}*/
	m.Normalize();

	l.x =  m.y * Normal.z - m.z * Normal.y;
	l.y = -m.x * Normal.z + m.z * Normal.x;
	l.z =  m.x * Normal.y - m.y * Normal.x;

	smq.x  = (LB.x + TB.x) * 0.5 - CollPt.x;
	smq.y  = (LB.y + TB.y) * 0.5 - CollPt.y;
	smq.z  = (LB.z + TB.z) * 0.5 - CollPt.z;
	smp.x  = (TB.x + TA.x) * 0.5 - CollPt.x;
	smp.y  = (TB.y + TA.y) * 0.5 - CollPt.y;
	smp.z  = (TB.z + TA.z) * 0.5 - CollPt.z;

	SMP = smp.VAbs();
	SMQ = smq.VAbs();

	Size = SMP + SMQ;

	//create the transformation matrix
	lij[0]=l.x;
	lij[1]=m.x;
	lij[2]=Normal.x;
	lij[3]=l.y;
	lij[4]=m.y;
	lij[5]=Normal.y;
	lij[6]=l.z;
	lij[7]=m.z;
	lij[8]=Normal.z;
	Invert33(lij);

	if(m_Pos>MIDSURFACE)
	{
		P1.x = lij[0]*(LA.x-CollPt.x) + lij[1]*(LA.y-CollPt.y) + lij[2]*(LA.z-CollPt.z);
		P1.y = lij[3]*(LA.x-CollPt.x) + lij[4]*(LA.y-CollPt.y) + lij[5]*(LA.z-CollPt.z);
		P1.z = lij[6]*(LA.x-CollPt.x) + lij[7]*(LA.y-CollPt.y) + lij[8]*(LA.z-CollPt.z);
		P2.x = lij[0]*(LB.x-CollPt.x) + lij[1]*(LB.y-CollPt.y) + lij[2]*(LB.z-CollPt.z);
		P2.y = lij[3]*(LB.x-CollPt.x) + lij[4]*(LB.y-CollPt.y) + lij[5]*(LB.z-CollPt.z);
		P2.z = lij[6]*(LB.x-CollPt.x) + lij[7]*(LB.y-CollPt.y) + lij[8]*(LB.z-CollPt.z);
		P3.x = lij[0]*(TB.x-CollPt.x) + lij[1]*(TB.y-CollPt.y) + lij[2]*(TB.z-CollPt.z);
		P3.y = lij[3]*(TB.x-CollPt.x) + lij[4]*(TB.y-CollPt.y) + lij[5]*(TB.z-CollPt.z);
		P3.z = lij[6]*(TB.x-CollPt.x) + lij[7]*(TB.y-CollPt.y) + lij[8]*(TB.z-CollPt.z);
		P4.x = lij[0]*(TA.x-CollPt.x) + lij[1]*(TA.y-CollPt.y) + lij[2]*(TA.z-CollPt.z);
		P4.y = lij[3]*(TA.x-CollPt.x) + lij[4]*(TA.y-CollPt.y) + lij[5]*(TA.z-CollPt.z);
		P4.z = lij[6]*(TA.x-CollPt.x) + lij[7]*(TA.y-CollPt.y) + lij[8]*(TA.z-CollPt.z);
	}
	else
	{
		P1.x = lij[0]*(LB.x-CollPt.x) + lij[1]*(LB.y-CollPt.y) + lij[2]*(LB.z-CollPt.z);
		P1.y = lij[3]*(LB.x-CollPt.x) + lij[4]*(LB.y-CollPt.y) + lij[5]*(LB.z-CollPt.z);
		P1.z = lij[6]*(LB.x-CollPt.x) + lij[7]*(LB.y-CollPt.y) + lij[8]*(LB.z-CollPt.z);
		P2.x = lij[0]*(LA.x-CollPt.x) + lij[1]*(LA.y-CollPt.y) + lij[2]*(LA.z-CollPt.z);
		P2.y = lij[3]*(LA.x-CollPt.x) + lij[4]*(LA.y-CollPt.y) + lij[5]*(LA.z-CollPt.z);
		P2.z = lij[6]*(LA.x-CollPt.x) + lij[7]*(LA.y-CollPt.y) + lij[8]*(LA.z-CollPt.z);
		P3.x = lij[0]*(TA.x-CollPt.x) + lij[1]*(TA.y-CollPt.y) + lij[2]*(TA.z-CollPt.z);
		P3.y = lij[3]*(TA.x-CollPt.x) + lij[4]*(TA.y-CollPt.y) + lij[5]*(TA.z-CollPt.z);
		P3.z = lij[6]*(TA.x-CollPt.x) + lij[7]*(TA.y-CollPt.y) + lij[8]*(TA.z-CollPt.z);
		P4.x = lij[0]*(TB.x-CollPt.x) + lij[1]*(TB.y-CollPt.y) + lij[2]*(TB.z-CollPt.z);
		P4.y = lij[3]*(TB.x-CollPt.x) + lij[4]*(TB.y-CollPt.y) + lij[5]*(TB.z-CollPt.z);
		P4.z = lij[6]*(TB.x-CollPt.x) + lij[7]*(TB.y-CollPt.y) + lij[8]*(TB.z-CollPt.z);
	}

    //////////////new code TW

    CVector MidL, MidT, MiL, MiT, A, B, lc ;

    VortPtA.x = LA.x*(1.0-s_VortexPos)+TA.x*s_VortexPos;
    VortPtA.y = LA.y*(1.0-s_VortexPos)+TA.y*s_VortexPos;
    VortPtA.z = LA.z*(1.0-s_VortexPos)+TA.z*s_VortexPos;

    VortPtB.x = LB.x*(1.0-s_VortexPos)+TB.x*s_VortexPos;
    VortPtB.y = LB.y*(1.0-s_VortexPos)+TB.y*s_VortexPos;
    VortPtB.z = LB.z*(1.0-s_VortexPos)+TB.z*s_VortexPos;

    A.x = LA.x*(1.0-s_VortexPos)+TA.x*s_VortexPos;
    A.y = LA.y*(1.0-s_VortexPos)+TA.y*s_VortexPos;
    A.z = LA.z*(1.0-s_VortexPos)+TA.z*s_VortexPos;

    B.x = LB.x*(1.0-s_VortexPos)+TB.x*s_VortexPos;
    B.y = LB.y*(1.0-s_VortexPos)+TB.y*s_VortexPos;
    B.z = LB.z*(1.0-s_VortexPos)+TB.z*s_VortexPos;

    MidL.x=LA.x+(LB.x-LA.x)*0.5;
    MidL.y=LA.y+(LB.y-LA.y)*0.5;
    MidL.z=LA.z+(LB.z-LA.z)*0.5;
    MidT.x=TA.x+(TB.x-TA.x)*0.5;
    MidT.y=TA.y+(TB.y-TA.y)*0.5;
    MidT.z=TA.z+(TB.z-TA.z)*0.5;



    //ControlPt.x =
    lc.x=B.x-A.x;
    lc.y=B.y-A.y;
    lc.z=B.z-A.z;

    // MiL und MiT
    MiL.x = MidL.x;
    MiL.y = MidL.y;
    MiL.z = MidL.z;

    MiT.x = MidT.x;
    MiT.y = MidT.y;
    MiT.z = MidT.z;

    a1.x = MiT.x-MiL.x;
    a1.y = MiT.y-MiL.y;
    a1.z = MiT.z-MiL.z;

    a3 = a1 *(lc);
    a2 = a1 * a3;
    a1.Normalize();
    a3.Normalize();
    a2.Normalize();
    ///end new code TW

    CtrlPt.x = (MidL.x + (MidT.x - MidL.x)*s_CtrlPos);
    CtrlPt.y = (MidL.y + (MidT.y - MidL.y)*s_CtrlPos);
    CtrlPt.z = (MidL.z + (MidT.z - MidL.z)*s_CtrlPos);

    if (pTB && pTA){
    LALB = CVector(VortPtB - VortPtA).VAbs();
    LBTB = CVector(*pTB - VortPtB).VAbs();
    TALA = CVector(VortPtA - *pTA).VAbs();
    TBTA = CVector(*pTA - *pTB).VAbs();
    }

}



bool CPanel::Invert33(double *l)
{
	memcpy(mat,l,sizeof(mat));
/*		a0 b1 c2
        d3 e4 f5
		g6 h7 i8

	          1                     (ei-fh)   (bi-ch)   (bf-ce)
-----------------------------   x   (fg-di)   (ai-cg)   (cd-af)
a(ei-fh) - b(di-fg) + c(dh-eg)      (dh-eg)   (bg-ah)   (ae-bd)*/

	det  = mat[0] *(mat[4] * mat[8] - mat[5]* mat[7]);
	det -= mat[1] *(mat[3] * mat[8] - mat[5]* mat[6]);
	det += mat[2] *(mat[3] * mat[7] - mat[4]* mat[6]);
	if(det==0.0) return false;
	
	* l     = (mat[4] * mat[8] - mat[5] * mat[7])/det;
	*(l+1)  = (mat[2] * mat[7] - mat[1] * mat[8])/det;
	*(l+2)  = (mat[1] * mat[5] - mat[2] * mat[4])/det;

	*(l+3)  = (mat[5] * mat[6] - mat[3] * mat[8])/det;
	*(l+4)  = (mat[0] * mat[8] - mat[2] * mat[6])/det;
	*(l+5)  = (mat[2] * mat[3] - mat[0] * mat[5])/det;

	*(l+6)  = (mat[3] * mat[7] - mat[4] * mat[6])/det;
	*(l+7)  = (mat[1] * mat[6] - mat[0] * mat[7])/det;
	*(l+8)  = (mat[0] * mat[4] - mat[1] * mat[3])/det;

	return true;
}


CVector CPanel::GlobalToLocal(CVector const &V)
{
	CVector L;
	L.x = lij[0]*V.x +lij[1]*V.y +lij[2]*V.z;
	L.y = lij[3]*V.x +lij[4]*V.y +lij[5]*V.z;
	L.z = lij[6]*V.x +lij[7]*V.y +lij[8]*V.z;
	return L;
}


CVector CPanel::GlobalToLocal(double const &Vx, double const &Vy, double const &Vz)
{
	CVector L;
	L.x = lij[0]*Vx +lij[1]*Vy +lij[2]*Vz;
	L.y = lij[3]*Vx +lij[4]*Vy +lij[5]*Vz;
	L.z = lij[6]*Vx +lij[7]*Vy +lij[8]*Vz;
	return L;
}


CVector CPanel::LocalToGlobal(CVector const &V)
{
	CVector L;
	L.x = V.x * l.x + V.y * m.x + V.z * Normal.x;
	L.y = V.x * l.y + V.y * m.y + V.z * Normal.y;
	L.z = V.x * l.z + V.y * m.z + V.z * Normal.z;
	return L;
}



bool CPanel::Intersect(CVector const &A, CVector const &U, CVector &I, double &dist)
{
	bool b1, b2, b3, b4;
	double r,s;

	ILA.Copy(s_pNode[m_iLA]);
	ITA.Copy(s_pNode[m_iTA]);
	ILB.Copy(s_pNode[m_iLB]);
	ITB.Copy(s_pNode[m_iTB]);
		
	r = (CollPt.x-A.x)*Normal.x + (CollPt.y-A.y)*Normal.y + (CollPt.z-A.z)*Normal.z ;
	s = U.x*Normal.x + U.y*Normal.y + U.z*Normal.z;

	dist = 10000.0;

	if(fabs(s)>0.0)
	{
		dist = r/s;

		//inline operations to save time
		P.x = A.x + U.x * dist;
		P.y = A.y + U.y * dist;
		P.z = A.z + U.z * dist;

		// P is inside the panel if on left side of each panel side
		W.x = P.x  - ITA.x;
		W.y = P.y  - ITA.y;
		W.z = P.z  - ITA.z;
		V.x = ITB.x - ITA.x;
		V.y = ITB.y - ITA.y;
		V.z = ITB.z - ITA.z;
		T.x =  V.y * W.z - V.z * W.y;
		T.y = -V.x * W.z + V.z * W.x;
		T.z =  V.x * W.y - V.y * W.x;
		if(T.x*T.x+T.y*T.y+T.z*T.z <1.0e-10 || T.x*Normal.x+T.y*Normal.y+T.z*Normal.z>=0.0) b1 = true; else b1 = false;

		W.x = P.x  - ITB.x;
		W.y = P.y  - ITB.y;
		W.z = P.z  - ITB.z;
		V.x = ILB.x - ITB.x;
		V.y = ILB.y - ITB.y;
		V.z = ILB.z - ITB.z;
		T.x =  V.y * W.z - V.z * W.y;
		T.y = -V.x * W.z + V.z * W.x;
		T.z =  V.x * W.y - V.y * W.x;
		if(T.x*T.x+T.y*T.y+T.z*T.z <1.0e-10 || T.x*Normal.x+T.y*Normal.y+T.z*Normal.z>=0.0) b2 = true; else b2 = false;

		W.x = P.x  - ILB.x;
		W.y = P.y  - ILB.y;
		W.z = P.z  - ILB.z;
		V.x = ILA.x - ILB.x;
		V.y = ILA.y - ILB.y;
		V.z = ILA.z - ILB.z;
		T.x =  V.y * W.z - V.z * W.y;
		T.y = -V.x * W.z + V.z * W.x;
		T.z =  V.x * W.y - V.y * W.x;
		if(T.x*T.x+T.y*T.y+T.z*T.z <1.0e-10 || T.x*Normal.x+T.y*Normal.y+T.z*Normal.z>=0.0) b3 = true; else b3 = false;

		W.x = P.x  - ILA.x;
		W.y = P.y  - ILA.y;
		W.z = P.z  - ILA.z;
		V.x = ITA.x - ILA.x;
		V.y = ITA.y - ILA.y;
		V.z = ITA.z - ILA.z;
		T.x =  V.y * W.z - V.z * W.y;
		T.y = -V.x * W.z + V.z * W.x;
		T.z =  V.x * W.y - V.y * W.x;
		if(T.x*T.x+T.y*T.y+T.z*T.z <1.0e-10 || T.x*Normal.x+T.y*Normal.y+T.z*Normal.z>=0.0) b4 = true; else b4 = false;
		
		if(b1 && b2 && b3 && b4) 
		{
			I.Set(P.x, P.y, P.z);
			return true;
		}
	}
	return false;
}

double CPanel::GetArea()
{
	return Area;
}


double CPanel::Width()
{
	return sqrt( (s_pNode[m_iLB].y - s_pNode[m_iLA].y)*(s_pNode[m_iLB].y - s_pNode[m_iLA].y)
	            +(s_pNode[m_iLB].z - s_pNode[m_iLA].z)*(s_pNode[m_iLB].z - s_pNode[m_iLA].z));
}


void CPanel::RotateBC(CVector const &HA, Quaternion &Qt)
{
	// HA is the rotation center
	//rotates the panels properties which are used in control analysis
//	Qt.Conjugate(Vortex);

	W.x = VortexPos.x - HA.x;
	W.y = VortexPos.y - HA.y;
	W.z = VortexPos.z - HA.z;
	Qt.Conjugate(W);
	VortexPos.x = W.x + HA.x;
	VortexPos.y = W.y + HA.y;
	VortexPos.z = W.z + HA.z;

	W.x = VA.x - HA.x;
	W.y = VA.y - HA.y;
	W.z = VA.z - HA.z;
	Qt.Conjugate(W);
	VA.x = W.x + HA.x;
	VA.y = W.y + HA.y;
	VA.z = W.z + HA.z;

	W.x = VB.x - HA.x;
	W.y = VB.y - HA.y;
	W.z = VB.z - HA.z;
	Qt.Conjugate(W);
	VB.x = W.x + HA.x;
	VB.y = W.y + HA.y;
	VB.z = W.z + HA.z;

	W.x = CtrlPt.x - HA.x;
	W.y = CtrlPt.y - HA.y;
	W.z = CtrlPt.z - HA.z;
	Qt.Conjugate(W);
	CtrlPt.x = W.x + HA.x;
	CtrlPt.y = W.y + HA.y;
	CtrlPt.z = W.z + HA.z;

	W.x = CollPt.x - HA.x;
	W.y = CollPt.y - HA.y;
	W.z = CollPt.z - HA.z;
	Qt.Conjugate(W);
	CollPt.x = W.x + HA.x;
	CollPt.y = W.y + HA.y;
	CollPt.z = W.z + HA.z;

	Qt.Conjugate(Vortex);
	Qt.Conjugate(Normal);
}



void CPanel::RotatePanel(CVector const &O, Quaternion &Qt)
{
	// Caution : do not use, the end nodes are rotated multiple times if part of more than one panel
	// O is the rotation center
	// Rotates the panel's corner points and resets the local fram
	W.x = s_pNode[m_iLA].x - O.x;
	W.y = s_pNode[m_iLA].y - O.y;
	W.z = s_pNode[m_iLA].z - O.z;
	Qt.Conjugate(W);
	s_pNode[m_iLA].x = W.x + O.x;
	s_pNode[m_iLA].y = W.y + O.y;
	s_pNode[m_iLA].z = W.z + O.z;

	W.x = s_pNode[m_iLB].x - O.x;
	W.y = s_pNode[m_iLB].y - O.y;
	W.z = s_pNode[m_iLB].z - O.z;
	Qt.Conjugate(W);
	s_pNode[m_iLB].x = W.x + O.x;
	s_pNode[m_iLB].y = W.y + O.y;
	s_pNode[m_iLB].z = W.z + O.z;

	W.x = s_pNode[m_iTA].x - O.x;
	W.y = s_pNode[m_iTA].y - O.y;
	W.z = s_pNode[m_iTA].z - O.z;
	Qt.Conjugate(W);
	s_pNode[m_iTA].x = W.x + O.x;
	s_pNode[m_iTA].y = W.y + O.y;
	s_pNode[m_iTA].z = W.z + O.z;

	W.x = s_pNode[m_iTB].x - O.x;
	W.y = s_pNode[m_iTB].y - O.y;
	W.z = s_pNode[m_iTB].z - O.z;
	Qt.Conjugate(W);
	s_pNode[m_iTB].x = W.x + O.x;
	s_pNode[m_iTB].y = W.y + O.y;
	s_pNode[m_iTB].z = W.z + O.z;

	if(m_Pos==BOTSURFACE) SetFrame(s_pNode[m_iLB], s_pNode[m_iLA], s_pNode[m_iTB], s_pNode[m_iTA]);
	else                  SetFrame(s_pNode[m_iLA], s_pNode[m_iLB], s_pNode[m_iTA], s_pNode[m_iTB]);
}














