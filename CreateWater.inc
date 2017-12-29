/***********************************************************************************************
	CreateWater.inc | Create custom waters in samp and have ability to swim in them
	            by: iAmir
				Download : https://github.com/AmyrAhmady/CreateWater


		Functions :
		    CreateWater(minx , miny , maxx , maxy , z , depth , world=-1 , interior=-1);
			IsPlayerInWaterEx(playerid);
			GetPlayerWaterID(playerid);
			GetWaterDepth(waterid);
			GetPlayerSwimStyle(playerid);
			DestroyWater(waterid);

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
	Float:waterD,
	waterAvailable
}

new waterInfo[MAX_WATERS][CW_waterInfo];

enum CW_playerWaterInfo
{
	bool:playerInWater,
	playerWaterID,
	playerSwimStyle,
	bool:playerWaterTD,
	playerSwimObj
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

new Text:waterTD;

forward OnPlayerEnterWater(playerid , waterid);
forward OnPlayerLeaveWater(playerid , waterid);


static Float:CW_abs(Float:val)
{
    if(val < 0.0) val = -1 * val;
    return val;
}

stock CreateWater(Float:minx , Float:miny , Float:maxx , Float: maxy , Float:z , Float:depth , world=-1 , interior=-1)
{
	static
	    xWayCount=0,
	    yWayCount=0,
	    xWay=1,
	    yWay=1;


    new waterCount = FindFreeWaterSlot();
    if(waterCount == -1) return printf("You've reached the limit %d in creating new waters" , MAX_WATERS);
    

    if((maxx - minx) < 0.0) xWay= -1;
	if((maxy - miny) < 0.0) yWay= -1;

	xWayCount = floatround(CW_abs((minx-maxx)) / 1.908086 , floatround_ceil) +1;
	yWayCount = floatround(CW_abs((miny-maxy)) / 2.13943 , floatround_ceil) +1;

	if(!xWayCount || !yWayCount) return -1;

	for(new i=0; i<xWayCount; i++)
	{
		new CW_shit = CreateDynamicObject(2691 , minx + ((2.13943 * i))*xWay , miny , z , -90.0 , 0.0 , 0.0 , world , interior);
		SetDynamicObjectMaterial(CW_shit , 0 , 19603 , "ballyswater" , "waterclear256");
		Streamer_SetIntData(STREAMER_TYPE_OBJECT, CW_shit, E_STREAMER_EXTRA_ID, 666);

		CW_shit = CreateDynamicObject(2691 , minx + ((2.13943 * i))*xWay , miny , z , 90.0 , 0.0 , 0.0 , world , interior);
		SetDynamicObjectMaterial(CW_shit , 0 , 19603 , "ballyswater" , "waterclear256");
		Streamer_SetIntData(STREAMER_TYPE_OBJECT, CW_shit, E_STREAMER_EXTRA_ID, 666);

		for(new j=1; j<yWayCount; j++)
		{
			new CW_shit1 = CreateDynamicObject(2691 , minx + ((2.13943 * i))*xWay  , miny + ((1.908086 * j))*yWay , z , -90.0 , 0.0 , 0.0 , world , interior);
		    SetDynamicObjectMaterial(CW_shit1 , 0 , 19603 , "ballyswater" , "waterclear256");
		    Streamer_SetIntData(STREAMER_TYPE_OBJECT, CW_shit1, E_STREAMER_EXTRA_ID, 666);

		    CW_shit1 = CreateDynamicObject(2691 , minx + ((2.13943 * i))*xWay  , miny + ((1.908086 * j))*yWay , z , 90.0 , 0.0 , 0.0 , world , interior);
		    SetDynamicObjectMaterial(CW_shit1 , 0 , 19603 , "ballyswater" , "waterclear256");
		    Streamer_SetIntData(STREAMER_TYPE_OBJECT, CW_shit1, E_STREAMER_EXTRA_ID, 666);
		}
	}

	waterInfo[waterCount][waterMinX] = minx - ((2.13943 / 2))*xWay;
	waterInfo[waterCount][waterMaxX] = minx + ((2.13943 * xWayCount) - (2.13943 / 2)*xWay)*xWay;
	waterInfo[waterCount][waterMinY] = miny - ((1.908086 / 2))*yWay;
	waterInfo[waterCount][waterMaxY] = miny + ((1.908086 * yWayCount) - (1.908086 / 2)*yWay)*yWay;
	waterInfo[waterCount][waterZ] = z-0.1;
	waterInfo[waterCount][waterD] = depth;
	waterInfo[waterCount][waterAvailable] = 0;


	waterInfo[waterCount][waterArea] = CreateDynamicCube(minx - ((2.13943 / 2))*xWay , miny - ((1.908086 / 2))*yWay , z-0.25 , minx + ((2.13943 * xWayCount) - (2.13943 / 2)*xWay)*xWay , miny + ((1.908086 * yWayCount) - (1.908086 / 2)*yWay)*yWay , depth  , world , interior);

    Streamer_SetIntData(STREAMER_TYPE_AREA, waterInfo[waterCount][waterArea], E_STREAMER_EXTRA_ID, waterCount);

	return waterCount;
}

stock DestroyWater(waterid)
{

    if(waterInfo[waterid][waterAvailable] == 0)
	{
        new j = Streamer_GetUpperBound(STREAMER_TYPE_OBJECT);
    	for(new i=0; i<j; i++)
    	{
			if(IsValidDynamicObject(i))
			{
        		new eid = Streamer_GetIntData(STREAMER_TYPE_OBJECT, i, E_STREAMER_EXTRA_ID);
        		if(eid == 666)
				{
					Streamer_SetIntData(STREAMER_TYPE_OBJECT, i, E_STREAMER_EXTRA_ID, 0);
					DestroyDynamicObject(i);
				}
			}
		}
		
		DestroyDynamicArea(waterInfo[waterid][waterArea]);
	    waterInfo[waterid][waterMinX] = 0.0;
		waterInfo[waterid][waterMaxX] = 0.0;
		waterInfo[waterid][waterMinY] = 0.0;
		waterInfo[waterid][waterMaxY] = 0.0;
		waterInfo[waterid][waterZ] = 0.0;
		waterInfo[waterid][waterD] = 0.0;
		waterInfo[waterid][waterAvailable] = 1;
		waterInfo[waterid][waterArea] = -1;
		
		return 1;
		
	}
	return 0;
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

stock FindFreeWaterSlot()
{
	for(new i=0; i<MAX_WATERS; i++)
	{
		if(waterInfo[i][waterAvailable] == 1) return i;
	}
	return -1;
}

public OnPlayerEnterDynamicArea(playerid , areaid)
{
	new i = Streamer_GetIntData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID);
	if(0 <= i < MAX_WATERS)
	{
		playerWater[playerid][playerInWater] = true;
		playerWater[playerid][playerWaterID] = i;
		CallRemoteFunction("OnPlayerEnterWater" , "ii" , playerid , i);
		return 1;

	}
	return CallLocalFunction("CW_OnPlayerEnterDynamicArea", "ii", playerid , areaid);
}

#if defined _ALS_OnPlayerEnterDynamicArea
	#undef OnPlayerEnterDynamicArea
#else
	#define _ALS_OnPlayerEnterDynamicArea
#endif
#define OnPlayerEnterDynamicArea CW_OnPlayerEnterDynamicArea
forward CW_OnPlayerEnterDynamicArea(playerid, areaid);

public OnPlayerLeaveDynamicArea(playerid , areaid)
{
	new i = Streamer_GetIntData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID);
	if(0 <= i < MAX_WATERS)
	{
		playerWater[playerid][playerInWater] = false;
		playerWater[playerid][playerWaterID] = -1;
		if(GetPlayerSwimStyle(playerid) != SWIM_STYLE_INVALID) ClearAnimations(playerid);
		playerWater[playerid][playerSwimStyle] = SWIM_STYLE_INVALID;
		TextDrawHideForPlayer(playerid , waterTD);
		CallRemoteFunction("OnPlayerLeaveWater" , "ii" , playerid , i);
		return 1;

	}
	return CallLocalFunction("CW_OnPlayerLeaveDynamicArea", "ii", playerid , areaid);
}

#if defined _ALS_OnPlayerLeaveDynamicArea
	#undef OnPlayerLeaveDynamicArea
#else
	#define _ALS_OnPlayerLeaveDynamicArea
#endif
#define OnPlayerLeaveDynamicArea CW_OnPlayerLeaveDynamicArea
forward CW_OnPlayerLeaveDynamicArea(playerid, areaid);

public OnPlayerUpdate(playerid)
{
		if(!IsPlayerNPC(playerid) && playerWater[playerid][playerInWater] == true)
		{
		
			new Float:x,Float:y,Float:z;
			GetPlayerPos(playerid,x,y,z);

			if(GetPlayerSwimStyle(playerid) == SWIM_STYLE_IDLE_ON_WATER)
			{
			    if(!IsValidDynamicObject(playerWater[playerid][playerSwimObj]))
				{
					playerWater[playerid][playerSwimObj] = CreateDynamicObject(2928 , x , y , waterInfo[playerWater[playerid][playerWaterID]][waterZ] - 1.34 , 90.0 , 0.0 , 0.0);
					SetDynamicObjectMaterial(playerWater[playerid][playerSwimObj] , 0 , 0, "none", "none", 0x00000000);
				}
			} 
			else if(GetPlayerSwimStyle(playerid) == SWIM_STYLE_MOVE_ON_WATER)
			{
			    if(!IsValidDynamicObject(playerWater[playerid][playerSwimObj]))
				{
					playerWater[playerid][playerSwimObj] = CreateDynamicObject(2928 , x , y , waterInfo[playerWater[playerid][playerWaterID]][waterZ] - 1.36 , 90.0 , 0.0 , 0.0);
					SetDynamicObjectMaterial(playerWater[playerid][playerSwimObj] , 0 , 0, "none", "none", 0x00000000);
				}
				MoveDynamicObject(playerWater[playerid][playerSwimObj] , x , y , waterInfo[playerWater[playerid][playerWaterID]][waterZ] - 1.36 , 5.54);
			}
			else if(GetPlayerSwimStyle(playerid) == SWIM_STYLE_MOVE_FAST_ON_WATER)
			{
			    if(!IsValidDynamicObject(playerWater[playerid][playerSwimObj]))
				{
					playerWater[playerid][playerSwimObj] = CreateDynamicObject(2928 , x , y , waterInfo[playerWater[playerid][playerWaterID]][waterZ] - 1.36 , 90.0 , 0.0 , 0.0);
					SetDynamicObjectMaterial(playerWater[playerid][playerSwimObj] , 0 , 0, "none", "none", 0x00000000);
				}
				MoveDynamicObject(playerWater[playerid][playerSwimObj] , x , y , waterInfo[playerWater[playerid][playerWaterID]][waterZ] - 1.36 , 17.24);
			}
			else { if(IsValidDynamicObject(playerWater[playerid][playerSwimObj])) DestroyDynamicObject(playerWater[playerid][playerSwimObj]); }
			
			
			
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
                	v_x = (xv+0.001)/4000.0;
                	v_y = (yv+0.001)/4000.0;
        	} else 
        	if(k & KEY_FIRE)
			{
                if(GetPlayerSwimStyle(playerid) == SWIM_STYLE_MOVE_ON_WATER
				|| GetPlayerSwimStyle(playerid) == SWIM_STYLE_MOVE_FAST_ON_WATER
				|| GetPlayerSwimStyle(playerid) == SWIM_STYLE_IDLE_ON_WATER)
				{
                	v_z = -0.22;
                	ApplyAnimation(playerid,"SWIM","Swim_Dive_Under",4.1,0,1,1,0,0,1);
                	playerWater[playerid][playerSwimStyle] = SWIM_STYLE_DIVE_UNDER_WATER;
                	if(IsValidDynamicObject(playerWater[playerid][playerSwimObj])) DestroyDynamicObject(playerWater[playerid][playerSwimObj]);
                }
            }
        	if(k & KEY_SPRINT)
        	{
                v_x *=2.0;
                v_y *=2.0;
                v_z *=2.0;
        	}
        	if(v_z == 0.0 && z < waterInfo[playerWater[playerid][playerWaterID]][waterZ] - 0.32 && GetPlayerSwimStyle(playerid) == SWIM_STYLE_COMING_UP)
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

		return CallLocalFunction("CW_OnPlayerUpdate", "i", playerid);
}

#if defined _ALS_OnPlayerUpdate
	#undef OnPlayerUpdate
#else
	#define _ALS_OnPlayerUpdate
#endif
#define OnPlayerUpdate CW_OnPlayerUpdate
forward CW_OnPlayerUpdate(playerid);

stock SetPlayerLookAt(playerid, Float:X, Float:Y) // credits to someone in samp forums I can not remember
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

public OnFilterScriptInit()
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
    
    for(new i=0; i<MAX_WATERS; i++) waterInfo[i][waterAvailable] = 1;
    
    return CallLocalFunction("CW_OnFilterScriptInit", "");
}

#if defined _ALS_OnFilterScriptInit
	#undef OnFilterScriptInit
#else
	#define _ALS_OnFilterScriptInit
#endif
#define OnFilterScriptInit CW_OnFilterScriptInit
forward CW_OnFilterScriptInit(playerid);

public OnFilterScriptExit()
{
	TextDrawDestroy(waterTD);
    return CallLocalFunction("CW_OnFilterScriptExit", "");
}

#if defined _ALS_OnFilterScriptExit
	#undef OnFilterScriptExit
#else
	#define _ALS_OnFilterScriptExit
#endif
#define OnFilterScriptExit CW_OnFilterScriptExit
forward CW_OnFilterScriptExit(playerid);

#else

public OnGameModeInit()
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
    
    for(new i=0; i<MAX_WATERS; i++) waterInfo[i][waterAvailable] = 1;
    
	return CallLocalFunction("CW_OnGameModeInit", "");
}

#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif
#define OnGameModeInit CW_OnGameModeInit
forward CW_OnGameModeInit(playerid);

public OnGameModeExit()
{
    TextDrawDestroy(waterTD);
    return CallLocalFunction("CW_OnGameModeExit", "");
}

#if defined _ALS_OnGameModeExit
	#undef OnGameModeExit
#else
	#define _ALS_OnGameModeExit
#endif
#define OnGameModeExit CW_OnGameModeExit
forward CW_OnGameModeExit(playerid);

#endif
