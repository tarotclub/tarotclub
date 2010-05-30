/*=============================================================================
 * TarotClub - TarotEngine.h
 *=============================================================================
 * Moteur de jeu principal + serveur de jeu réseau
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
#ifndef _TAROTENGINE_H
#define _TAROTENGINE_H

#include <QtNetwork>
#include <QMap>
#include <QThread>
#include "Card.h"
#include "Deck.h"
#include "Player.h"
#include "Score.h"
#include "Jeu.h"
#include "defines.h"

enum {
   MsgStartGame = QEvent::User+1,
   MsgRandomDeal,
   MsgStopGame,
   MsgExitGame
};


/*****************************************************************************/
class EvStartGame : public QEvent
{
public:
   EvStartGame() : QEvent( (QEvent::Type)MsgStartGame ) {}
};
/*****************************************************************************/
class EvStopGame : public QEvent
{
public:
   EvStopGame() : QEvent( (QEvent::Type)MsgStopGame ) {}
};
/*****************************************************************************/
class EvExitGame : public QEvent
{
public:
   EvExitGame() : QEvent( (QEvent::Type)MsgExitGame ) {}
};

/*****************************************************************************/
class TarotEngine : public QThread
{
   Q_OBJECT

private:
   QMap<QTcpSocket*, Player*> players;
   QTcpServer  server;

   QTimer      timerBetweenPlayers;
   QTimer      timerBetweenTurns;
   QString     gamePath;
   GameInfos   infos;
   Score       score;
   Deck        deckChien;     // le chien
   Deck        mainDeck;      // le tas principal
   Place       donneur;       // qui est le donneur ?
   Place       tour;          // A qui le tour (d'enchérir ou de jouer)
   Sequence    sequence;      // indique la séquence de jeu actuelle
   bool        newGame;       // vrai si une nouvelle partie a été commencée
   int         cptVu;         // counter of chien seen by clients
   DealType    dealType;
   int         dealNumber;
   QString     dealFile;

protected:
   void customEvent( QEvent *e );

public:
   TarotEngine();
    ~TarotEngine();

   void run();

   int getConnectedPlayers( Identity *idents );
   int getNumberOfConnectedPlayers();
   Score *getScore();
   int getDealNumber();
   Player *getPlayer(Place p);
   QTcpSocket *getConnection(Place p);

   void setTimerBetweenPlayers(int t);
   void setTimerBetweenTurns(int t);
   void setDealType(DealType type);
   void setDealNumber(int deal);
   void setDealFile(QString file);

   void newServerGame( int port );
   void closeServerGame();
   void closeClients();
   bool cardIsValid( Card *c, Place p );
   bool cardExists( Card *c, Place p );
   void customDeal();
   void randomDeal();
   Place calculGagnantPli();
   void nouvelleDonne();
   void jeu();
   void jeuNext();
   Place nextPlayer( Place j );
   bool finLevee();
   void sequenceEncheres();
   void montreChien();

   // Fonctions réseau
   void sendCards( Place p, quint8 *params );
   void askBid( Place p, Contrat c );
   void sendBid( Place p, Contrat c );
   void broadcast( QByteArray &block );
   void doAction( QDataStream &in, QTcpSocket* cnx );
   void sendMessage( const QString &message, Place p );
   void sendPlayersList();
   void sendShowChien();
   void sendDoChien();
   void sendJoueCarte();
   void sendCard( Card *c );
   void sendDepartDonne();
   void sendRedist();
   void sendFinDonne( ScoreInfos *score_inf );
   void sendWaitPli();
   void selectPlayer( Place p );

signals:
   void sigPrintMessage(const QString &);

public slots:
   void slotNewConnection();
   void slotClientClosed();
   void slotReadData();

   void slotTimerBetweenPlayers();
   void slotTimerBetweenTurns();

};

#endif // _TAROTENGINE_H

//=============================================================================
// End of file TarotEngine.h
//=============================================================================
