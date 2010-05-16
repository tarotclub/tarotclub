/*=============================================================================
 * TarotClub - OptionsWindow.cpp
 *=============================================================================
 * OptionsWindow : fenêtre d'options
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

#include "OptionsWindow.h"
#include <QDir>
#include <QMessageBox>
#include <QColorDialog>
#include <QString>

/*****************************************************************************/
OptionsWindow::OptionsWindow( QWidget* parent, Qt::WFlags fl )
   : QDialog( parent, fl )
{
    ui.setupUi(this);

   connect( ui.btnDefaut, SIGNAL(clicked()), this, SLOT(slotBtnDefaut()) );
   connect( ui.btnOk, SIGNAL(clicked()), this, SLOT(slotBtnOk()) );

   // Défilement des cartes
   connect( ui.slider1, SIGNAL(valueChanged(int)), this, SLOT(slider1Changed(int)) );
   connect( ui.slider2, SIGNAL(valueChanged(int)), this, SLOT(slider2Changed(int)) );

   // Boutons choix de l'avatar
   connect( ui.btnPixSud, SIGNAL(clicked()), this, SLOT(slotBtnPixSud()) );
   connect( ui.btnPixEst, SIGNAL(clicked()), this, SLOT(slotBtnPixEst()) );
   connect( ui.btnPixNord, SIGNAL(clicked()), this, SLOT(slotBtnPixNord()) );
   connect( ui.btnPixOuest, SIGNAL(clicked()), this, SLOT(slotBtnPixOuest()) );
   connect( ui.btnPixNordOuest, SIGNAL(clicked()), this, SLOT(slotBtnPixNordOuest()) );

   // Choix de la couleur du tapis
   connect( ui.tapisColor, SIGNAL(clicked()), this, SLOT(slotColorPicker()) );

   QStringList listeNiveaux;
   listeNiveaux.append("Amibe");

   ui.niveauEst->addItems( listeNiveaux );
   ui.niveauNord->addItems( listeNiveaux );
   ui.niveauOuest->addItems( listeNiveaux );
   ui.niveauNordOuest->addItems( listeNiveaux );

}
/*****************************************************************************/
void OptionsWindow::slotColorPicker()
{
   QColor color = QColorDialog::getColor(Qt::darkGreen, this);
   if (color.isValid()) {
       colorName = color.name();
       ui.tapisColor->setPalette(QPalette(color));
       ui.tapisColor->setAutoFillBackground(true);
   }
}
/*****************************************************************************/
void OptionsWindow::setOptions( GameOptions *opt )
{
   options = *opt;
   refresh();
}
/*****************************************************************************/
void OptionsWindow::slotBtnOk()
{
   options.identities[0].name = ui.nomJoueurSud->text();
   options.identities[1].name = ui.nomJoueurEst->text();
   options.identities[2].name = ui.nomJoueurNord->text();
   options.identities[3].name = ui.nomJoueurOuest->text();
   options.identities[4].name = ui.nomJoueurNordOuest->text();

   // TODO: three players version !
/*   if( ui.troisJoueurs->isChecked() ) {
      options.nbPlayers = 3;
   } else if( ui.cinqJoueurs->isChecked() ) {
      options.nbPlayers = 5;
   } else {*/
      options.nbPlayers = 4;
//   }

   options.timer1 = ui.slider1->value();
   options.timer2 = ui.slider2->value();

   options.identities[0].quote = ui.citationSud->text();

   if( ui.sexeM->isChecked() ) {
      options.identities[0].sex = MALE;
   } else {
      options.identities[0].sex = FEMALE;
   }

   if( ui.afficheAvatars->isChecked() ) {
      options.showAvatars = true;
   } else {
      options.showAvatars = false;
   }

   options.port = QString( ui.portReseau->text()).toInt();
   options.langue = ui.langList->currentIndex();
   if( indexLangue != options.langue ) {
     QMessageBox::information( this, trUtf8("Information"),
                    trUtf8("Vous devez redémarrer le jeu pour que le changement de langue soit actif.\n\n") );
   }
   options.tapis = colorName;
   accept();
}
/*****************************************************************************/
void OptionsWindow::slotBtnDefaut()
{
   ConfigFile::setDefault( &options );
   refresh();
}
/*****************************************************************************/
GameOptions *OptionsWindow::getOptions()
{
   return(&options);
}
/*****************************************************************************/
void OptionsWindow::slider1Changed( int value )
{
   ui.temps1->setText( trUtf8("%1 secondes").arg((float)(value/100)/10) );
}
/*****************************************************************************/
void OptionsWindow::slider2Changed( int value )
{
   ui.temps2->setText( trUtf8("%1 secondes").arg((float)(value/100)/10) );
}
/*****************************************************************************/
QString OptionsWindow::choixAvatar()
{
   QString defaultAvatar = ":/images/avatars/vide.png";
   Ui::Avatars ui;
   QDialog *diag = new QDialog(this);
   ui.setupUi(diag);

   // populate the list with internal avatar
   ui.avatarsList->clear();
   QDir dir(":/images/avatars");
   dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
   QFileInfoList list = dir.entryInfoList();

   // On affiche la liste des avatars
   for(int i = 0; i < list.size(); ++i) {
       QFileInfo fileInfo = list.at(i);
       QListWidgetItem *item = new QListWidgetItem(ui.avatarsList);
       item->setText(fileInfo.baseName());
       item->setIcon(QIcon(fileInfo.absoluteFilePath()));
       item->setData(Qt::UserRole, fileInfo.absoluteFilePath());
       item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
   }

   if( diag->exec() == QDialog::Rejected ) {
      return(defaultAvatar);
   } else if (ui.avatarsList->currentItem() != NULL) {
      // On retourne le nom de l'image
      return(ui.avatarsList->currentItem()->data(Qt::UserRole).toString());
   } else {
      return(defaultAvatar);
   }
}
/*****************************************************************************/
void OptionsWindow::slotBtnPixSud()
{
   QString s;
   QPixmap im;

   s = choixAvatar();
   if( im.load( s ) == false ) {
      return;
   }
   options.identities[0].avatar = s;
   ui.pixSud->setPixmap( im );
}
/*****************************************************************************/
void OptionsWindow::slotBtnPixEst()
{
   QString s;
   QPixmap im;

   s = choixAvatar();
   if( im.load( s ) == false ) {
      return;
   }
   options.identities[1].avatar = s;
   ui.pixEst->setPixmap( im );
}
/*****************************************************************************/
void OptionsWindow::slotBtnPixNord()
{
   QString s;
   QPixmap im;

   s = choixAvatar();
   if( im.load( s ) == false ) {
      return;
   }
   options.identities[2].avatar = s;
   ui.pixNord->setPixmap( im );
}
/*****************************************************************************/
void OptionsWindow::slotBtnPixOuest()
{
   QString s;
   QPixmap im;

   s = choixAvatar();
   if( im.load( s ) == false ) {
      return;
   }
   options.identities[3].avatar = s;
   ui.pixOuest->setPixmap( im );
}
/*****************************************************************************/
void OptionsWindow::slotBtnPixNordOuest()
{
   QString s;
   QPixmap im;

   s = choixAvatar();
   if( im.load( s ) == false ) {
      return;
   }
   options.identities[4].avatar = s;
   ui.pixNordOuest->setPixmap( im );
}
/*****************************************************************************/
/**
 * Rafraichi les Widgets graphiques
 */
void OptionsWindow::refresh()
{
   QPixmap im;

   ui.nomJoueurSud->setText( options.identities[0].name );
   ui.nomJoueurEst->setText( options.identities[1].name );
   ui.nomJoueurNord->setText( options.identities[2].name );
   ui.nomJoueurOuest->setText( options.identities[3].name );
   ui.nomJoueurNordOuest->setText( options.identities[4].name );

   // TODO: three players version !
   //ui.quatreJoueurs->setChecked( true );

   ui.slider1->setValue( options.timer1 );
   ui.slider2->setValue( options.timer2 );
   ui.citationSud->setText( options.identities[0].quote );

   if( options.identities[0].sex == MALE ) {
      ui.sexeM->setChecked( true );
   } else {
      ui.sexeF->setChecked( true );
   }

   ui.afficheAvatars->setChecked( options.showAvatars );

   ui.langList->setCurrentIndex( options.langue );
   indexLangue = options.langue;

   if( im.load( options.identities[0].avatar ) == true ) {
      ui.pixSud->setPixmap( im );
   }
   if( im.load( options.identities[1].avatar ) == true ) {
      ui.pixEst->setPixmap( im );
   }
   if( im.load( options.identities[2].avatar ) == true ) {
      ui.pixNord->setPixmap( im );
   }
   if( im.load( options.identities[3].avatar ) == true ) {
      ui.pixOuest->setPixmap( im );
   }
   if( im.load( options.identities[4].avatar ) == true ){
      ui.pixNordOuest->setPixmap( im );
   }

   ui.niveauEst->setCurrentIndex(  0  );
   ui.niveauNord->setCurrentIndex( 0 );
   ui.niveauOuest->setCurrentIndex( 0 );
   ui.niveauNordOuest->setCurrentIndex( 0 );

   ui.portReseau->setValue( options.port );

   QColor color(options.tapis);
   if (color.isValid()) {
       colorName = color.name();
       ui.tapisColor->setPalette(QPalette(color));
       ui.tapisColor->setAutoFillBackground(true);
   }
}

//=============================================================================
// Fin du fichier OptionsWindow.cpp
//=============================================================================
