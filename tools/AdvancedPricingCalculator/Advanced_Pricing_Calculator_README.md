## mod-ah-bot Advanced Pricing Calculator
This spreadsheet helps show the buy and sell prices that the Bot(s) will use based on your configuration when `AuctionHouseBot.AdvancedPricing.<Category>.<Subclass>.Enabled` is `true` .  

### How to Use
**Edit only the “Config” tab**
   - This tab contains many adjustable settings from the module’s config file
   - Change these values to match your server’s setup
   - Items in other tabs reference the `Config` tab, so values will update immediately

<br>

**You shouldn’t need to modify anything outside the Config sheet unless you know what you're doing.**  
*There are hidden columns in each tab that can show items' Quality, Vendor Price, and Item Level (according to the database).  
If you make custom changes to your database info, consider updating these as well for accurate pricing.*

### Notes
- The spreadsheet does not consider the following settings:  
    - AuctionHouseBot.Buyer.AcceptablePriceModifier  
    - AuctionHouseBot.BidVariationHighReducePercent  
    - AuctionHouseBot.BidVariationLowReducePercent  
    - AuctionHouseBot.BuyoutBelowVendorVariationAddPercentEnabled  
    - AuctionHouseBot.BuyoutBelowVendorVariationAddPercent 
