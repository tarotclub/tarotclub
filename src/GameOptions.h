/*=============================================================================
 * TarotClub - GameOptions.h
 *=============================================================================
 * Manage the options
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

#ifndef GAMEOPTIONS_H
#define GAMEOPTIONS_H

#include "defines.h"
#include "Identity.h"

typedef struct {
   //---- client stuff ----
   QString  deckFilePath;
   bool     showAvatars;
   int      langue;
   QString  tapis;
   Identity client;
   //---- server stuff ----
   int      timer;
   int      port;
   Identity bots[3];
} GameOptions;

#endif // GAMEOPTIONS_H

//=============================================================================
// End of file GameOptions.h
//=============================================================================
