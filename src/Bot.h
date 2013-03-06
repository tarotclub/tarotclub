/*=============================================================================
 * TarotClub - Bot.h
 *=============================================================================
 * Modélise un robot joueur. Interface vers les scripts d'IA
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

#ifndef _BOT_H
#define _BOT_H

#include "Client.h"
#include <QScriptEngine>
#include <QtScriptTools>


class DebugObject: public QObject
{
   Q_OBJECT
public:

public slots:
    void Print(const char *message)
    {
        qDebug(message);
    }
};

class StatsWrapper: public QObject
{
   Q_OBJECT
public:

    StatsWrapper(DeckStats &stats)
        : mDeck(stats)
    {

    }

    DeckStats &mDeck;

public slots:
    int getNumberOfCards()
    {
        return mDeck.nbCards;
    }

    int getNumberOfAtouts()
    {
        return mDeck.atouts;
    }

    int getNumberOfBouts()
    {
        return mDeck.bouts;   // 0, 1, 2 ou 3
    }

    int getNumberOfAtoutsMajeurs()
    {
        return mDeck.atoutsMajeurs; // atouts >= 15
    }

    int getNumberOfRois()
    {
        return mDeck.rois;
    }

    int getNumberOfDames()
    {
        return mDeck.dames;
    }

    int getNumberOfCavaliers()
    {
        return mDeck.cavaliers;
    }

    int getNumberOfValets()
    {
        return mDeck.valets;
    }

    int getNumberOfMariages()
    {
        return mDeck.mariages;   // nombre de mariages dans la main
    }

    int getNumberOfLongues()
    {
        return mDeck.longues;
    }

    int getNumberOfCoupes()
    {
        return mDeck.coupes;     // aucune carte dans une couleur
    }

    int getNumberOfSingletons()
    {
        return mDeck.singletons; // une seule carte dans une couleur
    }

    int getNumberOfSequences()
    {
        return mDeck.sequences;  // cartes qui se suivent (au moins 5 cartes pour être comptées)
    }

    int getNumberOfTrefles()
    {
        return mDeck.trefles;
    }

    int getNumberOfPics()
    {
        return mDeck.pics;
    }

    int getNumberOfCoeurs()
    {
        return mDeck.coeurs;
    }

    int getNumberOfCarreaux()
    {
        return mDeck.carreaux;
    }

    bool hasPetits()
    {
        return mDeck.petit;
    }

    bool hasVingtEtUn()
    {
        return mDeck.vingtEtUn;
    }

    bool hasExcuse()
    {
        return mDeck.excuse;
    }

};

/*****************************************************************************/
class Bot : public Client
{
   Q_OBJECT

private:
   QTimer  timeBeforeSend;
   QScriptEngine botEngine;
   QScriptEngineDebugger debugger;
   StatsWrapper statsObj;
   DebugObject dbgObj;

   bool initializeScriptContext();

public:
   Bot();
   ~Bot();



   void setTimeBeforeSend(int t);

// Private slots are not visible in a QtScript
private slots:
   void slotTimeBeforeSend();

   // client events
   void slotMessage( const QString &text );
   void slotReceptionCartes();
   void slotAfficheSelection( Place p );
   void slotChoixEnchere( Contrat c );
   void slotAfficheEnchere( Place, Contrat );
   void slotRedist();
   void slotAfficheChien();
   void slotPrepareChien();
   void slotDepartDonne(Place p,Contrat c);
   void slotJoueCarte();
   void slotAfficheCarte(int id, Place p);
   void slotFinDonne(Place, float, bool lastDeal);
   void slotWaitPli(Place p, float points);


};

#endif // _BOT_H

//=============================================================================
// End of file Bot.h
//=============================================================================
