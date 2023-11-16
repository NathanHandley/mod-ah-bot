/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* ScriptData
Name: ah_bot_commandscript
%Complete: 100
Comment: All ah_bot related commands
Category: commandscripts
EndScriptData */

#include "ScriptMgr.h"
#include "Chat.h"
#include "AuctionHouseBot.h"
#include "Config.h"

#if AC_COMPILER == AC_COMPILER_GNU
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

using namespace Acore::ChatCommands;

class ah_bot_commandscript : public CommandScript
{
public:
    ah_bot_commandscript() : CommandScript("ah_bot_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> commandTable =
        {
            { "ahbotoptions",   SEC_GAMEMASTER,     true,   &HandleAHBotOptionsCommand,     "" },
        };
        return commandTable;
    }

    static bool HandleAHBotOptionsCommand(ChatHandler* handler, const char*args)
    {
        uint32 ahMapID = 0;
        char* opt = strtok((char*)args, " ");
        char* ahMapIdStr = strtok(NULL, " ");

        if (ahMapIdStr)
        {
            ahMapID = uint32(strtoul(ahMapIdStr, NULL, 0));
            switch (ahMapID)
            {
                case 2:
                case 6:
                case 7:
                    break;
                default:
                    opt = NULL;
                    break;
            }
        }

        if (!opt)
        {
            handler->PSendSysMessage("Syntax is: ahbotoptions $option $ahMapID (2, 6 or 7) $parameter");
            handler->PSendSysMessage("Try ahbotoptions help to see a list of options.");
            return false;
        }

        int l = strlen(opt);

        if (strncmp(opt, "help", l) == 0)
        {
            handler->PSendSysMessage("AHBot commands:");
            handler->PSendSysMessage("ahexpire");
            handler->PSendSysMessage("minitems");
            handler->PSendSysMessage("maxitems");
            handler->PSendSysMessage("minprice");
            handler->PSendSysMessage("maxprice");
            handler->PSendSysMessage("bidinterval");
            handler->PSendSysMessage("bidsperinterval");
            return true;
        }
        else if (strncmp(opt, "ahexpire", l) == 0)
        {
            if (!ahMapIdStr)
            {
                handler->PSendSysMessage("Syntax is: ahbotoptions ahexpire $ahMapID (2, 6 or 7)");
                return false;
            }

            auctionbot->Commands(0, ahMapID, 0, NULL);
        }
        else if (strncmp(opt, "minitems", l) == 0)
        {
            char* param1 = strtok(NULL, " ");
            if (!ahMapIdStr || !param1)
            {
                handler->PSendSysMessage("Syntax is: ahbotoptions minitems $ahMapID (2, 6 or 7) $minItems");
                return false;
            }

            auctionbot->Commands(1, ahMapID, 0, param1);
        }
        else if (strncmp(opt, "maxitems", l) == 0)
        {
            char* param1 = strtok(NULL, " ");
            if (!ahMapIdStr || !param1)
            {
                handler->PSendSysMessage("Syntax is: ahbotoptions maxitems $ahMapID (2, 6 or 7) $maxItems");
                return false;
            }

            auctionbot->Commands(2, ahMapID, 0, param1);
        }
        else if (strncmp(opt, "bidinterval", l) == 0)
        {
            char* param1 = strtok(NULL, " ");

            if (!ahMapIdStr || !param1)
            {
                handler->PSendSysMessage("Syntax is: ahbotoptions bidinterval $ahMapID (2, 6 or 7) $interval(in minutes)");
                return false;
            }

            auctionbot->Commands(12, ahMapID, 0, param1);
        }
        else if (strncmp(opt, "bidsperinterval", l) == 0)
        {
            char* param1 = strtok(NULL, " ");

            if (!ahMapIdStr || !param1)
            {
                handler->PSendSysMessage("Syntax is: ahbotoptions bidsperinterval $ahMapID (2, 6 or 7) $bids");
                return false;
            }

            auctionbot->Commands(13, ahMapID, 0, param1);
        }
        else
        {
            handler->PSendSysMessage("Syntax is: ahbotoptions $option $ahMapID (2, 6 or 7) $parameter");
            handler->PSendSysMessage("Try ahbotoptions help to see a list of options.");
            return false;
        }

        return true;
    }
};

void AddAHBotCommandScripts()
{
    new ah_bot_commandscript();
}
