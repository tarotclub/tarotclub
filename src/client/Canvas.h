/*=============================================================================
 * TarotClub - Canvas.h
 *=============================================================================
 * Visual game contents
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

#ifndef CANVAS_H
#define CANVAS_H

// Qt includes
#include <QtSvg>
#include <QtGui>
#include <QVector>
#include <QList>
#include <QTemporaryFile>

// Game includes
#include "../defines.h"
#include "../Deck.h"
#include "TextBox.h"
#include "PlayerBox.h"
#include "ClientConfig.h"
#include "GfxCard.h"
#include "BidsForm.h"

/*****************************************************************************/
class Canvas : public QGraphicsView
{

    Q_OBJECT

public:
    enum Filter
    {
        BLOCK_ALL,
        MENU,
        GAME_ONLY
    };

    enum CursorType
    {
        FORBIDDEN,
        ARROW
    };

    Canvas(QWidget *parent);

    // Helpers
    bool Initialize(ClientOptions &opt);
    void ShowTaker(Place taker, Place myPlace);
    void ShowSelection(Place p, Place myPlace);
    void DrawCard(GfxCard *c, Place p, Place myPlace);
    void DrawSouthCards(const Deck &cards);
    void ShowBidsChoice(Contract contract);
    void ShowBid(Place p, Contract contract, Place myPlace);
    void cacheEncheres();
    void HideBidsChoice();
    void ShowAvatars(bool b);
    void InitBoard();
    void resetCards();
    Place SwapPlace(Place my_place, Place absolute);
    void AddGfxCard(const QString &filename);

    // Getters
    GfxCard *GetGfxCard(int i);
    Card *GetObjectCard(GfxCard *gc);
    bool GetSlamOption();

    // Setters
    void SetCursorType(CursorType t);
    void SetAvatar(Place p, const QString &file);
    void setFilter(Filter);
    void SetBackground(const QString &code);
    void setAccepterChienVisible(bool v);
    void setBoutonPoigneeVisible(bool v);
    void SetPlayerIdentity(QMap<Place, Identity> &players, Place myPlace);

public slots:
    void slotAccepteChien();
    void slotPresenterPoignee();

signals:
    void sigViewportClicked();
    void sigClickCard(GfxCard *);
    void sigMoveCursor(GfxCard *);
    void sigContract(Contract c);
    void sigAccepteChien();
    void sigPresenterPoignee();

protected:
    void  mousePressEvent(QMouseEvent *e);
    void  mouseMoveEvent(QMouseEvent *e);
    void  resizeEvent(QResizeEvent *event);

private:
    Filter filter;
    QVector<GfxCard *> cardsPics;
    QGraphicsScene scene;

    // Graphiques
    QMap<Place, PlayerBox *> playerBox;
    BidsForm    bidsForm;

    QPushButton    *boutonAccepterChien;
    QPushButton    *boutonPresenterPoignee;

    void DrawCardShadows();
};

#endif // CANVAS_H

//=============================================================================
// End of file Canvas.h
//=============================================================================
