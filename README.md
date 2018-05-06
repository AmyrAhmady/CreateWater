# CreateWater
Create CUSTOM WATERS in SA:MP and swim like a real one!
---------------------------------------------------------


# Functions : 
- CreateWater(minx, miny, maxx, maxy, z, depth, world=-1, interior=-1, type=WATER_TYPE_NORMAL);
- IsPlayerInCustomWater(playerid);
- IsPlayerUnderCustomWater(playerid);
- GetPlayerWaterID(playerid);
- GetWaterDepth(waterid);
- GetPlayerSwimStyle(playerid);
- DestroyWater(waterid);
- GetPlayerCustomOxygen(playerid);
- SetPlayerCustomOxygen(playerid, ox_value);
- ShowCustomOxygenBarForPlayer(playerid);
- HideCustomOxygenBarForPlayer(playerid);

# Callbacks : 
- OnPlayerEnterWater(playerid, waterid)
- OnPlayerLeaveWater(playerid, waterid)
