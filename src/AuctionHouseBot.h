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
#include "SharedDefines.h"

class FactionSpecificAuctionHouseConfig
{
private:
    uint32 AHID;
    uint32 AHFID;
    uint32 minItems;
    uint32 maxItems;

public:
    FactionSpecificAuctionHouseConfig(uint32 ahid)
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
    FactionSpecificAuctionHouseConfig()
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
    }
    uint32 GetMaxItems()
    {
        return maxItems;
    }
    ~FactionSpecificAuctionHouseConfig()
    {
    }
};

class AuctionHouseBotCharacter
{
public:
    AuctionHouseBotCharacter(uint32 accountID, uint32 characterGUID) :
        AccountID(accountID),
        CharacterGUID(characterGUID) { }
    uint32 AccountID;
    ObjectGuid::LowType CharacterGUID;
};

class AuctionHouseBot
{
public:
    std::vector<AuctionHouseBotCharacter> AHCharacters;
    uint32 CurrentBotCharGUID;

private:
    bool debug_Out;
    bool debug_Out_Filters;

    bool SellingBotEnabled;
    bool BuyingBotEnabled;
    int CyclesBetweenBuyOrSell;
    uint32 MaxBuyoutPriceInCopper;
    float BuyoutVariationReducePercent;
    float BuyoutVariationAddPercent;
    float BidVariationHighReducePercent;
    float BidVariationLowReducePercent;
    float BuyoutBelowVendorVariationAddPercent;
    uint32 BuyingBotBuyCandidatesPerBuyCycle;
    uint32 ListingExpireTimeInSecondsMin;
    uint32 ListingExpireTimeInSecondsMax;
    float BuyingBotAcceptablePriceModifier;
    bool BuyingBotAlwaysBidMaxCalculatedPrice;
    bool BuyingBotWillBidAgainstPlayers;
    std::vector<uint32> vendorItemsPrices;
    std::string AHCharactersGUIDsForQuery;
    uint32 ItemsPerCycle;
    bool DisabledItemTextFilter;
    std::set<uint32> DisabledItems;
    bool ListedItemLevelRestrictedEnabled;
    bool ListedItemLevelRestrictedUseCraftedItemForCalculation;
    uint32 ListedItemLevelMax;
    uint32 ListedItemLevelMin;
    std::set<uint32> ListedItemLevelExceptionItems;
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
    uint32 RandomStackIncrementConsumable;
    uint32 RandomStackIncrementContainer;
    uint32 RandomStackIncrementWeapon;
    uint32 RandomStackIncrementGem;
    uint32 RandomStackIncrementArmor;
    uint32 RandomStackIncrementReagent;
    uint32 RandomStackIncrementProjectile;
    uint32 RandomStackIncrementTradeGood;
    uint32 RandomStackIncrementGeneric;
    uint32 RandomStackIncrementRecipe;
    uint32 RandomStackIncrementQuiver;
    uint32 RandomStackIncrementQuest;
    uint32 RandomStackIncrementKey;
    uint32 RandomStackIncrementMisc;
    uint32 RandomStackIncrementGlyph;
    std::vector<uint32> ItemCandidateClassWeightedProportionList;
    std::map<uint32, std::vector<uint32>> ItemCandidatesByItemClass;
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
    float PriceMultiplierItemLevelCategoryConsumable;
    float PriceMultiplierItemLevelCategoryContainer;
    float PriceMultiplierItemLevelCategoryWeapon;
    float PriceMultiplierItemLevelCategoryGem;
    float PriceMultiplierItemLevelCategoryArmor;
    float PriceMultiplierItemLevelCategoryReagent;
    float PriceMultiplierItemLevelCategoryProjectile;
    float PriceMultiplierItemLevelCategoryTradeGood;
    float PriceMultiplierItemLevelCategoryGeneric;
    float PriceMultiplierItemLevelCategoryRecipe;
    float PriceMultiplierItemLevelCategoryQuiver;
    float PriceMultiplierItemLevelCategoryQuest;
    float PriceMultiplierItemLevelCategoryKey;
    float PriceMultiplierItemLevelCategoryMisc;
    float PriceMultiplierItemLevelCategoryGlyph;
    float PriceMultiplierQualityPoor;
    float PriceMultiplierQualityNormal;
    float PriceMultiplierQualityUncommon;
    float PriceMultiplierQualityRare;
    float PriceMultiplierQualityEpic;
    float PriceMultiplierQualityLegendary;
    float PriceMultiplierQualityArtifact;
    float PriceMultiplierQualityHeirloom;
    float PriceMultiplierCategoryQuality[MAX_ITEM_CLASS][MAX_ITEM_QUALITY];
    float PriceMultiplierCategoryMountQualityPoor;
    float PriceMultiplierCategoryMountQualityNormal;
    float PriceMultiplierCategoryMountQualityUncommon;
    float PriceMultiplierCategoryMountQualityRare;
    float PriceMultiplierCategoryMountQualityEpic;
    float PriceMultiplierCategoryMountQualityLegendary;
    float PriceMultiplierCategoryMountQualityArtifact;
    float PriceMultiplierCategoryMountQualityHeirloom;
    bool AdvancedPricingConsumablePotionEnabled;
    bool AdvancedPricingConsumableElixirEnabled;
    bool AdvancedPricingConsumableFlaskEnabled;
    bool AdvancedPricingGemEnabled;
    bool AdvancedPricingTradeGoodClothEnabled;
    bool AdvancedPricingTradeGoodHerbEnabled;
    bool AdvancedPricingTradeGoodMetalStoneEnabled;
    bool AdvancedPricingTradeGoodLeatherEnabled;
    bool AdvancedPricingTradeGoodEnchantingEnabled;
    bool AdvancedPricingTradeGoodElementalEnabled;
    bool AdvancedPricingTradeGoodMeatEnabled;
    bool AdvancedPricingMiscJunkEnabled;
    bool AdvancedPricingMiscMountEnabled;
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
    bool ListedItemIDRestrictedEnabled;
    uint32 ListedItemIDMin;
    uint32 ListedItemIDMax;
    std::set<uint32> ListedItemIDExceptionItems;
    bool PreventOverpayingForVendorItems;

    FactionSpecificAuctionHouseConfig AllianceConfig;
    FactionSpecificAuctionHouseConfig HordeConfig;
    FactionSpecificAuctionHouseConfig NeutralConfig;

    int LastCycleCount;

    uint32 getStackSizeForItem(ItemTemplate const* itemProto) const;
    void calculateItemValue(ItemTemplate const* itemProto, uint64& outBidPrice, uint64& outBuyoutPrice);
    float getAdvancedPricingMultiplier(ItemTemplate const* itemProto);
    void populatetemClassSeedListForItemClass(uint32 itemClass, uint32 itemClassSeedWeight);
    void populateItemClassProportionList();
    ItemTemplate const* getProducedItemFromRecipe(ItemTemplate const* recipeItemTemplate);
    void populateItemCandidateList();
    int getRandomValidItemClassForNewListing();
    void addNewAuctions(Player* AHBplayer, FactionSpecificAuctionHouseConfig *config);
    void addNewAuctionBuyerBotBid(Player* AHBplayer, FactionSpecificAuctionHouseConfig *config);
    void populateVendorItemsPrices();

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
    uint32 GetRandomStackIncrementValue(std::string configKeyString, uint32 defaultValue);

    void AddCharacters(std::string characterGUIDString);
    void AddPriceMinimumOverrides(std::string priceMinimimOverridesString);
    void AddItemIDsFromString(std::set<uint32>& workingItemIDSet, std::string itemString, const char* parentOperationName);
    void AddToItemIDSet(std::set<uint32>& workingItemIDSet, uint32 itemID, const char* parentOperationName);
    const char* GetQualityName(ItemQualities quality);
    const char* GetCategoryName(ItemClass category);
};

#define auctionbot AuctionHouseBot::instance()

#endif
