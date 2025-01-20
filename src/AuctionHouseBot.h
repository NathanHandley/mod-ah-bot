/*
 * Copyright (C) 2008-2010 Trinity <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 * Copyright (C) 2023+ Nathan Handley <https://github.com/NathanHandley>
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

    uint32 AHBplayerAccount;
    ObjectGuid::LowType AHBplayerGUID;
    uint32 ItemsPerCycle;

    bool DisabledItemTextFilter;
    std::set<uint32> DisabledItems;
    uint32 RandomStackRatioConsumable;
    uint32 RandomStackRatioContainer;
    uint32 RandomStackRatioWeapon;
    uint32 RandomStackRatioGem;
    uint32 RandomStackRatioArmor;
    uint32 RandomStackRatioReagent;
    uint32 RandomStackRatioProjectile;
    uint32 RandomStackRatioTradeGood;
    uint32 RandomStackRatioGeneric;
    uint32 RandomStackRatioRecipe;
    uint32 RandomStackRatioQuiver;
    uint32 RandomStackRatioQuest;
    uint32 RandomStackRatioKey;
    uint32 RandomStackRatioMisc;
    uint32 RandomStackRatioGlyph;
    std::vector<uint32> itemCandidateClassWeightedProportionList;
    std::map<uint32, std::vector<uint32>> itemCandidatesByItemClass;
    uint32 ListProportionConsumable;
    uint32 ListProportionContainer;
    uint32 ListProportionWeapon;
    uint32 ListProportionGem;
    uint32 ListProportionArmor;
    uint32 ListProportionReagent;
    uint32 ListProportionProjectile;
    uint32 ListProportionTradeGood;
    uint32 ListProportionGeneric;
    uint32 ListProportionRecipe;
    uint32 ListProportionQuiver;
    uint32 ListProportionQuest;
    uint32 ListProportionKey;
    uint32 ListProportionMisc;
    uint32 ListProportionGlyph;
    float PriceMultiplierCategoryConsumable;
    float PriceMultiplierCategoryContainer;
    float PriceMultiplierCategoryWeapon;
    float PriceMultiplierCategoryGem;
    float PriceMultiplierCategoryArmor;
    float PriceMultiplierCategoryReagent;
    float PriceMultiplierCategoryProjectile;
    float PriceMultiplierCategoryTradeGood;
    float PriceMultiplierCategoryGeneric;
    float PriceMultiplierCategoryRecipe;
    float PriceMultiplierCategoryQuiver;
    float PriceMultiplierCategoryQuest;
    float PriceMultiplierCategoryKey;
    float PriceMultiplierCategoryMisc;
    float PriceMultiplierCategoryGlyph;
    float PriceMultiplierQualityPoor;
    float PriceMultiplierQualityNormal;
    float PriceMultiplierQualityUncommon;
    float PriceMultiplierQualityRare;
    float PriceMultiplierQualityEpic;
    float PriceMultiplierQualityLegendary;
    float PriceMultiplierQualityArtifact;
    float PriceMultiplierQualityHeirloom;
    uint32 PriceMinimumCenterBaseConsumable;
    uint32 PriceMinimumCenterBaseContainer;
    uint32 PriceMinimumCenterBaseWeapon;
    uint32 PriceMinimumCenterBaseGem;
    uint32 PriceMinimumCenterBaseArmor;
    uint32 PriceMinimumCenterBaseReagent;
    uint32 PriceMinimumCenterBaseProjectile;
    uint32 PriceMinimumCenterBaseTradeGood;
    uint32 PriceMinimumCenterBaseGeneric;
    uint32 PriceMinimumCenterBaseRecipe;
    uint32 PriceMinimumCenterBaseQuiver;
    uint32 PriceMinimumCenterBaseQuest;
    uint32 PriceMinimumCenterBaseKey;
    uint32 PriceMinimumCenterBaseMisc;
    uint32 PriceMinimumCenterBaseGlyph;
    std::unordered_map<uint32, uint64> PriceMinimumCenterBaseOverridesByItemID;

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
    void populateItemClassProportionList();
    void populateItemCandidateList();
    void addNewAuctions(Player *AHBplayer, AHBConfig *config);
    void addNewAuctionBuyerBotBid(Player *AHBplayer, AHBConfig *config, WorldSession *session);

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
    uint32 GetRandomStackValue(std::string configKeyString, uint32 defaultValue);
    void AddToDisabledItems(std::set<uint32>& workingDisabledItemIDs, uint32 disabledItemID);
    void AddDisabledItems(std::string disabledItemIdString);
    void AddPriceMinimumOverrides(std::string priceMinimimOverridesString);
    ObjectGuid::LowType GetAHBplayerGUID() { return AHBplayerGUID; };
};

#define auctionbot AuctionHouseBot::instance()

#endif
