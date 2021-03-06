/****************************************************************************

	WindFieldCreator Class
		Copyright (C) 2012 Nikolai Moesus

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

#ifndef MODULE_H
#define MODULE_H

#include <QObject>
class QAction;
class QMainWindow;
class QToolBar;
class QPainter;

#include "TwoDWidgetEventInterface.h"
class GLWidget;
class TwoDWidget;
class MainFrame;


class ModuleBase : public QObject
{
	Q_OBJECT
	
public:
	ModuleBase ();
	virtual void addMainMenuEntries ();  // adds the module specific Menu entries. Override if needed.
    void SetToolbarVisibility(bool visible);
	
protected:
	QAction *m_activationAction;  // the Action that provides the toolbar button
	bool m_firstView;  // if the module is shown for the first time
	int m_globalModuleIndentifier;  // DO NOT USE IF POSSIBLE! Saves the identifier of module such as WINDFIELDMODULE
									// won't be needed any longer, as soon as all modules inherit this
									// class and can compare (g_mainFrame->currentModule == g_windFieldModule)
	
	// registrates an action at the given toolbar
	void registrateAtToolbar (QString name, QString tooltip, QString imagePath, QToolBar *toolbar);

	virtual void initView () = 0;  // fill the form with standart values etc.
		
public slots:
	virtual void onActivationActionTriggered();  // reacts on the button click in toolbar
	virtual void onModuleChanged ();  // will hide this module if no longer active
};



class GLModule
{	
public:
	void reportGLChange ();  // reports to this module the need for redrawing to the GLWidget
	virtual void onRedraw () = 0;  // is called when GLWidget redraws its content
	
protected:
	GLModule ();
	virtual void configureGL () = 0;  // configures openGL for the needs of the shown module
	virtual void showModule ();
	virtual void hideModule ();
	
	GLWidget *m_glWidget;  // pointer to the GLWidget of QBlade

	
private:
    int m_cameraSaveIndex;  // index where the camera setting for this module is saved

};



class TwoDModule : public TwoDWidgetInterface
{
public:
	void reportGraphChange ();  // reports to this module the need for redrawing
	
protected:
	TwoDModule ();	
	
	virtual void showModule();
	virtual void hideModule ();
};



class DualModule : public GLModule, public TwoDModule {
public:
	void setTwoDView ();
	void setGLView ();
	
protected:
	DualModule ();
		
	void showModule();
    void hideModule ();
};

#endif // MODULE_H
