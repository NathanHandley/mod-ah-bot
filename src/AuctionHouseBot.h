/*
 * Copyright (C) 2008-2010 Trinity <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef AUCTION_HOUSE_BOT_H
#define AUCTION_HOUSE_BOT_H

#include "Common.h"
#include "ObjectGuid.h"

#include <map>
#include <vector>

struct AuctionEntry;
class Player;
class WorldSession;

#include "ItemTemplate.h"

#define AHB_GREY        0
#define AHB_WHITE       1
#define AHB_GREEN       2
#define AHB_BLUE        3
#define AHB_PURPLE      4
#define AHB_ORANGE      5
#define AHB_YELLOW      6
#define AHB_MAX_QUALITY 6
#define AHB_GREY_TG     0
#define AHB_WHITE_TG    1
#define AHB_GREEN_TG    2
#define AHB_BLUE_TG     3
#define AHB_PURPLE_TG   4
#define AHB_ORANGE_TG   5
#define AHB_YELLOW_TG   6
#define AHB_GREY_I      7
#define AHB_WHITE_I     8
#define AHB_GREEN_I     9
#define AHB_BLUE_I      10
#define AHB_PURPLE_I    11
#define AHB_ORANGE_I    12
#define AHB_YELLOW_I    13

class AHBConfig
{
private:
    uint32 AHID;
    uint32 AHFID;
    uint32 minItems;
    uint32 maxItems;

    uint32 buyerBiddingInterval;
    uint32 buyerBidsPerInterval;

public:
    AHBConfig(uint32 ahid)
    {
        AHID = ahid;
        switch(ahid)
        {
        case 2:
            AHFID = 55;
            break;
        case 6:
            AHFID = 29;
            break;
        case 7:
            AHFID = 120;
            break;
        default:
            AHFID = 120;
            break;
        }
    }
    AHBConfig()
    {
    }
    uint32 GetAHID()
    {
        return AHID;
    }
    uint32 GetAHFID()
    {
        return AHFID;
    }
    void SetMinItems(uint32 value)
    {
        minItems = value;
    }
    uint32 GetMinItems()
    {
        if ((minItems == 0) && (maxItems))
            return maxItems;
        else if ((maxItems) && (minItems > maxItems))
            return maxItems;
        else
            return minItems;
    }
    void SetMaxItems(uint32 value)
    {
        maxItems = value;
    // CalculatePercents() needs to be called, but only if
    // SetPercentages() has been called at least once already.
    }
    uint32 GetMaxItems()
    {
        return maxItems;
    }

    void SetBiddingInterval(uint32 value)
    {
        buyerBiddingInterval = value;
    }
    uint32 GetBiddingInterval()
    {
        return buyerBiddingInterval;
    }

    void SetBidsPerInterval(uint32 value)
    {
        buyerBidsPerInterval = value;
    }
    uint32 GetBidsPerInterval()
    {
        return buyerBidsPerInterval;
    }
    ~AHBConfig()
    {
    }
};
class AuctionHouseBot
{
private:

    bool debug_Out;
    bool debug_Out_Filters;

    bool AHBSeller;
    bool AHBBuyer;
    bool BuyMethod;
    bool SellMethod;

    uint32 AHBplayerAccount;
    ObjectGuid::LowType AHBplayerGUID;
    uint32 ItemsPerCycle;

    std::set<uint32> DisableItemStore;
    std::vector<uint32> itemCandidateClassWeightedSeedList;
    std::map<uint32, std::vector<uint32>> itemCandidatesByItemClass;

    AHBConfig AllianceConfig;
    AHBConfig HordeConfig;
    AHBConfig NeutralConfig;

    time_t _lastrun_a;
    time_t _lastrun_h;
    time_t _lastrun_n;

    inline uint32 minValue(uint32 a, uint32 b) { return a <= b ? a : b; };
    uint32 getStackSizeForItem(ItemTemplate const* itemProto) const;
    void calculateItemValue(ItemTemplate const* itemProto, uint64& outBidPrice, uint64& outBuyoutPrice);
    void populatetemClassSeedListForItemClass(uint32 itemClass, uint32 itemClassSeedWeight);
    void populateItemClassSeedList();
    void populateItemCandidateList();
    void addNewAuctions(Player *AHBplayer, AHBConfig *config);
    void addNewAuctionBuyerBotBid(Player *AHBplayer, AHBConfig *config, WorldSession *session);


//    friend class ACE_Singleton<AuctionHouseBot, ACE_Null_Mutex>;
    AuctionHouseBot();

public:
    static AuctionHouseBot* instance()
    {
        static AuctionHouseBot instance;
        return &instance;
    }

    ~AuctionHouseBot();
    void Update();
    void Initialize();
    void InitializeConfiguration();
    void LoadValues(AHBConfig*);
    void Commands(uint32, uint32, uint32, char*);
    ObjectGuid::LowType GetAHBplayerGUID() { return AHBplayerGUID; };
};

#define auctionbot AuctionHouseBot::instance()

#endif
