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
    uint32 minPriceGrey;
    uint32 maxPriceGrey;
    uint32 minBidPriceGrey;
    uint32 maxBidPriceGrey;
    uint32 maxStackGrey;
    uint32 minPriceWhite;
    uint32 maxPriceWhite;
    uint32 minBidPriceWhite;
    uint32 maxBidPriceWhite;
    uint32 maxStackWhite;
    uint32 minPriceGreen;
    uint32 maxPriceGreen;
    uint32 minBidPriceGreen;
    uint32 maxBidPriceGreen;
    uint32 maxStackGreen;
    uint32 minPriceBlue;
    uint32 maxPriceBlue;
    uint32 minBidPriceBlue;
    uint32 maxBidPriceBlue;
    uint32 maxStackBlue;
    uint32 minPricePurple;
    uint32 maxPricePurple;
    uint32 minBidPricePurple;
    uint32 maxBidPricePurple;
    uint32 maxStackPurple;
    uint32 minPriceOrange;
    uint32 maxPriceOrange;
    uint32 minBidPriceOrange;
    uint32 maxBidPriceOrange;
    uint32 maxStackOrange;
    uint32 minPriceYellow;
    uint32 maxPriceYellow;
    uint32 minBidPriceYellow;
    uint32 maxBidPriceYellow;
    uint32 maxStackYellow;

    uint32 buyerPriceGrey;
    uint32 buyerPriceWhite;
    uint32 buyerPriceGreen;
    uint32 buyerPriceBlue;
    uint32 buyerPricePurple;
    uint32 buyerPriceOrange;
    uint32 buyerPriceYellow;
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

    void SetMinPrice(uint32 color, uint32 value)
    {
        switch(color)
        {
        case AHB_GREY:
            minPriceGrey = value;
            break;
        case AHB_WHITE:
            minPriceWhite = value;
            break;
        case AHB_GREEN:
            minPriceGreen = value;
            break;
        case AHB_BLUE:
            minPriceBlue = value;
            break;
        case AHB_PURPLE:
            minPricePurple = value;
            break;
        case AHB_ORANGE:
            minPriceOrange = value;
            break;
        case AHB_YELLOW:
            minPriceYellow = value;
            break;
        default:
            break;
        }
    }

    uint32 GetMinPrice(uint32 color)
    {
        switch(color)
        {
        case AHB_GREY:
            {
                if (minPriceGrey == 0)
                    return 100;
                else if (minPriceGrey > maxPriceGrey)
                    return maxPriceGrey;
                else
                    return minPriceGrey;
                break;
            }
        case AHB_WHITE:
            {
                if (minPriceWhite == 0)
                    return 150;
                else if (minPriceWhite > maxPriceWhite)
                    return maxPriceWhite;
                else
                    return minPriceWhite;
                break;
            }
        case AHB_GREEN:
            {
                if (minPriceGreen == 0)
                    return 200;
                else if (minPriceGreen > maxPriceGreen)
                    return maxPriceGreen;
                else
                    return minPriceGreen;
                break;
            }
        case AHB_BLUE:
            {
                if (minPriceBlue == 0)
                    return 250;
                else if (minPriceBlue > maxPriceBlue)
                    return maxPriceBlue;
                else
                    return minPriceBlue;
                break;
            }
        case AHB_PURPLE:
            {
                if (minPricePurple == 0)
                    return 300;
                else if (minPricePurple > maxPricePurple)
                    return maxPricePurple;
                else
                    return minPricePurple;
                break;
            }
        case AHB_ORANGE:
            {
                if (minPriceOrange == 0)
                    return 400;
                else if (minPriceOrange > maxPriceOrange)
                    return maxPriceOrange;
                else
                    return minPriceOrange;
                break;
            }
        case AHB_YELLOW:
            {
                if (minPriceYellow == 0)
                    return 500;
                else if (minPriceYellow > maxPriceYellow)
                    return maxPriceYellow;
                else
                    return minPriceYellow;
                break;
            }
        default:
            {
                return 0;
                break;
            }
        }
    }
    void SetMaxPrice(uint32 color, uint32 value)
    {
        switch(color)
        {
        case AHB_GREY:
            maxPriceGrey = value;
            break;
        case AHB_WHITE:
            maxPriceWhite = value;
            break;
        case AHB_GREEN:
            maxPriceGreen = value;
            break;
        case AHB_BLUE:
            maxPriceBlue = value;
            break;
        case AHB_PURPLE:
            maxPricePurple = value;
            break;
        case AHB_ORANGE:
            maxPriceOrange = value;
            break;
        case AHB_YELLOW:
            maxPriceYellow = value;
            break;
        default:
            break;
        }
    }
    uint32 GetMaxPrice(uint32 color)
    {
        switch(color)
        {
        case AHB_GREY:
            {
                if (maxPriceGrey == 0)
                    return 150;
                else
                    return maxPriceGrey;
                break;
            }
        case AHB_WHITE:
            {
                if (maxPriceWhite == 0)
                    return 250;
                else
                    return maxPriceWhite;
                break;
            }
        case AHB_GREEN:
            {
                if (maxPriceGreen == 0)
                    return 300;
                else
                    return maxPriceGreen;
                break;
            }
        case AHB_BLUE:
            {
                if (maxPriceBlue == 0)
                    return 350;
                else
                    return maxPriceBlue;
                break;
            }
        case AHB_PURPLE:
            {
                if (maxPricePurple == 0)
                    return 450;
                else
                    return maxPricePurple;
                break;
            }
        case AHB_ORANGE:
            {
                if (maxPriceOrange == 0)
                    return 550;
                else
                    return maxPriceOrange;
                break;
            }
        case AHB_YELLOW:
            {
                if (maxPriceYellow == 0)
                    return 650;
                else
                    return maxPriceYellow;
                break;
            }
        default:
            {
                return 0;
                break;
            }
        }
    }
    void SetMinBidPrice(uint32 color, uint32 value)
    {
        switch(color)
        {
        case AHB_GREY:
            minBidPriceGrey = value;
            break;
        case AHB_WHITE:
            minBidPriceWhite = value;
            break;
        case AHB_GREEN:
            minBidPriceGreen = value;
            break;
        case AHB_BLUE:
            minBidPriceBlue = value;
            break;
        case AHB_PURPLE:
            minBidPricePurple = value;
            break;
        case AHB_ORANGE:
            minBidPriceOrange = value;
            break;
        case AHB_YELLOW:
            minBidPriceYellow = value;
            break;
        default:
            break;
        }
    }
    uint32 GetMinBidPrice(uint32 color)
    {
        switch(color)
        {
        case AHB_GREY:
            {
                if (minBidPriceGrey > 100)
                    return 100;
                else
                    return minBidPriceGrey;
                break;
            }
        case AHB_WHITE:
            {
                if (minBidPriceWhite > 100)
                    return 100;
                else
                    return minBidPriceWhite;
                break;
            }
        case AHB_GREEN:
            {
                if (minBidPriceGreen > 100)
                    return 100;
                else
                    return minBidPriceGreen;
                break;
            }
        case AHB_BLUE:
            {
                if (minBidPriceBlue > 100)
                    return 100;
                else
                    return minBidPriceBlue;
                break;
            }
        case AHB_PURPLE:
            {
                if (minBidPricePurple > 100)
                    return 100;
                else
                    return minBidPricePurple;
                break;
            }
        case AHB_ORANGE:
            {
                if (minBidPriceOrange > 100)
                    return 100;
                else
                    return minBidPriceOrange;
                break;
            }
        case AHB_YELLOW:
            {
                if (minBidPriceYellow > 100)
                    return 100;
                else
                    return minBidPriceYellow;
                break;
            }
        default:
            {
                return 0;
                break;
            }
        }
    }
    void SetMaxBidPrice(uint32 color, uint32 value)
    {
        switch(color)
        {
        case AHB_GREY:
            maxBidPriceGrey = value;
            break;
        case AHB_WHITE:
            maxBidPriceWhite = value;
            break;
        case AHB_GREEN:
            maxBidPriceGreen = value;
            break;
        case AHB_BLUE:
            maxBidPriceBlue = value;
            break;
        case AHB_PURPLE:
            maxBidPricePurple = value;
            break;
        case AHB_ORANGE:
            maxBidPriceOrange = value;
            break;
        case AHB_YELLOW:
            maxBidPriceYellow = value;
            break;
        default:
            break;
        }
    }
    uint32 GetMaxBidPrice(uint32 color)
    {
        switch(color)
        {
        case AHB_GREY:
            {
                if (maxBidPriceGrey > 100)
                    return 100;
                else
                    return maxBidPriceGrey;
                break;
            }
        case AHB_WHITE:
            {
                if (maxBidPriceWhite > 100)
                    return 100;
                else
                    return maxBidPriceWhite;
                break;
            }
        case AHB_GREEN:
            {
                if (maxBidPriceGreen > 100)
                    return 100;
                else
                    return maxBidPriceGreen;
                break;
            }
        case AHB_BLUE:
            {
                if (maxBidPriceBlue > 100)
                    return 100;
                else
                    return maxBidPriceBlue;
                break;
            }
        case AHB_PURPLE:
            {
                if (maxBidPricePurple > 100)
                    return 100;
                else
                    return maxBidPricePurple;
                break;
            }
        case AHB_ORANGE:
            {
                if (maxBidPriceOrange > 100)
                    return 100;
                else
                    return maxBidPriceOrange;
                break;
            }
        case AHB_YELLOW:
            {
                if (maxBidPriceYellow > 100)
                    return 100;
                else
                    return maxBidPriceYellow;
                break;
            }
        default:
            {
                return 0;
                break;
            }
        }
    }
    void SetMaxStack(uint32 color, uint32 value)
    {
        switch(color)
        {
        case AHB_GREY:
            maxStackGrey = value;
            break;
        case AHB_WHITE:
            maxStackWhite = value;
            break;
        case AHB_GREEN:
            maxStackGreen = value;
            break;
        case AHB_BLUE:
            maxStackBlue = value;
            break;
        case AHB_PURPLE:
            maxStackPurple = value;
            break;
        case AHB_ORANGE:
            maxStackOrange = value;
            break;
        case AHB_YELLOW:
            maxStackYellow = value;
            break;
        default:
            break;
        }
    }
    uint32 GetMaxStack(uint32 color)
    {
        switch(color)
        {
        case AHB_GREY:
            {
                return maxStackGrey;
                break;
            }
        case AHB_WHITE:
            {
                return maxStackWhite;
                break;
            }
        case AHB_GREEN:
            {
                return maxStackGreen;
                break;
            }
        case AHB_BLUE:
            {
                return maxStackBlue;
                break;
            }
        case AHB_PURPLE:
            {
                return maxStackPurple;
                break;
            }
        case AHB_ORANGE:
            {
                return maxStackOrange;
                break;
            }
        case AHB_YELLOW:
            {
                return maxStackYellow;
                break;
            }
        default:
            {
                return 0;
                break;
            }
        }
    }
    void SetBuyerPrice(uint32 color, uint32 value)
    {
        switch(color)
        {
        case AHB_GREY:
            buyerPriceGrey = value;
            break;
        case AHB_WHITE:
            buyerPriceWhite = value;
            break;
        case AHB_GREEN:
            buyerPriceGreen = value;
            break;
        case AHB_BLUE:
            buyerPriceBlue = value;
            break;
        case AHB_PURPLE:
            buyerPricePurple = value;
            break;
        case AHB_ORANGE:
            buyerPriceOrange = value;
            break;
        case AHB_YELLOW:
            buyerPriceYellow = value;
            break;
        default:
            break;
        }
    }
    uint32 GetBuyerPrice(uint32 color)
    {
        switch(color)
        {
        case AHB_GREY:
            return buyerPriceGrey;
            break;
        case AHB_WHITE:
            return buyerPriceWhite;
            break;
        case AHB_GREEN:
            return buyerPriceGreen;
            break;
        case AHB_BLUE:
            return buyerPriceBlue;
            break;
        case AHB_PURPLE:
            return buyerPricePurple;
            break;
        case AHB_ORANGE:
            return buyerPriceOrange;
            break;
        case AHB_YELLOW:
            return buyerPriceYellow;
            break;
        default:
            return 0;
            break;
        }
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
