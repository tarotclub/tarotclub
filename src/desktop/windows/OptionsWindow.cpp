/*=============================================================================
 * TarotClub - OptionsWindow.cpp
 *=============================================================================
 * OptionsWindow : fenêtre d'options
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

#include "OptionsWindow.h"
#include "Translations.h"
#include <QDir>
#include <QString>
#include <QtWidgets>

static const QPoint labelPos[5] =
{
    {10, 2},
    {60, 2},
    {110, 2},
    {160, 2},
    {210, 2}
};

static const QString iconName[5] =
{
    ":/icons/spade.svg",
    ":/icons/heart.svg",
    ":/icons/diamond.svg",
    ":/icons/club.svg",
    ":/icons/trump.svg"
};

static const std::uint8_t suitList[5] =
{
    Card::SPADES,
    Card::HEARTS,
    Card::DIAMONDS,
    Card::CLUBS,
    Card::TRUMPS
};

static const QSize labelSize(40, 40);

/*****************************************************************************/
DragWidget::DragWidget(QWidget *parent)
    : QFrame(parent)
{
    setMinimumSize(260, 44);
    setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
    setAcceptDrops(true);

    for (int i = 0; i < 5; i++)
    {
        mIcons[i].label = new QLabel(this);
        mIcons[i].label->setPixmap(QPixmap(iconName[i]));
        mIcons[i].label->move(labelPos[i]);
        mIcons[i].label->resize(labelSize);
        mIcons[i].label->setScaledContents(true);
        mIcons[i].label->show();

        mIcons[i].icon = iconName[i];
        mIcons[i].suit = suitList[i];
    }
}
/*****************************************************************************/
std::string DragWidget::GetOrder()
{
    std::string order;
    for (int i = 0; i < 5; i++)
    {
        order += Card::SuitName(mIcons[i].suit);
    }
    return order;
}
/*****************************************************************************/
void DragWidget::SetOrder(const std::string &order)
{
    if (order.size() >= 5)
    {
        for (int i = 0; i < 5; i++)
        {
            std::string letter;
            letter.push_back(order[i]);
            std::uint8_t suit = Card::SuitFromName(letter);
            mIcons[i].icon = iconName[suit];
            mIcons[i].suit = suit;

            // update Pixmap
            mIcons[i].label->setPixmap(mIcons[i].icon);
        }
    }
}
/*****************************************************************************/
void DragWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata"))
    {
        if (event->source() == this)
        {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }
        else
        {
            event->acceptProposedAction();
        }
    }
    else
    {
        event->ignore();
    }
}
/*****************************************************************************/
void DragWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata"))
    {
        if (event->source() == this)
        {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }
        else
        {
            event->acceptProposedAction();
        }
    }
    else
    {
        event->ignore();
    }
}
/*****************************************************************************/
void DragWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata"))
    {
        // Look for the selected suit
        int destination = DetectLabel(event->pos().x());
        if ((destination >= 0) && (destination < 5))
        {
            QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
            QDataStream dataStream(&itemData, QIODevice::ReadOnly);

            int source = -1;
            dataStream >> source;

            if ((source >= 0) && (source < 5))
            {
                // Swap icons
                DragIcon temp = mIcons[destination];
                mIcons[destination].icon = mIcons[source].icon;
                mIcons[destination].suit = mIcons[source].suit;

                mIcons[source].icon = temp.icon;
                mIcons[source].suit = temp.suit;

                // update Pixmap
                mIcons[source].label->setPixmap(mIcons[source].icon);
                mIcons[destination].label->setPixmap(mIcons[destination].icon);

                if (event->source() == this)
                {
                    event->setDropAction(Qt::MoveAction);
                    event->accept();
                }
                else
                {
                    event->acceptProposedAction();
                }
            }
            else
            {
                event->ignore();
            }
        }
        else
        {
            event->ignore();
        }
    }
    else
    {
        event->ignore();
    }
}
/*****************************************************************************/
void DragWidget::mousePressEvent(QMouseEvent *event)
{
    QLabel *child = static_cast<QLabel *>(childAt(event->pos()));
    if (!child)
    {
        return;
    }

    QPixmap pixmap = child->pixmap()->scaled(40, 40);

    // Look for the selected suit
    int selected = DetectLabel(child->pos().x());
    if (selected < 0)
    {
        return;
    }

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << selected;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-dnditemdata", itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot(event->pos() - child->pos());

    (void)drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);
}
/*****************************************************************************/
int DragWidget::DetectLabel(int x)
{
    int selected = -1;
    for (int i = 4; i >= 0; i--)
    {
        if ((x >= labelPos[i].x()) &&
                (x < (labelPos[i].x() + labelSize.width())))
        {
            selected = i;
        }
    }
    return selected;
}
/*****************************************************************************/
OptionsWindow::OptionsWindow(QWidget *parent)
    : QDialog(parent)
    , mPreviousSelectedBot(-1)
{
    ui.setupUi(this);

    // Build the Avatar window
    mAvatarsDiag = new QDialog(this);
    mAvatarsUi.setupUi(mAvatarsDiag);

    // Create the scroll area widget
    QWidget *contents = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout();
    contents->setLayout(layout);

    mAvatarsUi.avatarsList->setContentsMargins(0, 0, 0, 0);
    mAvatarsUi.avatarsList->setAlignment(Qt::AlignCenter);
    mAvatarsUi.avatarsList->setFrameStyle(0);

    // Show the avatars
    mSelectedAvatar = -1;
    QDir dir(":/avatars");
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size();)
    {
        QHBoxLayout *hLayout = new QHBoxLayout();
        layout->addLayout(hLayout);

        for (int j = 0; j < 4; j++)
        {
            if (i < list.size())
            {
                QFileInfo fileInfo = list.at(i);
                QString fileName = fileInfo.absoluteFilePath();

                AvatarLabel *item = new AvatarLabel(i, fileName);
                mAvatarFiles[i] = item;
                QImage image(fileName);
                item->setPixmap(QPixmap::fromImage(image.scaled(160, 160)));
                item->setScaledContents(false);
                connect(item, &AvatarLabel::sigClicked, this, &OptionsWindow::slotAvatarSelected);

                hLayout->addWidget(item);
                i++;
            }
        }
    }

    // We must call the set widget _after_ adding elements
    mAvatarsUi.avatarsList->setWidget(contents);
    mAvatarsUi.avatarsList->show();

    connect(ui.btnDefaut, SIGNAL(clicked()), this, SLOT(slotBtnDefaut()));
    connect(ui.btnOk, SIGNAL(clicked()), this, SLOT(slotBtnOk()));

    // Sliders slots
    connect(ui.slider1, SIGNAL(valueChanged(int)), this, SLOT(slider1Changed(int)));
    connect(ui.slider2, SIGNAL(valueChanged(int)), this, SLOT(slider2Changed(int)));

    // Avatar choice
    connect(ui.btnPixSud, SIGNAL(clicked()), this, SLOT(slotBtnPixSud()));
    connect(ui.buttonBotAvatar, SIGNAL(clicked()), this, SLOT(slotButtonBotAvatar()));

    // Background color choice
    connect(ui.tapisColor, SIGNAL(clicked()), this, SLOT(slotColorPicker()));

    // Disable the delay when the click option has been choosen
    connect(ui.clic, SIGNAL(stateChanged(int)), this, SLOT(slotClickOptionChanged(int)));

    mLevelList.append(tr("Beginner"));

    connect(ui.botsList, &QListWidget::currentRowChanged, this, &OptionsWindow::slotBotSelected);
    ui.botsList->addItem(PlaceToString(Place::EAST));
    ui.botsList->addItem(PlaceToString(Place::NORTH));
    ui.botsList->addItem(PlaceToString(Place::WEST));

    dragWidget = new DragWidget();
    ui.iconsLayout->addWidget(dragWidget);
}
/*****************************************************************************/
void OptionsWindow::SetClientOptions(const ClientOptions &opt)
{
    clientOptions = opt;
    refresh();
}
/*****************************************************************************/
void OptionsWindow::SetServerOptions(const ServerOptions &opt)
{
    serverOptions = opt;
    refresh();
    ui.botsList->setCurrentRow(0);
}
/*****************************************************************************/
ClientOptions &OptionsWindow::GetClientOptions()
{
    return clientOptions;
}
/*****************************************************************************/
ServerOptions &OptionsWindow::GetServerOptions()
{
    return serverOptions;
}
/*****************************************************************************/
void OptionsWindow::slotColorPicker()
{
    QColor color = QColorDialog::getColor(Qt::darkGreen, this);
    if (color.isValid())
    {
        colorName = color.name();
        ui.tapisColor->setPalette(QPalette(color));
        ui.tapisColor->setAutoFillBackground(true);
    }
}
/*****************************************************************************/
void OptionsWindow::slotBotSelected(int currentRow)
{
    if (currentRow >= 0)
    {
        if (currentRow != mPreviousSelectedBot)
        {
            QPixmap im;

            if (mPreviousSelectedBot >= 0)
            {
                Place place(mPreviousSelectedBot + 1U);
                // Save previous bot configuration
                serverOptions.bots[place].identity.name = ui.botName->text().toStdString();
                serverOptions.bots[place].identity.quote = ui.botQuote->text().toStdString();
                if (ui.botMale->isChecked())
                {
                    serverOptions.bots[place].identity.gender = Identity::MALE;
                }
                else
                {
                    serverOptions.bots[place].identity.gender = Identity::FEMALE;
                }
            }

            mPreviousSelectedBot = currentRow;

            ui.botAiLevel->clear();
            ui.botAiLevel->addItems(mLevelList);
            ui.botAiLevel->setCurrentIndex(0);

            // Get the place of the selected bot
            Place place(currentRow + 1U);

            ui.botName->setText(QString(serverOptions.bots[place].identity.name.data()));
            ui.botQuote->setText(QString(serverOptions.bots[place].identity.quote.data()));

            if (serverOptions.bots[place].identity.gender == Identity::MALE)
            {
                ui.botMale->setChecked(true);
            }
            else
            {
                ui.botFemale->setChecked(true);
            }

            if (im.load(QString(serverOptions.bots[place].identity.avatar.data())) == true)
            {
                ui.botAvatar->setPixmap(im);
            }
        }
    }
}
/*****************************************************************************/
void OptionsWindow::slotBtnOk()
{
    clientOptions.identity.name = ui.nomJoueurSud->text().toStdString();
    clientOptions.identity.quote = ui.citationSud->text().toStdString();
    if (ui.sexeM->isChecked())
    {
        clientOptions.identity.gender = Identity::MALE;
    }
    else
    {
        clientOptions.identity.gender = Identity::FEMALE;
    }

    if (ui.afficheAvatars->isChecked())
    {
        clientOptions.showAvatars = true;
    }
    else
    {
        clientOptions.showAvatars = false;
    }
    clientOptions.language = ui.langList->currentIndex();

    if (indexLangue != clientOptions.language)
    {
        QMessageBox::information(this, tr("Information"),
                                 tr("You must restart TarotClub to enable the new language.") + "\n\n");
    }
    clientOptions.backgroundColor = colorName.toStdString();
    clientOptions.delayBeforeCleaning = ui.slider2->value();

    if (ui.clic->isChecked())
    {
        clientOptions.clickToClean = true;
    }
    else
    {
        clientOptions.clickToClean = false;
    }
    clientOptions.cardsOrder = dragWidget->GetOrder();

    // Server stuff
    serverOptions.timer = ui.slider1->value();
    accept();
}
/*****************************************************************************/
void OptionsWindow::slotBtnDefaut()
{
    clientOptions = ClientConfig::GetDefault();
    serverOptions = ServerConfig::GetDefault();
    refresh();
}
/*****************************************************************************/
void OptionsWindow::slider1Changed(int value)
{
    ui.temps1->setText(tr("%1 seconds").arg((float)(value / 100) / 10));
}
/*****************************************************************************/
void OptionsWindow::slider2Changed(int value)
{
    ui.temps2->setText(tr("%1 seconds").arg((float)(value / 100) / 10));
}
/*****************************************************************************/
void OptionsWindow::slotClickOptionChanged(int state)
{
    if (state == Qt::Checked)
    {
        ui.temps2->setEnabled(true);
        ui.slider2->setEnabled(true);
        ui.label2->setEnabled(true);
    }
    else
    {
        ui.temps2->setEnabled(false);
        ui.slider2->setEnabled(false);
        ui.label2->setEnabled(false);
    }
}
/*****************************************************************************/
QString OptionsWindow::ChooseAvatar(const QString &defaultAvatar)
{
    mSelectedAvatar = -1;
    QString avatar = defaultAvatar;
    if (mAvatarsDiag->exec() == QDialog::Accepted)
    {
        if (mAvatarFiles.contains(mSelectedAvatar))
        {
            AvatarLabel *label = mAvatarFiles.value(mSelectedAvatar);
            if (label != NULL)
            {
                avatar = label->GetFileName();
            }
        }
    }
    return (avatar);
}
/*****************************************************************************/
void OptionsWindow::slotAvatarSelected(int id)
{
    mSelectedAvatar  = id;

    for (int i = 0; i < mAvatarFiles.size(); i++)
    {
        if (mAvatarFiles.contains(i))
        {
            AvatarLabel *label = mAvatarFiles.value(i);
            if (label != NULL)
            {
                if (i == id)
                {
                    // Select it
                    label->setFrameStyle(QFrame::Box);
                    label->setLineWidth(4);
                }
                else
                {
                    // deselect it
                    label->setFrameStyle(QFrame::NoFrame);
                }
            }
        }
    }
}
/*****************************************************************************/
void OptionsWindow::slotBtnPixSud()
{
    QString s;
    QPixmap im;

    s = ChooseAvatar(QString(clientOptions.identity.avatar.data()));
    if (im.load(s) == false)
    {
        return;
    }
    clientOptions.identity.avatar = s.toStdString();
    ui.pixSud->setPixmap(im);
}
/*****************************************************************************/
void OptionsWindow::slotButtonBotAvatar()
{
    QString s;
    QPixmap im;

    Place place(ui.botsList->currentRow() + 1U);

    s = ChooseAvatar(QString(serverOptions.bots[place].identity.avatar.data()));
    if (im.load(s) == false)
    {
        return;
    }
    serverOptions.bots[place].identity.avatar = s.toStdString();
    ui.botAvatar->setPixmap(im);
}
/*****************************************************************************/
/**
 * @brief Refresh widgets with current configuration
 */
void OptionsWindow::refresh()
{
    QPixmap im;

    ui.nomJoueurSud->setText(QString(clientOptions.identity.name.data()));
    ui.citationSud->setText(QString(clientOptions.identity.quote.data()));
    if (clientOptions.identity.gender == Identity::MALE)
    {
        ui.sexeM->setChecked(true);
    }
    else
    {
        ui.sexeF->setChecked(true);
    }
    ui.afficheAvatars->setChecked(clientOptions.showAvatars);
    ui.langList->setCurrentIndex(clientOptions.language);
    indexLangue = clientOptions.language;
    if (im.load(QString(clientOptions.identity.avatar.data())) == true)
    {
        ui.pixSud->setPixmap(im);
    }
    QColor color(clientOptions.backgroundColor.c_str());
    if (color.isValid())
    {
        colorName = color.name();
        ui.tapisColor->setPalette(QPalette(color));
        ui.tapisColor->setAutoFillBackground(true);
    }
    ui.slider2->setValue(clientOptions.delayBeforeCleaning);

    if (clientOptions.clickToClean == true)
    {
        ui.clic->setChecked(true);
        // Force refresh
        slotClickOptionChanged(Qt::Checked);
    }
    else
    {
        ui.clic->setChecked(false);
        // Force refresh
        slotClickOptionChanged(Qt::Unchecked);
    }
    dragWidget->SetOrder(clientOptions.cardsOrder);

    // server stuff
    ui.slider1->setValue(serverOptions.timer);
}

//=============================================================================
// Fin du fichier OptionsWindow.cpp
//=============================================================================
