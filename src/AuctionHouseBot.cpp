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

#include "ObjectMgr.h"
#include "AuctionHouseMgr.h"
#include "AuctionHouseBot.h"
#include "Config.h"
#include "Player.h"
#include "WorldSession.h"
#include "GameTime.h"
#include "DatabaseEnv.h"

#include <set>

using namespace std;

AuctionHouseBot::AuctionHouseBot()
{
    debug_Out = false;
    debug_Out_Filters = false;
    AHBSeller = false;
    AHBBuyer = false;

    _lastrun_a = time(NULL);
    _lastrun_h = time(NULL);
    _lastrun_n = time(NULL);

    AllianceConfig = AHBConfig(2);
    HordeConfig = AHBConfig(6);
    NeutralConfig = AHBConfig(7);
}

AuctionHouseBot::~AuctionHouseBot()
{
}

uint32 AuctionHouseBot::getStackSizeForItem(ItemTemplate const* itemProto) const
{
    // Determine the stack ratio based on class type
    if (itemProto == NULL)
        return 1;

    // TODO: Move this to a config
    uint32 stackRatio = 0;
    switch (itemProto->Class)
    {
    case ITEM_CLASS_CONSUMABLE:     stackRatio = 50; break;
    case ITEM_CLASS_CONTAINER:      stackRatio = 0; break;
    case ITEM_CLASS_WEAPON:         stackRatio = 0; break;
    case ITEM_CLASS_GEM:            stackRatio = 5; break;
    case ITEM_CLASS_REAGENT:        stackRatio = 50; break;
    case ITEM_CLASS_ARMOR:          stackRatio = 0; break;
    case ITEM_CLASS_PROJECTILE:     stackRatio = 100; break;
    case ITEM_CLASS_TRADE_GOODS:    stackRatio = 50; break;
    case ITEM_CLASS_GENERIC:        stackRatio = 100; break;
    case ITEM_CLASS_RECIPE:         stackRatio = 0; break;
    case ITEM_CLASS_QUIVER:         stackRatio = 0; break;
    case ITEM_CLASS_QUEST:          stackRatio = 10; break;
    case ITEM_CLASS_KEY:            stackRatio = 10; break;
    case ITEM_CLASS_MISC:           stackRatio = 100; break;
    case ITEM_CLASS_GLYPH:          stackRatio = 0; break;
    default:                        stackRatio = 0; break;
    }

    if (stackRatio > urand(0, 99))
        return urand(1, itemProto->GetMaxStackSize());
    else
        return 1;
}

void AuctionHouseBot::calculateItemValue(ItemTemplate const* itemProto, uint64& outBidPrice, uint64& outBuyoutPrice)
{
    // Start with a buyout price related to the sell price
    outBuyoutPrice = itemProto->SellPrice;

    // Set a minimum base buyoutPrice to 5-15 silver for non-projectiles and non-common trade goods
    if (outBuyoutPrice < 500 && (itemProto->Class != ITEM_CLASS_PROJECTILE && !(itemProto->Class == ITEM_CLASS_TRADE_GOODS && itemProto->Quality == ITEM_QUALITY_NORMAL)))
    {
        // TODO: Move to a config
        outBuyoutPrice = urand(500, 1500);
    }

    // If still no buy price, give it something low
    if (outBuyoutPrice == 0)
    {
        outBuyoutPrice = urand(500, 1500);
    }

    // Multiply the price based on quality
    switch (itemProto->Quality)
    {
    case ITEM_QUALITY_UNCOMMON:     outBuyoutPrice *= 2; break;
    case ITEM_QUALITY_RARE:         outBuyoutPrice *= 5; break;
    case ITEM_QUALITY_EPIC:         outBuyoutPrice *= 7; break;
    case ITEM_QUALITY_LEGENDARY:    outBuyoutPrice *= 11; break;
    default: break;
    }

    // If a vendor sells this item, make the price at least that high
    if (itemProto->SellPrice > outBuyoutPrice)
        outBuyoutPrice = itemProto->SellPrice;

    // Calculate buyout price with a variance
    uint64 sellVarianceBuyoutPriceTopPercent = 130;
    uint64 sellVarianceBuyoutPriceBottomPercent = 70;
    outBuyoutPrice = urand(sellVarianceBuyoutPriceBottomPercent * outBuyoutPrice, sellVarianceBuyoutPriceTopPercent * outBuyoutPrice);
    outBuyoutPrice /= 100;

    // Calculate a bid price based on a variance against buyout price
    uint64 sellVarianceBidPriceTopPercent = 100;
    uint64 sellVarianceBidPriceBottomPercent = 75;
    outBidPrice = urand(sellVarianceBidPriceBottomPercent * outBuyoutPrice, sellVarianceBidPriceTopPercent * outBuyoutPrice);
    outBidPrice /= 100;

    // If variance brought price below sell price, bring it back up to avoid making money off vendoring AH items
    if (outBuyoutPrice < itemProto->SellPrice)
    {
        uint64 minLowPriceAddVariancePercent = 125;
        outBuyoutPrice = urand(100 * itemProto->SellPrice, minLowPriceAddVariancePercent * itemProto->SellPrice);
        outBuyoutPrice /= 100;
    }

    // Bid price can never be below sell price
    if (outBidPrice < itemProto->SellPrice)
    {
        outBidPrice = itemProto->SellPrice;
    }
}

void AuctionHouseBot::populatetemClassSeedListForItemClass(uint32 itemClass, uint32 itemClassSeedWeight)
{
    for (uint32 i = 0; i < itemClassSeedWeight; ++i)
        itemCandidateClassWeightedSeedList.push_back(itemClass);
}

void AuctionHouseBot::populateItemClassSeedList()
{
    // Determine how many of what kinds of items to use based on a seeded weight list, 0 = none

    // TODO: Move these weight items to a config
    uint32 itemClassSeedWeightConsumable = 4;
    uint32 itemClassSeedWeightContainer = 2;
    uint32 itemClassSeedWeightWeapon = 8;
    uint32 itemClassSeedWeightGem = 3;
    uint32 itemClassSeedWeightArmor = 8;
    uint32 itemClassSeedWeightReagent = 1;
    uint32 itemClassSeedWeightProjectile = 2;
    uint32 itemClassSeedWeightTradeGoods = 10;
    uint32 itemClassSeedWeightGeneric = 1;
    uint32 itemClassSeedWeightRecipe = 4;
    uint32 itemClassSeedWeightQuiver = 1;
    uint32 itemClassSeedWeightQuest = 2;
    uint32 itemClassSeedWeightKey = 1;
    uint32 itemClassSeedWeightMisc = 0;
    uint32 itemClassSeedWeightGlyph = 2;

    // Clear old list
    itemCandidateClassWeightedSeedList.clear();

    // Fill the list
    populatetemClassSeedListForItemClass(ITEM_CLASS_CONSUMABLE, itemClassSeedWeightConsumable);
    populatetemClassSeedListForItemClass(ITEM_CLASS_CONTAINER, itemClassSeedWeightContainer);
    populatetemClassSeedListForItemClass(ITEM_CLASS_WEAPON, itemClassSeedWeightWeapon);
    populatetemClassSeedListForItemClass(ITEM_CLASS_GEM, itemClassSeedWeightGem);
    populatetemClassSeedListForItemClass(ITEM_CLASS_ARMOR, itemClassSeedWeightArmor);
    populatetemClassSeedListForItemClass(ITEM_CLASS_REAGENT, itemClassSeedWeightReagent);
    populatetemClassSeedListForItemClass(ITEM_CLASS_PROJECTILE, itemClassSeedWeightProjectile);
    populatetemClassSeedListForItemClass(ITEM_CLASS_TRADE_GOODS, itemClassSeedWeightTradeGoods);
    populatetemClassSeedListForItemClass(ITEM_CLASS_GENERIC, itemClassSeedWeightGeneric);
    populatetemClassSeedListForItemClass(ITEM_CLASS_RECIPE, itemClassSeedWeightRecipe);
    populatetemClassSeedListForItemClass(ITEM_CLASS_QUIVER, itemClassSeedWeightQuiver);
    populatetemClassSeedListForItemClass(ITEM_CLASS_QUEST, itemClassSeedWeightQuest);
    populatetemClassSeedListForItemClass(ITEM_CLASS_KEY, itemClassSeedWeightKey);
    populatetemClassSeedListForItemClass(ITEM_CLASS_MISC, itemClassSeedWeightMisc);
    populatetemClassSeedListForItemClass(ITEM_CLASS_GLYPH, itemClassSeedWeightGlyph);
}

void AuctionHouseBot::populateItemCandidateList()
{
    // Clear old list and rebuild it
    itemCandidatesByItemClass.clear();
    itemCandidatesByItemClass[ITEM_CLASS_CONSUMABLE] = vector<uint32>();
    itemCandidatesByItemClass[ITEM_CLASS_CONTAINER] = vector<uint32>();
    itemCandidatesByItemClass[ITEM_CLASS_WEAPON] = vector<uint32>();
    itemCandidatesByItemClass[ITEM_CLASS_GEM] = vector<uint32>();
    itemCandidatesByItemClass[ITEM_CLASS_ARMOR] = vector<uint32>();
    itemCandidatesByItemClass[ITEM_CLASS_REAGENT] = vector<uint32>();
    itemCandidatesByItemClass[ITEM_CLASS_PROJECTILE] = vector<uint32>();
    itemCandidatesByItemClass[ITEM_CLASS_TRADE_GOODS] = vector<uint32>();
    itemCandidatesByItemClass[ITEM_CLASS_GENERIC] = vector<uint32>();
    itemCandidatesByItemClass[ITEM_CLASS_RECIPE] = vector<uint32>();
    itemCandidatesByItemClass[ITEM_CLASS_QUIVER] = vector<uint32>();
    itemCandidatesByItemClass[ITEM_CLASS_QUEST] = vector<uint32>();
    itemCandidatesByItemClass[ITEM_CLASS_KEY] = vector<uint32>();
    itemCandidatesByItemClass[ITEM_CLASS_MISC] = vector<uint32>();
    itemCandidatesByItemClass[ITEM_CLASS_GLYPH] = vector<uint32>();

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

    // Fill list
    ItemTemplateContainer const* its = sObjectMgr->GetItemTemplateStore();
    for (ItemTemplateContainer::const_iterator itr = its->begin(); itr != its->end(); ++itr)
    {
        // Always store items that are exceptions
        if (includeItemIDExecptions.find(itr->second.ItemId) != includeItemIDExecptions.end())
        {
            // Store the item ID
            itemCandidatesByItemClass[itr->second.Class].push_back(itr->second.ItemId);
            continue;
        }

        // Skip any items not in the seed list
        if (std::find(itemCandidateClassWeightedSeedList.begin(), itemCandidateClassWeightedSeedList.end(), itr->second.Class) == itemCandidateClassWeightedSeedList.end())
            continue;

        // Skip any BOP items
        if (itr->second.Bonding == BIND_WHEN_PICKED_UP)
            continue;

        // Restrict quality to anything under 7 (artifact and below) or above poor
        if (itr->second.Quality == 0 || itr->second.Quality > 6)
            continue;

        // Disabled items by Id
        if (DisableItemStore.find(itr->second.ItemId) != DisableItemStore.end())
        {
            if (debug_Out_Filters)
                LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (PTR/Beta/Unused Item)", itr->second.ItemId);
            continue;
        }

        // Disable conjured items
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

        // Disable items with duration
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

        // Disable items which are bind quest Items
        if (itr->second.Bonding == BIND_QUEST_ITEM)
        {
            if (debug_Out_Filters)
                LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (BOP or BQI and Required Level is less than Item Level)", itr->second.ItemId);
            continue;
        }

        // Disable anything with the string literal of a testing or depricated item
        if (itr->second.Name1.find("Test ") != std::string::npos ||
            itr->second.Name1.find("Item") != std::string::npos ||
            itr->second.Name1.find("Unused") != std::string::npos ||
            itr->second.Name1.find("Deprecated") != std::string::npos ||
            itr->second.Name1.find("Depricated") != std::string::npos ||
            itr->second.Name1.find(" Epic ") != std::string::npos ||
            itr->second.Name1.find("]") != std::string::npos ||
            itr->second.Name1.find("[") != std::string::npos ||
            itr->second.Name1.find("TEST") != std::string::npos ||
            itr->second.Name1.find("D'Sak") != std::string::npos ||
            itr->second.Name1.find("(") != std::string::npos ||
            itr->second.Name1.find(")") != std::string::npos ||
            itr->second.Name1.find("PVP") != std::string::npos ||
            itr->second.Name1.find("Art Demo") != std::string::npos ||
            itr->second.Name1.find("OLD") != std::string::npos)
        {
            if (debug_Out_Filters)
                LOG_ERROR("module", "AuctionHouseBot: Item {} disabled item with a temp or unused item name", itr->second.ItemId);
            continue;
        }

        // Disable "other" consumables
        if (itr->second.Class == ITEM_CLASS_CONSUMABLE && itr->second.SubClass == ITEM_SUBCLASS_CONSUMABLE_OTHER)
        {
            if (debug_Out_Filters)
                LOG_ERROR("module", "AuctionHouseBot: Item {} disabled consumber 'other' item", itr->second.ItemId);
            continue;
        }

        // Disable all items that have neither a sell or a buy price, with exception of item enhancements and trade goods
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
        itemCandidatesByItemClass[itr->second.Class].push_back(itr->second.ItemId);
    }
}

void AuctionHouseBot::addNewAuctions(Player *AHBplayer, AHBConfig *config)
{
    if (!AHBSeller)
    {
        if (debug_Out)
            LOG_INFO("module", "AHSeller: Disabled");
        return;
    }

    uint32 minItems = config->GetMinItems();
    uint32 maxItems = config->GetMaxItems();

    if (maxItems == 0)
    {
        //if (debug_Out) sLog->outString( "AHSeller: Auctions disabled");
        return;
    }

    AuctionHouseEntry const* ahEntry =  sAuctionMgr->GetAuctionHouseEntry(config->GetAHFID());
    if (!ahEntry)
    {
        return;
    }
    AuctionHouseObject* auctionHouse =  sAuctionMgr->GetAuctionsMap(config->GetAHFID());
    if (!auctionHouse)
    {
        return;
    }

    uint32 auctions = auctionHouse->Getcount();

    uint32 items = 0;

    if (auctions >= minItems)
    {
        if (debug_Out)
            LOG_ERROR("module", "AHSeller: Auctions above minimum");
        return;
    }

    if (auctions >= maxItems)
    {
        if (debug_Out)
            LOG_ERROR("module", "AHSeller: Auctions at or above maximum");
        return;
    }

    if ((maxItems - auctions) >= ItemsPerCycle)
        items = ItemsPerCycle;
    else
        items = (maxItems - auctions);

    if (debug_Out)
        LOG_INFO("module", "AHSeller: Adding {} Auctions", items);

    if (debug_Out)
        LOG_ERROR("module", "AHSeller: Current house id is {}", config->GetAHID());

    if (debug_Out)
        LOG_ERROR("module", "AHSeller: {} items", items);

    // only insert a few at a time, so as not to peg the processor
    for (uint32 cnt = 1; cnt <= items; cnt++)
    {
        if (debug_Out)
            LOG_ERROR("module", "AHSeller: {} count", cnt);

        // Pull a random item out of the candidate list
        uint32 chosenItemClass = itemCandidateClassWeightedSeedList[urand(0, itemCandidateClassWeightedSeedList.size() - 1)];
        uint32 itemID = 0;
        if (itemCandidatesByItemClass[chosenItemClass].size() != 0)
            itemID = itemCandidatesByItemClass[chosenItemClass][urand(0, itemCandidatesByItemClass[chosenItemClass].size() - 1)];

        // Prevent invalid IDs
        if (itemID == 0)
        {
            if (debug_Out)
                LOG_ERROR("module", "AHSeller: Item::CreateItem() - ItemID is 0", chosenItemClass);
            continue;
        }

        ItemTemplate const* prototype = sObjectMgr->GetItemTemplate(itemID);
        if (prototype == NULL)
        {
            if (debug_Out)
                LOG_ERROR("module", "AHSeller: prototype == NULL");
            continue;
        }

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
        calculateItemValue(prototype, bidPrice, buyoutPrice);

        // Define a duration
        uint32 etime = urand(900, 43200);

        // Set stack size
        uint32 stackCount = getStackSizeForItem(prototype);
        item->SetCount(stackCount);

        uint32 dep =  sAuctionMgr->GetAuctionDeposit(ahEntry, etime, item, stackCount);

        auto trans = CharacterDatabase.BeginTransaction();
        AuctionEntry* auctionEntry = new AuctionEntry();
        auctionEntry->Id = sObjectMgr->GenerateAuctionID();
        auctionEntry->houseId = config->GetAHID();
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
        CharacterDatabase.CommitTransaction(trans);
    }
}

void AuctionHouseBot::addNewAuctionBuyerBotBid(Player *AHBplayer, AHBConfig *config, WorldSession *session)
{
    if (!AHBBuyer)
    {
        if (debug_Out)
            LOG_ERROR("module", "AHBuyer: Disabled");
        return;
    }

    QueryResult result = CharacterDatabase.Query("SELECT id FROM auctionhouse WHERE itemowner<>{} AND buyguid<>{}", AHBplayerGUID, AHBplayerGUID);

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
    }while (result->NextRow());

    for (uint32 count = 1; count <= config->GetBidsPerInterval(); ++count)
    {
        // Do we have anything to bid? If not, stop here.
        if (possibleBids.empty())
        {
            //if (debug_Out) sLog->outError( "AHBuyer: I have no items to bid on.");
            count = config->GetBidsPerInterval();
            continue;
        }

        // Choose random auction from possible auctions
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
        if (!pItem)
        {
			if (debug_Out)
                LOG_ERROR("module", "AHBuyer: Item {} doesn't exist, perhaps bought already?", auction->item_guid.ToString());
            continue;
        }

        // get item prototype
        ItemTemplate const* prototype = sObjectMgr->GetItemTemplate(auction->item_template);

        // check which price we have to use, startbid or if it is bidded already
        uint32 currentprice;
        if (auction->bid)
            currentprice = auction->bid;
        else
            currentprice = auction->startbid;

        // Prepare portion from maximum bid per item
        double bidrate = static_cast<double>(urand(1, 100)) / 100;
        uint64 discardBuyoutPrice = 0;
        uint64 bidPriceMaxSingle = 0;
        calculateItemValue(prototype, bidPriceMaxSingle, discardBuyoutPrice);
        uint64 bitPriceMaxCurStack = bidPriceMaxSingle * pItem->GetCount();

        // check some special items, and do recalculating to their prices
        switch (prototype->Class)
        {
        // ammo
        case 6:
            bitPriceMaxCurStack = 0;
            break;
        default:
            break;
        }

        if (bitPriceMaxCurStack == 0)
        {
            // quality check failed to get bidmax, let's get out of here
            continue;
        }

        // Calculate our bid
        long double bidvalue = currentprice + ((bitPriceMaxCurStack - currentprice) * bidrate);
        // Convert to uint32
        uint32 bidprice = static_cast<uint32>(bidvalue);

        // Check our bid is high enough to be valid. If not, correct it to minimum.
        if ((currentprice + auction->GetAuctionOutBid()) > bidprice)
            bidprice = currentprice + auction->GetAuctionOutBid();

        if (debug_Out)
        {
            LOG_INFO("module", "-------------------------------------------------");
            LOG_INFO("module", "AHBuyer: Info for Auction #{}:", auction->Id);
            LOG_INFO("module", "AHBuyer: AuctionHouse: {}", auction->GetHouseId());
            LOG_INFO("module", "AHBuyer: Owner: {}", auction->owner.ToString());
            LOG_INFO("module", "AHBuyer: Bidder: {}", auction->bidder.ToString());
            LOG_INFO("module", "AHBuyer: Starting Bid: {}", auction->startbid);
            LOG_INFO("module", "AHBuyer: Current Bid: {}", currentprice);
            LOG_INFO("module", "AHBuyer: Buyout: {}", auction->buyout);
            LOG_INFO("module", "AHBuyer: Deposit: {}", auction->deposit);
            LOG_INFO("module", "AHBuyer: Expire Time: {}", uint32(auction->expire_time));
            LOG_INFO("module", "AHBuyer: Bid Rate: {}", bidrate);
            LOG_INFO("module", "AHBuyer: Bid Max: {}", bitPriceMaxCurStack);
            LOG_INFO("module", "AHBuyer: Bid Value: {}", bidvalue);
            LOG_INFO("module", "AHBuyer: Bid Price: {}", bidprice);
            LOG_INFO("module", "AHBuyer: Item GUID: {}", auction->item_guid.ToString());
            LOG_INFO("module", "AHBuyer: Item Template: {}", auction->item_template);
            LOG_INFO("module", "AHBuyer: Item Info:");
            LOG_INFO("module", "AHBuyer: Item ID: {}", prototype->ItemId);
            LOG_INFO("module", "AHBuyer: Buy Price: {}", prototype->BuyPrice);
            LOG_INFO("module", "AHBuyer: Sell Price: {}", prototype->SellPrice);
            LOG_INFO("module", "AHBuyer: Bonding: {}", prototype->Bonding);
            LOG_INFO("module", "AHBuyer: Quality: {}", prototype->Quality);
            LOG_INFO("module", "AHBuyer: Item Level: {}", prototype->ItemLevel);
            LOG_INFO("module", "AHBuyer: Ammo Type: {}", prototype->AmmoType);
            LOG_INFO("module", "-------------------------------------------------");
        }

        // Check whether we do normal bid, or buyout
        if ((bidprice < auction->buyout) || (auction->buyout == 0))
        {
            if (auction->bidder)
            {
                if (auction->bidder == AHBplayer->GetGUID())
                {
                    //pl->ModifyMoney(-int32(price - auction->bid));
                }
                else
                {
                    // mail to last bidder and return money
                    auto trans = CharacterDatabase.BeginTransaction();
                    sAuctionMgr->SendAuctionOutbiddedMail(auction, bidprice, session->GetPlayer(), trans);
                    CharacterDatabase.CommitTransaction(trans);
                    //pl->ModifyMoney(-int32(price));
                }
           }

            auction->bidder = AHBplayer->GetGUID();
            auction->bid = bidprice;

            // Saving auction into database
            CharacterDatabase.Execute("UPDATE auctionhouse SET buyguid = '{}',lastbid = '{}' WHERE id = '{}'", auction->bidder.GetCounter(), auction->bid, auction->Id);
        }
        else
        {
            auto trans = CharacterDatabase.BeginTransaction();
            //buyout
            if ((auction->bidder) && (AHBplayer->GetGUID() != auction->bidder))
            {
                sAuctionMgr->SendAuctionOutbiddedMail(auction, auction->buyout, session->GetPlayer(), trans);
            }
            auction->bidder = AHBplayer->GetGUID();
            auction->bid = auction->buyout;

            // Send mails to buyer & seller
            //sAuctionMgr->SendAuctionSalePendingMail(auction, trans);
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
    time_t _newrun = time(NULL);
    if ((!AHBSeller) && (!AHBBuyer))
        return;

    std::string accountName = "AuctionHouseBot" + std::to_string(AHBplayerAccount);

    WorldSession _session(AHBplayerAccount, std::move(accountName), nullptr, SEC_PLAYER, sWorld->getIntConfig(CONFIG_EXPANSION), 0, LOCALE_enUS, 0, false, false, 0);
    Player _AHBplayer(&_session);
    _AHBplayer.Initialize(AHBplayerGUID);
    ObjectAccessor::AddObject(&_AHBplayer);

    // Add New Bids
    if (!sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_AUCTION))
    {
        addNewAuctions(&_AHBplayer, &AllianceConfig);
        if (((_newrun - _lastrun_a) >= (AllianceConfig.GetBiddingInterval() * MINUTE)) && (AllianceConfig.GetBidsPerInterval() > 0))
        {
            //if (debug_Out) sLog->outError( "AHBuyer: %u seconds have passed since last bid", (_newrun - _lastrun_a));
            //if (debug_Out) sLog->outError( "AHBuyer: Bidding on Alliance Auctions");
            addNewAuctionBuyerBotBid(&_AHBplayer, &AllianceConfig, &_session);
            _lastrun_a = _newrun;
        }

        addNewAuctions(&_AHBplayer, &HordeConfig);
        if (((_newrun - _lastrun_h) >= (HordeConfig.GetBiddingInterval() * MINUTE)) && (HordeConfig.GetBidsPerInterval() > 0))
        {
            //if (debug_Out) sLog->outError( "AHBuyer: %u seconds have passed since last bid", (_newrun - _lastrun_h));
            //if (debug_Out) sLog->outError( "AHBuyer: Bidding on Horde Auctions");
            addNewAuctionBuyerBotBid(&_AHBplayer, &HordeConfig, &_session);
            _lastrun_h = _newrun;
        }
    }

    addNewAuctions(&_AHBplayer, &NeutralConfig);
    if (((_newrun - _lastrun_n) >= (NeutralConfig.GetBiddingInterval() * MINUTE)) && (NeutralConfig.GetBidsPerInterval() > 0))
    {
        //if (debug_Out) sLog->outError( "AHBuyer: %u seconds have passed since last bid", (_newrun - _lastrun_n));
        //if (debug_Out) sLog->outError( "AHBuyer: Bidding on Neutral Auctions");
        addNewAuctionBuyerBotBid(&_AHBplayer, &NeutralConfig, &_session);
        _lastrun_n = _newrun;
    }
    ObjectAccessor::RemoveObject(&_AHBplayer);
}

void AuctionHouseBot::Initialize()
{
    DisableItemStore.clear();
    // No longer make this a database thing
    //QueryResult result = WorldDatabase.Query("SELECT item FROM mod_auctionhousebot_disabled_items");

    //if (result)
    //{
    //    do
    //    {
    //        Field* fields = result->Fetch();
    //        DisableItemStore.insert(fields[0].Get<uint32>());
    //    } while (result->NextRow());
    //}
    DisableItemStore.insert(51809); // Portable Hole
    DisableItemStore.insert(38082); // Gigantique Bag
    DisableItemStore.insert(49334); // Scale of Onyxia 2.0

    //End Filters
    if (!sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_AUCTION))
    {
        LoadValues(&AllianceConfig);
        LoadValues(&HordeConfig);
    }
    LoadValues(&NeutralConfig);

    //
    // check if the AHBot account/GUID in the config actually exists
    //

    if ((AHBplayerAccount != 0) || (AHBplayerGUID != 0))
    {
        QueryResult result = CharacterDatabase.Query("SELECT 1 FROM characters WHERE account = {} AND guid = {}", AHBplayerAccount, AHBplayerGUID);
        if (!result)
        {
           LOG_ERROR("module", "AuctionHouseBot: The account/GUID-information set for your AHBot is incorrect (account: {} guid: {})", AHBplayerAccount, AHBplayerGUID);
           return;
        }
    }

    if (AHBSeller)
    {
        // Build a list of items that can be pulled from for auction
        populateItemClassSeedList();
        populateItemCandidateList();

        LOG_INFO("module", "AuctionHouseBot:");
        LOG_INFO("module", "{} disabled items", uint32(DisableItemStore.size()));
    }

    LOG_INFO("module", "AuctionHouseBot and AuctionHouseBuyer have been loaded.");
}

void AuctionHouseBot::InitializeConfiguration()
{
    debug_Out = sConfigMgr->GetOption<bool>("AuctionHouseBot.DEBUG", false);
    debug_Out_Filters = sConfigMgr->GetOption<bool>("AuctionHouseBot.DEBUG_FILTERS", false);

    AHBSeller = sConfigMgr->GetOption<bool>("AuctionHouseBot.EnableSeller", false);
    AHBBuyer = sConfigMgr->GetOption<bool>("AuctionHouseBot.EnableBuyer", false);

    AHBplayerAccount = sConfigMgr->GetOption<uint32>("AuctionHouseBot.Account", 0);
    AHBplayerGUID = sConfigMgr->GetOption<uint32>("AuctionHouseBot.GUID", 0);
    ItemsPerCycle = sConfigMgr->GetOption<uint32>("AuctionHouseBot.ItemsPerCycle", 200);
}

void AuctionHouseBot::Commands(uint32 command, uint32 ahMapID, char* args)
{
    AHBConfig *config = NULL;
    switch (ahMapID)
    {
    case 2:
        config = &AllianceConfig;
        break;
    case 6:
        config = &HordeConfig;
        break;
    case 7:
        config = &NeutralConfig;
        break;
    }
    switch (command)
    {
    case 0:     //ahexpire
        {
            AuctionHouseObject* auctionHouse =  sAuctionMgr->GetAuctionsMap(config->GetAHFID());

            AuctionHouseObject::AuctionEntryMap::iterator itr;
            itr = auctionHouse->GetAuctionsBegin();

            while (itr != auctionHouse->GetAuctionsEnd())
            {
                if (itr->second->owner.GetCounter() == AHBplayerGUID)
                {
                    itr->second->expire_time = GameTime::GetGameTime().count();
                    uint32 id = itr->second->Id;
                    uint32 expire_time = itr->second->expire_time;
                    CharacterDatabase.Execute("UPDATE auctionhouse SET time = '{}' WHERE id = '{}'", expire_time, id);
                }
                ++itr;
            }
        }
        break;
    case 1:     //min items
        {
            char * param1 = strtok(args, " ");
            uint32 minItems = (uint32) strtoul(param1, NULL, 0);
            WorldDatabase.Execute("UPDATE mod_auctionhousebot SET minitems = '{}' WHERE auctionhouse = '{}'", minItems, ahMapID);
            config->SetMinItems(minItems);
        }
        break;
    case 2:     //max items
        {
            char * param1 = strtok(args, " ");
            uint32 maxItems = (uint32) strtoul(param1, NULL, 0);
			WorldDatabase.Execute("UPDATE mod_auctionhousebot SET maxitems = '{}' WHERE auctionhouse = '{}'", maxItems, ahMapID);
            config->SetMaxItems(maxItems);
        }
        break;
    case 12:        //buyer bidding interval
        {
            char * param1 = strtok(args, " ");
            uint32 bidInterval = (uint32) strtoul(param1, NULL, 0);
			WorldDatabase.Execute("UPDATE mod_auctionhousebot SET buyerbiddinginterval = '{}' WHERE auctionhouse = '{}'", bidInterval, ahMapID);
            config->SetBiddingInterval(bidInterval);
        }
        break;
    case 13:        //buyer bids per interval
        {
            char * param1 = strtok(args, " ");
            uint32 bidsPerInterval = (uint32) strtoul(param1, NULL, 0);
			WorldDatabase.Execute("UPDATE mod_auctionhousebot SET buyerbidsperinterval = '{}' WHERE auctionhouse = '{}'", bidsPerInterval, ahMapID);
            config->SetBidsPerInterval(bidsPerInterval);
        }
        break;
    default:
        break;
    }
}

void AuctionHouseBot::LoadValues(AHBConfig *config)
{
    if (debug_Out)
        LOG_ERROR("module", "Start Settings for {} Auctionhouses:", WorldDatabase.Query("SELECT name FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<std::string_view>());

    if (AHBSeller)
    {
        //load min and max items
		config->SetMinItems(WorldDatabase.Query("SELECT minitems FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
		config->SetMaxItems(WorldDatabase.Query("SELECT maxitems FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
        if (debug_Out)
        {
            LOG_ERROR("module", "minItems                = {}", config->GetMinItems());
            LOG_ERROR("module", "maxItems                = {}", config->GetMaxItems());
        }
    }

    if (AHBBuyer)
    {
        //load bidding interval
		config->SetBiddingInterval(WorldDatabase.Query("SELECT buyerbiddinginterval FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
        //load bids per interval
		config->SetBidsPerInterval(WorldDatabase.Query("SELECT buyerbidsperinterval FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
        if (debug_Out)
        {
            LOG_ERROR("module", "buyerBiddingInterval    = {}", config->GetBiddingInterval());
            LOG_ERROR("module", "buyerBidsPerInterval    = {}", config->GetBidsPerInterval());
        }
    }

    if (debug_Out)
        LOG_ERROR("module", "End Settings for {} Auctionhouses:", WorldDatabase.Query("SELECT name FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<std::string>());
}
