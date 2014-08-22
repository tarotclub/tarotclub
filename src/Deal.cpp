/*=============================================================================
 * TarotClub - Deal.cpp
 *=============================================================================
 * This class stores a complete deal history and calculate final points
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

#include <sstream>
#include "JsonReader.h"
#include "JsonWriter.h"
#include "Deal.h"
#include "Common.h"
#include "Log.h"
#include "System.h"

static const std::string DEAL_RESULT_FILE_VERSION  = "2.0";

/*****************************************************************************/
Deal::Deal()
{
    Initialize();
    NewDeal();
}
/*****************************************************************************/
void Deal::Initialize()
{
    for (std::uint32_t i = 0U; i < MAX_ROUNDS; i++)
    {
        for (std::uint32_t j = 0U; j < 5U; j++)
        {
            scores[i][j] = 0;
        }
    }
    dealCounter = 0;
}
/*****************************************************************************/
void Deal::NewDeal()
{
    mDiscard.Clear();
    attackHandleDeck.Clear();
    defenseHandleDeck.Clear();

    for (int i = 0; i < 24; i++)
    {
        tricks[i].Clear();
    }

    littleEndianOudler = false;
    littleEndianOwner = NO_TEAM;
    slamDone = false;
    slamOwner = NO_TEAM;
    foolSwap = false;
    foolOwner = NO_TEAM;
    tricksWon = 0;
    statsAttack.Reset();
    score.Reset();
}
/*****************************************************************************/
/**
 * @brief SetTrick
 *
 * Store the current trick into memory and calculate the trick winner.
 *
 * This methods does not verify if the trick is consistent with the rules; this
 * is supposed to have been previously verified.
 *
 * @return The place of the winner of this trick
 */
Place Deal::SetTrick(const Deck &trick, const Tarot::Bid &bid, std::uint8_t trickCounter)
{
    std::uint8_t turn = trickCounter - 1U;
    Place winner;
    std::uint8_t numberOfPlayers = trick.Size();

    // Get the number of tricks we must play, it depends of the number of players
    int numberOfTricks = Tarot::NumberOfCardsInHand(numberOfPlayers);

    if (turn < 24U)
    {
        tricks[turn] = trick;
        Card *cLeader = NULL;

        // Each trick is won by the highest trump in it, or the highest card
        // of the suit led if no trumps were played.
        cLeader = trick.HighestTrump();
        if (cLeader == NULL)
        {
            // lead color is the first one, except if the first card is a fool. In that case, the second player plays the lead color
            cLeader = trick.HighestSuit();
        }

        if (cLeader == NULL)
        {
            TLogError("cLeader cannot be null!");
        }
        else
        {
            // The trick winner is the card leader owner
            winner = cLeader->GetOwner();

            if (tricks[turn].HasFool())
            {
                Card *cFool = tricks[turn].GetCardByName("00-T");
                if (cFool == NULL)
                {
                    TLogError("Card pointer cannot be NULL");
                }
                else
                {
                    if (Tarot::IsDealFinished(trickCounter, numberOfPlayers) == true)
                    {
                        // Special case of the fool: if played at last turn with a slam realized, it wins the trick
                        if ((tricksWon == (numberOfTricks - 1)) &&
                                (cFool->GetOwner() == bid.taker))
                        {
                            winner = bid.taker;
                        }
                        // Otherwise, the fool is _always_ lost if played at the last trick, even if the
                        // fool belongs to the same team than the winner of the trick.
                        else
                        {
                            foolSwap = true;
                            if (cFool->GetOwner() == bid.taker)
                            {
                                foolOwner = DEFENSE;
                            }
                            else
                            {
                                foolOwner = ATTACK;
                            }
                        }
                    }
                    else
                    {
                        // In all other cases, the fool is kept by the owner. If the trick is won by a
                        // different team than the fool owner, they must exchange 1 low card with the fool.
                        if (winner != cFool->GetOwner())
                        {
                            foolSwap = true;
                            if (winner == bid.taker)
                            {
                                foolOwner = DEFENSE;
                            }
                            else
                            {
                                foolOwner = ATTACK;
                            }
                        }
                        // else, the trick is won by the attacker, it keeps the fool
                    }
                }
            }
        }

        if (winner == bid.taker)
        {
            tricks[turn].SetOwner(ATTACK);
            tricks[turn].AnalyzeTrumps(statsAttack);
            tricksWon++;
        }
        else
        {
            tricks[turn].SetOwner(DEFENSE);
        }
    }
    else
    {
        TLogError("Index out of scope!");
    }

    return winner;
}
/*****************************************************************************/
Deck Deal::GetTrick(int turn)
{
    if ((turn < 0) || (turn >= 24))
    {
        turn = 0;
    }
    return tricks[turn];
}
/*****************************************************************************/
void Deal::SetHandle(const Deck &handle, Team team)
{
    if (team == ATTACK)
    {
        attackHandleDeck = handle;
    }
    else
    {
        defenseHandleDeck = handle;
    }
}
/*****************************************************************************/
int Deal::GetTotalPoints(Place p) const
{
    int total;

    total = 0;
    for (std::uint32_t i = 0U; i < dealCounter; i++)
    {
        total += scores[i][p.Value()];
    }
    return (total);
}
/*****************************************************************************/
/**
 * @brief Deal::GetPodium
 *
 * Gets the podium of the tournament. The first player has the highest number of points.
 *
 * @return The sorted list of players, by points
 */
std::map<int, Place> Deal::GetPodium()
{
    std::map<int, Place> podium;

    // init podium
    for (std::uint32_t i = 0U; i < 5U; i++)
    {
        int points = scores[dealCounter - 1][i];
        podium[points] = (Place)i;
    }

    // Since the map naturally sorts the list in the key order (points)
    // there is no need to sort anything
    return podium;
}
/*****************************************************************************/
Deck Deal::GetDog()
{
    return mDiscard;
}
/*****************************************************************************/
Score &Deal::GetScore()
{
    return score;
}
/*****************************************************************************/
void Deal::SetScore(const Score &s)
{
    score = s;
}
/*****************************************************************************/
/**
 * @brief Deal::AddScore
 * @param info
 * @return true if the tournament must continue, false if it is finished
 */
bool Deal::AddScore(const Tarot::Bid &bid, std::uint8_t numberOfPlayers)
{
    for (std::uint32_t i = 0U; i < numberOfPlayers; i++)
    {
        if (Place(i) == bid.taker)
        {
            scores[dealCounter][i] = score.GetAttackScore();
        }
        else
        {
            scores[dealCounter][i] = score.GetDefenseScore();
        }
    }
    dealCounter++;
    if (dealCounter < MAX_ROUNDS)
    {
        return true;
    }
    return false;
}
/*****************************************************************************/
void Deal::AnalyzeGame(std::uint8_t numberOfPlayers)
{
    std::uint8_t numberOfTricks = Tarot::NumberOfCardsInHand(numberOfPlayers);
    std::uint8_t lastTrick = numberOfTricks - 1U;

    // 1. Slam detection
    if (tricksWon == numberOfTricks)
    {
        slamDone = true;
        slamOwner = ATTACK;
    }

    if (tricksWon == 0)
    {
        slamDone = true;
        slamOwner = DEFENSE;
    }

    // 2. Attacker points, we add the dog if needed
    if (mDiscard.GetOwner() == ATTACK)
    {
        mDiscard.AnalyzeTrumps(statsAttack);
    }

    // 3. Fool owner. If the attacker has lost a fool, it changes some key elements such as:
    //    - The number of oudlers
    //    - The points realized

    if (foolSwap == true)
    {
        if (foolOwner == DEFENSE)
        {
            statsAttack.points -= 4; // defense keeps its points
            statsAttack.oudlers--; // attack looses an oudler! what a pity!
        }
        else
        {
            statsAttack.points += 4; // get back the points
            statsAttack.oudlers++; // hey, it was MY oudler!
        }
    }

    // 4. One of trumps in the last trick bonus detection
    if (slamDone)
    {
        // With a slam, the 1 of Trump bonus is valid if played
        // in the penultimate trick AND if the fool is played in the last trick
        if (tricks[lastTrick].HasFool())
        {
            lastTrick--;
        }
    }
    if (tricks[lastTrick].HasOneOfTrump())
    {
        littleEndianOudler = true;
        littleEndianOwner = tricks[lastTrick].GetOwner();
    }

    // 5. The number of oudler(s) decides the points to do
    score.oudlers = statsAttack.oudlers;

    // 6. We save the points done by the attacker
    score.pointsAttack = static_cast<int>(statsAttack.points); // voluntary ignore digits after the coma
}
/*****************************************************************************/
/**
 * @brief Calculate the final score of all players
 *
 * We calculate the points with the following formula:
 *   points = ((25 + pt + pb) * mu) + pg + ch
 *
 * pt = difference between the card points the taker actually won and the minimum number of points he needed
 * pb = the petit au bout bonus is added or subtracted if applicable
 * mu = factor (mu) depending on the bid (1, 2, 4 or 6)
 * pg = the handle bonus (20, 30 or 40)
 * ch = the slam bonus (200 or 400)
 *
 */
void Deal::CalculateScore(const Tarot::Bid &bid, Tarot::Handle attack, Tarot::Handle defense)
{
    if (bid.contract == Contract::TAKE)
    {
        score.multiplier = 1;
    }
    else if (bid.contract == Contract::GUARD)
    {
        score.multiplier = 2;
    }
    else if (bid.contract == Contract::GUARD_WITHOUT)
    {
        score.multiplier = 4;
    }
    else     // GUARD_AGAINST
    {
        score.multiplier = 6;
    }

    // Handle bonus: Ces primes gardent la même valeur quel que soit le contrat.
    // La prime est acquise au camp vainqueur de la donne.
    score.handlePoints += Tarot::GetHandlePoints(attack);
    score.handlePoints += Tarot::GetHandlePoints(defense);

    // Little endian bonus:
    // Le camp qui réalise la dernière levée, à condition que cette levée
    // comprenne le Petit, bénéficie d'une prime de 10 points,
    // multipliable selon le contrat, quel que soit le résultat de la donne.
    if (littleEndianOudler == true)
    {
        if (littleEndianOwner == score.Winner())
        {
            score.littleEndianPoints = 10;
        }
        else
        {
            score.littleEndianPoints = -10;
        }
    }

    // Slam bonus
    if (slamDone)
    {
        if (bid.slam == true)
        {
            score.slamPoints = 400;
        }
        else
        {
            score.slamPoints = 200;
        }
    }
    else
    {
        // announced but not realized
        if (bid.slam == true)
        {
            score.slamPoints = -200;
        }
    }

    score.difference = score.pointsAttack - Tarot::PointsToDo(score.oudlers);
    // Final scoring
    score.scoreAttack = (25 + abs(score.difference) + score.littleEndianPoints) * score.multiplier + score.handlePoints + score.slamPoints;
}
/*****************************************************************************/
/**
 * @brief Deal::GetSortedTrick
 *
 * Sort a trick so that the card are in the Place order (first is always south, second is east ...)
 * instead of the native order (first card of the trick is the first card played).
 *
 * @param trick
 * @return
 */
std::list<std::string> Deal::GetSortedTrick(int trick)
{
    std::list<std::string> list;

    for (Deck::ConstIterator i = tricks[trick].Begin(); i != tricks[trick].End(); ++i)
    {
        std::list<std::string>::iterator it = list.begin();
        Card *c = (*i);

        std::advance(it, c->GetOwner().Value());
        list.insert(it, c->GetName());
    }

    return list;
}
/*****************************************************************************/
void Deal::SetDiscard(const Deck &discard, Team owner)
{
    mDiscard = discard;
    mDiscard.SetOwner(owner);
}
/*****************************************************************************/
/**
 * @brief Generate a file with all played cards of the deal
 */
void Deal::GenerateEndDealLog(const Tarot::Bid &bid, const std::map<Place, Identity> &players)
{
    JsonWriter json;
    std::uint8_t numberOfPlayers = players.size();

    std::string fileName = System::GamePath() + "deal_result_" + Util::CurrentDateTime("%Y-%m-%d.%H%M%S") + ".json";

    json.CreateValuePair("version", DEAL_RESULT_FILE_VERSION);
    json.CreateValuePair("generator", "Generated by TarotClub " + TAROT_VERSION);

    // ========================== Game information ==========================
    JsonObject *obj = json.CreateObjectPair("deal_info");

    // Players are sorted from south to north-west, anti-clockwise (see Place class)
    JsonArray *obj2 = json.CreateArrayPair("players");
    std::map<Place, Identity>::const_iterator it;
    for (it = players.begin(); it != players.end(); ++it)
    {
        obj2->CreateValue((it->second).name);
    }

    obj->CreateValuePair("taker", bid.taker.ToString());
    obj->CreateValuePair("contract", bid.contract.ToString());

    Deck::ConstIterator firstPlayer = tricks[0].Begin();
    obj->CreateValuePair("first_trick_lead", (*firstPlayer)->GetOwner().ToString());
    obj->CreateValuePair("discard", mDiscard.GetCardList());

    // ========================== Score calculation ==========================
    JsonObject *scoreObj = obj->CreateObjectPair("score");
    scoreObj->CreateValuePair("attacker_points", static_cast<std::int32_t>(score.pointsAttack));
    scoreObj->CreateValuePair("attacker_score", static_cast<std::int32_t>(score.scoreAttack));
    scoreObj->CreateValuePair("oudlers", static_cast<std::int32_t>(score.oudlers));
    scoreObj->CreateValuePair("difference", static_cast<std::int32_t>(score.difference));
    scoreObj->CreateValuePair("multiplier", static_cast<std::int32_t>(score.multiplier));
    scoreObj->CreateValuePair("one_of_trump_bonus", static_cast<std::int32_t>(score.littleEndianPoints));
    scoreObj->CreateValuePair("handle_bonus", static_cast<std::int32_t>(score.handlePoints));
    scoreObj->CreateValuePair("slam_bonus", static_cast<std::int32_t>(score.slamPoints));

    // ========================== Played cards ==========================
    JsonArray *obj3 = json.CreateArrayPair("tricks");

    for (std::uint32_t i = 0U; i < Tarot::NumberOfCardsInHand(numberOfPlayers); i++)
    {
        std::string cards;
        std::list<std::string> list = GetSortedTrick(i);
        std::list<std::string>::iterator it;
        for (it = list.begin(); it != list.end(); ++it)
        {
            // Trick is always showed in the same order:
            // SOUTH EAST NORTH WEST FIFTH
            if (it != list.begin())
            {
                cards += ";";
            }
            cards += (*it);
        }
        obj3->CreateValue(cards);
    }

    if (!json.SaveToFile(fileName))
    {
        TLogError("Saving deal game result failed.");
    }
}
/*****************************************************************************/
bool Deal::LoadGameDealLog(const std::string &fileName)
{
    bool ret = false;
    JsonReader json;

    if (json.Open(fileName))
    {
        std::int32_t numberOfPlayers;
        std::vector<JsonValue> players = json.GetArray("deal_info:players", JsonValue::STRING);
        numberOfPlayers = players.size();
        if ((numberOfPlayers == 3U) ||
            (numberOfPlayers == 4U) ||
            (numberOfPlayers == 5U))
        {
            // Load played cards
            std::vector<JsonValue> tricks = json.GetArray("tricks", JsonValue::STRING);
            if (tricks.size() == Tarot::NumberOfCardsInHand(numberOfPlayers))
            {

            }
        }
    }
    return ret;
}

//=============================================================================
// End of file Deal.h
//=============================================================================
