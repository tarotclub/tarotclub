/*=============================================================================
 * TarotClub - Controller.h
 *=============================================================================
 * Manage TarotClub protocol requests within a Tarot context
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
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <thread>
#include <map>
#include "Protocol.h"
#include "TarotEngine.h"
#include "Observer.h"
#include "ByteArray.h"
#include "ThreadQueue.h"

/*****************************************************************************/
/**
 * @brief The Server class
 *
 * A server instance creates a thread. All requests to the server must be
 * performed using a request packet sent to the send for executions.
 *
 * All the requests are queued in the order of arrival and executed in a FIFO mode.
 *
 * The TarotEngine is not accessible to protect accesses and enforce all the
 * calls within the thread context.
 *
 */
class Controller : public Observer<TarotEngine::SignalInfo>
{

public:
    struct Signal
    {
        ByteArray data;
    };

    Controller();

    void RegisterListener(Observer<Signal> &sig);
    void Start();
    void ExecuteRequest(const ByteArray &packet);

private:
    TarotEngine engine;
    Subject<Signal> mSubject;
    std::thread mThread;
    ThreadQueue<ByteArray> mQueue; //!< Queue of network packets received

    /**
     * @brief Main server thread loop
     */
    void Run();
    static void EntryPoint(void * pthis);

    void NewServerGame(Game::Mode mode);

    void Update(const TarotEngine::SignalInfo &info);
    bool DoAction(const ByteArray &data);
    void SendPacket(const ByteArray &block);

};

#endif // CONTROLLER_H

//=============================================================================
// End of file Controller.h
//=============================================================================