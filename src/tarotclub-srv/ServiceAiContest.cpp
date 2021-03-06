#include "ServiceAiContest.h"

static const std::string cOpponents[3U] = {
     std::string("Bishop"),
     std::string("Marvin"),
     std::string("TARS")
};


ServiceAiContest::ServiceAiContest()
{

}

#if 0

/*****************************************************************************/
void SrvStats::Start(const ServerOptions &options, const TournamentOptions &tournamentOpt)
{
    // Init lobby
    mLobbyServer.Initialize(options);
    mLobbyServer.RegisterListener(*this);

    // Init server
    mAiContest = tournamentOpt.turns;
    mGamePort = options.game_tcp_port;
    mInitialized = true;
    mThread = std::thread(SrvStats::EntryPoint, this);

    // Blocking call, will work in the caller's context
    mConsole.Manage(mLobby, options.console_tcp_port);

    // Properly stop the threads
    mBotManager.Close();
    mLobbyServer.Stop();
    mLobbyServer.WaitForEnd();
    Stop();
}
/*****************************************************************************/
void SrvStats::Update(const LobbySrvStats::Event &event)
{
    if (event.mType == LobbySrvStats::Event::cIncPlayer)
    {
        IncPlayer();
    }
    else if (event.mType == LobbySrvStats::Event::cDecPlayer)
    {
        DecPlayer();
    }
    else if (event.mType == LobbySrvStats::Event::cDataSent)
    {
        Protocol::DataManagement(this, event.mBlock);
    }
}
/*****************************************************************************/
/**
 * @brief SrvStats::DoAction
 *
 * Executed within the Lobby server thread. Don't perform any lobby access in this context.
 *
 * @param cmd
 * @param src_uuid
 * @param dest_uuid
 * @param data
 * @return
 */
bool SrvStats::DoAction(std::uint8_t cmd, std::uint32_t src_uuid, std::uint32_t dest_uuid, const ByteArray &data)
{
    (void) src_uuid;
    (void) dest_uuid;
    // Catch interesting data for us
    if (cmd == Protocol::TABLE_END_OF_DEAL)
    {
        // Get JSON deal result
        Points points;
        std::string jsonResult;
        ByteStreamReader in(data);
        in >> points;
        in >> jsonResult;

        JsonValue json;

        if (JsonReader::ParseString(json, jsonResult))
        {
            if (mLobby.GetName() == "aicontest")
            {
                mAiMutex.lock();
                for (std::vector<BotMatch>::iterator iter = mPendingAi.begin(); iter != mPendingAi.end(); iter++)
                {
                    if (iter->tableId == src_uuid)
                    {
                        Deal deal;
                        iter->deals.AddValue(json);
                        deal.DecodeJsonDeal(json);
                        iter->score.AddPoints(points, deal.GetBid(), 4U);

                        std::stringstream ss;
                        ss << "Deal " << (int)iter->score.GetCurrentCounter() << "/" << (int)iter->score.GetNumberOfTurns() << " finished for bot: " + iter->identity.nickname;
                        TLogServer(ss.str());
                    }
                }
                mAiMutex.unlock();
            }
        }
        else
        {
            TLogError("Internal error: generated Json string is not valid.");
        }
    }
    else if (cmd == Protocol::TABLE_END_OF_GAME)
    {
        if (mLobby.GetName() == "aicontest")
        {
            mAiMutex.lock();
            for (std::vector<BotMatch>::iterator iter = mPendingAi.begin(); iter != mPendingAi.end(); iter++)
            {
                if (iter->tableId == src_uuid)
                {
                    iter->finished = true;
                    break;
                }
            }
            mAiMutex.unlock();
        }
    }

    return true;
}
/*****************************************************************************/
void SrvStats::EntryPoint(void *pthis)
{
    Server *pt = static_cast<Server *>(pthis);
    pt->Run();
}
/*****************************************************************************/
void SrvStats::Run()
{
#ifdef TAROT_DEBUG
    static const std::uint32_t cRefreshPeriodInSeconds = 60U;
#else
    static const std::uint32_t cRefreshPeriodInSeconds = 600U;
#endif

    time_t rawtime;
    time_t nextHour;

    if (mDb.Open(System::HomePath() + cDbFileName))
    {
        mDb.Query("CREATE TABLE IF NOT EXISTS stats (datetime INTEGER, min INTEGER, max INTEGER, current INTEGER, total INTEGER, curr_mem INTEGER, max_mem INTEGER);");
        mDb.Close();
    }

    time(&rawtime);
    nextHour = rawtime; // execute immediately at first run

    while(!mStopRequested)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1U));
        time(&rawtime);

#ifdef DATABASE_DEBUG
        std::cout << "Next database update in: " << (int)(nextHour - rawtime) << " seconds" << std::endl;
#endif
        if (rawtime >= nextHour)
        {
            nextHour = ((rawtime / cRefreshPeriodInSeconds) * cRefreshPeriodInSeconds) + cRefreshPeriodInSeconds;

            StoreStats(rawtime);

            if (mLobby.GetName() == "aicontest")
            {
                CheckNewAIBots();
                CheckFinishedGames();
            }
        }
    }
}
/*****************************************************************************/
void SrvStats::CheckFinishedGames()
{
    mAiMutex.lock();
    for (std::vector<BotMatch>::iterator iter = mPendingAi.begin(); iter != mPendingAi.end(); iter++)
    {
        if (iter->finished)
        {
            TLogServer("Game finished, storing result in database");

            // Compute duration of the match
            std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - iter->startTime);

            CouchDb db;
            db.Connect();

            // Save played deals in the database along with some tournament information
            JsonObject json;
            json.AddValue("deals", iter->deals);

            json.AddValue("duration", timeSpan.count());
            json.AddValue("score", iter->score.GetTotalPoints(Place::SOUTH));

            db.Post("/tc_aicontest", json.ToString(0U));
            // Copy the document id
            if (db.IsSuccess())
            {
                std::string id;
                if (db.GetValue("id", id))
                {
                    db.Close();

                    // Try 3 times before leaving (possible failure is:
                    // document can be modified externally at the same time resulting in a bad revision
                    for (int i = 0; i < 3; i++)
                    {
                        if (StoreBotScore(*iter, id))
                        {
                            TLogServer("Successfully stored bot score info.");
                            break;
                        }
                        else
                        {
                            TLogError("Recording bot game failed.");
                        }
                    }
                }
            }
            else
            {
                TLogError(db.GetString());
            }

            // Remove bots
            for (std::vector<std::uint32_t>::iterator botit = iter->botIds.begin();
                 botit != iter->botIds.end();
                 ++botit)
            {
                mBotManager.RemoveBot(*botit);
            }
            // Remove table
            mLobby.DestroyTable(iter->tableId);

            // Clear entry
            mPendingAi.erase(iter);
            break; // exit loop, we have modified the iterator context!!
        }
    }
    mAiMutex.unlock();
}
/*****************************************************************************/
void SrvStats::CheckNewAIBots()
{
    CouchDb db;

    db.Connect();
    db.Get("/tc_users/_design/users/_view/bots_to_run");
    if (db.IsValid())
    {
        JsonValue rows = db.FindValue("rows");

        for (JsonArray::Iterator it = rows.GetArray().Begin(); it != rows.GetArray().End(); ++it)
        {
            JsonValue username = it->FindValue("key");
            JsonValue bot = it->FindValue("value");

            if (bot.FindValue("status").GetString() == "waiting")
            {
                if (!IsPending(username.GetString()))
                {
                    TLogServer("Found bot belonging to: " + username.GetString());

                    // Create the bot identity
                    BotMatch aibot;
                    aibot.identity.username  = username.GetString();
                    aibot.identity.gender    = Identity::cGenderRobot;
                    aibot.identity.nickname  = bot.FindValue("botname").GetString();
                    aibot.score.NewGame(mAiContest.size());
                    aibot.finished = false;

                    TLogServer("Launching tournament ....");
                    aibot.startTime = std::chrono::high_resolution_clock::now();

                    // Create the table
                    Tarot::Game game;
                    game.deals = mAiContest;
                    game.mode = Tarot::Game::cSimpleTournament;

                    aibot.tableId = mLobby.CreateTable(aibot.identity.username, false, game);

                    // Add the AI bot file
                    std::uint32_t botId = mBotManager.AddBot(aibot.tableId, aibot.identity, 0U, bot.FindValue("aifile").GetString());
                    mBotManager.ConnectBot(botId, "127.0.0.1", mGamePort);
                    aibot.botIds.push_back(botId);

                    // Add opponents
                    for (std::uint32_t i = 0U; i < 3U; i++)
                    {
                        Identity ident;
                        ident.gender = Identity::cGenderMale;
                        ident.nickname = cOpponents[i];
                        botId = mBotManager.AddBot(aibot.tableId, ident, 0U, System::HomePath() + "ai.zip");
                        mBotManager.ConnectBot(botId, "127.0.0.1", mGamePort);
                        aibot.botIds.push_back(botId);
                    }

                    mAiMutex.lock();
                    mPendingAi.push_back(aibot);
                    mAiMutex.unlock();
                }
            }
        }
    }

    db.Close();
}
/*****************************************************************************/
bool SrvStats::IsPending(const std::string &username)
{
    bool ret = false;
    for (std::vector<BotMatch>::iterator iter = mPendingAi.begin(); iter != mPendingAi.end(); ++iter)
    {
        if (iter->identity.username == username)
        {
            ret = true;
        }
    }
    return ret;
}
/*****************************************************************************/
bool SrvStats::StoreBotScore(const BotMatch &match, const std::string &docId)
{
    bool ret = false;

    CouchDb db;
    db.Connect();

    db.Get("/tc_users/" + match.identity.username);
    if (db.IsValid())
    {
        // Update document
        JsonValue rows = db.FindValue("bots");

        std::uint32_t index = 0U;
        for (JsonArray::Iterator it = rows.GetArray().Begin(); it != rows.GetArray().End(); ++it)
        {
            if (it->FindValue("botname").GetString() == match.identity.nickname)
            {
                JsonValue bot = (*it);
                bot.ReplaceValue("status", docId);

                std::stringstream ss;
                ss << "bots:" << index;
                db.ReplaceValue(ss.str(), bot);

                std::cout << db.ToString(0U);
                db.Put("/tc_users/" + match.identity.username, db.ToString(0U));
                ret = db.IsSuccess();
                break;
            }
            index++;
        }
    }

    return ret;
}
#endif
