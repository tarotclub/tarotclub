/*=============================================================================
 * TarotClub - ServerManager.h
 *=============================================================================
 * Manage the TarotClubServer executable interactions
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

#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QThread>
#include <QtNetwork>
#include <QProcess>
#include <QDockWidget>

/*****************************************************************************/
class ServerManager : public QThread
{
   Q_OBJECT

private:
   QProcess tarotServer;

public:
   ServerManager();

   void run();

};


#endif // SERVERMANAGER_H
