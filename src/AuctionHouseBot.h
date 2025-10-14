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

struct ListProportionNode
{
    uint32 ItemClassID = 0;
    uint32 ItemQualityID = 0;
    uint32 Proportion = 0;
};

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
    bool ReturnExpiredAuctionItemsToBot;
    uint32 CyclesBetweenBuyActionMin;
    uint32 CyclesBetweenBuyAction;
    uint32 CyclesBetweenBuyActionMax;
    uint32 CyclesBetweenSellActionMin;
    uint32 CyclesBetweenSellAction;
    uint32 CyclesBetweenSellActionMax;
    uint32 MaxBuyoutPriceInCopper;
    float BuyoutVariationReducePercent;
    float BuyoutVariationAddPercent;
    float BidVariationHighReducePercent;
    float BidVariationLowReducePercent;
    bool BuyoutBelowVendorVariationAddPercentEnabled;
    float BuyoutBelowVendorVariationAddPercent;
    uint32 BuyingBotBuyCandidatesPerBuyCycleMin;
    uint32 BuyingBotBuyCandidatesPerBuyCycleMax;
    uint32 ListingExpireTimeInSecondsMin;
    uint32 ListingExpireTimeInSecondsMax;
    float BuyingBotAcceptablePriceModifier;
    bool BuyingBotAlwaysBidMaxCalculatedPrice;
    bool BuyingBotWillBidAgainstPlayers;
    std::vector<uint32> vendorItemsPrices;
    std::string AHCharactersGUIDsForQuery;
    uint32 ItemsPerCycle;
    bool DisabledItemTextFilter;
    bool DisabledRecipeProducedItemFilterEnabled;
    std::unordered_set<uint32> ItemIDsProducedByRecipes;
    std::map<uint32, std::unordered_set<uint32>> DisabledRecipeProducedItemClassSubClasses;
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
    std::vector<ListProportionNode> ItemListProportionNodesSeed;
    std::vector<ListProportionNode> ItemListProportionNodesLookup;
    std::unordered_map<uint32, uint64> ItemListProportionMultipliedItemIDs;
    std::map<uint32, std::map<uint32, std::vector<uint32>>> ItemCandidatesByItemClassAndQuality;
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
    bool UseItemSellPriceIfHigherThanPriceMinimumCenterBase;
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
    std::unordered_map<uint32, double> CachedItemDropRates;
    std::vector<std::vector<std::vector<std::vector<uint32>>>> ItemTiersByClassAndQuality;  // [Classes][Qualities][Tiers] .. [17][7][configurable]
    std::map<double, int, std::greater<double>> DropRatesToTierMap;
    std::set<uint32> AdvancedListingRuleUseDropRatesExceptionItems;
    bool AdvancedListingRuleUseDropRatesEnabled;
    bool AdvancedListingRuleUseDropRatesWeaponEnabled;
    bool AdvancedListingRuleUseDropRatesArmorEnabled;
    bool AdvancedListingRuleUseDropRatesRecipeEnabled;
    std::set<uint32> AdvancedListingRuleUseDropRatesWeaponAffectedQualities;
    std::set<uint32> AdvancedListingRuleUseDropRatesArmorAffectedQualities;
    std::set<uint32> AdvancedListingRuleUseDropRatesRecipeAffectedQualities;
    float AdvancedListingRuleUseDropRatesMinDropRate;
    std::unordered_set<uint32> QuestRewardItemIDs;

    FactionSpecificAuctionHouseConfig AllianceConfig;
    FactionSpecificAuctionHouseConfig HordeConfig;
    FactionSpecificAuctionHouseConfig NeutralConfig;

    uint32 LastBuyCycleCount;
    uint32 LastSellCycleCount;
    int ActiveListMultipleItemID;
    int RemainingListMultipleCount;

    AuctionHouseBot();

public:
    static AuctionHouseBot* instance()
    {
        static AuctionHouseBot instance;
        return &instance;
    }

    ~AuctionHouseBot();

    void Update();
    void InitializeConfiguration();
    void EmptyAuctionHouses();
    uint32 GetRandomStackValue(std::string configKeyString, uint32 defaultValue);
    uint32 GetRandomStackIncrementValue(std::string configKeyString, uint32 defaultValue);
    void SetCyclesBetweenBuyOrSell();
    void SetBuyingBotBuyCandidatesPerBuyCycle();
    void GetConfigMinAndMax(std::string config, uint32& min, uint32& max);
    void AddCharacters(std::string characterGUIDString);
    void ParseNumberListToSet(std::set<uint32>& workingItemIDSet, std::string itemString, const char* parentOperationName);
    void AddToNumberListSet(std::set<uint32>& workingItemIDSet, uint32 itemID, const char* parentOperationName);
    const char* GetQualityName(ItemQualities quality);
    const char* GetCategoryName(ItemClass category);
    uint32 GetStackSizeForItem(ItemTemplate const* itemProto) const;
    void CalculateItemValue(ItemTemplate const* itemProto, uint64& outBidPrice, uint64& outBuyoutPrice);
    void PopulateItemDropChances();
    void PopulateItemDropChancesForCategoryAndQuality(ItemClass category, std::string qualities);
    void InitializeAdvancedListingRuleUseDropRatesTiers();
    void PopulateQuestRewardItemIDs();
    bool IsItemQuestReward(uint32 itemID);
    bool IsItemCrafted(uint32 itemID);
    bool IsItemCategoryQualityInDBDropRatesConfig(ItemTemplate const* proto);
    bool IsItemEligibleForDBDropRates(ItemTemplate const* proto);
    bool HandleAdvancedListingRuleUseDropRates(ItemTemplate const*& proto);
    int GetItemDropChanceTier(double dropRate);
    float GetAdvancedPricingMultiplier(ItemTemplate const* itemProto);
    ItemTemplate const* GetProducedItemFromRecipe(ItemTemplate const* recipeItemTemplate);
    std::unordered_set<uint32> GetItemIDsProducedByRecipes();
    bool IsItemADisabledRecipeProducedClassSubclass(ItemTemplate const* itemTemplate);
    void PopulateItemCandidatesAndProportions();
    uint32 GetRandomItemIDForListing();
    void AddNewAuctions(std::vector<Player*> AHBPlayers, FactionSpecificAuctionHouseConfig* config);
    void AddNewAuctionBuyerBotBid(std::vector<Player*> AHBPlayers, FactionSpecificAuctionHouseConfig* config);
    void PopulateVendorItemsPrices();
    void CleanupExpiredAuctionItems();

    template <typename ValueType>
    void AddItemValuePairsToItemIDMap(std::unordered_map<uint32, ValueType>& workingValueToItemIDMap, std::string valueToItemIDMap);
    void AddValuesToSetByKeyMap(std::map<uint32, std::unordered_set<uint32>>& workingSetByKeyMap, std::string valuesToKeyMapString,
        uint32 wildcardLowValue, uint32 wildcardHighValue);
};

#define auctionbot AuctionHouseBot::instance()

#endif
