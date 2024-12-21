## Important notes

You have to use at least AzerothCore commit https://github.com/azerothcore/azerothcore-wotlk/commit/4d349e87b53b4921568163d8189369a62ab2d0f5.

## Description

A fork of the auction house bot for AzerothCore.  This fork gives a much more blizzlike experience in the offerings on the auction house.  Most notable differences:
 - Things like Glyphs show up
 - Auction item stack sizes are customizable and more 'realistic'
 - The buyer bot's buying amounts and behavior was changed substantially
 - A separate set of exclusion item IDs was added to have crafted items listed, to encourage gathing professions.
	- Empty the list to have crafted goods appear in the AH, or use other IDs you want to keep separate
 - Moved database configuration completely to config

## Installation

```
1. Simply place the module under the `modules` directory of your AzerothCore source. 
1. Import the SQL manually to the right Database (auth, world or characters) or with the `db_assembler.sh` (if `include.sh` provided).
1. Re-run cmake and launch a clean build of AzerothCore.
```

## Edit module configuration (optional)

If you need to change the module configuration, go to your server configuration folder (where your `worldserver` or `worldserver.exe` is)
edit the file mod_ahbot.conf.dist / mod_ahbot.conf.

## Usage

Edit the module configuration and add a player account ID and a character ID.
This character will sell and buy items in the auction house so give him a good name.

Notes:
- The account used does not need any security level and can be a player account.
- The character used by the ahbot is not meant to be used ingame. If you use it to browse the auction house, you might have issues like "Searching for items..." displaying forever.
- Important!  By default, most player crafted items (except glyphs, bolts, maybe a few other things) are disabled from showing up in the auction house in order to encourage player crafting on lower pop servers.  If you want different behavior, alter the config variable "AuctionHouseBot.DisabledCraftedItemIDs" by deleting IDs you wish to show up.  Note that fish are also disabled to encourage fishing, and that's also managed by disabled lists.
- It takes a few hours for the auction house to fully populate, as only 75 items gets added by default every 'tick'.  You can change this in the config with the AuctionHouseBot.ItemsPerCycle variable.

## Credits

- NathanHandley: Created this rewrite of the one that was ported to AzerothCore
- Ayase: ported the bot to AzerothCore
- Other contributors (check the contributors list)
