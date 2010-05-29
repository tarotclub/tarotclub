/*=============================================================================
 * TarotClub - MainWindow.h
 *=============================================================================
 * Main window of the game. Contains all the widgets.
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * This file must be used under the terms of the CeCILL.
 * This source file is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at
 * http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 *=============================================================================
 */

#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

// Includes Qt
#include <QMainWindow>
#include <QMenu>
#include <QAction>

// Includes locales
#include "../defines.h"
#include "Tapis.h"
#include "TextBox.h"
#include "AboutWindow.h"
#include "ResultWindow.h"
#include "OptionsWindow.h"
#include "ScoresDock.h"
#include "InfosDock.h"
#include "ChatDock.h"
#include "RoundDock.h"
#include "ui_NetClientUI.h"
#include "ui_ServerManagerUI.h"
#include "ui_RulesUI.h"

/*****************************************************************************/
class MainWindow : public QMainWindow
{
   Q_OBJECT

protected:

   // Menus
   QMenu *gameMenu;
   QMenu *paramsMenu;
   QMenu *helpMenu;

   // Menu Jeu
   //----local
   QAction    *newQuickGameAct;
   QAction    *newTournamentAct;
   QAction    *newNumberedDealAct;
   //----network
   QAction    *netGameServerAct;
   QAction    *netGameClientAct;
   //----misc
   QAction    *pliPrecAct;

   // Menu Paramètres
   QAction    *optionsAct;
   QAction    *scoresAct;
   QAction    *infosAct;
   QAction    *chatAct;
   QAction    *serverAct;

   Tapis      *tapis;      // QCanvasView

   // Autres fenêtres
   AboutWindow    *about;           // A propos
   ResultWindow   *resultWindow;    // Résultat d'une donne
   OptionsWindow  *optionsWindow;   // Options
   QDialog        *clientWindow;    // fenêtre pour joindre une partie réseau
   QDialog        *rulesWindow;

   // Dock windows
   ScoresDock     *scoresDock;
   InfosDock      *infosDock;
   ChatDock       *chatDock;
   RoundDock      *roundDock;
   QDockWidget    *serverDock;

   // UI classes
   Ui::NetClientUI  clientUI;
   Ui::serverDock serverUI;
   Ui::RulesUI    rulesUI;

public:
   MainWindow( QWidget* parent = 0, Qt::WFlags f = 0 );

public slots:
   void  slotScoresDock(void);
   void  slotInfosDock(void);
   void  slotChatDock(void);
   void  slotServerDock(void);
   void  closeChat();
   void  closeScores();
};

#endif // _MAINWINDOW_H

//=============================================================================
// End of file TarotWindow.h
//=============================================================================
