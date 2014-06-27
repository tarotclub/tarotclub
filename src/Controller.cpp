/*=============================================================================
 * TarotClub - Server.cpp
 *=============================================================================
 * Host a Tarot game and manage network clients
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

#include <chrono>
#include <string>
#include "Log.h"
#include "Controller.h"
#include "ByteStreamReader.h"

/*****************************************************************************/
Controller::Controller()
    : mInitialized(false)
{

}
/*****************************************************************************/
void Controller::RegisterListener(Observer<Controller::Signal> &observer)
{
    mSubject.Attach(observer);
}
/*****************************************************************************/
void Controller::Start()
{
    engine.Initialize();
    if (!mInitialized)
    {
        mThread = std::thread(Controller::EntryPoint, this);
    }
}
/*****************************************************************************/
void Controller::Stop()
{
    mQueue.Push(Protocol::AdminQuitGame());
    mThread.join();
}
/*****************************************************************************/
void Controller::ExecuteRequest(const ByteArray &packet)
{
    mQueue.Push(packet);
}
/*****************************************************************************/
void Controller::EntryPoint(void *pthis)
{
    Controller * pt = (Controller*)pthis;
    pt->Run();
}
/*****************************************************************************/
void Controller::Run()
{
    ByteArray data;
    while(true)
    {
        mQueue.WaitAndPop(data);

        std::vector<Protocol::PacketInfo> packets = Protocol::DecodePacket(data);

        // Execute all packets
        for (std::uint16_t i = 0U; i< packets.size(); i++)
        {
            Protocol::PacketInfo inf = packets[i];

            ByteArray subArray = data.SubArray(inf.offset, inf.size);
            if (!DoAction(subArray))
            {
                // Quit thread
                return;
            }
        }
    }
}
/*****************************************************************************/
bool Controller::DoAction(const ByteArray &data)
{
    bool ret = true;
    ByteStreamReader in(data);

    // Jump over some header bytes
    in.Seek(3U);

    // Get the id of the sender
    std::uint32_t uuid;
    in >> uuid;

    // Get the command
    std::uint8_t cmd;
    in >> cmd;

    switch (cmd)
    {

    case Protocol::ADMIN_QUIT_GAME:
    {
        ret = false;
        break;
    }

    case Protocol::ADMIN_CREATE_GAME:
    {
        std::uint8_t gameMode;
        std::uint8_t nbPlayers;
        Tarot::Shuffle shuffle;

        in >> gameMode;
        in >> nbPlayers;
        in >> shuffle;
        engine.CreateGame((Tarot::GameMode)gameMode, shuffle, nbPlayers);
        break;
    }

    case Protocol::ADMIN_ADD_PLAYER:
    {
        std::uint32_t newplayer_uuid;
        in >> newplayer_uuid;
        // Look for free Place and assign the uuid to this player
        Place assigned = engine.AddPlayer(newplayer_uuid);
        if (assigned.Value() != Place::NOWHERE)
        {
            // New player connected, ask for its identity
            SendPacket(Protocol::ServerRequestIdentity(
                           assigned,
                           engine.GetNbPlayers(),
                           engine.GetGameMode(),
                           newplayer_uuid));
        }
        else
        {
            // Server is full, send an error message
            SendPacket(Protocol::ServerFullMessage(uuid));
        }
        break;
    }

    case Protocol::CLIENT_INFOS:
    {
        Identity ident;
        in >> ident;

        // Check if the uuid exists
        if (engine.GetPlayerPlace(uuid) != Place::NOWHERE)
        {
            bool full = engine.SetIdentity(uuid, ident);

            std::string message = "The player " + ident.name + " has joined the game.";
            SendPacket(Protocol::ServerChatMessage(message));

            if (full)
            {
                SendPacket(Protocol::ServerPlayersList(engine.GetPlayersList()));
                SignalGameFull();
            }
        }
        break;
    }

    case Protocol::CLIENT_MESSAGE:
    {
        std::string message;
        in >> message;

        // Check if the uuid exists
        Place place = engine.GetPlayerPlace(uuid);
        if (place != Place::NOWHERE)
        {
            std::map<Place, Identity> list = engine.GetPlayersList();
            std::string name = list[place].name;
            message = name + "> " + message;
            SendPacket(Protocol::ServerChatMessage(message));
        }
        break;
    }

    case Protocol::ADMIN_NEW_DEAL:
    {
        NewDeal();
        break;
    }

    case Protocol::CLIENT_SYNC_READY:
    {
        // Check if the uuid exists
        if (engine.GetPlayerPlace(uuid) != Place::NOWHERE)
        {
            if (engine.Sync(TarotEngine::WAIT_FOR_READY, uuid))
            {
                NewDeal();
            }
        }
        break;
    }

    case::Protocol::CLIENT_SYNC_CARDS:
    {
        // Check if the uuid exists
        if (engine.GetPlayerPlace(uuid) != Place::NOWHERE)
        {
            if (engine.Sync(TarotEngine::WAIT_FOR_CARDS, uuid))
            {
                // All the players have received their cards, launch the bid sequence
                BidSequence();
            }
        }
        break;
    }

    case Protocol::CLIENT_BID:
    {
        std::uint8_t c;
        std::uint8_t slam;

        in >> c;
        in >> slam;

        // Check if the uuid exists
        Place p = engine.GetPlayerPlace(uuid);
        if (p != Place::NOWHERE)
        {
            // Check if this is the right player
            if (p == engine.GetCurrentPlayer())
            {
                // Check if we are in the good sequence
                if (engine.GetSequence() == TarotEngine::WAIT_FOR_BID)
                {
                    Contract cont = engine.SetBid((Contract)c, (slam == 1U ? true : false), p);
                    // Broadcast player's bid, and wait for all acknowlegements
                    SendPacket(Protocol::ServerShowBid(cont, (slam == 1U ? true : false), p));
                }
                else
                {
                    TLogError("Wrong sequence");
                }
            }
            else
            {
                TLogError("Wrong player to bid");
            }
        }
        else
        {
            TLogError("Cannot get player place from UUID");
        }
        break;
    }

    case Protocol::CLIENT_SYNC_SHOW_BID:
    {
        // Check if the uuid exists
        if (engine.GetPlayerPlace(uuid) != Place::NOWHERE)
        {
            if (engine.Sync(TarotEngine::WAIT_FOR_SHOW_BID, uuid))
            {
                // Continue/finish the bid sequence
                BidSequence();
            }
        }
        break;
    }

    case Protocol::CLIENT_SYNC_SHOW_DOG:
    {
        // Check if the uuid exists
        if (engine.GetPlayerPlace(uuid) != Place::NOWHERE)
        {
            if (engine.Sync(TarotEngine::WAIT_FOR_SHOW_DOG, uuid))
            {
                // When all the players have seen the dog, ask to the taker to build a discard
                Player *player = engine.GetPlayer(engine.GetBid().taker);
                if (player != NULL)
                {
                    engine.DiscardSequence();
                    std::uint32_t id = player->GetUuid();
                    SendPacket(Protocol::ServerAskForDiscard(id));
                }
                else
                {
                    TLogError("Cannot get player pointer!");
                }
            }
        }
        break;
    }

    case Protocol::CLIENT_DISCARD:
    {
        Deck discard;
        in >> discard;

        // Check if the uuid exists
        Place p = engine.GetPlayerPlace(uuid);
        if (p != Place::NOWHERE)
        {
            // Check sequence
            if (engine.GetSequence() == TarotEngine::WAIT_FOR_DISCARD)
            {
                // Check if right player
                if (engine.GetBid().taker == p)
                {
                    engine.SetDiscard(discard);
                    // Then start the deal
                    SendPacket(Protocol::ServerStartDeal(engine.GetBid(), engine.GetShuffle()));
                }
            }
        }
        break;
    }

    case Protocol::CLIENT_SYNC_START:
    {
        // Check if the uuid exists
        if (engine.GetPlayerPlace(uuid) != Place::NOWHERE)
        {
            if (engine.Sync(TarotEngine::WAIT_FOR_START_DEAL, uuid))
            {
                engine.StartDeal();
                GameSequence();
            }
        }
        break;
    }

    case Protocol::CLIENT_HANDLE:
    {
        Deck handle;
        in >> handle;

        Place p = engine.GetPlayerPlace(uuid);

        // Check if the uuid exists
        if (p.Value() != Place::NOWHERE)
        {
            // Check sequence
            if (engine.GetSequence() == TarotEngine::WAIT_FOR_HANDLE)
            {
                // Check if right player
                if (engine.GetCurrentPlayer() == p)
                {
                    if (engine.SetHandle(handle, p))
                    {
                        // Handle is valid, show it to all players
                        SendPacket(Protocol::ServerShowHandle(handle, p));
                    }
                    else
                    {
                        // Invalid or no handle, continue game (player has to play a card)
                        GameSequence();
                    }
                }
            }
        }
        else
        {
            TLogError("Cannot get player place from UUID");
        }
        break;
    }

    case Protocol::CLIENT_SYNC_HANDLE:
    {
        // Check if the uuid exists
        if (engine.GetPlayerPlace(uuid) != Place::NOWHERE)
        {
            if (engine.Sync(TarotEngine::WAIT_FOR_SHOW_HANDLE, uuid))
            {
                GameSequence();
            }
        }
        break;
    }

    case Protocol::CLIENT_CARD:
    {
        std::string card;
        Card *c;

        in >> card;
        c = TarotDeck::GetCard(card);

        // Check if the card name exists
        if (c != NULL)
        {
            Place p = engine.GetPlayerPlace(uuid);

            // Check if the uuid exists
            if (p.Value() != Place::NOWHERE)
            {
                // Check sequence
                if (engine.GetSequence() == TarotEngine::WAIT_FOR_PLAYED_CARD)
                {
                    // Check if right player
                    if (engine.GetCurrentPlayer() == p)
                    {
                        if (engine.SetCard(c, p))
                        {
                            // Broadcast played card, and wait for all acknowlegements
                            SendPacket(Protocol::ServerShowCard(c, p));
                        }
                    }
                    else
                    {
                        TLogError("Wrong player");
                    }
                }
                else
                {
                    TLogError("Bad sequence");
                }
            }
            else
            {
                TLogError("Cannot get player place from UUID");
            }
        }
        else
        {
            TLogError("Bad card name!");
        }
        break;
    }

    case Protocol::CLIENT_SYNC_SHOW_CARD:
    {
        // Check if the uuid exists
        if (engine.GetPlayerPlace(uuid) != Place::NOWHERE)
        {
            if (engine.Sync(TarotEngine::WAIT_FOR_SHOW_CARD, uuid))
            {
                GameSequence();
            }
        }
        break;
    }

    case Protocol::CLIENT_SYNC_TRICK:
    {
        // Check if the uuid exists
        if (engine.GetPlayerPlace(uuid) != Place::NOWHERE)
        {
            if (engine.Sync(TarotEngine::WAIT_FOR_END_OF_TRICK, uuid))
            {
                if (engine.IsLastTrick())
                {
                    engine.EndOfDeal();
                    SendPacket(Protocol::ServerEndOfDeal(engine.GetScore()));
                }
                else
                {
                    GameSequence();
                }
            }
        }
        break;
    }

    default:
        TLogError("Unknown packet received");
        break;
    }

    return ret;
}
/*****************************************************************************/
void Controller::NewDeal()
{
    engine.NewDeal();
    // Send the cards to all the players
    for (std::uint32_t i = 0U; i < engine.GetNbPlayers(); i++)
    {
        Player *player = engine.GetPlayer(Place(i));
        if (player != NULL)
        {
            SendPacket(Protocol::ServerSendCards(player));
        }
        else
        {
            TLogError("Cannot get player deck");
        }
    }
}
/*****************************************************************************/
void Controller::BidSequence()
{
    // Launch/continue the bid sequence
    TarotEngine::BidResult res = engine.BidSequence();

    switch (res)
    {
    case TarotEngine::NEXT_PLAYER:
        SendPacket(Protocol::ServerBidRequest(engine.GetBid().contract, engine.GetCurrentPlayer()));
        break;

    case TarotEngine::ALL_PASSED:
        NewDeal();
        break;

    case TarotEngine::START_DEAL:
        SendPacket(Protocol::ServerStartDeal(engine.GetBid(), engine.GetShuffle()));
        break;

    case TarotEngine::SHOW_DOG:
        SendPacket(Protocol::ServerShowDog(engine.GetDog()));
        break;

    default:
        TLogError("Dad enum TarotEngine::BidResult value");
        break;
    }
}
/*****************************************************************************/
void Controller::GameSequence()
{
    engine.GameSequence();
    TarotEngine::Sequence seq = engine.GetSequence();

    Place p = engine.GetCurrentPlayer();

    switch (seq)
    {
    case TarotEngine::WAIT_FOR_END_OF_TRICK:
        SendPacket(Protocol::ServerEndOfTrick(p));
        break;

    case TarotEngine::WAIT_FOR_PLAYED_CARD:
        SendPacket(Protocol::ServerPlayCard(p));
        break;

    case TarotEngine::WAIT_FOR_HANDLE:
    {
        Player *player = engine.GetPlayer(p);
        if (player != NULL)
        {
            SendPacket(Protocol::ServerAskForHandle(player->GetUuid()));
        }
        else
        {
            TLogError("Big problem here...");
        }
    }
        break;

    default:
        TLogError("Bad sequence, game engine state problem");
        break;
    }
}
/*****************************************************************************/
void Controller::SendPacket(const ByteArray &block)
{
    Signal sig;

    if (block.Size() > 0U)
    {
        sig.type = SIG_SEND_DATA;
        sig.data = block;
        mSubject.Notify(sig);
    }
    else
    {
        TLogError("Packet cannot be empty");
    }
}
/*****************************************************************************/
void Controller::SignalGameFull()
{
    Signal sig;

    sig.type = SIG_GAME_FULL;
    mSubject.Notify(sig);
}


//=============================================================================
// End of file Server.cpp
//=============================================================================
