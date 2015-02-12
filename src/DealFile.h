/*=============================================================================
 * TarotClub - DealFile.h
 *=============================================================================
 * Manage saving and loading XML files of custom deals
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
#ifndef DEAL_FILE_H
#define DEAL_FILE_H

#include <string>
#include "Deck.h"

/*****************************************************************************/
class DealFile
{
public:
    DealFile();

    bool LoadFile(const std::string &fileName);
    void SaveFile(const std::string &fileName);

    void Clear();

    Deck &GetDogDeck();
    Deck &GetWestDeck();
    Deck &GetEastDeck();
    Deck &GetSouthDeck();
    Deck &GetNorthDeck();

    void SetFirstPlayer(Place p);
    Place GetFirstPlayer() { return mFirstPlayer; }

    bool IsValid(std::uint8_t numberOfPlayers);

private:
    Deck    mPlayers[5]; //!< five players max in Tarot
    Deck    mDogDeck;
    Place   mFirstPlayer;
};

#endif // DEAL_FILE_H

//=============================================================================
// End of file DealFile.h
//=============================================================================
