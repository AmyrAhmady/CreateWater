/***********************************************************************************************
	CreateWater.inc | Create custom waters in samp and have ability to swim in them
	            by: iAmir
				Download : https://github.com/AmyrAhmady/CreateWater
				
				
		Functions :
		    CreateWater(minx , miny , maxx , maxy , z , depth);
			IsPlayerInWaterEx(playerid);
			GetPlayerWaterID(playerid);
			GetWaterDepth(waterid);
			GetPlayerSwimStyle(playerid);
			
		Callbacks :
		    OnPlayerEnterWater(playerid , waterid)
		    OnPlayerLeaveWater(playerid , waterid)

************************************************************************************************/
	            


#include a_samp
#include streamer

#define MAX_WATERS 500

enum CW_waterInfo
{
	waterArea,
	Float:waterMinX,
	Float:waterMaxX,
	Float:waterMinY,
	Float:waterMaxY,
	Float:waterZ,
	Float:waterD
}

new waterInfo[MAX_WATERS][CW_waterInfo];

enum CW_playerWaterInfo
{
	bool:playerInWater,
	playerWaterID,
	playerSwimStyle,
	bool:playerWaterTD
}

new playerWater[MAX_PLAYERS][CW_playerWaterInfo];

enum
{
	SWIM_STYLE_IDLE_ON_WATER,
	SWIM_STYLE_MOVE_ON_WATER,
	SWIM_STYLE_DIVE_UNDER_WATER,
	SWIM_STYLE_MOVE_UNDER_WATER,
	SWIM_STYLE_COMING_UP,
	SWIM_STYLE_MOVE_FAST_ON_WATER,
	SWIM_STYLE_INVALID
}

new waterCount=-1;
new Text:waterTD;
new waterCheckTimer;

forward CW_CheckWater();

forward OnPlayerEnterWater(playerid , waterid);
forward OnPlayerLeaveWater(playerid , waterid);


static Float:CW_abs(Float:val)
{
    if(val < 0.0) val = -1 * val;
    return val;
}

stock CreateWater(Float:minx , Float:miny , Float:maxx , Float: maxy , Float:z , Float:depth)
{
	static
	    xWayCount=0,
	    yWayCount=0,
	    xWay=1,
	    yWay=1;
	    
	    
	if(waterCount >= MAX_WATERS) return printf("You've reached the limit %d in creating new waters" , MAX_WATERS);
	    
    if((maxx - minx) < 0.0) xWay= -1;
	if((maxy - miny) < 0.0) yWay= -1;
	
	xWayCount = floatround(CW_abs((minx-maxx)) / 1.908086 , floatround_ceil) +1;
	yWayCount = floatround(CW_abs((miny-maxy)) / 2.13943 , floatround_ceil) +1;
	
	if(!xWayCount || !yWayCount) return -1;

	for(new i=0; i<xWayCount; i++)
	{
		new CW_shit = CreateDynamicObject(2691 , minx + ((2.13943 * i))*xWay , miny , z , -90.0 , 0.0 , 0.0);
		SetDynamicObjectMaterial(CW_shit , 0 , 19603 , "ballyswater" , "waterclear256");
		
		CW_shit = CreateDynamicObject(2691 , minx + ((2.13943 * i))*xWay , miny , z , 90.0 , 0.0 , 0.0);
		SetDynamicObjectMaterial(CW_shit , 0 , 19603 , "ballyswater" , "waterclear256");
		
		for(new j=1; j<yWayCount; j++)
		{
			new CW_shit1 = CreateDynamicObject(2691 , minx + ((2.13943 * i))*xWay  , miny + ((1.908086 * j))*yWay , z , -90.0 , 0.0 , 0.0);
		    SetDynamicObjectMaterial(CW_shit1 , 0 , 19603 , "ballyswater" , "waterclear256");
		    
		    CW_shit1 = CreateDynamicObject(2691 , minx + ((2.13943 * i))*xWay  , miny + ((1.908086 * j))*yWay , z , 90.0 , 0.0 , 0.0);
		    SetDynamicObjectMaterial(CW_shit1 , 0 , 19603 , "ballyswater" , "waterclear256");
		}
	}
	
	waterCount++;
	waterInfo[waterCount][waterMinX] = minx - ((2.13943 / 2))*xWay;
	waterInfo[waterCount][waterMaxX] = minx + ((2.13943 * xWayCount) - (2.13943 / 2)*xWay)*xWay;
	waterInfo[waterCount][waterMinY] = miny - ((1.908086 / 2))*yWay;
	waterInfo[waterCount][waterMaxY] = miny + ((1.908086 * yWayCount) - (1.908086 / 2)*yWay)*yWay;
	waterInfo[waterCount][waterZ] = z-0.1;
	waterInfo[waterCount][waterD] = depth;

	
	waterInfo[waterCount][waterArea] = CreateDynamicCube(minx - ((2.13943 / 2))*xWay , miny - ((1.908086 / 2))*yWay , z-0.30 , minx + ((2.13943 * xWayCount) - (2.13943 / 2)*xWay)*xWay , miny + ((1.908086 * yWayCount) - (1.908086 / 2)*yWay)*yWay , depth);

	return waterCount;
}

stock GetPlayerWaterID(playerid)
{
	return playerWater[playerid][playerWaterID];
}

stock IsPlayerInWaterEx(playerid)
{
	if(playerWater[playerid][playerWaterID] > -1) return 1;
	return 0;
}

stock Float:GetWaterDepth(waterid)
{
	return waterInfo[waterid][waterD];
}
		    
hook OnPlayerEnterDynamicArea(playerid , areaid)
{
	for(new i=0; i<MAX_WATERS; i++)
	{
	    if(areaid == waterInfo[i][waterArea])
	    {
			playerWater[playerid][playerInWater] = true;
			playerWater[playerid][playerWaterID] = i;
			CallRemoteFunction("OnPlayerEnterWater" , "ii" , playerid , i);
			return 1;
		}
	}
	return 1;
}

hook OnPlayerLeaveDynamicArea(playerid , areaid)
{
	for(new i=0; i<MAX_WATERS; i++)
	{
	    if(areaid == waterInfo[i][waterArea])
	    {
			playerWater[playerid][playerInWater] = false;
			playerWater[playerid][playerWaterID] = -1;
			
			if(GetPlayerSwimStyle(playerid) != SWIM_STYLE_INVALID) ClearAnimations(playerid);
			
			playerWater[playerid][playerSwimStyle] = SWIM_STYLE_INVALID;
			
			CallRemoteFunction("OnPlayerLeaveWater" , "ii" , playerid , i);
			return 1;
		}
	}
	return 1;
}

hook OnPlayerUpdate(playerid)
{

    	new Float:x,Float:y,Float:z;
		GetPlayerPos(playerid,x,y,z);

		if(IsPlayerConnected(playerid) && !IsPlayerNPC(playerid) && playerWater[playerid][playerInWater] == true)
		{
		
		    if(z < waterInfo[playerWater[playerid][playerWaterID]][waterZ] - 0.45)
		    {
		        if(!playerWater[playerid][playerWaterTD])
		        {
                	TextDrawShowForPlayer(playerid , waterTD);
                	playerWater[playerid][playerWaterTD] = true;
                }
			}
			else
			{
			    if(playerWater[playerid][playerWaterTD])
		        {
                	TextDrawHideForPlayer(playerid , waterTD);
                	playerWater[playerid][playerWaterTD] = false;
                }
            }
			
 			new k, ud,lr;
        	GetPlayerKeys(playerid,k,ud,lr);
        	new Float:v_x,Float:v_y,Float:v_z,
                Float:xv,Float:yv,Float:zv;
                
            GetPlayerCameraFrontVector(playerid,xv,yv,zv);
                
        	if(ud < 0)     
        	{
               		
                	v_x = (xv+0.01)/30.0;
                	v_y = (yv+0.01)/30.0;
                	
        	}
        	if(k & KEY_FIRE)
			{
                if(GetPlayerSwimStyle(playerid) == SWIM_STYLE_MOVE_ON_WATER
				|| GetPlayerSwimStyle(playerid) == SWIM_STYLE_MOVE_FAST_ON_WATER
				|| GetPlayerSwimStyle(playerid) == SWIM_STYLE_IDLE_ON_WATER)
				{
                	v_z = -0.18;
                	ApplyAnimation(playerid,"SWIM","Swim_Dive_Under",4.1,0,1,1,0,0,1);
                	playerWater[playerid][playerSwimStyle] = SWIM_STYLE_DIVE_UNDER_WATER;
                }
            }
        	if(k & KEY_SPRINT)      
        	{
                v_x *=2.0;
                v_y *=2.0;
                v_z *=2.0;
        	}
        	if(v_z == 0.0 && z < waterInfo[playerWater[playerid][playerWaterID]][waterZ] - 0.32 && GetPlayerSwimStyle(playerid) != SWIM_STYLE_MOVE_UNDER_WATER)
                v_z = 0.035;
        	
        	if(v_x == 0 && v_y == 0)
        	{
                if(GetPlayerSwimStyle(playerid) == SWIM_STYLE_MOVE_ON_WATER
				|| GetPlayerSwimStyle(playerid) == SWIM_STYLE_MOVE_FAST_ON_WATER
				|| z >= waterInfo[playerWater[playerid][playerWaterID]][waterZ] - 0.46)
				{
					ApplyAnimation(playerid,"SWIM","Swim_Tread",6.1,1,1,1,1,0,1);
					playerWater[playerid][playerSwimStyle] = SWIM_STYLE_IDLE_ON_WATER;
				}
				
				if(GetPlayerSwimStyle(playerid) == SWIM_STYLE_DIVE_UNDER_WATER || GetPlayerSwimStyle(playerid) == SWIM_STYLE_MOVE_UNDER_WATER || z < waterInfo[playerWater[playerid][playerWaterID]][waterZ] - 0.46)
				{
					ApplyAnimation(playerid,"SWIM","Swim_Glide",6.1,1,1,1,1,0,1);
                    playerWater[playerid][playerSwimStyle] = SWIM_STYLE_COMING_UP;
				}
                        
        	}
        	else
        	{
        	    new Float:cx , Float:cy , Float:cz;
        	    new Float:cx1 , Float:cy1 , Float:cz1;
                GetPlayerCameraFrontVector(playerid,cx,cy,cz);
                GetPlayerCameraPos(playerid,cx1,cy1,cz1);
                SetPlayerLookAt(playerid,cx*500.0+cx1,cy*500.0+cy1);
                
                if(k & KEY_SPRINT)
                {
                	if(GetPlayerSwimStyle(playerid) == SWIM_STYLE_IDLE_ON_WATER || GetPlayerSwimStyle(playerid) == SWIM_STYLE_MOVE_ON_WATER)
					{
						ApplyAnimation(playerid,"SWIM","SWIM_crawl",6.1,1,1,1,1,0,1);
						playerWater[playerid][playerSwimStyle] = SWIM_STYLE_MOVE_FAST_ON_WATER;
					}
                	
                	if(GetPlayerSwimStyle(playerid) == SWIM_STYLE_DIVE_UNDER_WATER || GetPlayerSwimStyle(playerid) == SWIM_STYLE_COMING_UP)
					{
						ApplyAnimation(playerid,"SWIM","Swim_Under",6.1,1,1,1,1,0,1);
                        playerWater[playerid][playerSwimStyle] = SWIM_STYLE_MOVE_UNDER_WATER;
                    }
                	
                	
                }
				else if(GetPlayerSwimStyle(playerid) == SWIM_STYLE_IDLE_ON_WATER || GetPlayerSwimStyle(playerid) == SWIM_STYLE_MOVE_FAST_ON_WATER)
				{
					ApplyAnimation(playerid,"SWIM","Swim_Breast",6.1,1,1,1,1,0,1);
					playerWater[playerid][playerSwimStyle] = SWIM_STYLE_MOVE_ON_WATER;
				}

                        
        	}
        	
        	SetPlayerVelocity(playerid,v_x,v_y,v_z);
 		
 		
 		
		}

		return 1;
}

stock SetPlayerLookAt(playerid, Float:X, Float:Y)
{
	new Float:Px, Float:Py, Float: Pa;
	GetPlayerPos(playerid, Px, Py, Pa);
	Pa = floatabs(atan((Y-Py)/(X-Px)));
	if (X <= Px && Y >= Py) Pa = floatsub(180, Pa);
	else if (X < Px && Y < Py) Pa = floatadd(Pa, 180);
	else if (X >= Px && Y <= Py) Pa = floatsub(360.0, Pa);
	Pa = floatsub(Pa, 90.0);
	if (Pa >= 360.0) Pa = floatsub(Pa, 360.0);
	SetPlayerFacingAngle(playerid, Pa);
}

stock GetPlayerSwimStyle(playerid)
{
	return playerWater[playerid][playerSwimStyle];
}

#if defined FILTERSCRIPT

hook OnFilterScriptInit()
{
    waterTD = TextDrawCreate(-20.000000,2.000000,"|");
    TextDrawUseBox(waterTD,1);
    TextDrawBoxColor(waterTD,0x22446644);
    TextDrawTextSize(waterTD,660.000000,22.000000);
    TextDrawAlignment(waterTD,0);
    TextDrawBackgroundColor(waterTD,0x22446644);
    TextDrawFont(waterTD,3);
    TextDrawLetterSize(waterTD,1.000000,52.200000);
    TextDrawColor(waterTD,0x22446644);
    TextDrawSetOutline(waterTD,1);
    TextDrawSetProportional(waterTD,1);
    TextDrawSetShadow(waterTD,1);
    return 1;
}

hook OnFilterScriptExit()
{
	TextDrawDestroy(waterTD);
    return 1;
}

#else

hook OnGameModeInit()
{

    waterTD = TextDrawCreate(-20.000000,2.000000,"|");
    TextDrawUseBox(waterTD,1);
    TextDrawBoxColor(waterTD,0x22446644);
    TextDrawTextSize(waterTD,660.000000,22.000000);
    TextDrawAlignment(waterTD,0);
    TextDrawBackgroundColor(waterTD,0x22446644);
    TextDrawFont(waterTD,3);
    TextDrawLetterSize(waterTD,1.000000,52.200000);
    TextDrawColor(waterTD,0x22446644);
    TextDrawSetOutline(waterTD,1);
    TextDrawSetProportional(waterTD,1);
    TextDrawSetShadow(waterTD,1);

    return 1;
}

#endif
