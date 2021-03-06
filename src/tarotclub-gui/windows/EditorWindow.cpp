/*=============================================================================
 * TarotClub - EditorWindow.cpp
 *=============================================================================
 * Edit custom deal for each player
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

#include <QObject>
#include <QFileDialog>
#include <QMessageBox>
#include "Translations.h"
#include "EditorWindow.h"

/*****************************************************************************/
EditorWindow::EditorWindow(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    connect(ui.openButton, SIGNAL(clicked()), this, SLOT(slotOpenDeal()));
    connect(ui.saveButton, SIGNAL(clicked()), this, SLOT(slotSaveDeal()));
    connect(ui.buttonClear, &QPushButton::clicked, this, &EditorWindow::slotClear);
    connect(ui.closeButton, &QPushButton::clicked, this, &EditorWindow::slotClose);

    // Arrow to dispatch cards
    connect(ui.toSouth, SIGNAL(clicked()), this, SLOT(slotToSouth()));
    connect(ui.toNorth, SIGNAL(clicked()), this, SLOT(slotToNorth()));
    connect(ui.toWest, SIGNAL(clicked()), this, SLOT(slotToWest()));
    connect(ui.toEast, SIGNAL(clicked()), this, SLOT(slotToEast()));

    // Double click to return the card into the main list
    connect(ui.southList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(slotRemoveSouthCard(QListWidgetItem *)));
    connect(ui.northList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(slotRemoveNorthCard(QListWidgetItem *)));
    connect(ui.westList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(slotRemoveWestCard(QListWidgetItem *)));
    connect(ui.eastList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(slotRemoveEastCard(QListWidgetItem *)));

    connect(ui.randomButton, &QPushButton::clicked, this, &EditorWindow::slotRandomDeal);

    ui.firstPlayerCombo->addItem(PlaceToString(Place(Place::SOUTH)));
    ui.firstPlayerCombo->addItem(PlaceToString(Place(Place::EAST)));
    ui.firstPlayerCombo->addItem(PlaceToString(Place(Place::NORTH)));
    ui.firstPlayerCombo->addItem(PlaceToString(Place(Place::WEST)));
}
/*****************************************************************************/
void EditorWindow::Initialize()
{
    Clear();

    Deck deck;
    deck.CreateTarotDeck();
    for (const auto &c : deck)
    {
        ui.mainCardList->addItem(new QListWidgetItem(c.ToString().data()));
    }
}
/*****************************************************************************/
void EditorWindow::Clear()
{
    ui.mainCardList->clear();
    ui.eastList->clear();
    ui.northList->clear();
    ui.southList->clear();
    ui.westList->clear();
}
/*****************************************************************************/
void EditorWindow::slotRandomDeal()
{
    Clear();

    DealGenerator editor;
    editor.CreateRandomDeal(4U);
    editor.SetFirstPlayer(DealGenerator::RandomPlace(4U));
    RefreshUi(editor);
}
/*****************************************************************************/
void EditorWindow::slotClear()
{
    Initialize();
}
/*****************************************************************************/
void EditorWindow::slotClose()
{
    emit sigHide();
}
/*****************************************************************************/
void EditorWindow::slotToSouth()
{
    if (ui.southList->count() == 18)
    {
        return;
    }

    int row = ui.mainCardList->currentRow();
    QListWidgetItem *element = ui.mainCardList->takeItem(row);
    ui.southList->addItem(element);
}
/*****************************************************************************/
void EditorWindow::slotToNorth()
{
    if (ui.northList->count() == 18)
    {
        return;
    }

    int row = ui.mainCardList->currentRow();
    QListWidgetItem *element = ui.mainCardList->takeItem(row);
    ui.northList->addItem(element);
}
/*****************************************************************************/
void EditorWindow::slotToWest()
{
    if (ui.westList->count() == 18)
    {
        return;
    }

    int row = ui.mainCardList->currentRow();
    QListWidgetItem *element = ui.mainCardList->takeItem(row);
    ui.westList->addItem(element);
}
/*****************************************************************************/
void EditorWindow::slotToEast()
{
    if (ui.eastList->count() == 18)
    {
        return;
    }

    int row = ui.mainCardList->currentRow();
    QListWidgetItem *element = ui.mainCardList->takeItem(row);
    ui.eastList->addItem(element);
}
/*****************************************************************************/
void EditorWindow::slotRemoveSouthCard(QListWidgetItem *item)
{
    Q_UNUSED(item);

    int row = ui.southList->currentRow();
    QListWidgetItem *element = ui.southList->takeItem(row);
    ui.mainCardList->addItem(element);
}
/*****************************************************************************/
void EditorWindow::slotRemoveNorthCard(QListWidgetItem *item)
{
    Q_UNUSED(item);

    int row = ui.northList->currentRow();
    QListWidgetItem *element = ui.northList->takeItem(row);
    ui.mainCardList->addItem(element);
}
/*****************************************************************************/
void EditorWindow::slotRemoveWestCard(QListWidgetItem *item)
{
    Q_UNUSED(item);

    int row = ui.westList->currentRow();
    QListWidgetItem *element = ui.westList->takeItem(row);
    ui.mainCardList->addItem(element);
}
/*****************************************************************************/
void EditorWindow::slotRemoveEastCard(QListWidgetItem *item)
{
    Q_UNUSED(item);

    int row = ui.eastList->currentRow();
    QListWidgetItem *element = ui.eastList->takeItem(row);
    ui.mainCardList->addItem(element);
}
/*****************************************************************************/
void EditorWindow::slotOpenDeal()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open a deal file"),
                       "tarot.json",
                       tr("JSON (*json)"));
    if (fileName.isEmpty())
    {
        return;
    }

    DealGenerator editor;
    if (editor.LoadFile(fileName.toStdString()))
    {
        Clear();
        RefreshUi(editor);
    }
    else
    {
        QMessageBox::critical(this, tr("Loading error"), tr("Cannot read the input deal file!"));
    }
}
/*****************************************************************************/
void EditorWindow::RefreshUi(const DealGenerator &editor)
{
    // Dog
    for (const auto &c : editor.GetDogDeck())
    {
        ui.mainCardList->addItem(new QListWidgetItem(c.ToString().data()));
    }
    // East
    for (const auto &c : editor.GetPlayerDeck(Place(Place::EAST)))
    {
        ui.eastList->addItem(new QListWidgetItem(c.ToString().data()));
    }
    // West
    for (const auto &c : editor.GetPlayerDeck(Place(Place::WEST)))
    {
        ui.westList->addItem(new QListWidgetItem(c.ToString().data()));
    }
    // South
    for (const auto &c : editor.GetPlayerDeck(Place(Place::SOUTH)))
    {
        ui.southList->addItem(new QListWidgetItem(c.ToString().data()));
    }
    // North
    for (const auto &c : editor.GetPlayerDeck(Place(Place::NORTH)))
    {
        ui.northList->addItem(new QListWidgetItem(c.ToString().data()));
    }

    ui.firstPlayerCombo->setCurrentIndex(editor.GetFirstPlayer().Value());

}
/*****************************************************************************/
void EditorWindow::slotSaveDeal()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save deal"),
                       "tarot.json",
                       tr("JSON (*json)"));
    if (fileName.isEmpty())
    {
        return;
    }

    DealGenerator editor;
    Deck deck;

    // Dog
    for (int i = 0; i < ui.mainCardList->count(); i++)
    {
        QListWidgetItem *item = ui.mainCardList->item(i);
        if (item != NULL)
        {
            deck.Append(Card(item->text().toStdString()));
        }
    }
    editor.SetDogDeck(deck);
    deck.Clear();

    // East
    for (int i = 0; i < ui.eastList->count(); i++)
    {
        QListWidgetItem *item = ui.eastList->item(i);
        if (item != NULL)
        {
            deck.Append(Card(item->text().toStdString()));
        }
    }
    editor.SetPlayerDeck(Place(Place::EAST), deck);
    deck.Clear();

    // West
    for (int i = 0; i < ui.westList->count(); i++)
    {
        QListWidgetItem *item = ui.westList->item(i);
        if (item != NULL)
        {
            deck.Append(Card(item->text().toStdString()));
        }
    }
    editor.SetPlayerDeck(Place(Place::WEST), deck);
    deck.Clear();

    // South
    for (int i = 0; i < ui.southList->count(); i++)
    {
        QListWidgetItem *item = ui.southList->item(i);
        if (item != NULL)
        {
            deck.Append(Card(item->text().toStdString()));
        }
    }
    editor.SetPlayerDeck(Place(Place::SOUTH), deck);
    deck.Clear();

    // North
    for (int i = 0; i < ui.northList->count(); i++)
    {
        QListWidgetItem *item = ui.northList->item(i);
        if (item != NULL)
        {
            deck.Append(Card(item->text().toStdString()));
        }
    }
    editor.SetPlayerDeck(Place(Place::NORTH), deck);

    editor.SetFirstPlayer(Place(ui.firstPlayerCombo->currentIndex()));
    editor.SaveFile(fileName.toStdString());
}

//=============================================================================
// Fin du fichier EditorWindow.cpp
//=============================================================================
