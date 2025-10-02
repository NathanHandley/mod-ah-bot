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

#include "ObjectMgr.h"
#include "AuctionHouseMgr.h"
#include "AuctionHouseBot.h"
#include "AuctionHouseSearcher.h"
#include "Config.h"
#include "Player.h"
#include "WorldSession.h"
#include "DatabaseEnv.h"
#include "ItemTemplate.h"
#include "SharedDefines.h"
#include "SpellMgr.h"
#include <cmath>

#include <set>
#include <unordered_map>

using namespace std;

AuctionHouseBot::AuctionHouseBot() :
    debug_Out(false),
    debug_Out_Filters(false),
    SellingBotEnabled(false),
    BuyingBotEnabled(false),
    CyclesBetweenBuyOrSellMin(1),
    CyclesBetweenBuyOrSell(1),
    CyclesBetweenBuyOrSellMax(1),
    MaxBuyoutPriceInCopper(1000000000),
    BuyoutVariationReducePercent(0.15f),
    BuyoutVariationAddPercent(0.25f),
    BidVariationHighReducePercent(0),
    BidVariationLowReducePercent(0.25f),
    BuyoutBelowVendorVariationAddPercent(0.25f),
    BuyingBotBuyCandidatesPerBuyCycleMin(1),
    BuyingBotBuyCandidatesPerBuyCycleMax(1),
    ListingExpireTimeInSecondsMin(900),
    ListingExpireTimeInSecondsMax(86400),
    BuyingBotAcceptablePriceModifier(1),
    BuyingBotAlwaysBidMaxCalculatedPrice(false),
    BuyingBotWillBidAgainstPlayers(false),
    AHCharactersGUIDsForQuery(""),
    ItemsPerCycle(75),
    DisabledItemTextFilter(true),
    ListedItemLevelRestrictedEnabled(false),
    ListedItemLevelRestrictedUseCraftedItemForCalculation(true),
    ListedItemLevelMax(999),
    ListedItemLevelMin(0),
    RandomStackRatioConsumable(1),
    RandomStackRatioContainer(1),
    RandomStackRatioWeapon(1),
    RandomStackRatioGem(1),
    RandomStackRatioArmor(1),
    RandomStackRatioReagent(1),
    RandomStackRatioProjectile(1),
    RandomStackRatioTradeGood(1),
    RandomStackRatioGeneric(1),
    RandomStackRatioRecipe(1),
    RandomStackRatioQuiver(1),
    RandomStackRatioQuest(1),
    RandomStackRatioKey(1),
    RandomStackRatioMisc(1),
    RandomStackRatioGlyph(1),
    RandomStackIncrementConsumable(1),
    RandomStackIncrementContainer(1),
    RandomStackIncrementWeapon(1),
    RandomStackIncrementGem(1),
    RandomStackIncrementArmor(1),
    RandomStackIncrementReagent(1),
    RandomStackIncrementProjectile(1),
    RandomStackIncrementTradeGood(1),
    RandomStackIncrementGeneric(1),
    RandomStackIncrementRecipe(1),
    RandomStackIncrementQuiver(1),
    RandomStackIncrementQuest(1),
    RandomStackIncrementKey(1),
    RandomStackIncrementMisc(1),
    RandomStackIncrementGlyph(1),
    PriceMultiplierCategoryConsumable(1),
    PriceMultiplierCategoryContainer(1),
    PriceMultiplierCategoryWeapon(1),
    PriceMultiplierCategoryGem(1),
    PriceMultiplierCategoryArmor(1),
    PriceMultiplierCategoryReagent(1),
    PriceMultiplierCategoryProjectile(1),
    PriceMultiplierCategoryTradeGood(1),
    PriceMultiplierCategoryGeneric(1),
    PriceMultiplierCategoryRecipe(1),
    PriceMultiplierCategoryQuiver(1),
    PriceMultiplierCategoryQuest(1),
    PriceMultiplierCategoryKey(1),
    PriceMultiplierCategoryMisc(1),
    PriceMultiplierCategoryGlyph(1),
    PriceMultiplierItemLevelCategoryConsumable(0),
    PriceMultiplierItemLevelCategoryContainer(0),
    PriceMultiplierItemLevelCategoryWeapon(0),
    PriceMultiplierItemLevelCategoryGem(0),
    PriceMultiplierItemLevelCategoryArmor(0),
    PriceMultiplierItemLevelCategoryReagent(0),
    PriceMultiplierItemLevelCategoryProjectile(0),
    PriceMultiplierItemLevelCategoryTradeGood(0),
    PriceMultiplierItemLevelCategoryGeneric(0),
    PriceMultiplierItemLevelCategoryRecipe(0),
    PriceMultiplierItemLevelCategoryQuiver(0),
    PriceMultiplierItemLevelCategoryQuest(0),
    PriceMultiplierItemLevelCategoryKey(0),
    PriceMultiplierItemLevelCategoryMisc(0),
    PriceMultiplierItemLevelCategoryGlyph(0),
    PriceMultiplierQualityPoor(1),
    PriceMultiplierQualityNormal(1),
    PriceMultiplierQualityUncommon(1),
    PriceMultiplierQualityRare(1),
    PriceMultiplierQualityEpic(1),
    PriceMultiplierQualityLegendary(1),
    PriceMultiplierQualityArtifact(1),
    PriceMultiplierQualityHeirloom(1),
    PriceMinimumCenterBaseConsumable(1),
    PriceMinimumCenterBaseContainer(1),
    PriceMinimumCenterBaseWeapon(1),
    PriceMinimumCenterBaseGem(1),
    PriceMinimumCenterBaseArmor(1),
    PriceMinimumCenterBaseReagent(1),
    PriceMinimumCenterBaseProjectile(1),
    PriceMinimumCenterBaseTradeGood(1),
    PriceMinimumCenterBaseGeneric(1),
    PriceMinimumCenterBaseRecipe(1),
    PriceMinimumCenterBaseQuiver(1),
    PriceMinimumCenterBaseQuest(1),
    PriceMinimumCenterBaseKey(1),
    PriceMinimumCenterBaseMisc(1),
    PriceMinimumCenterBaseGlyph(1),
    ListedItemIDRestrictedEnabled(false),
    ListedItemIDMin(0),
    ListedItemIDMax(200000),
    LastCycleCount(0),
    ActiveListMultipleItemID(0),
    RemainingListMultipleCount(0),
    _AHItemsCountSnapshot(0),
    _AHCategoriesCountSnapshot(),
    MaxDuplicatesCategoryArmorQualityEpic(-1),
    MaxDuplicatesCategoryWeaponQualityEpic(-1),
    MaxDuplicatesCategoryMount(-1),
    MaxDuplicatesCategoryPet(-1),
    MaxItemsCategoryPet(-1),
    MaxItemsCategoryMount(-1)
{
    AllianceConfig = FactionSpecificAuctionHouseConfig(2);
    HordeConfig = FactionSpecificAuctionHouseConfig(6);
    NeutralConfig = FactionSpecificAuctionHouseConfig(7);
}

AuctionHouseBot::~AuctionHouseBot()
{
}

uint32 AuctionHouseBot::GetStackSizeForItem(ItemTemplate const* itemProto) const
{
    // Determine the stack ratio based on class type
    if (itemProto == NULL)
        return 1;

    uint32 stackRatio = 0;
    switch (itemProto->Class)
    {
        case ITEM_CLASS_CONSUMABLE:     stackRatio = RandomStackRatioConsumable; break;
        case ITEM_CLASS_CONTAINER:      stackRatio = RandomStackRatioContainer; break;
        case ITEM_CLASS_WEAPON:         stackRatio = RandomStackRatioWeapon; break;
        case ITEM_CLASS_GEM:            stackRatio = RandomStackRatioGem; break;
        case ITEM_CLASS_REAGENT:        stackRatio = RandomStackRatioReagent; break;
        case ITEM_CLASS_ARMOR:          stackRatio = RandomStackRatioArmor; break;
        case ITEM_CLASS_PROJECTILE:     stackRatio = RandomStackRatioProjectile; break;
        case ITEM_CLASS_TRADE_GOODS:    stackRatio = RandomStackRatioTradeGood; break;
        case ITEM_CLASS_GENERIC:        stackRatio = RandomStackRatioGeneric; break;
        case ITEM_CLASS_RECIPE:         stackRatio = RandomStackRatioRecipe; break;
        case ITEM_CLASS_QUIVER:         stackRatio = RandomStackRatioQuiver; break;
        case ITEM_CLASS_QUEST:          stackRatio = RandomStackRatioQuest; break;
        case ITEM_CLASS_KEY:            stackRatio = RandomStackRatioKey; break;
        case ITEM_CLASS_MISC:           stackRatio = RandomStackRatioMisc; break;
        case ITEM_CLASS_GLYPH:          stackRatio = RandomStackRatioGlyph; break;
        default:                        stackRatio = 0; break;
    }

    uint32 stackIncrement = 1;
    switch (itemProto->Class)
    {
        case ITEM_CLASS_CONSUMABLE:     stackIncrement = RandomStackIncrementConsumable; break;
        case ITEM_CLASS_CONTAINER:      stackIncrement = RandomStackIncrementContainer; break;
        case ITEM_CLASS_WEAPON:         stackIncrement = RandomStackIncrementWeapon; break;
        case ITEM_CLASS_GEM:            stackIncrement = RandomStackIncrementGem; break;
        case ITEM_CLASS_REAGENT:        stackIncrement = RandomStackIncrementReagent; break;
        case ITEM_CLASS_ARMOR:          stackIncrement = RandomStackIncrementArmor; break;
        case ITEM_CLASS_PROJECTILE:     stackIncrement = RandomStackIncrementProjectile; break;
        case ITEM_CLASS_TRADE_GOODS:    stackIncrement = RandomStackIncrementTradeGood; break;
        case ITEM_CLASS_GENERIC:        stackIncrement = RandomStackIncrementGeneric; break;
        case ITEM_CLASS_RECIPE:         stackIncrement = RandomStackIncrementRecipe; break;
        case ITEM_CLASS_QUIVER:         stackIncrement = RandomStackIncrementQuiver; break;
        case ITEM_CLASS_QUEST:          stackIncrement = RandomStackIncrementQuest; break;
        case ITEM_CLASS_KEY:            stackIncrement = RandomStackIncrementKey; break;
        case ITEM_CLASS_MISC:           stackIncrement = RandomStackIncrementMisc; break;
        case ITEM_CLASS_GLYPH:          stackIncrement = RandomStackIncrementGlyph; break;
        default:                        stackIncrement = 1; break;
    }

    if (stackRatio > urand(0, 99))
    {
        uint32 numOfPossibleStackIncrements = (uint32)std::ceil((float)itemProto->GetMaxStackSize() / (float)stackIncrement);
        uint32 numOfStacks = urand(1, numOfPossibleStackIncrements);
        uint32 randomStackSize = numOfStacks * stackIncrement;
        if (randomStackSize > itemProto->GetMaxStackSize())
            return itemProto->GetMaxStackSize();
        else
            return randomStackSize;
    }
    else
        return 1;
}

void AuctionHouseBot::CalculateItemValue(ItemTemplate const* itemProto, uint64& outBidPrice, uint64& outBuyoutPrice)
{
    // Start with a buyout price related to the sell price
    outBuyoutPrice = itemProto->SellPrice;

    // Get the price multipliers
    float classPriceMultiplier = 1;
    switch (itemProto->Class)
    {
    case ITEM_CLASS_CONSUMABLE:     classPriceMultiplier = PriceMultiplierCategoryConsumable; break;
    case ITEM_CLASS_CONTAINER:      classPriceMultiplier = PriceMultiplierCategoryContainer; break;
    case ITEM_CLASS_WEAPON:         classPriceMultiplier = PriceMultiplierCategoryWeapon; break;
    case ITEM_CLASS_GEM:            classPriceMultiplier = PriceMultiplierCategoryGem; break;
    case ITEM_CLASS_REAGENT:        classPriceMultiplier = PriceMultiplierCategoryReagent; break;
    case ITEM_CLASS_ARMOR:          classPriceMultiplier = PriceMultiplierCategoryArmor; break;
    case ITEM_CLASS_PROJECTILE:     classPriceMultiplier = PriceMultiplierCategoryProjectile; break;
    case ITEM_CLASS_TRADE_GOODS:    classPriceMultiplier = PriceMultiplierCategoryTradeGood; break;
    case ITEM_CLASS_GENERIC:        classPriceMultiplier = PriceMultiplierCategoryGeneric; break;
    case ITEM_CLASS_RECIPE:         classPriceMultiplier = PriceMultiplierCategoryRecipe; break;
    case ITEM_CLASS_QUIVER:         classPriceMultiplier = PriceMultiplierCategoryQuiver; break;
    case ITEM_CLASS_QUEST:          classPriceMultiplier = PriceMultiplierCategoryQuest; break;
    case ITEM_CLASS_KEY:            classPriceMultiplier = PriceMultiplierCategoryKey; break;
    case ITEM_CLASS_MISC:           classPriceMultiplier = PriceMultiplierCategoryMisc; break;
    case ITEM_CLASS_GLYPH:          classPriceMultiplier = PriceMultiplierCategoryGlyph; break;
    default:                        break;
    }

    float qualityPriceMultplier = 1;
    switch (itemProto->Quality)
    {
    case ITEM_QUALITY_POOR:         qualityPriceMultplier = PriceMultiplierQualityPoor; break;
    case ITEM_QUALITY_NORMAL:       qualityPriceMultplier = PriceMultiplierQualityNormal; break;
    case ITEM_QUALITY_UNCOMMON:     qualityPriceMultplier = PriceMultiplierQualityUncommon; break;
    case ITEM_QUALITY_RARE:         qualityPriceMultplier = PriceMultiplierQualityRare; break;
    case ITEM_QUALITY_EPIC:         qualityPriceMultplier = PriceMultiplierQualityEpic; break;
    case ITEM_QUALITY_LEGENDARY:    qualityPriceMultplier = PriceMultiplierQualityLegendary; break;
    case ITEM_QUALITY_ARTIFACT:     qualityPriceMultplier = PriceMultiplierQualityArtifact; break;
    case ITEM_QUALITY_HEIRLOOM:     qualityPriceMultplier = PriceMultiplierQualityHeirloom; break;
    default: break;
    }

    float classQualityPriceMultiplier = PriceMultiplierCategoryQuality[itemProto->Class][itemProto->Quality];

    float advancedPricingMultiplier = GetAdvancedPricingMultiplier(itemProto);

    // Grab the minimum prices
    uint64 PriceMinimumCenterBase = 1000;
    auto it = PriceMinimumCenterBaseOverridesByItemID.find(itemProto->ItemId);
    if (it != PriceMinimumCenterBaseOverridesByItemID.end())
        PriceMinimumCenterBase = it->second;
    else
    {
        switch (itemProto->Class)
        {
        case ITEM_CLASS_CONSUMABLE:     PriceMinimumCenterBase = PriceMinimumCenterBaseConsumable; break;
        case ITEM_CLASS_CONTAINER:      PriceMinimumCenterBase = PriceMinimumCenterBaseContainer; break;
        case ITEM_CLASS_WEAPON:         PriceMinimumCenterBase = PriceMinimumCenterBaseWeapon; break;
        case ITEM_CLASS_GEM:            PriceMinimumCenterBase = PriceMinimumCenterBaseGem; break;
        case ITEM_CLASS_REAGENT:        PriceMinimumCenterBase = PriceMinimumCenterBaseReagent; break;
        case ITEM_CLASS_ARMOR:          PriceMinimumCenterBase = PriceMinimumCenterBaseArmor; break;
        case ITEM_CLASS_PROJECTILE:     PriceMinimumCenterBase = PriceMinimumCenterBaseProjectile; break;
        case ITEM_CLASS_TRADE_GOODS:    PriceMinimumCenterBase = PriceMinimumCenterBaseTradeGood; break;
        case ITEM_CLASS_GENERIC:        PriceMinimumCenterBase = PriceMinimumCenterBaseGeneric; break;
        case ITEM_CLASS_RECIPE:         PriceMinimumCenterBase = PriceMinimumCenterBaseRecipe; break;
        case ITEM_CLASS_QUIVER:         PriceMinimumCenterBase = PriceMinimumCenterBaseQuiver; break;
        case ITEM_CLASS_QUEST:          PriceMinimumCenterBase = PriceMinimumCenterBaseQuest; break;
        case ITEM_CLASS_KEY:            PriceMinimumCenterBase = PriceMinimumCenterBaseKey; break;
        case ITEM_CLASS_MISC:           PriceMinimumCenterBase = PriceMinimumCenterBaseMisc; break;
        case ITEM_CLASS_GLYPH:          PriceMinimumCenterBase = PriceMinimumCenterBaseGlyph; break;
        default:                        break;
        }
    }

    // Set the minimum price
    if (outBuyoutPrice < PriceMinimumCenterBase)
        outBuyoutPrice = urand(PriceMinimumCenterBase * (1.0f - BuyoutVariationReducePercent), PriceMinimumCenterBase * (1.0f + BuyoutVariationAddPercent));
    else
        outBuyoutPrice = urand(outBuyoutPrice * (1.0f - BuyoutVariationReducePercent), outBuyoutPrice * (1.0f + BuyoutVariationAddPercent));

    // Ensure no multipliers are zero or negative
    if (classPriceMultiplier <= 0.0f)
        classPriceMultiplier = 1.0f;
    if (qualityPriceMultplier <= 0.0f)
        qualityPriceMultplier = 1.0f;
    if (classQualityPriceMultiplier <= 0.0f)
        classQualityPriceMultiplier = 1.0f;
    if (advancedPricingMultiplier <= 0.0f)
        advancedPricingMultiplier = 1.0f;

    // Grab any item level price multipliers
    float itemLevelPriceMultplier = 0.0f;
    switch (itemProto->Class)
    {
        case ITEM_CLASS_CONSUMABLE:     itemLevelPriceMultplier = PriceMultiplierItemLevelCategoryConsumable; break;
        case ITEM_CLASS_CONTAINER:      itemLevelPriceMultplier = PriceMultiplierItemLevelCategoryContainer; break;
        case ITEM_CLASS_WEAPON:         itemLevelPriceMultplier = PriceMultiplierItemLevelCategoryWeapon; break;
        case ITEM_CLASS_GEM:            itemLevelPriceMultplier = PriceMultiplierItemLevelCategoryGem; break;
        case ITEM_CLASS_REAGENT:        itemLevelPriceMultplier = PriceMultiplierItemLevelCategoryReagent; break;
        case ITEM_CLASS_ARMOR:          itemLevelPriceMultplier = PriceMultiplierItemLevelCategoryArmor; break;
        case ITEM_CLASS_PROJECTILE:     itemLevelPriceMultplier = PriceMultiplierItemLevelCategoryProjectile; break;
        case ITEM_CLASS_TRADE_GOODS:    itemLevelPriceMultplier = PriceMultiplierItemLevelCategoryTradeGood; break;
        case ITEM_CLASS_GENERIC:        itemLevelPriceMultplier = PriceMultiplierItemLevelCategoryGeneric; break;
        case ITEM_CLASS_RECIPE:         itemLevelPriceMultplier = PriceMultiplierItemLevelCategoryRecipe; break;
        case ITEM_CLASS_QUIVER:         itemLevelPriceMultplier = PriceMultiplierItemLevelCategoryQuiver; break;
        case ITEM_CLASS_QUEST:          itemLevelPriceMultplier = PriceMultiplierItemLevelCategoryQuest; break;
        case ITEM_CLASS_KEY:            itemLevelPriceMultplier = PriceMultiplierItemLevelCategoryKey; break;
        case ITEM_CLASS_MISC:           itemLevelPriceMultplier = PriceMultiplierItemLevelCategoryMisc; break;
        case ITEM_CLASS_GLYPH:          itemLevelPriceMultplier = PriceMultiplierItemLevelCategoryGlyph; break;
        default:                        break;
    }

    // Multiply the price based on multipliers
    outBuyoutPrice *= qualityPriceMultplier;
    outBuyoutPrice *= classPriceMultiplier;
    outBuyoutPrice *= classQualityPriceMultiplier;
    outBuyoutPrice *= static_cast<float>(advancedPricingMultiplier);

    // Only apply item level multiplier if set, and no advanced pricing has been enabled
    if (itemLevelPriceMultplier > 0.0f && itemProto->ItemLevel > 0 && advancedPricingMultiplier == 1.0f)
        outBuyoutPrice *= itemProto->ItemLevel * itemLevelPriceMultplier;

    // If a vendor sells this item, make the price at least that high
    if (itemProto->SellPrice > outBuyoutPrice)
        outBuyoutPrice = itemProto->SellPrice;

    // Avoid price overflows
    if (outBuyoutPrice > MaxBuyoutPriceInCopper)
        outBuyoutPrice = MaxBuyoutPriceInCopper;

    // Calculate a bid price based on a variance against buyout price
    float sellVarianceBidPriceTopPercent = 1.0f - BidVariationHighReducePercent;
    float sellVarianceBidPriceBottomPercent = 1.0f - BidVariationLowReducePercent;
    outBidPrice = urand(sellVarianceBidPriceBottomPercent * outBuyoutPrice, sellVarianceBidPriceTopPercent * outBuyoutPrice);

    // If variance brought price below sell price, bring it back up to avoid making money off vendoring AH newItemsToListCount
    if (outBuyoutPrice < itemProto->SellPrice)
    {
        float minLowPriceAddVariancePercent = 1.0f + BuyoutBelowVendorVariationAddPercent;
        outBuyoutPrice = urand(itemProto->SellPrice, minLowPriceAddVariancePercent * itemProto->SellPrice);
    }

    // Bid price can never be below sell price
    if (outBidPrice < itemProto->SellPrice)
        outBidPrice = itemProto->SellPrice;
}

float AuctionHouseBot::GetAdvancedPricingMultiplier(ItemTemplate const* itemProto)
{
    /* "ADVANCED" SUBCLASS PRICE MULTIPLIER FORMULA NOTES

      1. multiplierHelper = log(1 + b * ItemLevel)
      2. 
            clothMultiplierHelper ^ p
        ---------------------------------   +   c * (clothMultiplierHelper ^ r)  -  d
          1 + a * clothMultiplierHelper

      Variables:
        b  // Influences the growth rate at low item levels
        p  // Exponent for first term (controls curve steepness)
        a  // Coefficient in denominator (dampens growth, especially at high item levels)
        c  // Scaling coefficient for second term
        r  // Exponent for second term (adds nonlinear boost)
        d  // Constant shift (adjusts baseline multiplier). This becomes apparent if you graph the equation - it shifts the entire curve down.
     
      Notes:
      - This formula produces a multiplier that grows logarithmically with ItemLevel (uses natural log, not base10)
      - The first term (before '+') heavily influences low item levels, the second term adds some fine-tuning for higher levels.
      - The subtraction of 'r' can help ensure low-level newItemsToListCount don't get inflated excessively. Sometimes it isn't necessary
    */

    // Try to approximate real world prices for Trade Goods based on subclass and item level
    double advancedPricingMultiplier = 1.0f;
    if (itemProto->Class == ITEM_CLASS_CONSUMABLE)
    {
        switch (itemProto->SubClass)
        {
            case ITEM_SUBCLASS_POTION:
            {
                if (!AdvancedPricingConsumablePotionEnabled) 
                    break;
                double potionMultiplierHelper = std::log(1.0 + (0.08 * itemProto->ItemLevel));
                advancedPricingMultiplier = ((std::pow(potionMultiplierHelper,3.0)) / (1 + (4.0 * potionMultiplierHelper))) + (std::pow(potionMultiplierHelper,2.5));
                break;
            }
            case ITEM_SUBCLASS_ELIXIR:
            {
                if (!AdvancedPricingConsumableElixirEnabled) 
                    break;
                double elixirMultiplierHelper = std::log(1.0 + (1.6 * itemProto->ItemLevel));
                advancedPricingMultiplier = ((std::pow(elixirMultiplierHelper,3.1)) / (1 + (5.0 * elixirMultiplierHelper))) + (0.05 * std::pow(elixirMultiplierHelper,3.2)) - 1.0;
                break;
            }
            case ITEM_SUBCLASS_FLASK:
            {
                if (!AdvancedPricingConsumableFlaskEnabled)
                    break;
                // Use logarithmic scaling to compress large differences in vendorSellPrice to a range of ~22g-25g
                // advPricingMultiplier = LowTargetRange + (UpperTargetRange - LowTargetRange) * ( ln(vendorSellPrice) - ln(minVendorPrice) ) / ( ln(maxVendorPrice) - ln(minVendorPrice) ) / vendorSellPrice
                advancedPricingMultiplier = (220000 + (250000-220000) * (std::log(itemProto->SellPrice) - std::log(1250)) / (std::log(10000) - std::log(1250))) / itemProto->SellPrice;
            }
            default:
                break;
        }
    }
    else if (itemProto->Class == ITEM_CLASS_GEM && AdvancedPricingGemEnabled)
    {
        // No switch for subclass needed since Gem subclass represents gem color
        double gemMultiplierHelper = std::log(1.0 + (0.05 * itemProto->ItemLevel));
        advancedPricingMultiplier = ((std::pow(gemMultiplierHelper,1.0)) / (1 + (10.0 * gemMultiplierHelper))) + (std::pow(gemMultiplierHelper,3.0));
    }
    else if (itemProto->Class == ITEM_CLASS_TRADE_GOODS) 
    {
        switch (itemProto->SubClass)
        {
            case ITEM_SUBCLASS_CLOTH:
            {
                if (!AdvancedPricingTradeGoodClothEnabled) 
                    break;
                double clothMultiplierHelper = std::log(1.0 + (itemProto->ItemLevel));
                advancedPricingMultiplier = ((std::pow(clothMultiplierHelper,2.0)) / (1 + (0.8 * clothMultiplierHelper))) + (0.001 * std::pow(clothMultiplierHelper,3.5)) - 0.3;
                break;
            }
            case ITEM_SUBCLASS_HERB:
            {
                if (!AdvancedPricingTradeGoodHerbEnabled) 
                    break;
                double herbMultiplierHelper = std::log(1.0 + (5.0 * itemProto->ItemLevel));
                advancedPricingMultiplier = (std::pow(herbMultiplierHelper,3.0) / (1 + (1.8 * herbMultiplierHelper))) - 4.2; 
                break;
            }
            case ITEM_SUBCLASS_METAL_STONE:
            {
                if (!AdvancedPricingTradeGoodMetalStoneEnabled) 
                    break;
                double metalMultiplierHelper = std::log(1.0 + (75.0 * itemProto->ItemLevel));
                advancedPricingMultiplier =  ((std::pow(metalMultiplierHelper,3.0)) / (1 + (7.0 * metalMultiplierHelper))) + (0.001 * std::pow(metalMultiplierHelper,3.5)) - 5.2;
                break;
            }
            case ITEM_SUBCLASS_LEATHER:
            {
                if (!AdvancedPricingTradeGoodLeatherEnabled) 
                    break;
                double leatherMultiplierHelper = std::log(1.0 + (0.25 * itemProto->ItemLevel));
                advancedPricingMultiplier = ((std::pow(leatherMultiplierHelper,0.15)) / (1 + (2.0 * leatherMultiplierHelper))) + (0.4 * std::pow(leatherMultiplierHelper,3.0)) - 0.2;
                break;
            }
            case ITEM_SUBCLASS_ENCHANTING:
            {
                if (!AdvancedPricingTradeGoodEnchantingEnabled) 
                    break;
                double enchantingMultiplierHelper = std::log(1.0 + (0.25 * itemProto->ItemLevel));
                advancedPricingMultiplier = ((std::pow(enchantingMultiplierHelper,0.15)) / (1 + (2.0 * enchantingMultiplierHelper))) + (0.4 * std::pow(enchantingMultiplierHelper,3.0)) - 0.2; 
                break;
            }
            case ITEM_SUBCLASS_ELEMENTAL:
            {
                if (!AdvancedPricingTradeGoodElementalEnabled) 
                    break;
                advancedPricingMultiplier = 85 - (itemProto->ItemLevel / 0.97);
                break;
            }
            case ITEM_SUBCLASS_MEAT:
            {
                if (!AdvancedPricingTradeGoodMeatEnabled)
                    break;
                double meatMultiplierHelper = std::log(1.0 + (0.5 * itemProto->ItemLevel));
                advancedPricingMultiplier = ((std::pow(meatMultiplierHelper,3.2)) / (1 + (2.0 * meatMultiplierHelper))) + (0.05 * std::pow(meatMultiplierHelper,3.2)) - 0.1; 
                break;
            }
            default:
                break;
        }
    }
    else if (itemProto->Class == ITEM_CLASS_MISC)
    {
        switch (itemProto->SubClass)
        {
            // Tuned for pricing lockboxes
            case ITEM_SUBCLASS_JUNK:
            {
                if (!AdvancedPricingMiscJunkEnabled) 
                    break;
                double miscMultiplierHelper = std::log(1.0 + (0.12 * itemProto->ItemLevel));
                advancedPricingMultiplier = (std::pow(miscMultiplierHelper,3.2) / (1 + miscMultiplierHelper)); 
                break;
            }
            case ITEM_SUBCLASS_JUNK_MOUNT:
            {
                if (!AdvancedPricingMiscMountEnabled) 
                    break;
                switch (itemProto->Quality)
                {
                    case ITEM_QUALITY_POOR:         advancedPricingMultiplier = PriceMultiplierCategoryMountQualityPoor;      break;
                    case ITEM_QUALITY_NORMAL:       advancedPricingMultiplier = PriceMultiplierCategoryMountQualityNormal;    break;
                    case ITEM_QUALITY_UNCOMMON:     advancedPricingMultiplier = PriceMultiplierCategoryMountQualityUncommon;  break;
                    case ITEM_QUALITY_RARE:         advancedPricingMultiplier = PriceMultiplierCategoryMountQualityRare;      break;
                    case ITEM_QUALITY_EPIC:         advancedPricingMultiplier = PriceMultiplierCategoryMountQualityEpic;      break;
                    case ITEM_QUALITY_LEGENDARY:    advancedPricingMultiplier = PriceMultiplierCategoryMountQualityLegendary; break;
                    case ITEM_QUALITY_ARTIFACT:     advancedPricingMultiplier = PriceMultiplierCategoryMountQualityArtifact;  break;
                    case ITEM_QUALITY_HEIRLOOM:     advancedPricingMultiplier = PriceMultiplierCategoryMountQualityHeirloom;  break;
                    default: break;
                }
                break;
            }
            default: break;
        }
    }

    return static_cast<float>(advancedPricingMultiplier);
}

ItemTemplate const* AuctionHouseBot::GetProducedItemFromRecipe(ItemTemplate const* recipeItemTemplate)
{
    if (!recipeItemTemplate)
        return nullptr;
    for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
    {
        if (recipeItemTemplate->Spells[i].SpellId)
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(recipeItemTemplate->Spells[i].SpellId);
            if (!spellInfo)
                continue;

            for (auto const& effect : spellInfo->Effects)
            {
                if (effect.Effect == SPELL_EFFECT_CREATE_ITEM)
                {
                    uint32 createdItemId = effect.ItemType;
                    if (createdItemId)
                    {
                        ItemTemplate const* producedItem = sObjectMgr->GetItemTemplate(createdItemId);
                        if (producedItem)
                            return producedItem;
                    }
                }
            }
        }
    }

    return nullptr;
}

void AuctionHouseBot::PopulateItemCandidatesAndProportions()
{
    // Clear old list and rebuild it
    ItemCandidatesByItemClassAndQuality.clear();

    // Item include exceptions
    set<uint32> includeItemIDExecptions;
    includeItemIDExecptions.insert(11732);
    includeItemIDExecptions.insert(11733);
    includeItemIDExecptions.insert(11734);
    includeItemIDExecptions.insert(11736);
    includeItemIDExecptions.insert(11737);
    includeItemIDExecptions.insert(18332);
    includeItemIDExecptions.insert(18333);
    includeItemIDExecptions.insert(18334);
    includeItemIDExecptions.insert(18335);

    // Fill candidate item templates
    ItemTemplateContainer const* its = sObjectMgr->GetItemTemplateStore();
    for (ItemTemplateContainer::const_iterator itr = its->begin(); itr != its->end(); ++itr)
    {
        // Never store curBlock zero
        if (itr->second.ItemId == 0)
            continue;

        // If there is an iLevel exception, honor it
        if (ListedItemLevelRestrictedEnabled == true)
        {
            // Only test if it's not an exception
            if (ListedItemLevelExceptionItems.find(itr->second.ItemId) == ListedItemLevelExceptionItems.end())
            {
                uint32 itemLevelToCompare = itr->second.ItemLevel;

                // Recipes might need to consider produced newItemsToListCount
                if (ListedItemLevelRestrictedUseCraftedItemForCalculation == true && itr->second.Class == ITEM_CLASS_RECIPE)
                {
                    ItemTemplate const* producedItemTemplate = GetProducedItemFromRecipe(&itr->second);
                    if (producedItemTemplate != nullptr)
                    {
                        if (debug_Out_Filters)
                            LOG_ERROR("module", "AuctionHouseBot: Using item {} for recipe {} for item level comparison since ListedItemLevelRestrictedUseCraftedItemForCalculation is true", producedItemTemplate->ItemId, itr->second.ItemId);
                        itemLevelToCompare = producedItemTemplate->ItemLevel;
                    }
                }

                if (itemLevelToCompare < ListedItemLevelMin)
                {
                    if (debug_Out_Filters)
                        LOG_ERROR("module", "AuctionHouseBot: Item {} disabled since item level is lower than ListedItemLevelRestrict.MinItemLevel", itr->second.ItemId);
                    continue;
                }
                if (itemLevelToCompare > ListedItemLevelMax)
                {
                    if (debug_Out_Filters)
                        LOG_ERROR("module", "AuctionHouseBot: Item {} disabled since item level is higher than ListedItemLevelRestrict.MaxItemLevel", itr->second.ItemId);
                    continue;
                }
            }
        }

        // If there is an item ID exception, honor it
        if (ListedItemIDRestrictedEnabled == true)
        {
            // Only test if it's not an exception
            if (ListedItemIDExceptionItems.find(itr->second.ItemId) == ListedItemIDExceptionItems.end())
            {
                if (itr->second.ItemId < ListedItemIDMin)
                {
                    if (debug_Out_Filters)
                        LOG_ERROR("module", "AuctionHouseBot: Item {} disabled since item id is lower than ListedItemLevelRestrict.MinItemID", itr->second.ItemId);
                    continue;
                }
                if (itr->second.ItemId > ListedItemIDMax)
                {
                    if (debug_Out_Filters)
                        LOG_ERROR("module", "AuctionHouseBot: Item {} disabled since item id is higher than ListedItemLevelRestrict.MaxItemID", itr->second.ItemId);
                    continue;
                }
            }
        }

        // Disabled newItemsToListCount by Id
        if (DisabledItems.find(itr->second.ItemId) != DisabledItems.end())
        {
            if (debug_Out_Filters)
                LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Configured by DisabledItemIDs and DisabledCraftedItemIDs)", itr->second.ItemId);
            continue;
        }

        // These newItemsToListCount should be included and would otherwise be skipped due to conditions below
        if (includeItemIDExecptions.find(itr->second.ItemId) != includeItemIDExecptions.end())
        {
            ItemCandidatesByItemClassAndQuality[itr->second.Class][itr->second.Quality].push_back(itr->second.ItemId);
            continue;
        }

        // Skip any BOP newItemsToListCount
        if (itr->second.Bonding == BIND_WHEN_PICKED_UP || itr->second.Bonding == BIND_QUEST_ITEM)
        {
            if (debug_Out_Filters)
                LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (BOP or BQI)", itr->second.ItemId);
            continue;
        }

        // Restrict quality to anything under 7 (artifact and below) or above poor
        if (itr->second.Quality == 0 || itr->second.Quality > 6)
            continue;

        // Disable conjured newItemsToListCount
        if (itr->second.IsConjuredConsumable())
        {
            if (debug_Out_Filters)
                LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Conjured Consumable)", itr->second.ItemId);
            continue;
        }

        // Disable money
        if (itr->second.Class == ITEM_CLASS_MONEY)
        {
            if (debug_Out_Filters)
                LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Money)", itr->second.ItemId);
            continue;
        }

        // Disable moneyloot
        if (itr->second.MinMoneyLoot > 0)
        {
            if (debug_Out_Filters)
                LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (MoneyLoot)", itr->second.ItemId);
            continue;
        }

        // Disable newItemsToListCount with duration
        if (itr->second.Duration > 0)
        {
            if (debug_Out_Filters)
                LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Has a Duration)", itr->second.ItemId);
            continue;
        }

        // Disable containers with zero slots
        if (itr->second.Class == ITEM_CLASS_CONTAINER && itr->second.ContainerSlots == 0)
        {
            if (debug_Out_Filters)
                LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Container with no slots)", itr->second.ItemId);
            continue;
        }

        // Disable normal class 'book' recipes, since they are junk
        if (itr->second.Class == ITEM_CLASS_RECIPE && itr->second.SubClass == ITEM_SUBCLASS_BOOK && itr->second.Quality <= ITEM_QUALITY_NORMAL)
        {
            if (debug_Out_Filters)
                LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Normal or lower recipe book)", itr->second.ItemId);
            continue;
        }

        // Disable anything with the string literal of a testing or depricated item
        if (DisabledItemTextFilter == true && 
            (itr->second.Name1.find("Test ") != std::string::npos ||
            itr->second.Name1.find("TEST") != std::string::npos ||
            itr->second.Name1.find("Deprecated") != std::string::npos ||
            itr->second.Name1.find("Depricated") != std::string::npos ||
            itr->second.Name1.find(" Epic ") != std::string::npos ||
            itr->second.Name1.find("]") != std::string::npos ||            
            itr->second.Name1.find("D'Sak") != std::string::npos ||
            itr->second.Name1.find("(") != std::string::npos ||
            itr->second.Name1.find("OLD") != std::string::npos))
        {
            if (debug_Out_Filters)
                LOG_ERROR("module", "AuctionHouseBot: Item {} disabled item with a temp or unused item name", itr->second.ItemId);
            continue;
        }

        // Disabled crafted gems that start with "Perfect"
        if (itr->second.Class == ITEM_CLASS_GEM && itr->second.Name1.find("Perfect ") != std::string::npos)
        {
            if (debug_Out_Filters)
                LOG_ERROR("module", "AuctionHouseBot: Item {} disabled as it's a perfect crafted gem", itr->second.ItemId);
            continue;
        }

        // Disable all newItemsToListCount that have neither a sell or a buy price, with exception of item enhancements and trade goods
        bool isEnchantingTradeGood = (itr->second.Class == ITEM_CLASS_TRADE_GOODS && itr->second.SubClass == ITEM_SUBCLASS_ENCHANTING);
        bool isItemEnhancement = (itr->second.Class == ITEM_CLASS_CONSUMABLE && itr->second.SubClass == ITEM_SUBCLASS_ITEM_ENHANCEMENT);
        bool hasNoPrice = (itr->second.SellPrice == 0 && itr->second.BuyPrice == 0);
        if (hasNoPrice == true && isItemEnhancement == false && isEnchantingTradeGood == false)
        {
            if (debug_Out_Filters)
                LOG_ERROR("module", "AuctionHouseBot: Item {} disabled misc item", itr->second.ItemId);
            continue;
        }

        //  Disable common weapons
        if (itr->second.Quality == ITEM_QUALITY_NORMAL && itr->second.Class == ITEM_CLASS_WEAPON)
        {
            if (debug_Out_Filters)
                LOG_ERROR("module", "AuctionHouseBot: Item {} disabled common weapon", itr->second.ItemId);
            continue;
        }

        // Disable common armor
        if (itr->second.Quality == ITEM_QUALITY_NORMAL && itr->second.Class == ITEM_CLASS_ARMOR)
        {
            if (debug_Out_Filters)
                LOG_ERROR("module", "AuctionHouseBot: Item {} disabled common non-misc armor", itr->second.ItemId);
            continue;
        }

        // Store the item ID
        ItemCandidatesByItemClassAndQuality[itr->second.Class][itr->second.Quality].push_back(itr->second.ItemId);
    }

    // Show any debugging information
    if (debug_Out)
    {
        LOG_INFO("module", "AHBot Candidate item counts by item category (class) and quality after appyling filters:");
        for (auto& itemCandidateQualityGroupInClass : ItemCandidatesByItemClassAndQuality)
            for (auto& itemCandidateInQualityGroup : itemCandidateQualityGroupInClass.second)
            {
                uint32 classID = itemCandidateQualityGroupInClass.first;
                uint32 qualityID = itemCandidateInQualityGroup.first;
                size_t elementCount = itemCandidateInQualityGroup.second.size();
                LOG_INFO("module", "Item count in class {} quality {} is {}", classID, qualityID, elementCount);
            }
    }

    // Fill list proportions
    ItemListProportionNodesLookup.clear();
    for (uint32 i = 0; i < ItemListProportionNodesSeed.size(); ++i)
    {
        ListProportionNode curNode = ItemListProportionNodesSeed[i];
        if (ItemCandidatesByItemClassAndQuality[curNode.ItemClassID][curNode.ItemQualityID].size() > 0)
        {
            for (uint32 j = 0; j < curNode.Proportion; j++)
                ItemListProportionNodesLookup.push_back(curNode);
        }
    }
}

uint32 AuctionHouseBot::GetRandomItemIDForListing()
{
    // Start with a listing proportion
    if (ItemListProportionNodesLookup.size() == 0)
    {
        LOG_ERROR("module", "No valid list proportion for new listing could be found (ItemListProportionNodesLookup was empty)");
        SellingBotEnabled = false;
        return 0;
    }
    ListProportionNode listProportionNode = ItemListProportionNodesLookup[urand(0, ItemListProportionNodesLookup.size() - 1)];

    // Grab an item
    size_t numOfValidItemsInGroup = ItemCandidatesByItemClassAndQuality[listProportionNode.ItemClassID][listProportionNode.ItemQualityID].size();
    if (numOfValidItemsInGroup == 0)
    {
        LOG_ERROR("module", "Unable to find a candidate item with Category (class) {} and Quality {}", listProportionNode.ItemClassID, listProportionNode.ItemQualityID);
        return 0;
    }
    return ItemCandidatesByItemClassAndQuality[listProportionNode.ItemClassID][listProportionNode.ItemQualityID][urand(0, numOfValidItemsInGroup-1)];
}

void AuctionHouseBot::AddNewAuctions(std::vector<Player*> AHBPlayers, FactionSpecificAuctionHouseConfig *config)
{
    if (!SellingBotEnabled)
    {
        if (debug_Out)
            LOG_INFO("module", "AHSeller: Disabled");
        return;
    }

    uint32 minItems = config->GetMinItems();
    uint32 maxItems = config->GetMaxItems();

    if (maxItems == 0)
        return;

    AuctionHouseEntry const* ahEntry = sAuctionMgr->GetAuctionHouseEntryFromFactionTemplate(config->GetAHFID());
    if (!ahEntry)
    {
        return;
    }
    AuctionHouseObject* auctionHouse =  sAuctionMgr->GetAuctionsMap(config->GetAHFID());
    if (!auctionHouse)
    {
        return;
    }

    uint32 currentAuctionItemListCount = auctionHouse->Getcount();
    if (currentAuctionItemListCount >= minItems)
    {
        if (debug_Out)
            LOG_INFO("module", "AHSeller: Auctions above minimum, so no auctions will be listed this cycle");
        return;
    }

    if (currentAuctionItemListCount >= maxItems)
    {
        if (debug_Out)
            LOG_INFO("module", "AHSeller: Auctions at or above maximum, so no auctions will be listed this cycle");
        return;
    }

    uint32 newItemsToListCount = 0;
    if ((maxItems - currentAuctionItemListCount) >= ItemsPerCycle)
        newItemsToListCount = ItemsPerCycle;
    else
        newItemsToListCount = (maxItems - currentAuctionItemListCount);

    if (debug_Out)
        LOG_INFO("module", "AHSeller: Adding {} Auctions", newItemsToListCount);

    if (debug_Out)
        LOG_INFO("module", "AHSeller: Current house id is {}", config->GetAHID());

    uint32 itemsGenerated = 0;
    _AHItemsCountSnapshot.clear();
    for (auto& row : _AHCategoriesCountSnapshot)
        row.fill(0);
    PopulateAHItemsAndCategoriesCountSnapshot(config->GetAHID());

    // only insert a few at a time, so as not to peg the processor
    for (uint32 cnt = 1; cnt <= newItemsToListCount; cnt++)
    {
        auto trans = CharacterDatabase.BeginTransaction();
        uint32 batchCount = 0;

        while (batchCount < 500 && itemsGenerated < newItemsToListCount)
        {
            // Either generate a new item ID to list, or grab from the remaining list
            uint32 itemID;
            if (ActiveListMultipleItemID != 0)
            {
                itemID = ActiveListMultipleItemID;
                RemainingListMultipleCount--;
                if (RemainingListMultipleCount <= 0)
                    ActiveListMultipleItemID = 0;
            }
            else
            {
                itemID = GetRandomItemIDForListing();
                if (itemID != 0 && ItemListProportionMultipliedItemIDs.find(itemID) != ItemListProportionMultipliedItemIDs.end() &&
                    ItemListProportionMultipliedItemIDs[itemID] > 1)
                {
                    ActiveListMultipleItemID = itemID;
                    RemainingListMultipleCount = ItemListProportionMultipliedItemIDs[itemID] - 1;
                    if (debug_Out)
                        LOG_INFO("module", "AHSeller: Is listing item ID {} which is configured for {} multiples from ListMultipliedItemIDs", itemID, ItemListProportionMultipliedItemIDs[itemID]);
                }
            }

            // Prevent invalid IDs
            if (itemID == 0)
            {
                if (debug_Out)
                    LOG_ERROR("module", "AHSeller: Item::CreateItem() failed as the ItemID is 0");
                continue;
            }

            ItemTemplate const* prototype = sObjectMgr->GetItemTemplate(itemID);
            if (prototype == NULL)
            {
                if (debug_Out)
                    LOG_ERROR("module", "AHSeller: prototype == NULL");
                continue;
            }

            if (IsItemOverpopulated(prototype))
            {
                if (debug_Out)
                    LOG_INFO("module", "Item {} {} is overpopulated in the AH. Skipping...", prototype->ItemId, prototype->Name1);
                ActiveListMultipleItemID = 0;
                continue;
            }

            if (IsItemCategoryOverpopulated(prototype))
            {
                if (debug_Out)
                    LOG_INFO("module", "Class {} SubClass {} is overpopulated in the AH. Skipping...", GetCategoryName((ItemClass)prototype->Class), prototype->SubClass);
                ActiveListMultipleItemID = 0;
                continue;
            }

            Player* AHBplayer = AHBPlayers[urand(0, AHBPlayers.size() - 1)];

            Item* item = Item::CreateItem(itemID, 1, AHBplayer);
            if (item == NULL)
            {
                if (debug_Out)
                    LOG_ERROR("module", "AHSeller: Item::CreateItem() returned NULL");
                break;
            }
            item->AddToUpdateQueueOf(AHBplayer);

            uint32 randomPropertyId = Item::GenerateItemRandomPropertyId(itemID);
            if (randomPropertyId != 0)
                item->SetItemRandomProperties(randomPropertyId);

            // Determine price
            uint64 buyoutPrice = 0;
            uint64 bidPrice = 0;
            CalculateItemValue(prototype, bidPrice, buyoutPrice);

            // Define a duration
            uint32 etime = urand(ListingExpireTimeInSecondsMin, ListingExpireTimeInSecondsMax);

            // Set stack size
            uint32 stackCount = GetStackSizeForItem(prototype);
            item->SetCount(stackCount);

            uint32 dep =  sAuctionMgr->GetAuctionDeposit(ahEntry, etime, item, stackCount);

            AuctionEntry* auctionEntry = new AuctionEntry();
            auctionEntry->Id = sObjectMgr->GenerateAuctionID();
            auctionEntry->houseId = AuctionHouseId(config->GetAHID());
            auctionEntry->item_guid = item->GetGUID();
            auctionEntry->item_template = item->GetEntry();
            auctionEntry->itemCount = item->GetCount();
            auctionEntry->owner = AHBplayer->GetGUID();
            auctionEntry->startbid = bidPrice * stackCount;
            auctionEntry->buyout = buyoutPrice * stackCount;
            auctionEntry->bid = 0;
            auctionEntry->deposit = dep;
            auctionEntry->expire_time = (time_t) etime + time(NULL);
            auctionEntry->auctionHouseEntry = ahEntry;
            item->SaveToDB(trans);
            item->RemoveFromUpdateQueueOf(AHBplayer);
            sAuctionMgr->AddAItem(item);
            auctionHouse->AddAuction(auctionEntry);
            auctionEntry->SaveToDB(trans);
            itemsGenerated++;
            batchCount++;
            _AHItemsCountSnapshot[prototype->ItemId] += 1;
            _AHCategoriesCountSnapshot[prototype->Class][prototype->SubClass] += 1;
        }

        CharacterDatabase.CommitTransaction(trans);
    }
    if (debug_Out)
        LOG_INFO("module", "AHSeller: Added {} items", itemsGenerated);
}

void AuctionHouseBot::PopulateAHItemsAndCategoriesCountSnapshot(uint32 AHID)
{
    // Create snapshot of current AH's items and count # of each ItemID for "IsOverpopulated()" checks.
    // Since we can't simultaneously query the Character and World databases with a JOIN, we have to
    //   perform a roundabout way of collecting item_template information about each auction item.
    //   (i.e. Query both databases individually, then combine results in code)
    struct AHItem
    {
        uint32 itemID;
        uint32 count;
    };

    std::vector<uint32> uniqueItemEntries;
    std::unordered_map<uint32, ItemTemplate> uniqueItemEntriesTemplates;
    std::vector<AHItem> AHItems;

    // Get all auction items for this AH. Store itemID and stack count in AHItems for later use
    string characterQueryString = "SELECT instance.itemEntry, instance.count "
        "FROM auctionhouse ah "
        "JOIN item_instance instance ON ah.itemguid = instance.guid "
        "WHERE ah.houseid = {}";
    QueryResult characterResult = CharacterDatabase.Query(characterQueryString, AHID);
    if (characterResult)
    {
        do {
            Field* fields = characterResult->Fetch();
            uint32_t itemID = fields[0].Get<uint32>();
            uint32_t itemCount = fields[1].Get<uint32>();
            _AHItemsCountSnapshot[itemID] += itemCount;
            uniqueItemEntries.push_back(itemID);
            AHItems.push_back(AHItem(itemID, itemCount));
        } while (characterResult->NextRow());
    }
    else {
        return;
    }

    // Collect item_templates for each auction house item
    std::ostringstream worldQueryString;
    worldQueryString << "SELECT entry, class, subclass, name FROM item_template WHERE entry IN (";
    bool first = true;
    for (auto entry : uniqueItemEntries)
    {
        if (!first) worldQueryString << ",";
        worldQueryString << entry;
        first = false;
    }
    worldQueryString << ")";

    QueryResult worldResult = WorldDatabase.Query(worldQueryString.str().c_str());
    if (worldResult)
    {
        do {
            uint32 entry = (*worldResult)[0].Get<uint32>();
            uint32 iClass = (*worldResult)[1].Get<uint32>();
            uint32 iSubclass = (*worldResult)[2].Get<uint32>();
            string name = (*worldResult)[3].Get<string>();
            ItemTemplate it;
            it.ItemId = entry;
            it.Class = iClass;
            it.SubClass = iSubclass;
            it.Name1 = name;
            uniqueItemEntriesTemplates[entry] = it;
        } while (worldResult->NextRow());
    } else {
        return;
    }

    // Increment category counts
    for (AHItem auction : AHItems)
    {
        ItemTemplate it = uniqueItemEntriesTemplates[auction.itemID];
        _AHCategoriesCountSnapshot[it.Class][it.SubClass] += auction.count;
    }
}

bool AuctionHouseBot::IsItemOverpopulated(ItemTemplate const* itemProto)
{
    // LOG_INFO("module", "Number of existing auctions for {} {} = {}",itemProto->ItemId, itemProto->Name1, _AHItemsCountSnapshot[itemProto->ItemId]);
    int32 count = _AHItemsCountSnapshot[itemProto->ItemId];
    switch (itemProto->Class)
    {
        case ITEM_CLASS_ARMOR:
        {
            if (itemProto->Quality == ITEM_QUALITY_EPIC && MaxDuplicatesCategoryArmorQualityEpic > -1)
                return (count >= MaxDuplicatesCategoryArmorQualityEpic);
            break;
        }
        case ITEM_CLASS_WEAPON:
        {
            if (itemProto->Quality == ITEM_QUALITY_EPIC && MaxDuplicatesCategoryWeaponQualityEpic > -1)
                return (count >= MaxDuplicatesCategoryWeaponQualityEpic);
            break;
        }
        case ITEM_CLASS_MISC:
        {
            if (itemProto->SubClass == ITEM_SUBCLASS_JUNK_MOUNT && MaxDuplicatesCategoryMount > -1)
                return (count >= MaxDuplicatesCategoryMount);
            else if (itemProto->SubClass == ITEM_SUBCLASS_JUNK_PET && MaxDuplicatesCategoryPet > -1)
                return (count >= MaxDuplicatesCategoryPet);
            break;
        }
        /*
            Probably shouldn't attempt to expand to all
              possible Class/Subclass/Quality combinations.
              980 possible (140 valid Class/Subclass combinations * 7 Qualities)
            Implement only as requests come in?
        */
        default:
            break;
    }

    return false;
}

bool AuctionHouseBot::IsItemCategoryOverpopulated(ItemTemplate const* itemProto)
{
    // LOG_INFO("module", "Number of existing auctions for Class {}, Subclass {} = {}", itemProto->Class, itemProto->SubClass, _AHCategoriesCountSnapshot[itemProto->Class]);
    int32 count = _AHCategoriesCountSnapshot[itemProto->Class][itemProto->SubClass];
    switch (itemProto->Class)
    {
        case ITEM_CLASS_MISC:
        {
            if (itemProto->SubClass == ITEM_SUBCLASS_JUNK_MOUNT && MaxItemsCategoryMount > -1)
                return (count >= MaxItemsCategoryMount);
            else if (itemProto->SubClass == ITEM_SUBCLASS_JUNK_PET && MaxItemsCategoryPet > -1)
                return (count >= MaxItemsCategoryPet);
            break;
        }
        /*
            Probably shouldn't attempt to expand to all
              Class/Subclass combinations.
              (140 valid Class/Subclass combinations)
            Implement only as requests come in?
        */
        default:
            break;
    }

    return false;
}

void AuctionHouseBot::AddNewAuctionBuyerBotBid(std::vector<Player*> AHBPlayers, FactionSpecificAuctionHouseConfig *config)
{
    if (!BuyingBotEnabled)
    {
        if (debug_Out)
            LOG_INFO("module", "AHBuyer: Disabled");
        return;
    }

    // Pull currentAuctionItemListCount.
    string queryString = "SELECT id FROM auctionhouse WHERE itemowner NOT IN ({}) AND buyguid NOT IN ({})";
    if (BuyingBotWillBidAgainstPlayers == false)
        queryString = "SELECT id FROM auctionhouse WHERE itemowner NOT IN ({}) AND buyguid NOT IN ({}) AND lastbid = 0";
    QueryResult result = CharacterDatabase.Query(queryString, AHCharactersGUIDsForQuery, AHCharactersGUIDsForQuery);

    if (!result)
        return;

    if (result->GetRowCount() == 0)
        return;

    // Fetches content of selected AH
    AuctionHouseObject* auctionHouse =  sAuctionMgr->GetAuctionsMap(config->GetAHFID());
    vector<uint32> possibleBids;

    do
    {
        uint32 tmpdata = result->Fetch()->Get<uint32>();
        possibleBids.push_back(tmpdata);
    } while (result->NextRow());

    int randBuyingBotBuyCandidatesPerBuyCycle = urand(BuyingBotBuyCandidatesPerBuyCycleMin, BuyingBotBuyCandidatesPerBuyCycleMax);
    for (int count = 1; count <= randBuyingBotBuyCandidatesPerBuyCycle; ++count)
    {
        // Do we have anything to bid? If not, stop here.
        if (possibleBids.empty())
        {
            //if (debug_Out) sLog->outError( "AHBuyer: I have no newItemsToListCount to bid on.");
            count = randBuyingBotBuyCandidatesPerBuyCycle;
            continue;
        }

        // Choose random auction from possible currentAuctionItemListCount
        uint32 vectorPos = urand(0, possibleBids.size() - 1);
        vector<uint32>::iterator iter = possibleBids.begin();
        advance(iter, vectorPos);

        // from auctionhousehandler.cpp, creates auction pointer & player pointer
        AuctionEntry* auction = auctionHouse->GetAuction(*iter);

        // Erase the auction from the vector to prevent bidding on item in next iteration.
        possibleBids.erase(iter);

        if (!auction)
            continue;

        // get exact item information
		Item *pItem = sAuctionMgr->GetAItem(auction->item_guid);
        if (!pItem || pItem->GetCount() == 0)
        {
			if (debug_Out)
                LOG_ERROR("module", "AHBuyer: Item {} doesn't exist, perhaps bought already?", auction->item_guid.ToString());
            continue;
        }

        // get item prototype
        ItemTemplate const* prototype = sObjectMgr->GetItemTemplate(auction->item_template);

        // Calculate a potential price for the item
        uint64 willingToSpendPerItemPrice = 0;
        uint64 discardBidPrice = 0;
        CalculateItemValue(prototype, discardBidPrice, willingToSpendPerItemPrice);
        willingToSpendPerItemPrice = (uint64)((float)willingToSpendPerItemPrice * BuyingBotAcceptablePriceModifier);
        uint64 willingToPayForStackPrice = willingToSpendPerItemPrice * pItem->GetCount();

        // Determine if it's a bid, buyout, or skip
        bool doBuyout = false;
        bool doBid = false;
        uint64 calcBidAmount = 0;

        if (auction->buyout != 0 && auction->buyout < willingToPayForStackPrice)
            doBuyout = true;
        else
        {
            if (auction->bid == 0 && auction->startbid <= willingToPayForStackPrice)
            {
                doBid = true;
                if (BuyingBotAlwaysBidMaxCalculatedPrice == true)
                    calcBidAmount = willingToPayForStackPrice;
                else
                    calcBidAmount = auction->startbid;
            }
            else if (auction->bid != 0 && (auction->bid + auction->GetAuctionOutBid()) < willingToPayForStackPrice)
            {
                doBid = true;
                if (BuyingBotAlwaysBidMaxCalculatedPrice == true)
                    calcBidAmount = willingToPayForStackPrice;
                else
                    calcBidAmount = auction->bid + auction->GetAuctionOutBid();
            }
        }

        // Check that the item isn't listed above Vendor sell price
        bool preventedOverpayingForVendorItem = false;
        if (PreventOverpayingForVendorItems && vendorItemsPrices[prototype->ItemId] > 0)
        {
            if (doBuyout && auction->buyout > vendorItemsPrices[prototype->ItemId])
            {
                doBuyout = false;
                preventedOverpayingForVendorItem = true;
            }
            if (doBid && calcBidAmount > vendorItemsPrices[prototype->ItemId])
            {
                doBid = false;
                preventedOverpayingForVendorItem = true;
            }
        }

        if (debug_Out)
        {
            LOG_INFO("module", "-------------------------------------------------");
            LOG_INFO("module", "AHBuyer: Info for Auction #{}:", auction->Id);
            LOG_INFO("module", "AHBuyer: AuctionHouse: {}", auction->GetHouseId());
            LOG_INFO("module", "AHBuyer: Owner: {}", auction->owner.ToString());
            LOG_INFO("module", "AHBuyer: Bidder: {}", auction->bidder.ToString());
            LOG_INFO("module", "AHBuyer: Expire Time: {}", uint32(auction->expire_time));
            LOG_INFO("module", "AHBuyer: Item GUID: {}", auction->item_guid.ToString());
            LOG_INFO("module", "AHBuyer: Item Template: {}", auction->item_template);
            LOG_INFO("module", "AHBuyer: Item Info:");
            LOG_INFO("module", "AHBuyer: Item ID: {}", prototype->ItemId);
            LOG_INFO("module", "AHBuyer: Vendor Buy Price: {}", prototype->BuyPrice);
            LOG_INFO("module", "AHBuyer: Vendor Sell Price (Base): {}", prototype->SellPrice);
            if (PreventOverpayingForVendorItems == true)
                LOG_INFO("module", "AHBuyer: Vender Sell Price (Vendor): {}", vendorItemsPrices[prototype->ItemId]);
            LOG_INFO("module", "AHBuyer: Deposit: {}", auction->deposit);
            LOG_INFO("module", "AHBuyer: Bonding: {}", prototype->Bonding);
            LOG_INFO("module", "AHBuyer: Quality: {}", prototype->Quality);
            LOG_INFO("module", "AHBuyer: Item Level: {}", prototype->ItemLevel);
            LOG_INFO("module", "AHBuyer: Ammo Type: {}", prototype->AmmoType);
            LOG_INFO("module", "AHBuyer: Stack Size: {}", pItem->GetCount());
            LOG_INFO("module", "AHBuyer: Starting Bid: {}", auction->startbid);
            LOG_INFO("module", "AHBuyer: Current Bid: {}", auction->bid);
            LOG_INFO("module", "AHBuyer: Buyout Price: {}", auction->buyout);
            LOG_INFO("module", "AHBuyer: Willing To Pay Per Item Price (Buyout): {}", willingToSpendPerItemPrice);
            LOG_INFO("module", "AHBuyer: Willing To Pay For Stack Price (Buyout): {}", willingToPayForStackPrice);
            LOG_INFO("module", "AHBuyer: Calculated Bid Amount (0 means too expensive to bid): {}", calcBidAmount);
            LOG_INFO("module", "AHBuyer: Decided to Buyout?: {}", doBuyout);
            LOG_INFO("module", "AHBuyer: Decided to Bid?: {}", doBid);
            LOG_INFO("module", "AHBuyer: Stopped from buying due to 'PreventOverpayingForVendorItems'?: {}", preventedOverpayingForVendorItem);
            LOG_INFO("module", "-------------------------------------------------");
        }

        Player* AHBplayer = AHBPlayers[urand(0, AHBPlayers.size() - 1)];

        if (doBid)
        {
            auto trans = CharacterDatabase.BeginTransaction();

            if (auction->bidder)
                sAuctionMgr->SendAuctionOutbiddedMail(auction, calcBidAmount, AHBplayer, trans);

            auction->bidder = AHBplayer->GetGUID();
            auction->bid = calcBidAmount;

            sAuctionMgr->GetAuctionHouseSearcher()->UpdateBid(auction);

            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_AUCTION_BID);
            stmt->SetData(0, auction->bidder.GetCounter());
            stmt->SetData(1, auction->bid);
            stmt->SetData(2, auction->Id);
            trans->Append(stmt);

            CharacterDatabase.CommitTransaction(trans);
        }
        else if (doBuyout)
        {
            auto trans = CharacterDatabase.BeginTransaction();

            if ((auction->bidder) && (AHBplayer->GetGUID() != auction->bidder))
                sAuctionMgr->SendAuctionOutbiddedMail(auction, auction->buyout, AHBplayer, trans);
            auction->bidder = AHBplayer->GetGUID();
            auction->bid = auction->buyout;

            // Send mails to buyer & seller
            sAuctionMgr->SendAuctionSuccessfulMail(auction, trans);
            sAuctionMgr->SendAuctionWonMail(auction, trans);
            auction->DeleteFromDB(trans);

            sAuctionMgr->RemoveAItem(auction->item_guid);
            auctionHouse->RemoveAuction(auction);
            CharacterDatabase.CommitTransaction(trans);
        }
    }
}

void AuctionHouseBot::Update()
{
    if ((SellingBotEnabled == false) && (BuyingBotEnabled == false))
        return;
    if (AHCharacters.empty() == true)
        return;

    // Only update if the update cycle has been hit
    LastCycleCount++;
    if (LastCycleCount < CyclesBetweenBuyOrSell)
        return;
    LastCycleCount = 0;
    CyclesBetweenBuyOrSell = urand(CyclesBetweenBuyOrSellMin, CyclesBetweenBuyOrSellMax);

    // Load all AH Bot Players
    std::vector<std::pair<Player*, std::unique_ptr<WorldSession>>> AHBPlayers;
    AHBPlayers.reserve(AHCharacters.size());
    for (uint32 botIndex = 0; botIndex < AHCharacters.size(); ++botIndex)
    {
        CurrentBotCharGUID = AHCharacters[botIndex].CharacterGUID;
        std::string accountName = "AuctionHouseBot" + std::to_string(AHCharacters[botIndex].AccountID);

        // Wrap session in unique pointer to manage the session lifetime
        auto session = std::make_unique<WorldSession>(
            AHCharacters[botIndex].AccountID, std::move(accountName), 0, nullptr,
            SEC_PLAYER, sWorld->getIntConfig(CONFIG_EXPANSION), 0, LOCALE_enUS, 0, false, false, 0
        );
        Player* player = new Player(session.get());
        player->Initialize(AHCharacters[botIndex].CharacterGUID);
        ObjectAccessor::AddObject(player);
        AHBPlayers.emplace_back(player, std::move(session));
    }

    // Create a vector of Player* for passing to methods
    std::vector<Player*> playersPointerVector;
    playersPointerVector.reserve(AHBPlayers.size());
    for (const auto& pair : AHBPlayers)
        playersPointerVector.push_back(pair.first);

    // Add New Bids
    if (sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_AUCTION) == false)
    {
        AddNewAuctions(playersPointerVector, &AllianceConfig);
        if (BuyingBotBuyCandidatesPerBuyCycleMin > 0)
            AddNewAuctionBuyerBotBid(playersPointerVector, &AllianceConfig);

        AddNewAuctions(playersPointerVector, &HordeConfig);
        if (BuyingBotBuyCandidatesPerBuyCycleMin > 0)
            AddNewAuctionBuyerBotBid(playersPointerVector, &HordeConfig);
    }
    AddNewAuctions(playersPointerVector, &NeutralConfig);
    if (BuyingBotBuyCandidatesPerBuyCycleMin > 0)
        AddNewAuctionBuyerBotBid(playersPointerVector, &NeutralConfig);

    // Explicitly delete players to close sessions
    for (auto& [player, session] : AHBPlayers)
    {
        ObjectAccessor::RemoveObject(player);
        delete player;
    }
}

void AuctionHouseBot::InitializeConfiguration()
{
    debug_Out = sConfigMgr->GetOption<bool>("AuctionHouseBot.DEBUG", false);
    debug_Out_Filters = sConfigMgr->GetOption<bool>("AuctionHouseBot.DEBUG_FILTERS", false);

    // Bot enablement
    SellingBotEnabled = sConfigMgr->GetOption<bool>("AuctionHouseBot.EnableSeller", false);
    BuyingBotEnabled = sConfigMgr->GetOption<bool>("AuctionHouseBot.Buyer.Enabled", false);
    if (SellingBotEnabled == false && BuyingBotEnabled == false)
        return;
    string charString = sConfigMgr->GetOption<std::string>("AuctionHouseBot.GUIDs", "0");
    if (charString == "0")
    {
        BuyingBotEnabled = false;
        SellingBotEnabled = false;
        LOG_INFO("module", "AuctionHouseBot: AuctionHouseBot.GUIDs is '0' so this module will be disabled");
        return;
    }
    AddCharacters(charString);

    // Buyer & Seller core properties
    SetCyclesBetweenBuyOrSell();
    ItemsPerCycle = sConfigMgr->GetOption<uint32>("AuctionHouseBot.ItemsPerCycle", 75);
    MaxBuyoutPriceInCopper = sConfigMgr->GetOption<uint32>("AuctionHouseBot.MaxBuyoutPriceInCopper", 1000000000);
    BuyoutVariationReducePercent = sConfigMgr->GetOption<float>("AuctionHouseBot.BuyoutVariationReducePercent", 0.15f);
    BuyoutVariationAddPercent = sConfigMgr->GetOption<float>("AuctionHouseBot.BuyoutVariationAddPercent", 0.25f);
    BidVariationHighReducePercent = sConfigMgr->GetOption<float>("AuctionHouseBot.BidVariationHighReducePercent", 0);
    BidVariationLowReducePercent = sConfigMgr->GetOption<float>("AuctionHouseBot.BidVariationLowReducePercent", 0.25f);
    BuyoutBelowVendorVariationAddPercent = sConfigMgr->GetOption<float>("AuctionHouseBot.BuyoutBelowVendorVariationAddPercent", 0.25f);
    ListingExpireTimeInSecondsMin = sConfigMgr->GetOption<uint32>("AuctionHouseBot.ListingExpireTimeInSecondsMin", 900);
    if (ListingExpireTimeInSecondsMin < 900)
    {
        LOG_ERROR("module", "AuctionHouseBot: ListingExpireTimeInSecondsMin was set below 900 (15 min), so setting to 900");
        ListingExpireTimeInSecondsMin = 900;
    }
    ListingExpireTimeInSecondsMax = sConfigMgr->GetOption<uint32>("AuctionHouseBot.ListingExpireTimeInSecondsMax", 86400);
    if (ListingExpireTimeInSecondsMax > 172800)
    {
        LOG_ERROR("module", "AuctionHouseBot: ListingExpireTimeInSecondsMax was set above 172800 (48 hours), so setting to 172800");
        ListingExpireTimeInSecondsMax = 172800;
    }
    if (ListingExpireTimeInSecondsMax < ListingExpireTimeInSecondsMin)
    {
        LOG_ERROR("module", "AuctionHouseBot: ListingExpireTimeInSecondsMax was smaller than ListingExpireTimeInSecondsMin, setting to 172800 (48 hours) and 900 (15 min)");
        ListingExpireTimeInSecondsMin = 900;
        ListingExpireTimeInSecondsMax = 172800;
    }

    // Buyer Bot
    SetBuyingBotBuyCandidatesPerBuyCycle();
    BuyingBotAcceptablePriceModifier = sConfigMgr->GetOption<float>("AuctionHouseBot.Buyer.AcceptablePriceModifier", 1);
    BuyingBotAlwaysBidMaxCalculatedPrice = sConfigMgr->GetOption<bool>("AuctionHouseBot.Buyer.AlwaysBidMaxCalculatedPrice", false);
    PreventOverpayingForVendorItems = sConfigMgr->GetOption<bool>("AuctionHouseBot.Buyer.PreventOverpayingForVendorItems", true);
    if (PreventOverpayingForVendorItems)
        PopulateVendorItemsPrices();
    BuyingBotWillBidAgainstPlayers = sConfigMgr->GetOption<bool>("AuctionHouseBot.Buyer.BidAgainstPlayers", false);

    // Stack Ratios
    RandomStackRatioConsumable = GetRandomStackValue("AuctionHouseBot.ListingStack.RandomRatio.Consumable", 50);
    RandomStackRatioContainer = GetRandomStackValue("AuctionHouseBot.ListingStack.RandomRatio.Container", 0);
    RandomStackRatioWeapon = GetRandomStackValue("AuctionHouseBot.ListingStack.RandomRatio.Weapon", 0);
    RandomStackRatioGem = GetRandomStackValue("AuctionHouseBot.ListingStack.RandomRatio.Gem", 5);
    RandomStackRatioArmor = GetRandomStackValue("AuctionHouseBot.ListingStack.RandomRatio.Armor", 0);
    RandomStackRatioReagent = GetRandomStackValue("AuctionHouseBot.ListingStack.RandomRatio.Reagent", 50);
    RandomStackRatioProjectile = GetRandomStackValue("AuctionHouseBot.ListingStack.RandomRatio.Projectile", 100);
    RandomStackRatioTradeGood = GetRandomStackValue("AuctionHouseBot.ListingStack.RandomRatio.TradeGood", 50);
    RandomStackRatioGeneric = GetRandomStackValue("AuctionHouseBot.ListingStack.RandomRatio.Generic", 100);
    RandomStackRatioRecipe = GetRandomStackValue("AuctionHouseBot.ListingStack.RandomRatio.Recipe", 0);
    RandomStackRatioQuiver = GetRandomStackValue("AuctionHouseBot.ListingStack.RandomRatio.Quiver", 0);
    RandomStackRatioQuest = GetRandomStackValue("AuctionHouseBot.ListingStack.RandomRatio.Quest", 10);
    RandomStackRatioKey = GetRandomStackValue("AuctionHouseBot.ListingStack.RandomRatio.Key", 10);
    RandomStackRatioMisc = GetRandomStackValue("AuctionHouseBot.ListingStack.RandomRatio.Misc", 100);
    RandomStackRatioGlyph = GetRandomStackValue("AuctionHouseBot.ListingStack.RandomRatio.Glyph", 0);

    // Stack Ratios
    RandomStackIncrementConsumable = GetRandomStackIncrementValue("AuctionHouseBot.ListingStack.RandomStackIncrement.Consumable", 5);
    RandomStackIncrementContainer = GetRandomStackIncrementValue("AuctionHouseBot.ListingStack.RandomStackIncrement.Container", 1);
    RandomStackIncrementWeapon = GetRandomStackIncrementValue("AuctionHouseBot.ListingStack.RandomStackIncrement.Weapon", 1);
    RandomStackIncrementGem = GetRandomStackIncrementValue("AuctionHouseBot.ListingStack.RandomStackIncrement.Gem", 1);
    RandomStackIncrementArmor = GetRandomStackIncrementValue("AuctionHouseBot.ListingStack.RandomStackIncrement.Armor", 1);
    RandomStackIncrementReagent = GetRandomStackIncrementValue("AuctionHouseBot.ListingStack.RandomStackIncrement.Reagent", 1);
    RandomStackIncrementProjectile = GetRandomStackIncrementValue("AuctionHouseBot.ListingStack.RandomStackIncrement.Projectile", 1000);
    RandomStackIncrementTradeGood = GetRandomStackIncrementValue("AuctionHouseBot.ListingStack.RandomStackIncrement.TradeGood", 5);
    RandomStackIncrementGeneric = GetRandomStackIncrementValue("AuctionHouseBot.ListingStack.RandomStackIncrement.Generic", 1);
    RandomStackIncrementRecipe = GetRandomStackIncrementValue("AuctionHouseBot.ListingStack.RandomStackIncrement.Recipe", 1);
    RandomStackIncrementQuiver = GetRandomStackIncrementValue("AuctionHouseBot.ListingStack.RandomStackIncrement.Quiver", 1);
    RandomStackIncrementQuest = GetRandomStackIncrementValue("AuctionHouseBot.ListingStack.RandomStackIncrement.Quest", 1);
    RandomStackIncrementKey = GetRandomStackIncrementValue("AuctionHouseBot.ListingStack.RandomStackIncrement.Key", 1);
    RandomStackIncrementMisc = GetRandomStackIncrementValue("AuctionHouseBot.ListingStack.RandomStackIncrement.Misc", 1);
    RandomStackIncrementGlyph = GetRandomStackIncrementValue("AuctionHouseBot.ListingStack.RandomStackIncrement.Glyph", 1);

    // List proportions
    ItemListProportionNodesSeed.clear();
    for (int category = 0; category < MAX_ITEM_CLASS; category++)
    {
        for (int quality = 0; quality < MAX_ITEM_QUALITY; quality++)
        {
            if (category != ITEM_CLASS_MONEY && category != ITEM_CLASS_PERMANENT)
            {
                std::string key = std::string("AuctionHouseBot.ListProportion.Category") + GetCategoryName((ItemClass)category) + ".Quality" + GetQualityName((ItemQualities)quality);
                ListProportionNode node;
                node.ItemClassID = category;
                node.ItemQualityID = quality;
                node.Proportion = sConfigMgr->GetOption<uint32>(key.c_str(), 0);
                ItemListProportionNodesSeed.push_back(node);
            }
        }
    }
    ItemListProportionMultipliedItemIDs.clear();
    AddItemValuePairsToItemIDMap(ItemListProportionMultipliedItemIDs, sConfigMgr->GetOption<std::string>("AuctionHouseBot.ListProportion.ListMultipliedItemIDs", ""));

    // Price Multipliers
    PriceMultiplierCategoryConsumable = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.Category.Consumable", 1);
    PriceMultiplierCategoryContainer = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.Category.Container", 1);
    PriceMultiplierCategoryWeapon = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.Category.Weapon", 1);
    PriceMultiplierCategoryGem = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.Category.Gem", 1);
    PriceMultiplierCategoryArmor = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.Category.Armor", 1);
    PriceMultiplierCategoryReagent = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.Category.Reagent", 1);
    PriceMultiplierCategoryProjectile = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.Category.Projectile", 1);
    PriceMultiplierCategoryTradeGood = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.Category.TradeGood", 1);
    PriceMultiplierCategoryGeneric = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.Category.Generic", 1);
    PriceMultiplierCategoryRecipe = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.Category.Recipe", 1);
    PriceMultiplierCategoryQuiver = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.Category.Quiver", 1);
    PriceMultiplierCategoryQuest = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.Category.Quest", 1);
    PriceMultiplierCategoryKey = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.Category.Key", 1);
    PriceMultiplierCategoryMisc = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.Category.Misc", 1);
    PriceMultiplierCategoryGlyph = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.Category.Glyph", 1);
    PriceMultiplierItemLevelCategoryConsumable = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.ItemLevel.Category.Consumable", 0);
    PriceMultiplierItemLevelCategoryContainer = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.ItemLevel.Category.Container", 0);
    PriceMultiplierItemLevelCategoryWeapon = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.ItemLevel.Category.Weapon", 0);
    PriceMultiplierItemLevelCategoryGem = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.ItemLevel.Category.Gem", 0);
    PriceMultiplierItemLevelCategoryArmor = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.ItemLevel.Category.Armor", 0);
    PriceMultiplierItemLevelCategoryReagent = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.ItemLevel.Category.Reagent", 0);
    PriceMultiplierItemLevelCategoryProjectile = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.ItemLevel.Category.Projectile", 0);
    PriceMultiplierItemLevelCategoryTradeGood = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.ItemLevel.Category.TradeGood", 0);
    PriceMultiplierItemLevelCategoryGeneric = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.ItemLevel.Category.Generic", 0);
    PriceMultiplierItemLevelCategoryRecipe = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.ItemLevel.Category.Recipe", 0);
    PriceMultiplierItemLevelCategoryQuiver = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.ItemLevel.Category.Quiver", 0);
    PriceMultiplierItemLevelCategoryQuest = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.ItemLevel.Category.Quest", 0);
    PriceMultiplierItemLevelCategoryKey = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.ItemLevel.Category.Key", 0);
    PriceMultiplierItemLevelCategoryMisc = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.ItemLevel.Category.Misc", 0);
    PriceMultiplierItemLevelCategoryGlyph = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.ItemLevel.Category.Glyph", 0);
    PriceMultiplierQualityPoor = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.Quality.Poor", 1);
    PriceMultiplierQualityNormal = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.Quality.Normal", 1);
    PriceMultiplierQualityUncommon = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.Quality.Uncommon", 1.8);
    PriceMultiplierQualityRare = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.Quality.Rare", 1.9);
    PriceMultiplierQualityEpic = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.Quality.Epic", 2.1);
    PriceMultiplierQualityLegendary = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.Quality.Legendary", 3);
    PriceMultiplierQualityArtifact = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.Quality.Artifact", 3);
    PriceMultiplierQualityHeirloom = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.Quality.Heirloom", 3);
    for (int category = 0; category < MAX_ITEM_CLASS; category++)
    {
        for (int quality = 0; quality < MAX_ITEM_QUALITY; quality++)
        {
            std::string key = std::string("AuctionHouseBot.PriceMultiplier.Category") + GetCategoryName((ItemClass)category) +
                            ".Quality" + GetQualityName((ItemQualities)quality);

            float multiplier = sConfigMgr->GetOption<float>(key.c_str(), 1.0f);
            PriceMultiplierCategoryQuality[category][quality] = multiplier;
        }
    }
    PriceMultiplierCategoryMountQualityPoor = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.CategoryMount.QualityPoor", 1.0);
    PriceMultiplierCategoryMountQualityNormal = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.CategoryMount.QualityNormal", 1.0);
    PriceMultiplierCategoryMountQualityUncommon = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.CategoryMount.QualityUncommon", 1.0);
    PriceMultiplierCategoryMountQualityRare = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.CategoryMount.QualityRare", 3000.0);
    PriceMultiplierCategoryMountQualityEpic = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.CategoryMount.QualityEpic", 5750.0);
    PriceMultiplierCategoryMountQualityLegendary = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.CategoryMount.QualityLegendary", 1.0);
    PriceMultiplierCategoryMountQualityArtifact = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.CategoryMount.QualityArtifact", 1.0);
    PriceMultiplierCategoryMountQualityHeirloom = sConfigMgr->GetOption<float>("AuctionHouseBot.PriceMultiplier.CategoryMount.QualityHeirloom", 1.0);

    // Advanced Pricing
    AdvancedPricingConsumablePotionEnabled = sConfigMgr->GetOption<bool>("AuctionHouseBot.AdvancedPricing.Consumable.Potion.Enabled", true);
    AdvancedPricingConsumableElixirEnabled = sConfigMgr->GetOption<bool>("AuctionHouseBot.AdvancedPricing.Consumable.Elixir.Enabled", true);
    AdvancedPricingConsumableFlaskEnabled = sConfigMgr->GetOption<bool>("AuctionHouseBot.AdvancedPricing.Consumable.Flask.Enabled", true);
    AdvancedPricingGemEnabled = sConfigMgr->GetOption<bool>("AuctionHouseBot.AdvancedPricing.Gem.Enabled", true);
    AdvancedPricingTradeGoodClothEnabled = sConfigMgr->GetOption<bool>("AuctionHouseBot.AdvancedPricing.TradeGood.Cloth.Enabled", true);
    AdvancedPricingTradeGoodHerbEnabled = sConfigMgr->GetOption<bool>("AuctionHouseBot.AdvancedPricing.TradeGood.Herb.Enabled", true);
    AdvancedPricingTradeGoodMetalStoneEnabled = sConfigMgr->GetOption<bool>("AuctionHouseBot.AdvancedPricing.TradeGood.MetalStone.Enabled", true);
    AdvancedPricingTradeGoodLeatherEnabled = sConfigMgr->GetOption<bool>("AuctionHouseBot.AdvancedPricing.TradeGood.Leather.Enabled", true);
    AdvancedPricingTradeGoodEnchantingEnabled = sConfigMgr->GetOption<bool>("AuctionHouseBot.AdvancedPricing.TradeGood.Enchanting.Enabled", true);
    AdvancedPricingTradeGoodElementalEnabled = sConfigMgr->GetOption<bool>("AuctionHouseBot.AdvancedPricing.TradeGood.Elemental.Enabled", true);
    AdvancedPricingTradeGoodMeatEnabled = sConfigMgr->GetOption<bool>("AuctionHouseBot.AdvancedPricing.TradeGood.Meat.Enabled", true);
    AdvancedPricingMiscJunkEnabled = sConfigMgr->GetOption<bool>("AuctionHouseBot.AdvancedPricing.Misc.Junk.Enabled", true);
    AdvancedPricingMiscMountEnabled = sConfigMgr->GetOption<bool>("AuctionHouseBot.AdvancedPricing.Misc.Mount.Enabled", true);

    // Price minimums
    PriceMinimumCenterBaseConsumable = sConfigMgr->GetOption<uint32>("AuctionHouseBot.PriceMinimumCenterBase.Consumable",1000);
    PriceMinimumCenterBaseContainer = sConfigMgr->GetOption<uint32>("AuctionHouseBot.PriceMinimumCenterBase.Container", 1000);
    PriceMinimumCenterBaseWeapon = sConfigMgr->GetOption<uint32>("AuctionHouseBot.PriceMinimumCenterBase.Weapon", 1000);
    PriceMinimumCenterBaseGem = sConfigMgr->GetOption<uint32>("AuctionHouseBot.PriceMinimumCenterBase.Gem", 1000);
    PriceMinimumCenterBaseArmor = sConfigMgr->GetOption<uint32>("AuctionHouseBot.PriceMinimumCenterBase.Armor", 1000);
    PriceMinimumCenterBaseReagent = sConfigMgr->GetOption<uint32>("AuctionHouseBot.PriceMinimumCenterBase.Reagent", 1000);
    PriceMinimumCenterBaseProjectile = sConfigMgr->GetOption<uint32>("AuctionHouseBot.PriceMinimumCenterBase.Projectile", 5);
    PriceMinimumCenterBaseTradeGood = sConfigMgr->GetOption<uint32>("AuctionHouseBot.PriceMinimumCenterBase.TradeGood", 850);
    PriceMinimumCenterBaseGeneric = sConfigMgr->GetOption<uint32>("AuctionHouseBot.PriceMinimumCenterBase.Generic", 1000);
    PriceMinimumCenterBaseRecipe = sConfigMgr->GetOption<uint32>("AuctionHouseBot.PriceMinimumCenterBase.Recipe", 1000);
    PriceMinimumCenterBaseQuiver = sConfigMgr->GetOption<uint32>("AuctionHouseBot.PriceMinimumCenterBase.Quiver", 1000);
    PriceMinimumCenterBaseQuest = sConfigMgr->GetOption<uint32>("AuctionHouseBot.PriceMinimumCenterBase.Quest", 1000);
    PriceMinimumCenterBaseKey = sConfigMgr->GetOption<uint32>("AuctionHouseBot.PriceMinimumCenterBase.Key", 1000);
    PriceMinimumCenterBaseMisc = sConfigMgr->GetOption<uint32>("AuctionHouseBot.PriceMinimumCenterBase.Misc", 1000);
    PriceMinimumCenterBaseGlyph = sConfigMgr->GetOption<uint32>("AuctionHouseBot.PriceMinimumCenterBase.Glyph", 1000);
    AddItemValuePairsToItemIDMap(PriceMinimumCenterBaseOverridesByItemID, sConfigMgr->GetOption<std::string>("AuctionHouseBot.PriceMinimumCenterBase.OverrideItems", ""));

    // Item level Restrictions
    ListedItemLevelRestrictedEnabled = sConfigMgr->GetOption<bool>("AuctionHouseBot.ListedItemLevelRestrict.Enabled", false);
    ListedItemLevelRestrictedUseCraftedItemForCalculation = sConfigMgr->GetOption<bool>("AuctionHouseBot.ListedItemLevelRestrict.UseCraftedItemForCalculation", true);
    ListedItemLevelMin = sConfigMgr->GetOption("AuctionHouseBot.ListedItemLevelRestrict.MinItemLevel", 0);
    ListedItemLevelMax = sConfigMgr->GetOption("AuctionHouseBot.ListedItemLevelRestrict.MaxItemLevel", 999);
    ListedItemLevelExceptionItems.clear();
    AddItemIDsFromString(ListedItemLevelExceptionItems, sConfigMgr->GetOption<std::string>("AuctionHouseBot.ListedItemLevelRestrict.ExceptionItemIDs", ""), "ListedItemLevelRestrict.ExceptionItemIDs");

    // Item ID Restrictions
    ListedItemIDRestrictedEnabled = sConfigMgr->GetOption<bool>("AuctionHouseBot.ListedItemIDRestrict.Enabled", false);
    ListedItemIDMin = sConfigMgr->GetOption("AuctionHouseBot.ListedItemIDRestrict.MinItemID", 0);
    ListedItemIDMax = sConfigMgr->GetOption("AuctionHouseBot.ListedItemIDRestrict.MaxItemID", 200000);
    ListedItemIDExceptionItems.clear();
    AddItemIDsFromString(ListedItemIDExceptionItems, sConfigMgr->GetOption<std::string>("AuctionHouseBot.ListedItemIDRestrict.ExceptionItemIDs", ""), "ListedItemIDRestrict.ExceptionItemIDs");

    // Duplicate Item Count Restrictions
    MaxDuplicatesCategoryArmorQualityEpic = sConfigMgr->GetOption<int32>("AuctionHouseBot.ListedItem.CategoryArmor.QualityEpic.MaxCount", -1);
    MaxDuplicatesCategoryWeaponQualityEpic = sConfigMgr->GetOption<int32>("AuctionHouseBot.ListedItem.CategoryWeapon.QualityEpic.MaxCount", -1);
    MaxDuplicatesCategoryMount = sConfigMgr->GetOption<int32>("AuctionHouseBot.ListedItem.CategoryMount.MaxCount", -1);
    MaxDuplicatesCategoryPet = sConfigMgr->GetOption<int32>("AuctionHouseBot.ListedItem.CategoryPet.MaxCount", -1);

    // Category Max Item Count Restrictions
    MaxItemsCategoryPet = sConfigMgr->GetOption<int32>("AuctionHouseBot.CategoryPet.MaxCount", -1);
    MaxItemsCategoryMount = sConfigMgr->GetOption<int32>("AuctionHouseBot.CategoryMount.MaxCount", -1);

    // Disabled Items
    DisabledItemTextFilter = sConfigMgr->GetOption<bool>("AuctionHouseBot.DisabledItemTextFilter", true);
    DisabledItems.clear();
    AddItemIDsFromString(DisabledItems, sConfigMgr->GetOption<std::string>("AuctionHouseBot.DisabledItemIDs", ""), "AuctionHouseBot.DisabledItemIDs");
    AddItemIDsFromString(DisabledItems, sConfigMgr->GetOption<std::string>("AuctionHouseBot.DisabledCraftedItemIDs", ""), "AuctionHouseBot.DisabledCraftedItemIDs");

    if (!sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_AUCTION))
    {
        AllianceConfig.SetMinItems(sConfigMgr->GetOption<uint32>("AuctionHouseBot.Alliance.MinItems", 15000));
        AllianceConfig.SetMaxItems(sConfigMgr->GetOption<uint32>("AuctionHouseBot.Alliance.MaxItems", 15000));

        HordeConfig.SetMinItems(sConfigMgr->GetOption<uint32>("AuctionHouseBot.Horde.MinItems", 15000));
        HordeConfig.SetMaxItems(sConfigMgr->GetOption<uint32>("AuctionHouseBot.Horde.MaxItems", 15000));
    }
    NeutralConfig.SetMinItems(sConfigMgr->GetOption<uint32>("AuctionHouseBot.Neutral.MinItems", 15000));
    NeutralConfig.SetMaxItems(sConfigMgr->GetOption<uint32>("AuctionHouseBot.Neutral.MaxItems", 15000));
}

uint32 AuctionHouseBot::GetRandomStackValue(std::string configKeyString, uint32 defaultValue)
{
    uint32 stackValue = sConfigMgr->GetOption<uint32>(configKeyString, defaultValue);
    if (stackValue > 100 || stackValue < 0)
    {
        LOG_ERROR("module", "{} value is invalid.  Setting to default ({}).", configKeyString, defaultValue);
        stackValue = defaultValue;
    }
    return stackValue;
}

uint32 AuctionHouseBot::GetRandomStackIncrementValue(std::string configKeyString, uint32 defaultValue)
{
    uint32 stackIncrementValue = sConfigMgr->GetOption<uint32>(configKeyString, defaultValue);
    if (stackIncrementValue <= 0)
    {
        LOG_ERROR("module", "{} value is invalid.  Setting to default ({}).", configKeyString, defaultValue);
        stackIncrementValue = defaultValue;
    }
    return stackIncrementValue;
}

void AuctionHouseBot::SetCyclesBetweenBuyOrSell()
{
    std::string cyclesConfigString = sConfigMgr->GetOption<std::string>("AuctionHouseBot.AuctionHouseManagerCyclesBetweenBuyOrSell", "1");
    GetConfigMinAndMax(cyclesConfigString, CyclesBetweenBuyOrSellMin, CyclesBetweenBuyOrSellMax);
    CyclesBetweenBuyOrSell = urand(CyclesBetweenBuyOrSellMin, CyclesBetweenBuyOrSellMax);
}

void AuctionHouseBot::SetBuyingBotBuyCandidatesPerBuyCycle()
{
    std::string candidatesPerCycleString = sConfigMgr->GetOption<string>("AuctionHouseBot.Buyer.BuyCandidatesPerBuyCycle", "1");
    GetConfigMinAndMax(candidatesPerCycleString, BuyingBotBuyCandidatesPerBuyCycleMin, BuyingBotBuyCandidatesPerBuyCycleMax);
}

void AuctionHouseBot::GetConfigMinAndMax(std::string config, uint32& min, uint32& max)
{
    size_t pos = config.find(':');
    if (pos != std::string::npos)
    {
        min = std::stoul(config.substr(0, pos));
        max = std::stoul(config.substr(pos + 1));

        if (min < 1)
            min = 1;
        if (max < min)
            max = min;
    }
    else
        min = max = std::stoul(config);
}

void AuctionHouseBot::AddCharacters(std::string characterGUIDString)
{
    AHCharacters.clear();
    std::string delimitedValue;
    std::stringstream characterGUIDStream;
    std::set<uint32> characterGUIDs;

    // Grab from the string
    characterGUIDStream.str(characterGUIDString);
    while (std::getline(characterGUIDStream, delimitedValue, ',')) // Process each charecter GUID in the string, delimited by the comma ","
    {
        std::string valueOne;
        std::stringstream characterGUIDStream(delimitedValue);
        characterGUIDStream >> valueOne;
        auto characterGUID = atoi(valueOne.c_str());
        if (characterGUID == 0)
            continue;
        if (characterGUIDs.find(characterGUID) != characterGUIDs.end())
        {
            if (debug_Out)
                LOG_ERROR("module", "AuctionHouseBot: Duplicate character with GUID of {} found, skipping", characterGUID);
        }
        else
            characterGUIDs.insert(characterGUID);
    }

    // Lookup accounts and add them
    if (characterGUIDs.empty() == true)
    {
        LOG_ERROR("module", "AuctionHouseBot: No character GUIDs were supplied. Be sure to set AuctionHouseBot.GUIDs");
        return;
    }
    AHCharactersGUIDsForQuery = "";
    bool first = true;
    for (uint32 curGUID : characterGUIDs)
    {
        if (first == false)
        {
            AHCharactersGUIDsForQuery += ", ";
        }
        AHCharactersGUIDsForQuery += std::to_string(curGUID);
        first = false;
    }
    QueryResult queryResult = CharacterDatabase.Query("SELECT `guid`, `account` FROM `characters` WHERE guid IN ({})", AHCharactersGUIDsForQuery);
    if (!queryResult || queryResult->GetRowCount() == 0)
    {
        LOG_ERROR("module", "AuctionHouseBot: No character GUIDs found when looking up values from AuctionHouseBot.GUIDs from the character database 'characters.guid'.");
        return;
    }
    do
    {
        // Pull the data out
        Field* fields = queryResult->Fetch();
        uint32 guid = fields[0].Get<uint32>();
        uint32 account = fields[1].Get<uint32>();
        AuctionHouseBotCharacter curChar = AuctionHouseBotCharacter(account, guid);
        AHCharacters.push_back(curChar);
    } while (queryResult->NextRow());
}

template <typename ValueType>
void AuctionHouseBot::AddItemValuePairsToItemIDMap(std::unordered_map<uint32, ValueType>& workingValueToItemIDMap, std::string valueToItemIDMapString)
{
    std::string delimitedValue;
    std::stringstream valueToItemIDStream;
    valueToItemIDStream.str(valueToItemIDMapString);
    while (std::getline(valueToItemIDStream, delimitedValue, ',')) // Process each item ID in the string, delimited by the comma ","
    {
        std::string curBlock;
        std::stringstream itemPairStream(delimitedValue);
        itemPairStream >> curBlock;

        // Only process if it has a colon (:)
        if (curBlock.find(":") != std::string::npos)
        {
            std::string itemIDString = curBlock.substr(0, curBlock.find(":"));
            auto itemId = atoi(itemIDString.c_str());
            std::string valueString = curBlock.substr(curBlock.find(":") + 1);
            auto convertedValue = atoi(valueString.c_str());
            if (itemId > 0 && convertedValue > 0)
                workingValueToItemIDMap.insert({ itemId, convertedValue });
        }
    }
}

void AuctionHouseBot::AddItemIDsFromString(std::set<uint32>& workingItemIDSet, std::string itemString, const char* parentOperationName)
{
    std::string delimitedValue;
    std::stringstream itemIdStream;

    itemIdStream.str(itemString);
    while (std::getline(itemIdStream, delimitedValue, ',')) // Process each item ID in the string, delimited by the comma ","
    {
        std::string valueOne;
        std::stringstream itemPairStream(delimitedValue);
        itemPairStream >> valueOne;

        // If it has a hypen, then it's a range of numbers
        if (valueOne.find("-") != std::string::npos)
        {
            std::string leftIDString = valueOne.substr(0, valueOne.find("-"));
            std::string rightIDString = valueOne.substr(valueOne.find("-") + 1);

            auto leftId = atoi(leftIDString.c_str());
            auto rightId = atoi(rightIDString.c_str());

            if (leftId > rightId)
            {
                LOG_ERROR("module", "AuctionHouseBot: Duplicate item ID range of {} to {} needs to be smallest to largest for {}, skipping", leftId, rightId, parentOperationName);
            }
            else
            {
                for (int32 i = leftId; i <= rightId; ++i)
                    AddToItemIDSet(workingItemIDSet, i, parentOperationName);
            }
        }
        else
        {
            auto itemId = atoi(valueOne.c_str());
            AddToItemIDSet(workingItemIDSet, itemId, parentOperationName);
        }
    }
}

void AuctionHouseBot::AddToItemIDSet(std::set<uint32>& workingItemIDSet, uint32 itemID, const char* parentOperationName)
{
    if (workingItemIDSet.find(itemID) != workingItemIDSet.end())
    {
        if (debug_Out)
            LOG_ERROR("module", "AuctionHouseBot: Duplicate item id {} attempted to be put into a working item set from operation {}, skipping", itemID, parentOperationName);
    }
    else
    {
        workingItemIDSet.insert(itemID);
    }
}

const char* AuctionHouseBot::GetQualityName(ItemQualities quality)
{
    switch (quality)
    {
        case ITEM_QUALITY_POOR:       return "Poor";
        case ITEM_QUALITY_NORMAL:     return "Normal";
        case ITEM_QUALITY_UNCOMMON:   return "Uncommon";
        case ITEM_QUALITY_RARE:       return "Rare";
        case ITEM_QUALITY_EPIC:       return "Epic";
        case ITEM_QUALITY_LEGENDARY:  return "Legendary";
        case ITEM_QUALITY_ARTIFACT:   return "Artifact";
        case ITEM_QUALITY_HEIRLOOM:   return "Heirloom";
        default:                      return "Unknown";
    }
}

const char* AuctionHouseBot::GetCategoryName(ItemClass category)
{
    switch (category)
    {
        case ITEM_CLASS_CONSUMABLE:   return "Consumable";
        case ITEM_CLASS_CONTAINER:    return "Container";
        case ITEM_CLASS_WEAPON:       return "Weapon";
        case ITEM_CLASS_GEM:          return "Gem";
        case ITEM_CLASS_ARMOR:        return "Armor";
        case ITEM_CLASS_REAGENT:      return "Reagent";
        case ITEM_CLASS_PROJECTILE:   return "Projectile";
        case ITEM_CLASS_TRADE_GOODS:  return "TradeGood";
        case ITEM_CLASS_GENERIC:      return "Generic";
        case ITEM_CLASS_RECIPE:       return "Recipe";
        case ITEM_CLASS_MONEY:        return "Money";
        case ITEM_CLASS_QUIVER:       return "Quiver";
        case ITEM_CLASS_QUEST:        return "Quest";
        case ITEM_CLASS_KEY:          return "Key";
        case ITEM_CLASS_PERMANENT:    return "Permanent";
        case ITEM_CLASS_MISC:         return "Misc";
        case ITEM_CLASS_GLYPH:        return "Glyph";
        default:                      return "Unknown";
    }
}

void AuctionHouseBot::PopulateVendorItemsPrices()
{
    // Load vendor newItemsToListCount' prices into a vector for fast lookup
    QueryResult r = WorldDatabase.Query("SELECT MAX(entry) FROM item_template");
    Field* f = r->Fetch();
    uint32_t numItems = f[0].Get<uint32>();
    vendorItemsPrices = std::vector<uint32>(numItems, UINT32_MAX);
    
    QueryResult result = WorldDatabase.Query("SELECT v.entry, MIN(v.SellPrice) AS SellPrice FROM item_template v JOIN npc_vendor p ON v.entry = p.item WHERE v.class != {} GROUP BY v.entry", ITEM_CLASS_TRADE_GOODS);
    if (result)
    {
        do
        {
            Field* pFields = result->Fetch();
            uint32_t itemID = pFields[0].Get<uint32>();
            uint32_t itemPrice = pFields[1].Get<uint32>();
            vendorItemsPrices[itemID] = itemPrice;
        } while (result->NextRow()); 
    }
}
