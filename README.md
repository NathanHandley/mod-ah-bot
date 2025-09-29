## Description

A fork of the auction house bot for AzerothCore.  This fork gives a much more blizzlike experience in the offerings on the auction house.  Most notable differences:
 - Auction item stack sizes are customizable and more 'realistic'
 - The buyer bot's buying amounts and behavior was changed substantially
 - A separate set of exclusion item IDs was added to have crafted items listed, to encourage gathing professions.
	- Empty the list to have crafted goods appear in the AH, or use other IDs you want to keep separate
 - Moved database configuration completely to config

## Requirements
Requires an AzerothCore version that is caught up to at least change set 3f46e05 (https://github.com/azerothcore/azerothcore-wotlk/commit/3f46e05d3691895b6b8a5b3832d17ecb1e210791)

## Installation

```
1. Simply place the module under the `modules` directory of your AzerothCore source. 
2. Re-run cmake and launch a clean build of AzerothCore.
```

## Usage

**Before you do anything, make at least one character to use as the bot**.  Edit the module configuration and **add 1 or more character GUIDs ID to the "AuctionHouseBot.GUIDs"** variable, which is sourced from your character table in the character database.  These names will be visable in the auction house, so pick good names.  IMPORTANT: If you are using a bot mod (like playerbots), then ensure you use regular non-bot characters for your auctionhouse character(s).  After that, ensure you set **AuctionHouseBot.EnableSeller = true**.  Nothing else is required unless you want to tune stuff or enable the buyer bot, which is done with AuctionHouseBot.Buyer.Enabled.

Notes:
- These accounts do not need any security level and can be a player accounts.
- The characters used by the ahbot are not meant to be used ingame. If you use it to browse the auction house, you might have issues like "Searching for items..." displaying forever.
- Important!  By default, most player crafted items (except glyphs, bolts, maybe a few other things) are disabled from showing up in the auction house in order to encourage player crafting on lower pop servers.  If you want different behavior, alter the config variable "AuctionHouseBot.DisabledCraftedItemIDs" by deleting IDs you wish to show up.  Note that fish are also disabled to encourage fishing, and that's also managed by disabled lists.
- It takes a few hours for the auction house to fully populate, as only 75 items gets added by default every 'tick'.  You can change this in the config with the AuctionHouseBot.ItemsPerCycle variable.
- All price multpliers (along with the advanced pricing, see config) are applied multiplicative.  Example: A Category of 1.5x, Quality of 2x, and CategoryQuality of 1.4x would make the multiplier 4.2 (1.5 x 2 x 1.4).  The advanced pricing would then multiply that value further.  Using item level price multpliers, which create a multiplier of itemlevel x value, is also multiplicitive along with the others.  You cannot use item level price multipliers and advanced pricing, as advanced pricing will take priority between the two.
- Bot-listed prices will not exceed 100k gold buyout

## Buying Bot Behavior

1. **Determining Items to Buy:** Every minute the buyer bot will select (BuyCandidatesPerBuyCycle) items currently up for auction which are listed by players as potential purchase items.
2. **Price Willing to Pay:** The buyer bot will use the same item price calculation the seller bot uses, including the random +/- 25%, and that calculated price is then multiplied by (AcceptablePriceModifier) which then becomes the price the buyer will be willing to spend.
3. **Buying it:** If the price calculated is higher than the buy out price, then the bot will buy it out.  If not, it will test to see if it has been bidded on by a bot and if not, if the bid price is below the price it is willing to pay.  If so, it will bid (outbid, so just over current bid).

The above behavior is replicated on each enabled auction house.  If left to default settings, 1 item in each auction house will attempt to be bought from, if the price calculation seems favorable. Note that any item buy attempt, even items above buying price, consumes a buy candidate. That means that too many overpriced items can drown out potential sales.

## Credits

- NathanHandley: Created this rewrite of the one that was ported to AzerothCore
- Zeb139: Lots of great enhancements, like enhanced pricing formulas and performance improvements (plus more)
- Ayase: ported the bot to AzerothCore
- Other contributors (check the contributors list)
