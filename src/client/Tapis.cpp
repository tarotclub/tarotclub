/*=============================================================================
 * TarotClub - Tapis.cpp
 *=============================================================================
 * visual game contents
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

#include "Tapis.h"
#include <QResizeEvent>
#include <QMouseEvent>
#include "textes.h"
#include "../Jeu.h"

#define NORTH_BOX_POS_X   350
#define NORTH_BOX_POS_Y   10

/*****************************************************************************/
GfxCard::GfxCard ( const QString & fileName, QGraphicsItem * parent ) : QGraphicsSvgItem(fileName, parent)

{
   status = CARD_NORMAL;
}
/*****************************************************************************/
int GfxCard::type() const
{
  // Enable the use of qgraphicsitem_cast with this item.
  return Type;
}
/*****************************************************************************/
CardStatus GfxCard::getStatus()
{
   return status;
}
/*****************************************************************************/
void GfxCard::setStatus(CardStatus s)
{
   status = s;
}
/*****************************************************************************/
void GfxCard::toggleStatus()
{
   if (status == CARD_NORMAL) {
      status = CARD_SELECTED;
      this->moveBy(0.0, -20.0);
   } else {
      status = CARD_NORMAL;
      this->moveBy(0.0, 20.0);
   }
}


/*****************************************************************************/
//          *                          *                        *
/*****************************************************************************/


Tapis::Tapis( QWidget *parent )
    : QGraphicsView( parent )
{
   setScene(&scene);

   //==============================================================
   //       BOUTONS ENCHERES
   //==============================================================
   groupBoutons = new QGroupBox( trUtf8("Enchères"), this );
   groupBoutons->hide();

   boutonPasse = new QPushButton(STR_PASSE);
   boutonPrise = new QPushButton(STR_PRISE);
   boutonGarde = new QPushButton(STR_GARDE);
   boutonGardeSans = new QPushButton(STR_GARDE_SANS);
   boutonGardeContre = new QPushButton(STR_GARDE_CONTRE);
   chelem = new QCheckBox("Chelem");

   QVBoxLayout *vbox = new QVBoxLayout;
   vbox->addWidget(boutonPasse);
   vbox->addWidget(boutonPrise);
   vbox->addWidget(boutonGarde);
   vbox->addWidget(boutonGardeSans);
   vbox->addWidget(boutonGardeContre);
   vbox->addWidget(chelem);
   groupBoutons->setLayout(vbox);

   groupBoutons->move(10,300);

   //==============================================================

   boutonAccepterChien = new QPushButton(trUtf8("Accepter le chien"), this);
   boutonAccepterChien->move(800, 462);
   boutonAccepterChien->setMinimumSize( boutonAccepterChien->sizeHint() );
   boutonAccepterChien->hide();

   //==============================================================

   boutonPresenterPoignee = new QPushButton(trUtf8("Présenter poignée"), this);
   boutonPresenterPoignee->move(800, 462);
   boutonPresenterPoignee->setMinimumSize( boutonPresenterPoignee->sizeHint() );
   boutonPresenterPoignee->hide();

   //==============================================================
   //       ELEMENTS DU CANVAS
   //==============================================================
   btNord = new PlayerBox(NORTH_BOX_POS_X, NORTH_BOX_POS_Y, &scene);
   btOuest = new PlayerBox(NORTH_BOX_POS_X-230, NORTH_BOX_POS_Y+103, &scene);
   btSud = new PlayerBox(NORTH_BOX_POS_X, NORTH_BOX_POS_Y+452, &scene);
   btEst = new PlayerBox(NORTH_BOX_POS_X+230, NORTH_BOX_POS_Y+103, &scene);

   btNord->show();
   btOuest->show();
   btSud->show();
   btEst->show();

   enchNord = new TextBox(NORTH_BOX_POS_X, NORTH_BOX_POS_Y+40, &scene);
   enchOuest = new TextBox(NORTH_BOX_POS_X-230, NORTH_BOX_POS_Y+143, &scene);
   enchSud = new TextBox(NORTH_BOX_POS_X, NORTH_BOX_POS_Y+412, &scene);
   enchEst = new TextBox(NORTH_BOX_POS_X+230, NORTH_BOX_POS_Y+143, &scene);

   connect( boutonPasse, SIGNAL(clicked()), this, SLOT(slotBoutton1()) );
   connect( boutonPrise, SIGNAL(clicked()), this, SLOT(slotBoutton2()) );
   connect( boutonGarde, SIGNAL(clicked()), this, SLOT(slotBoutton3()) );
   connect( boutonGardeSans, SIGNAL(clicked()), this, SLOT(slotBoutton4()) );
   connect( boutonGardeContre, SIGNAL(clicked()), this, SLOT(slotBoutton5()) );
   connect( boutonAccepterChien, SIGNAL(clicked()), this, SLOT(slotAccepteChien()) );
   connect( boutonPresenterPoignee, SIGNAL(clicked()), this, SLOT(slotPresenterPoignee()) );

   // appelle mouseEvent dès que la souris bouge, sans appuis sur les boutons
   viewport()->setMouseTracking(true);
   filter = AUCUN;
}
/*****************************************************************************/
void Tapis::setBackground(const QString &fichier)
{
   QColor color(fichier);
   if (color.isValid()) {
      scene.setBackgroundBrush(color);
   }
}
/*****************************************************************************/
GfxCard *Tapis::getGfxCard(int i)
{
   return cardsPics.at(i);
}
/*****************************************************************************/
Card *Tapis::getObjectCard(GfxCard *gc)
{
   Card *c = NULL;
   for( int i=0; i<cardsPics.size(); i++ ) {
      if( cardsPics.at(i) == gc ) {
         c = Jeu::getCard(i);
         break;
      }
   }
   return(c);
}
/*****************************************************************************/
void Tapis::setAccepterChienVisible(bool v)
{
   if (v == true) {
      boutonAccepterChien->show();
   } else {
      boutonAccepterChien->hide();
   }
}
/*****************************************************************************/
void Tapis::setBoutonPoigneeVisible(bool v)
{
   if (v == true) {
      boutonPresenterPoignee->show();
   } else {
      boutonPresenterPoignee->hide();
   }
}
/*****************************************************************************/
int Tapis::loadCards(GameOptions *opt)
{
   int i,j,n;
   QString varImg;
   QString image;

   //----- 4 couleurs
   for( i=0; i<4; i++ ){
      if( i==0 ) {
         varImg = "pique";
      } else if( i==1 ) {
         varImg = "coeur";
      } else if( i==2 ) {
         varImg = "trefle";
      } else {
         varImg = "carreau";
      }

      // de l'as au roi (14 cartes)
      for( j=0; j<14; j++ ) {
         n = i*14+j;
         image = QString(opt->deckFilePath + "/") + varImg + QString("-") + QString().sprintf("%02d.svg",j+1);

         GfxCard *item = new GfxCard(image);
         item->hide();
         cardsPics.insert(n, item);
         scene.addItem(item);
      }
   }

   //----- 21 atouts
   for( i=56; i<77; i++) {
      image = QString(opt->deckFilePath + "/atout-") + QString().sprintf("%02d.svg",i-55);

      GfxCard *item = new GfxCard(image);
      item->hide();
      cardsPics.insert(i, item);
      scene.addItem(item);
   }

   //----- L'excuse
   image = QString(opt->deckFilePath + "/excuse.svg");
   GfxCard *item = new GfxCard(image);
   item->hide();
   cardsPics.insert(77, item);
   scene.addItem(item);

   setCardScale(1.5);

   return 0;
}
/*****************************************************************************/
void Tapis::resizeEvent( QResizeEvent *e )
{
   QSize s;
   s = e->size();
   setSceneRect( 0, 0, s.width(), s.height() );
}
/*****************************************************************************/
void Tapis::setCardScale(float factor)
{
   for(int i=0; i<cardsPics.size(); i++) {
      cardsPics.at(i)->setScale(factor);
   }
}
/*****************************************************************************/
void Tapis::mousePressEvent( QMouseEvent *e )
{
   QList<QGraphicsItem *> list;

   emit sgnlViewportClicked();

   if( filter == AUCUN ) {
      return;
   }
   if( filter == JEU ) {
      list = scene.items(e->pos());
      if ( !list.isEmpty() ) {
         if( list.first()->type() == GfxCard::Type ) {
            GfxCard *c = (GfxCard *)list.first();
            emit sgnlClickCard( c );
         }
      }
   }
}
/*****************************************************************************/
/*
 * Emit a signal each time the cursor goes over a card
 */
void Tapis::mouseMoveEvent( QMouseEvent * e )
{
   QList<QGraphicsItem *> list;

   if( filter == AUCUN ) {
      return;
   }

   list = scene.items(e->pos());

   if ( !list.isEmpty() ) {
      // Si c'est une carte, retourne l'obet, sinon 0
      if( list.first()->type() == GfxCard::Type ) {
         GfxCard *c = (GfxCard *)list.first();
         emit sgnlMoveCursor( c );
      }
   } else {
      setCursorType( ARROW );
   }
}
/*****************************************************************************/
/**
 * n==0 : curseur normal avec une flèche
 * n==1 : curseur interdiction
 */
void Tapis::setCursorType( CursorType t )
{
   if( t == ARROW ) {
      QCursor cu( Qt::ArrowCursor );
      setCursor( cu );
   } else {
      QCursor cu( Qt::ForbiddenCursor );
      setCursor( cu );
   }
}
/*****************************************************************************/
void Tapis::colorisePreneur( Place preneur )
{
   if( preneur == SUD ) {
      btSud->highlightPlayer(true);
   } else if( preneur == EST ) {
      btEst->highlightPlayer(true);
   } else if( preneur == NORD ) {
      btNord->highlightPlayer(true);
   } else { // OUEST
      btOuest->highlightPlayer(true);
   }
}
/*****************************************************************************/
void Tapis::setText(Place p, const QString &txt)
{
   if( p == SUD ) {
      btSud->setText( txt );
   } else if( p == EST ) {
      btEst->setText( txt );
   } else if( p == NORD ) {
      btNord->setText( txt );
   } else { // OUEST
      btOuest->setText( txt );
   }
}
/*****************************************************************************/
void Tapis::setAvatar(Place p, const QString &file)
{
   if( p == SUD ) {
      btSud->setAvatar( file );
   } else if( p == EST ) {
      btEst->setAvatar( file );
   } else if( p == NORD ) {
      btNord->setAvatar( file );
   } else { // OUEST
      btOuest->setAvatar( file );
   }
}
/*****************************************************************************/
void Tapis::setNbPlayers(int n)
{
   btOuest->show();
   btSud->show();
   btEst->show();
   if( n == 4 ) {
      btNord->show();
   }
}
/*****************************************************************************/
/**
 * Change la place absolue (vue du serveur, qui est toujours SUD) en relatif
 * au joueur
 */
Place Tapis::retournePlace( Place origine, Place place_absolue )
{
   Place pl = SUD;

   if( origine == OUEST) {
      Place tab[] = {EST, NORD, OUEST, SUD};
      pl = tab[place_absolue];
   } else if( origine == NORD) {
      Place tab[] = {NORD, OUEST, SUD, EST};
      pl = tab[place_absolue];
   } else if( origine == EST) {
      Place tab[] = {OUEST, SUD, EST, NORD};
      pl = tab[place_absolue];
   } else {
      Place tab[] = {SUD, EST, NORD, OUEST};
      pl = tab[place_absolue];
   }

   return (pl);
}
/*****************************************************************************/
/**
 * Affiche les noms sur le tapis selon la position relative du joueur
 */
void Tapis::printNames( Identity *identities, Place place )
{
   Place serveur; // place relative du serveur

   serveur = retournePlace( place, SUD );

    btSud->setText( identities[retournePlace( serveur, SUD )].name );
    btEst->setText( identities[retournePlace( serveur,EST )].name  );
    btNord->setText( identities[retournePlace( serveur,NORD )].name  );
    btOuest->setText( identities[retournePlace( serveur,OUEST )].name  );

    btSud->setAvatar( identities[retournePlace( serveur,SUD )].avatar );
    btEst->setAvatar( identities[retournePlace( serveur,EST )].avatar );
    btNord->setAvatar( identities[retournePlace( serveur,NORD )].avatar );
    btOuest->setAvatar( identities[retournePlace( serveur,OUEST )].avatar );
}
/*****************************************************************************/
/**
 * Affiche une carte "c" à  l'emplacement "p" de la table.
 * p = NORD, OUEST, SUD, EST
 */
void Tapis::afficheCarte( GfxCard *c, Place p )
{
   int x, y, height, width;
   QRectF rect = cardsPics.at(0)->boundingRect();
   height = rect.height();
   width = rect.width();

   if( p == NORD ) {
      x = btNord->rect().x();
      y = btNord->rect().y();
   } else if( p == OUEST ) {
      x = btOuest->rect().x();
      y = btOuest->rect().y();
   } else if( p == SUD ) {
      x = btSud->rect().x();
      y = btSud->rect().y();
   } else if( p == EST ) {
      x = btEst->rect().x();
      y = btEst->rect().y();
   } else {
      //TODO: Nord-ouest
      x = 0;
      y = 0;
   }

   x = x + (TEXT_BOX_WIDTH - width)/4;
   if (p == SUD) {
      y = y - height - TEXT_BOX_HEIGHT - 40;
   } else {
      y = y + TEXT_BOX_HEIGHT + 10;
   }

   c->setPos(x,y);
   c->show();
}
/*****************************************************************************/
void Tapis::afficheSelection( Place p )
{
   btNord->selectPlayer(false);
   btOuest->selectPlayer(false);
   btEst->selectPlayer(false);
   btSud->selectPlayer(false);

   if( p == NORD ) {
      btNord->selectPlayer(true);
   } else if( p == OUEST ) {
      btOuest->selectPlayer(true);
   } else if( p == SUD ) {
      btSud->selectPlayer(true);
   } else if( p == EST ) {
      btEst->selectPlayer(true);
   } else {// Nord ouest
      //TODO
   }
}
/*****************************************************************************/
void Tapis::slotAfficheEnchere( Place enchereur, Contrat cont )
{
   QString mot;

   if( cont == GARDE_CONTRE ) {
      mot = STR_GARDE_CONTRE;
   } else if( cont == GARDE_SANS ) {
      mot = STR_GARDE_SANS;
   } else if( cont == GARDE ) {
      mot = STR_GARDE;
   } else if( cont == PRISE ) {
      mot = STR_PRISE;
   } else {
      mot = STR_PASSE;
   }

   if( enchereur == NORD ) {
      enchNord->setText(mot);
      enchNord->show();
   } else if( enchereur == OUEST ) {
      enchOuest->setText(mot);
      enchOuest->show();
   } else if( enchereur == SUD ) {
      enchSud->setText(mot);
      enchSud->show();
   } else { // EST
      enchEst->setText(mot);
      enchEst->show();
   }
}
/*****************************************************************************/
void Tapis::cacheEncheres()
{
   enchNord->hide();
   enchOuest->hide();
   enchSud->hide();
   enchEst->hide();
}
/*****************************************************************************/
void Tapis::slotAfficheBoutons( Contrat contrat )
{
   boutonPrise->show();
   boutonGarde->show();
   boutonGardeSans->show();
   boutonGardeContre->show();

   if( contrat >= PRISE ) {
      boutonPrise->hide();
   }
   if( contrat >= GARDE ) {
      boutonGarde->hide();
   }
   if( contrat >= GARDE_SANS ) {
      boutonGardeSans->hide();
   }
   if( contrat >= GARDE_CONTRE ) {
      boutonGardeContre->hide();
   }
   groupBoutons->show();
}
/*****************************************************************************/
void Tapis::showAvatars( bool b, int nb_players )
{
   btSud->enableAvatar(b);
   btEst->enableAvatar(b);
   btOuest->enableAvatar(b);

   if( nb_players > 3 ) {
      btNord->enableAvatar(b);
   }
   if( nb_players > 4 ) {
      //TODO: 5 players
      //btNordOuest->enableAvatar(b);
   }
}
/*****************************************************************************/
void Tapis::cacheBoutons()
{
   groupBoutons->hide();
}
/*****************************************************************************/
void Tapis::razTapis()
{
   cacheEncheres();
   groupBoutons->hide();

   btNord->highlightPlayer(false);
   btOuest->highlightPlayer(false);
   btEst->highlightPlayer(false);
   btSud->highlightPlayer(false);

   btNord->selectPlayer(false);
   btOuest->selectPlayer(false);
   btEst->selectPlayer(false);
   btSud->selectPlayer(false);
}
/*****************************************************************************/
void Tapis::resetCards()
{
   for(int i=0; i<cardsPics.size(); i++ ) {
      cardsPics.at(i)->hide();
      cardsPics.at(i)->setStatus(CARD_NORMAL);
   }
}
/*****************************************************************************/
void Tapis::slotBoutton1()
{
   emit sgnlContrat( PASSE );
}
/*****************************************************************************/
void Tapis::slotBoutton2()
{
   emit sgnlContrat( PRISE );
}
/*****************************************************************************/
void Tapis::slotBoutton3()
{
   emit sgnlContrat( GARDE );
}
/*****************************************************************************/
void Tapis::slotBoutton4()
{
   emit sgnlContrat( GARDE_SANS );
}
/*****************************************************************************/
void Tapis::slotBoutton5()
{
   emit sgnlContrat( GARDE_CONTRE );
}
/*****************************************************************************/
void Tapis::slotAccepteChien()
{
   emit sgnlAccepteChien();
}
/*****************************************************************************/
void Tapis::slotPresenterPoignee()
{
   emit sgnlPresenterPoignee();
}
/*****************************************************************************/
void Tapis::setFilter( Filter f )
{
   filter = f;
}

//=============================================================================
// End of file Tapis.cpp
//=============================================================================
