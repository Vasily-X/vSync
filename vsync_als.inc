/*
	vSync wrote by RIDE2DAY, based on vFunc by MP2.
	Official thread: http://forum.sa-mp.com/showthread.php?t=642022
*/

// ================================== [DEFINITIONS] ================================== //
#if !defined isodd
	#define isodd(%1)				((%1) & 1)
#endif

#if !defined strcpy
 	#define strcpy(%0,%1) 			strcat((%0[0] = EOS, %0), %1)
#endif

#if !defined RELEASED
	#define RELEASED(%0)			(((newkeys & (%0)) != (%0)) && ((oldkeys & (%0)) == (%0)))
#endif

#define	NEON_LIGHT_RED				18647
#define NEON_LIGHT_BLUE				18648
#define NEON_LIGHT_GREEN			18649
#define NEON_LIGHT_YELLOW			18650
#define	NEON_LIGHT_PINK 			18651
#define NEON_LIGHT_WHITE			18652

#define MAX_VEHICLE_NAME			31
#define MAX_VEHICLE_NUMBERPLATE		32
#define INVALID_PAINTJOB_ID 		3
#define CARMODTYPE_FRONT_BULLBAR 	14
#define CARMODTYPE_REAR_BULLBAR 	15

#define IsComponentRearBullbar(%0)	(%0 == 1109 || %0 == 1110)

forward OnVehicleCreated(vehicleid);
forward OnVehicleChangePaintjob(playerid, vehicleid, paintjobid);


// ================================== [VARIABLES] ================================== //
static vSync_pTick[MAX_PLAYERS];
static vSync_pPaintjobID[MAX_PLAYERS char];

static bool:vSync_pInsideModShop[MAX_PLAYERS char];
static bool:vSync_pBuyingPaintjob[MAX_PLAYERS char];

enum vSync_Internal_Data
{
	vSync_Color1,
	vSync_Color2,
	vSync_OrgCol1,
	vSync_OrgCol2,
	vSync_Paintjob,
	vSync_Interior,
	vSync_NumberP[MAX_VEHICLE_NUMBERPLATE],
	vSync_Siren,
	vSync_Stereo,
	vSync_Bumpers[2],
	vSync_Bullbars[2],
	vSync_Neon[2]
}
static vSync_gVehicleInternal[MAX_VEHICLES][vSync_Internal_Data];
		
static Float:vSync_gPaynSprayPos[10][3] = {
    {1025.05, -1024.23, 32.1}, 	// LS Temple
    {487.68, -1740.87, 11.13}, 	// LS Santa Maria
    {-1420.73, 2583.37, 55.56}, // El Quebrados
    {-1904.39, 284.97, 40.75}, 	// Wang Cars
    {-2425.91, 1022.33, 50.10}, // Juniper Hill
    {1975.60, 2162.16, 10.77}, 	// LV Redsands
    {2065.38, -1831.51, 13.25}, // Idlewood
    {-99.55, 1118.36, 19.44}, 	// Fort Carson
    {721.07, -455.94, 16.04}, 	// Dillimore
    {2393.74, 1493.01, 10.52} 	// LV Unused (Pyramid)
};

enum vSync_Vehicle_Info
{
	Name[MAX_VEHICLE_NAME],
	Seats,
	Float:Mass,
	DriveType,
	EngineType,
	MaxSpeed,
	Float:NeonOffsetX,
	Float:NeonOffsetY,
    Float:NeonOffsetZ
}

static const vSync_VehicleInfo[212][vSync_Vehicle_Info] = {
	{"Landstalker", 4, 1700.0, '4', 'D', 159, -0.924997, 0.009998, -0.674997},
	{"Bravura", 2, 1300.0, 'F', 'P', 148, -0.979999, 0.074997, -0.489998},
	{"Buffalo", 2, 1500.0, 'R', 'P', 188, -1.034999, -0.029998, -0.579999},
	{"Linerunner", 2, 3800.0, 'R', 'D', 111, -0.884998, 1.834998, -1.099997},
	{"Perennial", 4, 1200.0, 'F', 'P', 134, -0.839999, -0.064998, -0.419999},
	{"Sentinel", 4, 1600.0, 'R', 'P', 165, -0.914999, 0.000000, -0.534999},
	{"Dumper", 1, 20000.0, 'R', 'D', 111, -1.129999, 0.354999, -1.219998},
	{"Firetruck", 2, 6500.0, 'R', 'D', 150, -1.214998, 0.125000, -0.759998},
	{"Trashmaster", 2, 5500.0, 'R', 'D', 101, -0.664999, 0.709999, -0.889998},
	{"Stretch", 4, 2200.0, 'R', 'P', 159, -0.914999, -0.294999, -0.474999},
	{"Manana", 2, 1000.0, 'F', 'P', 131, -0.879998, 0.000000, -0.389999},
	{"Infernus", 2, 1400.0, '4', 'P', 223, -0.969999, 0.004999, -0.559997},
	{"Voodoo", 2, 1800.0, 'R', 'P', 170, -0.994997, -0.154999, -0.569998},
	{"Pony", 4, 2600.0, 'R', 'D', 111, -0.919999, -0.069999, -0.719999},
	{"Mule", 2, 3500.0, 'R', 'D', 107, -0.759998, -0.469998, -0.524999},
	{"Cheetah", 2, 1200.0, 'R', 'P', 194, -0.949998, -0.009998, -0.524999},
	{"Ambulance", 4, 2600.0, '4', 'D', 155, -1.259999, -0.604999, -0.664999},
	{"Leviathan", 2, 15000.0, '4', 'P', 143, -2.255002, 0.859999, -0.124999},
	{"Moonbeam", 4, 2000.0, 'R', 'D', 116, -1.004999, 0.089997, -0.829999},
	{"Esperanto", 2, 1800.0, 'R', 'P', 150, -0.934997, -0.009998, -0.584999},
	{"Taxi", 4, 1450.0, 'F', 'P', 146, -0.954999, -0.009998, -0.444999},
	{"Washington", 2, 1850.0, 'R', 'P', 155, -0.929997, -0.029998, -0.589999},
	{"Bobcat", 2, 1700.0, '4', 'D', 141, -0.889998, 0.009998, -0.609999},
	{"Mr. Whoopee", 2, 1700.0, 'R', 'D', 100, -0.999997, 0.164997, -0.714999},
	{"BF Injection", 2, 1200.0, 'R', 'P', 137, -0.739997, 0.039999, -0.234999},
	{"Hunter", 1, 10000.0, '4', 'P', 214, -0.639998, 2.475008, -0.789999},
	{"Premier", 4, 1600.0, 'R', 'P', 175, -0.969999, 0.000000, -0.464998},
	{"Enforcer", 4, 4000.0, 'R', 'D', 167, -1.124999, -0.714999, -0.734999},
	{"Securicar", 4, 7000.0, 'R', 'D', 158, -1.124999, 0.054999, -0.759998},
	{"Banshee", 2, 1400.0, 'R', 'P', 203, -0.844999, 0.004999, -0.474999},
	{"Predator", 1, 2200.0, 'R', 'P', 184, 0.000000, 0.000000, 0.000000},
	{"Bus", 9, 5500.0, 'R', 'D', 131, -1.304998, 2.300004, -0.714999},
	{"Rhino", 1, 25000.0, '4', 'D', 95, 0.000000, 0.000000, 0.000000},
	{"Barracks", 2, 10500.0, '4', 'D', 112, -0.579999, 0.994997, -0.874997},
	{"Hotknife", 2, 1400.0, 'R', 'P', 169, 0.000000, 0.000000, 0.000000},
	{"Article Trailer", 1, 3800.0, 'R', 'D', 0, 0.000000, 0.000000, 0.000000},
	{"Previon", 2, 1400.0, 'F', 'P', 150, -0.859999, -0.024999, -0.489998},
	{"Coach", 9, 9500.0, 'R', 'D', 159, -1.314998, 2.245002, -0.724999},
	{"Cabbie", 4, 1750.0, 'R', 'P', 144, -0.969999, -0.034999, -0.674997},
	{"Stallion", 2, 1600.0, 'R', 'P', 170, -0.884998, 0.034999, -0.644999},
	{"Rumpo", 4, 2000.0, 'F', 'P', 137, -0.914999, -0.045000, -0.899999},
	{"RC Bandit", 1, 100.0, '4', 'E', 76, 0.000000, 0.000000, 0.000000},
	{"Romero", 2, 2500.0, 'R', 'P', 140, -1.004999, -0.074997, -0.604999},
	{"Packer", 2, 8000.0, 'R', 'D', 127, -0.809997, 2.770015, -1.174998},
	{"Monster", 2, 5000.0, '4', 'P', 111, -1.064998, 0.000000, 0.135000},
	{"Admiral", 4, 1650.0, 'F', 'P', 165, -0.949998, 0.000000, -0.519999},
	{"Squallo", 1, 2200.0, 'R', 'P', 240, 0.000000, 0.000000, 0.000000},
	{"Seasparrow", 2, 3000.0, '4', 'P', 145, -1.004999, 0.259999, -0.769999},
	{"Pizzaboy", 1, 350.0, 'R', 'P', 190, 0.000000, 0.000000, 0.000000},
	{"Tram", 2, 1900.0, 'R', 'P', 185, 0.000000, 0.000000, 0.000000},
	{"Article Trailer 2", 1, 3800.0, 'R', 'D', 0, 0.000000, 0.000000, 0.000000},
	{"Turismo", 2, 1400.0, '4', 'P', 195, -0.984999, -0.194997, -0.519999},
	{"Speeder", 1, 2200.0, 'R', 'P', 183, 0.000000, 0.000000, 0.000000},
	{"Reefer", 1, 5000.0, 'R', 'P', 57, 0.000000, 0.000000, 0.000000},
	{"Tropic", 1, 2200.0, 'R', 'P', 130, 0.000000, 0.000000, 0.000000},
	{"Flatbed", 2, 8500.0, 'R', 'D', 159, -0.579999, 0.979999, -0.859999},
	{"Yankee", 2, 4500.0, 'R', 'D', 107, -0.764998, -0.179997, -0.464998},
	{"Caddy", 2, 1000.0, '4', 'E', 96, 0.000000, 0.000000, 0.000000},
	{"Solair", 4, 2000.0, 'R', 'P', 159, -0.959999, -0.039999, -0.594999},
	{"Topfun Van (Berkley's RC)", 4, 1900.0, 'R', 'D', 137, -0.919999, -0.069999, -0.719999},
	{"Skimmer", 2, 5000.0, '4', 'P', 135, -1.399996, 0.554997, -1.819998},
	{"PCJ-600", 2, 500.0, 'R', 'P', 207, 0.000000, 0.000000, 0.000000},
	{"Faggio", 2, 350.0, 'R', 'P', 190, 0.000000, 0.000000, 0.000000},
	{"Freeway", 2, 800.0, 'R', 'P', 210, 0.000000, 0.000000, 0.000000},
	{"RC Baron", 1, 100.0, 'F', 'P', 45, 0.000000, 0.000000, 0.000000},
	{"RC Raider", 1, 100.0, '4', 'P', 50, 0.000000, 0.000000, 0.000000},
	{"Glendale", 4, 1600.0, 'R', 'P', 148, -0.989997, -0.064998, -0.489998},
	{"Oceanic", 4, 1900.0, 'R', 'P', 142, -0.989997, -0.044998, -0.484999},
	{"Sanchez", 2, 500.0, 'R', 'P', 186, 0.000000, 0.000000, 0.000000},
	{"Sparrow", 2, 2500.0, '4', 'P', 145, 0.000000, 0.000000, 0.000000},
	{"Patriot", 4, 2500.0, '4', 'P', 158, -1.119999, -0.149997, -0.359999},
	{"Quad", 2, 400.0, '4', 'P', 111, 0.000000, 0.000000, 0.000000},
	{"Coastguard", 1, 1200.0, 'R', 'P', 125, 0.000000, 0.000000, 0.000000},
	{"Dinghy", 1, 800.0, 'R', 'P', 107, 0.000000, 0.000000, 0.000000},
	{"Hermes", 2, 1950.0, 'F', 'P', 150, -0.994997, -0.064998, -0.544999},
	{"Sabre", 2, 1700.0, 'R', 'P', 175, -0.904999, -0.054999, -0.549997},
	{"Rustler", 1, 5000.0, '4', 'P', 220, 0.000000, 0.000000, 0.000000},
	{"ZR-350", 2, 1400.0, 'R', 'P', 188, -0.979999, 0.019997, -0.514998},
	{"Walton", 2, 1850.0, '4', 'D', 119, -0.804997, 0.064998, -0.519999},
	{"Regina", 4, 1500.0, 'F', 'P', 141, -0.854999, 0.090000, -0.454997},
	{"Comet", 2, 1400.0, '4', 'P', 186, -0.879998, 0.000000, -0.489998},
	{"BMX", 1, 100.0, 'R', 'P', 97, 0.000000, 0.000000, 0.000000},
	{"Burrito", 2, 1900.0, 'R', 'P', 158, -0.989997, 0.000000, -0.759998},
	{"Camper", 3, 1900.0, 'R', 'P', 124, -0.834999, -0.149997, -0.744997},
	{"Marquis", 1, 5000.0, 'R', 'P', 64, 0.000000, 0.000000, 0.000000},
	{"Baggage", 1, 1000.0, 'R', 'E', 100, -0.629998, 0.164997, -0.374998},
	{"Dozer", 1, 10000.0, '4', 'D', 65, 0.000000, 0.000000, 0.000000},
	{"Maverick", 4, 5000.0, '4', 'P', 181, 0.000000, 0.000000, 0.000000},
	{"News Chopper", 2, 3500.0, '4', 'P', 170, 0.000000, 0.000000, 0.000000},
	{"Rancher", 2, 2500.0, '4', 'P', 141, -1.019999, 0.109999, -0.679997},
	{"FBI Rancher", 4, 3500.0, '4', 'P', 158, -1.054998, 0.125000, -0.604999},
	{"Virgo", 2, 1700.0, 'R', 'P', 150, -0.874997, -0.009998, -0.569998},
	{"Greenwood", 4, 1600.0, 'R', 'P', 142, -0.909999, 0.000000, -0.459998},
	{"Jetmax", 1, 3000.0, 'R', 'P', 183, 0.000000, 0.000000, 0.000000},
	{"Hotring Racer", 2, 1600.0, 'R', 'P', 217, -0.909999, -0.199999, -0.639998},
	{"Sandking", 2, 2000.0, '4', 'P', 178, -1.024999, 0.184999, -0.809997},
	{"Blista Compact", 2, 1000.0, 'F', 'P', 164, -0.854999, 0.029998, -0.469998},
	{"Police Maverick", 4, 4500.0, '4', 'P', 190, 0.000000, 0.000000, 0.000000},
	{"Boxville", 4, 5500.0, 'R', 'D', 109, -1.149999, 0.269998, -0.654999},
	{"Benson", 2, 3500.0, 'R', 'D', 124, -0.969999, -1.404996, -0.199999},
	{"Mesa", 2, 1300.0, '4', 'D', 142, -0.969999, 0.189998, -0.574998},
	{"RC Goblin", 1, 100.0, '4', 'P', 30, 0.000000, 0.000000, 0.000000},
	{"Hotring Racer A", 2, 1600.0, 'R', 'P', 217, -0.909999, 0.029998, -0.639998},
	{"Hotring Racer B", 2, 1600.0, 'R', 'P', 217, -0.909999, -0.184999, -0.639998},
	{"Bloodring Banger", 2, 2100.0, 'R', 'P', 175, -0.989997, -0.059999, -0.494998},
	{"Rancher (black-tinted windows)", 2, 2500.0, '4', 'P', 141, -1.019999, 0.109999, -0.679997},
	{"Super GT", 2, 1400.0, 'R', 'P', 181, -0.904999, -0.249999, -0.509998},
	{"Elegant", 4, 2200.0, 'R', 'P', 167, -1.084998, -0.024999, -0.589999},
	{"Journey", 2, 3500.0, 'R', 'D', 109, -1.334998, -0.424997, -0.979999},
	{"Bike", 1, 100.0, 'R', 'P', 105, 0.000000, 0.000000, 0.000000},
	{"Mountain Bike", 1, 100.0, 'R', 'P', 130, 0.000000, 0.000000, 0.000000},
	{"Beagle", 2, 10000.0, '4', 'P', 133, 0.000000, 0.000000, 0.000000},
	{"Cropduster", 1, 5000.0, '4', 'P', 115, 0.000000, 0.000000, 0.000000},
	{"Stuntplane", 1, 5000.0, '4', 'P', 151, 0.000000, 0.000000, 0.000000},
	{"Tanker", 2, 3800.0, 'R', 'D', 122, -0.969999, 1.669998, -1.174998},
	{"Roadtrain", 2, 5000.0, 'R', 'D', 144, -1.454998, 0.439999, -1.854997},
	{"Nebula", 4, 1400.0, 'F', 'P', 159, -0.989997, 0.024999, -0.584999},
	{"Majestic", 2, 1400.0, 'R', 'P', 159, -0.959999, 0.004999, -0.554997},
	{"Buccaneer", 2, 1700.0, 'R', 'P', 166, -1.049998, 0.109999, -0.439999},
	{"Shamal", 1, 15000.0, '4', 'P', 272, 0.000000, 0.000000, 0.000000},
	{"Hydra", 1, 9000.0, '4', 'P', 272, 0.000000, 0.000000, 0.000000},
	{"FCR-900", 2, 500.0, 'R', 'P', 209, 0.000000, 0.000000, 0.000000},
	{"NRG-500", 2, 400.0, 'R', 'P', 209, 0.000000, 0.000000, 0.000000},
	{"HPV1000", 2, 500.0, 'R', 'P', 197, 0.000000, 0.000000, 0.000000},
	{"Cement Truck", 2, 5500.0, 'R', 'D', 131, -0.689997, 1.489997, -1.319998},
	{"Towtruck", 2, 3500.0, 'R', 'D', 162, -1.054998, 0.499998, -0.329997},
	{"Fortune", 2, 1700.0, 'R', 'P', 159, -0.899999, 0.000000, -0.564997},
	{"Cadrona", 2, 1200.0, 'R', 'P', 150, -0.944998, 0.154999, -0.419999},
	{"FBI Truck", 2, 4000.0, '4', 'D', 178, -0.924997, 0.059999, -0.529999},
	{"Willard", 4, 1800.0, 'R', 'P', 150, -0.934997, -0.109999, -0.429998},
	{"Forklift", 1, 1000.0, 'F', 'E', 61, 0.000000, 0.000000, 0.000000},
	{"Tractor", 1, 2000.0, 'R', 'D', 71, -0.144997, 0.314999, -0.444999},
	{"Combine Harvester", 1, 8500.0, '4', 'D', 111, 0.000000, 0.000000, 0.000000},
	{"Feltzer", 2, 1600.0, 'R', 'P', 169, -0.924997, -0.034999, -0.479999},
	{"Remington", 2, 1800.0, 'R', 'P', 170, -0.984999, 0.164997, -0.564997},
	{"Slamvan", 2, 1950.0, 'R', 'P', 160, -1.069998, -0.034999, -0.569998},
	{"Blade", 2, 1500.0, 'R', 'P', 175, -0.954999, -0.019997, -0.579999},
	{"Freight", 2, 5500.0, 'R', 'D', 0, 0.000000, 0.000000, 0.000000},
	{"Brown Streak", 2, 5500.0, 'R', 'D', 0, 0.000000, 0.000000, 0.000000},
	{"Vortex", 1, 1900.0, 'R', 'P', 101, -1.004999, -0.049998, -0.019997},
	{"Vincent", 4, 1800.0, 'F', 'P', 150, -1.024999, -0.090000, -0.639998},
	{"Bullet", 2, 1200.0, 'R', 'P', 205, -0.904999, 0.000000, -0.414999},
	{"Clover", 2, 1600.0, 'R', 'P', 166, -0.894999, 0.014999, -0.539999},
	{"Sadler", 2, 1700.0, '4', 'D', 152, -0.904999, 0.009998, -0.409999},
	{"Firetruck (ladder variant)", 2, 6500.0, 'R', 'D', 150, -1.089998, 0.000000, -0.709999},
	{"Hustler", 2, 1700.0, 'R', 'P', 148, -0.939997, -0.029998, -0.564997},
	{"Intruder", 4, 1800.0, 'R', 'P', 150, -1.039999, -0.100000, -0.474999},
	{"Primo", 4, 1600.0, 'R', 'P', 144, -0.884998, 0.059999, -0.479999},
	{"Cargobob", 2, 20000.0, '4', 'P', 158, -1.769997, 1.484997, -1.774996},
	{"Tampa", 2, 1700.0, 'R', 'P', 155, -0.949998, 0.104998, -0.439999},
	{"Sunrise", 4, 1600.0, 'F', 'P', 146, -1.089998, -0.059999, -0.554997},
	{"Merit", 4, 1800.0, 'R', 'P', 159, -0.964999, 0.004999, -0.564997},
	{"Utility Van", 2, 2600.0, 'R', 'D', 122, -1.034999, 0.749997, -0.254999},
	{"Nevada", 1, 25000.0, '4', 'P', 193, 0.000000, 0.000000, 0.000000},
	{"Yosemite", 2, 3000.0, 'R', 'P', 145, -1.109997, 0.009998, -0.534999},
	{"Windsor", 2, 1500.0, 'R', 'P', 160, -0.754998, 0.000000, -0.464998},
	{"Monster A", 2, 5000.0, '4', 'P', 111, -1.119999, 0.000000, 0.184999},
	{"Monster B", 2, 5000.0, '4', 'P', 111, -1.129999, 0.044998, 0.074997},
	{"Uranus", 2, 1400.0, 'R', 'P', 157, -0.894999, -0.089997, -0.374998},
	{"Jester", 2, 1500.0, 'F', 'P', 179, -0.964999, 0.000000, -0.444999},
	{"Sultan", 4, 1400.0, '4', 'P', 171, -1.004999, 0.054999, -0.414999},
	{"Stratum", 4, 1800.0, 'R', 'P', 155, -0.919999, 0.129998, -0.574998},
	{"Elegy", 2, 1500.0, 'R', 'P', 179, -0.909999, 0.119998, -0.404998},
	{"Raindance", 2, 10000.0, '4', 'P', 160, 0.000000, 0.000000, 0.000000},
	{"RC Tiger", 1, 100.0, '4', 'E', 89, 0.000000, 0.000000, 0.000000},
	{"Flash", 2, 1400.0, 'F', 'P', 166, -0.824998, 0.000000, -0.414999},
	{"Tahoma", 4, 1800.0, 'R', 'P', 161, -0.989997, -0.049998, -0.479999},
	{"Savanna", 4, 1500.0, 'R', 'P', 175, -1.089998, 0.309998, -0.609999},
	{"Bandito", 1, 1000.0, 'R', 'P', 148, -0.334998, 0.100000, -0.374998},
	{"Freight Flat Trailer", 1, 5500.0, 'R', 'D', 0, 0.000000, 0.000000, 0.000000},
	{"Brown Streak Trailer", 1, 5500.0, 'R', 'D', 0, 0.000000, 0.000000, 0.000000},
	{"Kart", 1, 300.0, 'R', 'P', 94, 0.000000, 0.000000, 0.000000},
	{"Mower", 1, 800.0, 'R', 'P', 61, 0.000000, 0.000000, 0.000000},
	{"Dune", 2, 10000.0, '4', 'P', 111, 0.000000, 0.000000, 0.000000},
	{"Sweeper", 1, 800.0, 'R', 'P', 61, 0.000000, 0.000000, 0.000000},
	{"Broadway", 2, 1700.0, 'R', 'P', 159, -0.924997, 0.004999, -0.334998},
	{"Tornado", 2, 1700.0, 'R', 'P', 159, -1.019999, -0.140000, -0.419999},
	{"AT-400", 2, 60000.0, '4', 'P', 250, 0.000000, 0.000000, 0.000000},
	{"DFT-30", 2, 5500.0, 'R', 'D', 131, -1.429998, -0.649999, -0.879998},
	{"Huntley", 4, 2500.0, '4', 'P', 159, -1.014999, 0.000000, -0.484999},
	{"Stafford", 4, 2200.0, 'R', 'P', 154, -1.089998, 0.014999, -0.474999},
	{"BF-400", 2, 500.0, 'R', 'P', 200, 0.000000, 0.000000, 0.000000},
	{"Newsvan", 4, 1900.0, 'R', 'D', 137, -0.984999, -0.194997, -0.709999},
	{"Tug", 1, 800.0, 'R', 'P', 86, -0.674997, 0.259999, 0.129998},
	{"Petrol Trailer", 1, 3800.0, 'R', 'D', 0, 0.000000, 0.000000, 0.000000},
	{"Emperor", 4, 1800.0, 'R', 'P', 154, -1.019999, -0.009998, -0.354999},
	{"Wayfarer", 2, 800.0, 'R', 'P', 211, 0.000000, 0.000000, 0.000000},
	{"Euros", 2, 1400.0, '4', 'P', 166, -1.009999, -0.064998, -0.534999},
	{"Hotdog", 2, 5500.0, 'R', 'D', 109, -1.374997, 0.000000, -0.664999},
	{"Club", 2, 1400.0, 'F', 'P', 164, -0.909999, 0.000000, -0.359999},
	{"Freight Box Trailer", 1, 5500.0, 'R', 'D', 0, 0.000000, 0.000000, 0.000000},
	{"Article Trailer 3", 1, 3800.0, 'R', 'D', 0, 0.000000, 0.000000, 0.000000},
	{"Andromada", 2, 40000.0, '4', 'P', 266, 0.000000, 0.000000, 0.000000},
	{"Dodo", 2, 5000.0, '4', 'P', 135, 0.000000, 0.000000, 0.000000},
	{"RC Cam", 1, 100.0, '4', 'E', 62, 0.000000, 0.000000, 0.000000},
	{"Launch", 1, 2200.0, 'R', 'P', 118, 0.000000, 0.000000, 0.000000},
	{"Police Car (LSPD)", 4, 1600.0, 'R', 'P', 177, -0.974999, 0.000000, -0.469998},
	{"Police Car (SFPD)", 4, 1600.0, 'R', 'P', 177, -0.969999, 0.000000, -0.464998},
	{"Police Car (LVPD)", 4, 1600.0, 'R', 'P', 177, -1.004999, -0.014999, -0.414999},
	{"Ranger", 2, 2500.0, '4', 'D', 159, -1.019999, 0.115000, -0.684997},
	{"Picador", 2, 1600.0, 'R', 'D', 152, -0.959999, 0.204998, -0.499998},
	{"S.W.A.T.", 2, 5000.0, '4', 'D', 111, 0.000000, 0.000000, 0.000000},
	{"Alpha", 2, 1500.0, 'R', 'P', 171, -1.024999, 0.000000, -0.549997},
	{"Phoenix", 2, 1500.0, 'R', 'P', 173, -0.954999, 0.034999, -0.629998},
	{"Glendale (destroyed)", 4, 1600.0, 'R', 'P', 148, -0.989997, -0.064998, -0.489998},
	{"Sadler (destroyed)", 2, 1700.0, '4', 'D', 152, -0.904999, 0.009998, -0.409999},
	{"Baggage Trailer A", 1, 1000.0, 'R', 'E', 0, 0.000000, 0.000000, 0.000000},
	{"Baggage Trailer B", 1, 1000.0, 'R', 'E', 0, 0.000000, 0.000000, 0.000000},
	{"Tug Stairs Trailer", 1, 1000.0, 'R', 'E', 0, 0.000000, 0.000000, 0.000000},
	{"Boxville (burglary)", 4, 5500.0, 'R', 'D', 109, -1.149999, 0.269998, -0.654999},
	{"Farm Trailer", 1, 400.0, 'R', 'E', 0, 0.000000, 0.000000, 0.000000},
	{"Utility Trailer", 1, 1000.0, 'R', 'E', 0, 0.000000, 0.000000, 0.000000}
};

static const vSync_gVehDefaultColors[212][19] = {
	{16,4,1,123,1,113,1,101,1,75,1,62,1,40,1,36,1}, 				// Landstalker
	{16,41,41,47,47,52,52,66,66,74,74,87,87,91,91,113,113}, 		// Bravura
	{16,10,10,13,13,22,22,30,30,39,39,90,90,98,98,110,110}, 		// Buffalo
	{16,36,1,37,1,30,1,28,1,25,1,40,1,101,1,113,1}, 				// Linerunner
	{16,113,39,119,50,123,92,109,100,101,101,95,105,83,110,66,25}, 	// Perennial
	{16,11,1,24,1,36,1,40,1,75,1,91,1,123,1,4,1}, 					// Sentinel
	{2,1,1}, 														// Dumper
	{2,3,1}, 														// Firetruck
	{2,26,26}, 														// Trashmaster
	{2,1,1}, 														// Stretch
	{16,4,1,9,1,10,1,25,1,36,1,40,1,45,1,84,1}, 					// Manana
	{16,12,1,64,1,123,1,116,1,112,1,106,1,80,1,75,1}, 				// Infernus
	{16,9,1,10,8,11,1,25,8,27,1,29,8,30,1,37,8}, 					// Voodoo
	{16,87,1,88,1,91,1,105,1,109,1,119,1,4,1,25,1}, 				// Pony
	{16,25,1,28,1,43,1,67,1,72,1,9,1,95,1,24,1}, 					// Mule
	{16,20,1,25,1,36,1,40,1,62,1,75,1,92,1,0,1}, 					// Cheetah
	{2,1,3}, 														// Ambulance
	{2,0,0}, 														// Leviathan
	{16,119,119,117,227,114,114,108,108,95,95,81,81,61,61,41,41}, 	// Moonbeam
	{16,45,75,47,76,33,75,13,76,54,75,69,76,59,75,87,76}, 			// Esperanto
	{2,6,1}, 														// Taxi
	{16,4,1,13,1,25,1,30,1,36,1,40,1,75,1,95,1}, 					// Washington
	{14,96,25,97,25,101,25,111,31,113,36,83,57,67,59}, 				// Bobcat
	{12,1,16,1,56,1,17,1,53,1,5,1,35}, 								// Mr Whoopee
	{16,1,0,2,2,3,2,3,6,6,16,15,30,24,53,35,61}, 					// BF Injection
	{2,43,0}, 														// Hunter
	{16,37,37,42,42,53,53,62,62,7,7,10,10,11,11,15,15}, 			// Premier
	{2,0,1}, 														// Enforcer
	{2,4,75}, 														// Securicar
	{16,12,12,13,13,14,14,1,2,2,1,1,3,3,1,10,10}, 					// Banshee
	{2,46,26}, 														// Predator
	{16,71,59,75,59,92,72,47,74,55,83,59,83,71,87,82,87}, 			// Bus
	{2,43,0}, 														// Rhino
	{2,43,0}, 														// Barracks
	{14,1,1,12,12,2,2,6,6,4,4,46,46,53,53}, 						// Hotknife
	{2,1,1}, 														// Trailer 1
	{14,83,1,87,1,92,1,95,1,109,1,119,45,11,1}, 					// Previon
	{16,54,7,79,7,87,7,95,16,98,20,105,20,123,20,125,21}, 			// Coach
	{2,6,76}, 														// Cabbie
	{16,57,8,8,17,43,21,54,38,67,8,37,78,65,79,25,78}, 				// Stallion
	{16,34,34,32,32,20,20,110,110,66,66,84,84,118,118,121,121}, 	// Rumpo
	{16,2,96,79,42,82,54,67,86,126,96,70,96,110,54,67,98}, 			// RC Bandit
	{16,0,0,11,105,25,109,36,0,40,36,75,36,0,36,0,109}, 			// Romero
	{16,4,1,20,1,24,1,25,1,36,1,40,1,54,1,84,1}, 					// Packer
	{12,32,36,32,42,32,53,32,66,32,14,32,32}, 						// Monster
	{16,34,34,35,35,37,37,39,39,41,41,43,43,45,45,47,47}, 			// Admiral
	{0}, 															// Squalo
	{2,75,2}, 														// Seasparrow
	{2,3,6}, 														// Pizzaboy
	{2,1,74}, 														// Tram
	{2,1,1}, 														// Trailer 2
	{16,123,123,125,125,36,36,16,16,18,18,46,46,61,61,75,75}, 		// Turismo
	{16,1,3,1,5,1,16,1,22,1,35,1,44,1,53,1,57}, 					// Speeder
	{2,56,56}, 														// Reefer
	{2,26,26}, 														// Tropic
	{16,84,15,84,58,84,31,32,74,43,31,1,31,77,31,32,74}, 			// Flatbed
	{16,84,63,91,63,102,65,105,72,110,93,121,93,12,95,23,1}, 		// Yankee
	{16,58,1,2,1,63,1,18,1,32,1,45,1,13,1,34,1}, 					// Caddy
	{16,91,1,101,1,109,1,113,1,4,1,25,1,30,1,36,1}, 				// Solair
	{16,26,26,28,28,44,44,51,51,57,57,72,72,106,106,112,112}, 		// Berkley's RC Van
	{16,1,3,1,9,1,18,1,30,17,23,46,23,46,32,57,34}, 				// Skimmer
	{16,36,1,37,1,43,1,53,1,61,1,75,1,79,1,88,1}, 					// PCJ-600
	{16,12,12,13,13,14,14,1,2,2,1,1,3,3,1,10,10}, 					// Faggio
	{16,79,79,84,84,7,7,11,11,19,19,22,22,36,36,53,53}, 			// Freeway
	{2,14,75}, 														// RC Baron
	{2,14,75}, 														// RC Raider
	{16,67,76,68,76,78,76,2,76,16,76,18,76,25,76,45,88}, 			// Glendale
	{16,51,1,58,8,60,1,68,8,2,1,13,8,22,1,36,8}, 					// Oceanic
	{8,6,6,46,46,53,53,3,3}, 										// Sanchez
	{2,1,3}, 														// Sparrow
	{2,43,0}, 														// Patriot
	{16,120,117,103,111,120,114,74,91,120,112,74,83,120,113,66,71}, // Quad
	{4,56,15,56,53}, 												// Coastguard
	{4,56,15,56,53}, 												// Dinghy
	{16,97,1,81,1,105,1,110,1,91,1,74,1,84,1,83,1}, 				// Hermes
	{16,2,39,9,39,17,1,21,1,33,0,37,0,41,29,56,29}, 				// Sabre
	{16,6,7,7,6,1,6,89,91,119,117,103,102,77,87,71,77}, 			// Rustler
	{16,92,1,94,1,101,1,121,1,0,1,22,1,36,1,75,1}, 					// ZR-350
	{16,72,1,66,1,59,1,45,1,40,1,39,1,35,1,20,1}, 					// Walton
	{16,27,36,59,36,60,35,55,41,54,31,49,23,45,32,40,29}, 			// Regina
	{14,73,45,12,12,2,2,6,6,4,4,46,46,53,53}, 						// Comet
	{16,1,1,3,3,6,6,46,46,65,9,14,1,12,9,26,1}, 					// BMX
	{16,41,41,48,48,52,52,64,64,71,71,85,85,10,10,62,62}, 			// Burrito
	{0}, 															// Camper
	{8,12,35,50,32,40,26,66,36}, 									// Marquis
	{14,1,73,1,74,1,75,1,76,1,77,1,78,1,79}, 						// Baggage
	{2,1,1}, 														// Dozer
	{16,26,14,29,42,26,57,54,29,26,3,3,29,12,39,74,35}, 			// Maverick
	{4,2,26,2,29}, 													// News Chopper
	{12,13,118,14,123,120,123,112,120,84,110,76,102}, 				// Rancher
	{2,0,0}, 														// FBI Rancher
	{12,40,65,71,72,52,66,64,72,30,72,60,72}, 						// Virgo
	{16,30,26,77,26,81,27,24,55,28,56,49,59,52,69,71,107}, 			// Greenwood
	{2,36,13}, 														// Jetmax
	{16,36,117,36,13,42,30,42,33,54,36,75,79,92,101,98,109}, 		// Hotring
	{16,123,124,119,122,118,117,116,115,114,108,101,106,88,99,5,6}, // Sandking
	{16,74,72,66,72,53,56,37,19,22,22,20,20,9,14,0,0}, 				// Blista Compact
	{2,0,1}, 														// Police Maverick
	{16,11,123,13,120,20,117,24,112,27,107,36,105,37,107,43,93}, 	// Boxville
	{16,109,25,109,32,112,32,10,32,30,44,32,52,84,66,84,69}, 		// Benson
	{16,75,84,40,84,40,110,28,119,25,119,21,119,13,119,4,119}, 		// Mesa
	{2,14,75}, 														// RC Goblin
	{16,7,94,36,88,51,75,53,75,58,67,75,67,75,61,79,62}, 			// Hotring Racer A
	{16,83,66,87,74,87,75,98,83,101,100,103,101,117,116,123,36}, 	// Hotring Racer B
	{16,51,39,57,38,45,29,34,9,65,9,14,1,12,9,26,1}, 				// Bloodring Banger
	{12,13,118,14,123,120,123,112,120,84,110,76,102}, 				// Rancher Lure
	{10,3,3,6,6,7,7,52,52,76,76}, 									// Super GT
	{16,37,37,42,42,53,53,62,62,7,7,10,10,11,11,15,15}, 			// Elegant
	{2,1,1}, 														// Journey
	{16,7,1,74,1,61,1,16,1,25,1,30,1,36,1,53,1}, 					// Bike
	{16,43,43,46,46,39,39,28,28,16,16,6,6,5,5,2,2}, 				// Mountain Bike
	{16,3,90,4,90,7,68,8,66,12,60,27,97,34,51,37,51}, 				// Beagle
	{16,17,39,15,123,32,112,45,88,52,71,57,67,61,96,96,96}, 		// Cropdust
	{16,38,51,21,36,21,34,30,34,54,34,55,20,48,18,51,6}, 			// Stunt
	{16,10,1,25,1,28,1,36,1,40,1,54,1,75,1,113,1}, 					// Tanker
	{16,13,76,24,77,63,78,42,76,54,77,39,78,11,76,62,77}, 			// Roadtrain
	{16,116,1,119,1,122,1,4,1,9,1,24,1,27,1,36,1}, 					// Nebula
	{16,37,36,36,36,40,36,43,41,47,41,51,72,54,75,55,84}, 			// Majestic
	{16,2,39,9,39,17,1,21,1,33,0,37,0,41,29,56,29}, 				// Buccaneer
	{2,1,1}, 														// Shamal
	{0}, 															// Hydra
	{16,74,74,75,13,87,118,92,3,115,118,25,118,36,0,118,118}, 		// FCR-900
	{16,3,3,3,8,6,25,7,79,8,82,36,105,39,106,51,118}, 				// NRG-500
	{0}, 															// HPV1000
	{0}, 															// Cement Truck
	{14,1,1,17,20,18,20,22,30,36,43,44,51,52,54}, 					// Tow Truck
	{16,2,39,9,39,17,1,21,1,33,0,37,0,41,29,56,29}, 				// Fortune
	{16,52,1,53,1,66,1,75,1,76,1,81,1,95,1,109,1}, 					// Cadrona
	{0}, 															// FBI Truck
	{16,37,37,42,42,53,53,62,62,7,7,10,10,11,11,15,15}, 			// Willard
	{16,110,1,111,1,112,1,114,1,119,1,122,1,4,1,13,1}, 				// Forklift
	{12,2,35,36,2,51,53,91,2,11,22,40,35}, 							// Tractor
	{0}, 															// Combine
	{16,73,1,74,1,75,1,77,1,79,1,83,1,84,1,91,1}, 					// Feltzer
	{16,37,37,42,42,53,53,62,62,7,7,10,10,11,11,15,15}, 			// Remington
	{16,3,1,28,1,31,1,55,1,66,1,97,1,123,1,118,1}, 					// Slamvan
	{16,9,1,12,1,26,96,30,96,32,1,37,1,57,96,71,96}, 				// Blade
	{2,1,1}, 														// Freight
	{2,1,1}, 														// Streak
	{14,96,67,86,70,79,74,70,86,61,98,75,75,75,91}, 				// Vortex
	{16,37,37,42,42,53,53,62,62,7,7,10,10,11,11,15,15}, 			// Vincent
	{16,51,1,58,8,60,1,68,8,2,1,13,8,22,1,36,8}, 					// Bullet
	{16,13,118,24,118,31,93,32,92,45,92,113,92,119,113,122,113}, 	// Clover
	{16,76,8,32,8,43,8,67,8,11,11,8,90,2,2,83,13}, 					// Sadler
	{2,3,1}, 														// Firetruck LA
	{16,50,1,47,1,44,96,40,96,39,1,30,1,28,96,9,96}, 				// Hustler
	{16,62,37,78,38,2,62,3,87,2,78,113,78,119,62,7,78}, 			// Intruder
	{16,122,1,123,1,125,1,10,1,24,1,37,1,55,1,66,1}, 				// Primo
	{2,1,1}, 														// Cargobob
	{16,74,39,72,39,75,39,79,39,83,36,84,36,89,35,91,35}, 			// Tampa
	{16,37,37,42,42,53,53,62,62,7,7,10,10,11,11,15,15}, 			// Sunrise
	{16,67,1,72,1,75,1,83,1,91,1,101,1,109,1,20,1}, 				// Merit
	{6,56,56,49,49,26,124}, 										// Utility
	{16,38,9,55,23,61,74,71,87,91,87,98,114,102,119,111,3}, 		// Nevada
	{16,53,32,15,32,45,32,34,30,65,32,14,32,12,32,43,32}, 			// Yosemite
	{16,51,1,58,1,60,1,68,1,2,1,13,1,22,1,36,1}, 					// Windsor
	{2,1,1}, 														// Monster A
	{2,1,1}, 														// Monster B
	{16,112,1,116,1,117,1,24,1,30,1,35,1,36,1,40,1}, 				// Uranus
	{16,51,1,58,8,60,1,68,8,2,1,13,8,22,1,36,8}, 					// Jester
	{16,52,39,9,39,17,1,21,1,33,0,37,0,41,29,56,29}, 				// Sultan
	{16,57,8,8,17,43,21,54,38,67,8,37,78,65,79,25,78}, 				// Stratum
	{16,36,1,35,1,17,1,11,1,116,1,113,1,101,1,92,1}, 				// Elegy
	{2,1,6}, 														// Raindance
	{0},						 									// RC Tiger
	{16,37,37,42,42,53,53,62,62,7,7,10,10,11,11,15,15}, 			// Flash
	{16,109,1,30,8,95,1,84,8,83,1,72,8,71,1,52,8}, 					// Tahoma
	{16,97,96,88,64,90,96,93,64,97,96,99,81,102,114,114,1}, 		// Savanna
	{16,2,39,9,39,17,1,21,1,33,0,37,0,41,29,56,29}, 				// Bandito
	{2,1,1}, 														// Freight Flat
	{2,1,1}, 														// Streak Carriage
	{12,2,35,36,2,51,53,91,2,11,22,40,35}, 							// Kart
	{16,94,1,101,1,116,1,117,1,4,1,25,1,30,1,37,1}, 				// Mower
	{16,91,38,115,43,85,6,79,7,78,8,77,18,79,18,86,24}, 			// Duneride
	{2,26,26}, 														// Sweeper
	{16,12,1,19,96,31,64,25,96,38,1,51,96,57,1,66,96}, 				// Broadway
	{16,67,1,68,96,72,1,74,8,75,96,76,8,79,1,84,96}, 				// Tornado
	{12,1,3,8,7,8,10,8,16,23,31,40,44}, 							// AT-400
	{2,1,1}, 														// DFT-30
	{16,37,37,42,42,53,53,62,62,7,7,10,10,11,11,15,15}, 			// Huntley
	{18,92,92,81,81,67,67,66,66,61,61,53,53,51,51,47,47,43,43}, 	// Stafford
	{16,54,1,58,1,66,1,72,1,75,1,87,1,101,1,36,1}, 					// BF-400
	{16,41,10,41,20,49,11,56,123,110,113,112,116,114,118,119,101}, 	// Newsvan
	{2,1,1}, 														// Tug
	{2,1,1}, 														// Trailer 3
	{16,37,37,42,42,53,53,62,62,7,7,10,10,11,11,15,15}, 			// Emperor
	{16,119,1,122,1,8,1,10,1,13,1,25,1,27,1,32,1}, 					// Wayfarer
	{16,36,1,40,1,43,1,53,1,72,1,75,1,95,1,101,1}, 					// Euros
	{2,1,1}, 														// Hotdog
	{16,37,37,31,31,23,23,22,22,7,7,124,124,114,114,112,112}, 		// Club
	{0}, 															// Freight Carriage
	{2,1,1}, 														// Trailer 3
	{2,1,1}, 														// Andromada
	{16,51,1,58,8,60,1,68,8,2,1,13,8,22,1,36,8}, 					// Dodo
	{0}, 															// RC Cam
	{2,112,20}, 													// Launch
	{2,0,1}, 														// Police Car (LSPD)
	{2,0,1}, 														// Police Car (SFPD)
	{2,0,1}, 														// Police Car (LVPD)
	{2,0,1}, 														// Police Ranger
	{16,81,8,32,8,43,8,67,8,11,11,8,90,2,2,83,13}, 					// Picador
	{2,1,1}, 														// S.W.A.T. Van
	{16,58,1,69,1,75,77,18,1,32,1,45,45,13,1,34,1}, 				// Alpha
	{16,58,1,69,1,75,77,18,1,32,1,45,45,13,1,34,1}, 				// Phoenix
	{16,67,76,68,76,78,76,2,76,16,76,18,76,25,76,45,88}, 			// Glendale shit
	{16,61,8,32,8,43,8,67,8,11,11,8,90,2,2,83,13}, 					// Sadler shit
	{0}, 															// Luggage Trailer A
	{0}, 															// Luggage Trailer B
	{2,1,1}, 														// Stair Trailer
	{2,36,36}, 														// Boxville 2
	{0}, 															// Farm Plow
	{0} 															// Utility Trailer
};

static const vSync_gVehFrontBullbars[5] = {
	1100, 1115, 1116, 1123, 1125
};


// ================================== [HOOKED FUNCTIONS] ================================== //
stock vSync_CreateVehicle(modelid, Float:x, Float:y, Float:z, Float:rotation, color1, color2, respawn_delay, addsiren=0)
{
	// Check vehicle's model ID.
	if(!(400 <= modelid <= 611))
	{
		return INVALID_VEHICLE_ID;
	}
	
	// Add siren to the Firetruck (ladder variant), by default it's disabled.
	if(modelid == 544)
	{
		addsiren = 1;
	}

	// Generate random colors (if it proceeds)
	if((color1 == -1 || color2 == -1) && vSync_gVehDefaultColors[modelid - 400][0] != 0)
	{

		new rand_col = random(vSync_gVehDefaultColors[modelid - 400][0]) + 1;

		if(!isodd(rand_col))
		{
			rand_col--;
		}

		if(color1 == -1)
		{
			color1 = vSync_gVehDefaultColors[modelid - 400][rand_col];
		}

		if(color2 == -1)
		{
			color2 = vSync_gVehDefaultColors[modelid - 400][rand_col + 1];
		}
	}

	new vehicleid = CreateVehicle(modelid, Float:x, Float:y, Float:z, Float:rotation, color1, color2, respawn_delay, addsiren);

	// Check vehicle's ID (to avoid using INVALID_VEHICLE_ID as array index).
	if(1 <= vehicleid <= MAX_VEHICLES)
	{
		SetVehicleHealth(vehicleid, 999.0);

		vSync_gVehicleInternal[vehicleid - 1][vSync_Color1] = color1;
		vSync_gVehicleInternal[vehicleid - 1][vSync_Color2] = color2;

		vSync_gVehicleInternal[vehicleid - 1][vSync_OrgCol1] = color1;
		vSync_gVehicleInternal[vehicleid - 1][vSync_OrgCol2] = color2;
		
		vSync_gVehicleInternal[vehicleid - 1][vSync_Paintjob] = INVALID_PAINTJOB_ID;
		vSync_gVehicleInternal[vehicleid - 1][vSync_Interior] = 0;

		vSync_gVehicleInternal[vehicleid - 1][vSync_Siren] = addsiren;

		vSync_gVehicleInternal[vehicleid - 1][vSync_Stereo] = 0;
		vSync_gVehicleInternal[vehicleid - 1][vSync_Bumpers][0] = 0;
		vSync_gVehicleInternal[vehicleid - 1][vSync_Bumpers][1] = 0;
		vSync_gVehicleInternal[vehicleid - 1][vSync_Bullbars][0] = 0;
		vSync_gVehicleInternal[vehicleid - 1][vSync_Bullbars][1] = 0;

		strcpy(vSync_gVehicleInternal[vehicleid - 1][vSync_NumberP], "XYZSR998", MAX_VEHICLE_NUMBERPLATE);

		CallLocalFunction("OnVehicleCreated", "i", vehicleid);
	}
	return vehicleid;
}
#if defined _ALS_CreateVehicle
	#undef CreateVehicle
#else
	#define _ALS_CreateVehicle
#endif
#define CreateVehicle vSync_CreateVehicle

stock vSync_AddStaticVehicle(modelid, Float:x, Float:y, Float:z, Float:rotation, color1, color2)
{
	// Check vehicle's model ID.
	if(!(400 <= modelid <= 611))
	{
		return INVALID_VEHICLE_ID;
	}

	// Generate random colors (if it proceeds)
	if((color1 == -1 || color2 == -1) && vSync_gVehDefaultColors[modelid - 400][0] != 0)
	{
		new rand_col = random(vSync_gVehDefaultColors[modelid - 400][0]) + 1;

		if(!isodd(rand_col))
		{
			rand_col--;
		}

		if(color1 == -1)
		{
			color1 = vSync_gVehDefaultColors[modelid - 400][rand_col];
		}

		if(color2 == -1)
		{
			color2 = vSync_gVehDefaultColors[modelid - 400][rand_col + 1];
		}
	}

	new vehicleid = AddStaticVehicle(modelid, Float:x, Float:y, Float:z, Float:rotation, color1, color2);

	// Check vehicle's ID (to avoid using INVALID_VEHICLE_ID as array index).
	if(1 <= vehicleid <= MAX_VEHICLES)
	{
		SetVehicleHealth(vehicleid, 999.0);
		
		vSync_gVehicleInternal[vehicleid - 1][vSync_Color1] = color1;
		vSync_gVehicleInternal[vehicleid - 1][vSync_Color2] = color2;

		vSync_gVehicleInternal[vehicleid - 1][vSync_OrgCol1] = color1;
		vSync_gVehicleInternal[vehicleid - 1][vSync_OrgCol2] = color2;
		
		vSync_gVehicleInternal[vehicleid - 1][vSync_Paintjob] = INVALID_PAINTJOB_ID;
		vSync_gVehicleInternal[vehicleid - 1][vSync_Interior] = 0;

		vSync_gVehicleInternal[vehicleid - 1][vSync_Siren] = addsiren;

		vSync_gVehicleInternal[vehicleid - 1][vSync_Stereo] = 0;
		vSync_gVehicleInternal[vehicleid - 1][vSync_Bumpers][0] = 0;
		vSync_gVehicleInternal[vehicleid - 1][vSync_Bumpers][1] = 0;
		vSync_gVehicleInternal[vehicleid - 1][vSync_Bullbars][0] = 0;
		vSync_gVehicleInternal[vehicleid - 1][vSync_Bullbars][1] = 0;

		strcpy(vSync_gVehicleInternal[vehicleid - 1][vSync_NumberP], "XYZSR998", MAX_VEHICLE_NUMBERPLATE);

		CallLocalFunction("OnVehicleCreated", "i", vehicleid);
	}
	return vehicleid;
}
#if defined _ALS_AddStaticVehicle
	#undef AddStaticVehicle
#else
	#define _ALS_AddStaticVehicle
#endif
#define AddStaticVehicle vSync_AddStaticVehicle

stock vSync_AddStaticVehicleEx(modelid, Float:x, Float:y, Float:z, Float:rotation, color1, color2, respawn_delay, addsiren=0)
{
	// Check vehicle's model ID.
	if(!(400 <= modelid <= 611))
	{
		return INVALID_VEHICLE_ID;
	}
	
	// Add siren to the Firetruck (ladder variant), by default it's disabled.
	if(modelid == 544)
	{
		addsiren = 1;
	}

	// Generate random colors (if it proceeds)
	if((color1 == -1 || color2 == -1) && vSync_gVehDefaultColors[modelid - 400][0] != 0)
	{
		new rand_col = random(vSync_gVehDefaultColors[modelid - 400][0]) + 1;

		if(!isodd(rand_col))
		{
			rand_col--;
		}

		if(isodd(rand_col))
		{
			rand_col--;
		}

		if(color1 == -1)
		{
			color1 = vSync_gVehDefaultColors[modelid - 400][rand_col];
		}

		if(color2 == -1)
		{
			color2 = vSync_gVehDefaultColors[modelid - 400][rand_col + 1];
		}
	}

	new vehicleid = AddStaticVehicleEx(modelid, Float:x, Float:y, Float:z, Float:rotation, color1, color2, respawn_delay, addsiren);

	// Check vehicle's ID (to avoid using INVALID_VEHICLE_ID as array index).
	if(1 <= vehicleid <= MAX_VEHICLES)
	{
		SetVehicleHealth(vehicleid, 999.0);

		vSync_gVehicleInternal[vehicleid - 1][vSync_Color1] = color1;
		vSync_gVehicleInternal[vehicleid - 1][vSync_Color2] = color2;

		vSync_gVehicleInternal[vehicleid - 1][vSync_OrgCol1] = color1;
		vSync_gVehicleInternal[vehicleid - 1][vSync_OrgCol2] = color2;
		
		vSync_gVehicleInternal[vehicleid - 1][vSync_Paintjob] = INVALID_PAINTJOB_ID;
		vSync_gVehicleInternal[vehicleid - 1][vSync_Interior] = 0;

		vSync_gVehicleInternal[vehicleid - 1][vSync_Siren] = addsiren;

		vSync_gVehicleInternal[vehicleid - 1][vSync_Stereo] = 0;
		vSync_gVehicleInternal[vehicleid - 1][vSync_Bumpers][0] = 0;
		vSync_gVehicleInternal[vehicleid - 1][vSync_Bumpers][1] = 0;
		vSync_gVehicleInternal[vehicleid - 1][vSync_Bullbars][0] = 0;
		vSync_gVehicleInternal[vehicleid - 1][vSync_Bullbars][1] = 0;

		strcpy(vSync_gVehicleInternal[vehicleid - 1][vSync_NumberP], "XYZSR998", MAX_VEHICLE_NUMBERPLATE);

		CallLocalFunction("OnVehicleCreated", "i", vehicleid);
	}
	return vehicleid;
}
#if defined _ALS_AddStaticVehicleEx
	#undef AddStaticVehicleEx
#else
	#define _ALS_AddStaticVehicleEx
#endif
#define AddStaticVehicleEx vSync_AddStaticVehicleEx

stock vSync_SetVehicleNumberPlate(vehicleid, numberplate[])
{
	if(1 <= vehicleid <= MAX_VEHICLES)
	{
		strcpy(vSync_gVehicleInternal[vehicleid - 1][vSync_NumberP], numberplate, MAX_VEHICLE_NUMBERPLATE);
		return SetVehicleNumberPlate(vehicleid, numberplate);
	}
	return 0;
}
#if defined _ALS_SetVehicleNumberPlate
	#undef SetVehicleNumberPlate
#else
	#define _ALS_SetVehicleNumberPlate
#endif
#define SetVehicleNumberPlate vSync_SetVehicleNumberPlate

stock vSync_LinkVehicleToInterior(vehicleid, interior)
{
	if(1 <= vehicleid <= MAX_VEHICLES)
	{
		vSync_gVehicleInternal[vehicleid - 1][vSync_Interior] = interior;
		return LinkVehicleToInterior(vehicleid, interior);
	}
	return 0;
}
#if defined _ALS_LinkVehicleToInterior
	#undef LinkVehicleToInterior
#else
	#define _ALS_LinkVehicleToInterior
#endif
#define LinkVehicleToInterior vSync_LinkVehicleToInterior

stock vSync_ChangeVehicleColor(vehicleid, color1, color2)
{
	new modelid = GetVehicleModel(vehicleid);

	if(!(400 <= modelid <= 611))
	{
		return 0;
	}

	// Generate random colors (if it proceeds)
	if((color1 == -1 || color2 == -1) && vSync_gVehDefaultColors[modelid - 400][0] != 0)
	{
		new rand_col = random(vSync_gVehDefaultColors[modelid - 400][0]) + 1;

		if(!isodd(rand_col))
		{
			rand_col--;
		}

		if(color1 == -1)
		{
			color1 = vSync_gVehDefaultColors[modelid - 400][rand_col];
		}

		if(color2 == -1)
		{
			color2 = vSync_gVehDefaultColors[modelid - 400][rand_col + 1];
		}
	}

	vSync_gVehicleInternal[vehicleid - 1][vSync_Color1] = color1;
	vSync_gVehicleInternal[vehicleid - 1][vSync_Color2] = color2;

	return ChangeVehicleColor(vehicleid, vSync_gVehicleInternal[vehicleid - 1][vSync_Color1], vSync_gVehicleInternal[vehicleid - 1][vSync_Color2]);
}
#if defined _ALS_ChangeVehicleColor
	#undef ChangeVehicleColor
#else
	#define _ALS_ChangeVehicleColor
#endif
#define ChangeVehicleColor vSync_ChangeVehicleColor

stock vSync_ChangeVehiclePaintjob(vehicleid, paintjobid)
{
	if(1 <= vehicleid <= MAX_VEHICLES)
	{
		vSync_gVehicleInternal[vehicleid - 1][vSync_Paintjob] = paintjobid;
		ChangeVehiclePaintjob(vehicleid, vSync_gVehicleInternal[vehicleid - 1][vSync_Paintjob]);
		ChangeVehicleColor(vehicleid, vSync_gVehicleInternal[vehicleid - 1][vSync_Color1], vSync_gVehicleInternal[vehicleid - 1][vSync_Color2]);
		return 1;
	}
	return 1;
}
#if defined _ALS_ChangeVehiclePaintjob
	#undef ChangeVehiclePaintjob
#else
	#define _ALS_ChangeVehiclePaintjob
#endif
#define ChangeVehiclePaintjob vSync_ChangeVehiclePaintjob

stock vSync_GetVehicleComponentType(componentid)
{
	if(IsComponentFrontBullbar(componentid))
	{
		return CARMODTYPE_FRONT_BULLBAR;
	}
	else if(IsComponentRearBullbar(componentid))
	{
		return CARMODTYPE_REAR_BULLBAR;
	}
	return GetVehicleComponentType(componentid);
}
#if defined _ALS_GetVehicleComponentType
	#undef GetVehicleComponentType
#else
	#define _ALS_GetVehicleComponentType
#endif
#define GetVehicleComponentType vSync_GetVehicleComponentType

stock vSync_GetVehicleComponentInSlot(vehicleid, slot)
{
	if(1 <= vehicleid <= MAX_VEHICLES)
	{
		switch(slot)
		{
			case CARMODTYPE_STEREO:
			{
				return vSync_gVehicleInternal[vehicleid - 1][vSync_Stereo];
			}
			case CARMODTYPE_FRONT_BUMPER:
			{
				return vSync_gVehicleInternal[vehicleid - 1][vSync_Bumpers][0];
			}
			case CARMODTYPE_REAR_BUMPER:
			{
				return vSync_gVehicleInternal[vehicleid - 1][vSync_Bumpers][1];
			}
			case CARMODTYPE_FRONT_BULLBAR:
			{
				return vSync_gVehicleInternal[vehicleid - 1][vSync_Bullbars][0];
			}
			case CARMODTYPE_REAR_BULLBAR:
			{
				return vSync_gVehicleInternal[vehicleid - 1][vSync_Bullbars][1];
			}
		}
		return GetVehicleComponentInSlot(vehicleid, slot);
	}
	return 0;
}
#if defined _ALS_GetVehicleComponentInSlot
	#undef GetVehicleComponentInSlot
#else
	#define _ALS_GetVehicleComponentInSlot
#endif
#define GetVehicleComponentInSlot vSync_GetVehicleComponentInSlot

stock vSync_RemoveVehicleComponent(vehicleid, componentid)
{
	if(1 <= vehicleid <= MAX_VEHICLES)
	{
		switch(GetVehicleComponentType(componentid))
		{
			case CARMODTYPE_STEREO:
			{
				vSync_gVehicleInternal[vehicleid - 1][vSync_Stereo] = 0;
			}
			case CARMODTYPE_FRONT_BUMPER:
			{
				vSync_gVehicleInternal[vehicleid - 1][vSync_Bumpers][0];
			}
			case CARMODTYPE_REAR_BUMPER:
			{
				vSync_gVehicleInternal[vehicleid - 1][vSync_Bumpers][1];
			}
			case CARMODTYPE_FRONT_BULLBAR:
			{
				vSync_gVehicleInternal[vehicleid - 1][vSync_Bullbars][0];
			}
			case CARMODTYPE_REAR_BULLBAR:
			{
				vSync_gVehicleInternal[vehicleid - 1][vSync_Bullbars][1];
			}
		}
		return RemoveVehicleComponent(vehicleid, componentid);
	}
	return 0;
}
#if defined _ALS_RemoveVehicleComponent
	#undef RemoveVehicleComponent
#else
	#define _ALS_RemoveVehicleComponent
#endif
#define RemoveVehicleComponent vSync_RemoveVehicleComponent

stock vSync_DestroyVehicle(vehicleid)
{
	if(1 <= vehicleid <= MAX_VEHICLES)
	{
		SetVehicleNeonLights(vehicleid, false);
		return DestroyVehicle(vehicleid);
	}
	return 0;
}
#if defined _ALS_DestroyVehicle
	#undef DestroyVehicle
#else
	#define _ALS_DestroyVehicle
#endif
#define DestroyVehicle vSync_DestroyVehicle


// ================================== [HOOKED CALLBACKS] ================================== //
public OnPlayerConnect(playerid)
{
	vSync_pInsideModShop{playerid} = false;
	vSync_pBuyingPaintjob{playerid} = false;

    #if defined vSync_OnPlayerConnect
         return vSync_OnPlayerConnect(playerid);
    #else
   	 	return 1;
	#endif
}
#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect vSync_OnPlayerConnect
#if defined vSync_OnPlayerConnect
	forward vSync_OnPlayerConnect(playerid);
#endif

public OnPlayerUpdate(playerid)
{
	if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
    {
        new Float:v_health;
        new vehicleid = GetPlayerVehicleID(playerid);

        GetVehicleHealth(vehicleid, v_health);

        if(v_health >= 1000.0)
        {
            SetVehicleHealth(vehicleid, 999.0);

           	if(IsPlayerInsidePNS(playerid))
           	{
           		ChangeVehicleColor(vehicleid, -1, -1);
              	CallRemoteFunction("OnVehicleRespray", "iiii", playerid, vehicleid, vSync_gVehicleInternal[vehicleid - 1][vSync_Color1], vSync_gVehicleInternal[vehicleid - 1][vSync_Color2]);
       		}
        }
    }

    #if defined vSync_OnPlayerUpdate
         return vSync_OnPlayerUpdate(playerid);
    #else
   	 	return 1;
	#endif
}
#if defined _ALS_OnPlayerUpdate
	#undef OnPlayerUpdate
#else
	#define _ALS_OnPlayerUpdate
#endif
#define OnPlayerUpdate vSync_OnPlayerUpdate
#if defined vSync_OnPlayerUpdate
	forward vSync_OnPlayerUpdate(playerid);
#endif

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(RELEASED(KEY_SPRINT))
	{
		if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER && vSync_pBuyingPaintjob{playerid} && GetTickDiff(GetTickCount(), vSync_pTick[playerid]) > 50 && GetPlayerMoney(playerid) >= 500)
		{
			new vehicleid = GetPlayerVehicleID(playerid);

			vSync_pBuyingPaintjob{playerid} = false;

			vSync_gVehicleInternal[vehicleid - 1][vSync_Paintjob] = vSync_pPaintjobID{playerid};
			CallLocalFunction("OnVehicleChangePaintjob", "iii", playerid, vehicleid, vSync_pPaintjobID{playerid});
		}
	}
	else if(RELEASED(KEY_SECONDARY_ATTACK))
	{
		if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER && vSync_pBuyingPaintjob{playerid})
		{
			vSync_pBuyingPaintjob{playerid} = false;
		}
	}

	#if defined vSync_OnPlayerKeyStateChange
        return vSync_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);
    #else
        return 1;
    #endif
}
#if defined _ALS_OnPlayerKeyStateChange
	#undef OnPlayerKeyStateChange
#else
	#define _ALS_OnPlayerKeyStateChange
#endif
#define OnPlayerKeyStateChange vSync_OnPlayerKeyStateChange
#if defined vSync_OnPlayerKeyStateChange
	forward vSync_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);
#endif

public OnEnterExitModShop(playerid, enterexit, interiorid)
{
	new vehicleid = GetPlayerVehicleID(playerid);
	new max_passengers = GetVehicleSeats(GetVehicleModel(vehicleid));
	new count_passengers;

	if(enterexit)
	{
		vSync_pInsideModShop{playerid} = true;
		SetVehicleVirtualWorld(vehicleid, playerid);

		for(new x = 0, t = GetPlayerPoolSize(); x <= t; x++)
		{
			if(!IsPlayerConnected(x)) continue;

			if(GetPlayerVehicleID(x) == vehicleid)
			{
				SetPlayerVirtualWorld(x, playerid);
				count_passengers++;

				if(count_passengers >= max_passengers)
				{
					break;
				}
			}
		}
	}
	else
	{
		vSync_pInsideModShop{playerid} = false;
		SetVehicleVirtualWorld(vehicleid, 0);

		for(new x = 0, t = GetPlayerPoolSize(); x <= t; x++)
		{
			if(!IsPlayerConnected(x)) continue;

			if(GetPlayerVehicleID(x) == vehicleid)
			{
				SetPlayerVirtualWorld(x, 0);
				count_passengers++;

				if(count_passengers >= max_passengers)
				{
					break;
				}
			}
		}
	}
	#if defined vSync_OnEnterExitModShop
         return vSync_OnEnterExitModShop(playerid, enterexit, interiorid);
    #else
   	 	return 1;
	#endif
}
#if defined _ALS_OnEnterExitModShop
	#undef OnEnterExitModShop
#else
	#define _ALS_OnEnterExitModShop
#endif
#define OnEnterExitModShop vSync_OnEnterExitModShop
#if defined vSync_OnEnterExitModShop
	forward vSync_OnEnterExitModShop(playerid, enterexit, interiorid);
#endif

public OnVehiclePaintjob(playerid, vehicleid, paintjobid)
{
	if(paintjobid <= 2 && paintjobid != vSync_gVehicleInternal[vehicleid - 1][vSync_Paintjob])
	{
		vSync_pTick[playerid] = GetTickCount();
		vSync_pPaintjobID{playerid} = paintjobid;
		vSync_pBuyingPaintjob{playerid} = true;
	}
	#if defined vSync_OnVehiclePaintjob
         return vSync_OnVehiclePaintjob(playerid, vehicleid, paintjobid);
    #else
   	 	return 1;
	#endif
}
#if defined _ALS_OnVehiclePaintjob
	#undef OnVehiclePaintjob
#else
	#define _ALS_OnVehiclePaintjob
#endif
#define OnVehiclePaintjob vSync_OnVehiclePaintjob
#if defined vSync_OnVehiclePaintjob
	forward vSync_OnVehiclePaintjob(playerid, vehicleid, paintjobid);
#endif

public OnVehicleRespray(playerid, vehicleid, color1, color2)
{
	if(vSync_pInsideModShop{playerid} || (color1 == vSync_gVehicleInternal[vehicleid - 1][vSync_Color1] && color2 == vSync_gVehicleInternal[vehicleid - 1][vSync_Color2] && !IsPlayerInsidePNS(playerid)))
	{
		return 0;
	}

	vSync_gVehicleInternal[vehicleid - 1][vSync_Color1] = color1;
	vSync_gVehicleInternal[vehicleid - 1][vSync_Color2] = color2;

	#if defined vSync_OnVehicleRespray
         return vSync_OnVehicleRespray(playerid, vehicleid, color1, color2);
    #else
   	 	return 1;
	#endif
}
#if defined _ALS_OnVehicleRespray
	#undef OnVehicleRespray
#else
	#define _ALS_OnVehicleRespray
#endif
#define OnVehicleRespray vSync_OnVehicleRespray
#if defined vSync_OnVehicleRespray
	forward vSync_OnVehicleRespray(playerid, vehicleid, color1, color2);
#endif

public OnVehicleSpawn(vehicleid)
{
	vSync_gVehicleInternal[vehicleid - 1][vSync_Paintjob] = 3;

	vSync_gVehicleInternal[vehicleid - 1][vSync_Stereo] = 0;
	vSync_gVehicleInternal[vehicleid - 1][vSync_Bumpers][0] = 0;
	vSync_gVehicleInternal[vehicleid - 1][vSync_Bumpers][1] = 0;
	vSync_gVehicleInternal[vehicleid - 1][vSync_Bullbars][0] = 0;
	vSync_gVehicleInternal[vehicleid - 1][vSync_Bullbars][1] = 0;

	ChangeVehicleColor(vehicleid, vSync_gVehicleInternal[vehicleid - 1][vSync_OrgCol1], vSync_gVehicleInternal[vehicleid - 1][vSync_OrgCol2]);

	#if defined vSync_OnVehicleSpawn
         return vSync_OnVehicleSpawn(vehicleid);
    #else
   	 	return 1;
	#endif
}
#if defined _ALS_OnVehicleSpawn
	#undef OnVehicleSpawn
#else
	#define _ALS_OnVehicleSpawn
#endif
#define OnVehicleSpawn vSync_OnVehicleSpawn
#if defined vSync_OnVehicleSpawn
	forward vSync_OnVehicleSpawn(vehicleid);
#endif

public OnVehicleMod(playerid, vehicleid, componentid)
{
	switch(GetVehicleComponentType(componentid))
	{
		case CARMODTYPE_STEREO:
		{
			vSync_gVehicleInternal[vehicleid - 1][vSync_Stereo] = componentid;
		}
		case CARMODTYPE_FRONT_BUMPER:
		{
			vSync_gVehicleInternal[vehicleid - 1][vSync_Bumpers][0] = componentid;
		}
		case CARMODTYPE_REAR_BUMPER:
		{
			vSync_gVehicleInternal[vehicleid - 1][vSync_Bumpers][1] = componentid;
		}
		case CARMODTYPE_FRONT_BULLBAR:
		{
			vSync_gVehicleInternal[vehicleid - 1][vSync_Bullbars][0] = componentid;
		}
		case CARMODTYPE_REAR_BULLBAR:
		{
			vSync_gVehicleInternal[vehicleid - 1][vSync_Bullbars][1] = componentid;
		}
	}
	#if defined vSync_OnVehicleMod
         return vSync_OnVehicleMod(playerid, vehicleid, componentid);
    #else
   	 	return 1;
	#endif
}
#if defined _ALS_OnVehicleMod
	#undef OnVehicleMod
#else
	#define _ALS_OnVehicleMod
#endif
#define OnVehicleMod vSync_OnVehicleMod
#if defined vSync_OnVehicleMod
	forward vSync_OnVehicleMod(playerid, vehicleid, componentid);
#endif


// ================================== [NEW FUNCTIONS] ================================== //
stock GetVehicleName(vehicleid, name[], len = sizeof(name))
{
	new modelid = GetVehicleModel(vehicleid);

	if(400 <= modelid <= 611)
	{
		strcpy(name, vSync_VehicleInfo[modelid - 400][Name], len);
	}
	return strlen(name);
}

stock GetVehicleColor(vehicleid, &color1, &color2)
{
	if(1 <= vehicleid <= MAX_VEHICLES)
	{
		color1 = vSync_gVehicleInternal[vehicleid - 1][vSync_Color1];
		color2 = vSync_gVehicleInternal[vehicleid - 1][vSync_Color2];
		return 1;
	}
	return 0;
}

stock GetVehiclePaintjob(vehicleid)
{
	if(1 <= vehicleid <= MAX_VEHICLES)
	{
		return vSync_gVehicleInternal[vehicleid - 1][vSync_Paintjob];
	}
	return -1;
}

stock GetVehicleNumberPlate(vehicleid, dest[], len = sizeof(dest))
{
	if(1 <= vehicleid <= MAX_VEHICLES)
	{
		strcpy(dest, vSync_gVehicleInternal[vehicleid - 1][vSync_NumberP], len);
	}
	return strlen(dest);
}

stock GetVehicleInterior(vehicleid)
{
	if(1 <= vehicleid <= MAX_VEHICLES)
	{
		return vSync_gVehicleInternal[vehicleid - 1][vSync_Interior];
	}
	return 0;
}

stock GetVehicleSiren(vehicleid)
{
	if(1 <= vehicleid <= MAX_VEHICLES)
	{
		return vSync_gVehicleInternal[vehicleid - 1][vSync_Siren];
	}
	return 0;
}

stock GetVehicleSeats(modelid)
{
	if(400 <= modelid <= 611)
	{
		return vSync_VehicleInfo[modelid - 400][Seats];
	}
	return -1;
}

stock GetVehicleMaxSpeed(modelid)
{
	if(400 <= modelid <= 611)
	{
		return vSync_VehicleInfo[modelid - 400][MaxSpeed];
	}
	return 0;
}

stock Float:GetVehicleWeight(modelid)
{
	if(400 <= modelid <= 611)
	{
		return vSync_VehicleInfo[modelid - 400][Mass];
	}
	return 0.0;
}

stock GetVehicleSpeed(vehicleid)
{
	new Float:v_x;
	new Float:v_y;
	new Float:v_z;

	GetVehicleVelocity(vehicleid, v_x, v_y, v_z);

	return floatround(floatsqroot((v_x * v_x) + (v_y * v_y) + (v_z * v_z)) * 181.5);
}

stock GetVehicleEngineType(modelid)
{
	if(400 <= modelid <= 611)
	{
		if(modelid == 481 || modelid == 509 || modelid == 510) // Bikes
		{
			return 'H'; // Human-powered vehicle
		}
		else
		{
			return vSync_VehicleInfo[modelid - 400][EngineType];
		}
	}
	return 0;
}

stock GetVehicleDriveType(modelid)
{
	if(400 <= modelid <= 611)
	{
		if(modelid == 481 || modelid == 509 || modelid == 510) // Bikes
		{
			return 'H'; // Human-powered vehicle
		}
		else
		{
			return vSync_VehicleInfo[modelid - 400][DriveType];
		}
	}
	return 0;
}

stock VehicleSupportsNeonLights(modelid)
{
    new idx = modelid - 400;

	if(0 <= idx <= 211)
	{
        if(vSync_VehicleInfo[idx][NeonOffsetX] == 0.0 && vSync_VehicleInfo[idx][NeonOffsetY] == 0.0 && vSync_VehicleInfo[idx][NeonOffsetZ] == 0.0)
		{
	    	return 0;
		}
		return 1;
	}
	return 0;
}

stock SetVehicleNeonLights(vehicleid, bool:enable = true, color = NEON_LIGHT_RED)
{
	new idx = GetVehicleModel(vehicleid) - 400;

	if(0 <= idx <= 211)
	{
		for(new x = 0; x < 2; x++)
		{
			#if defined _streamer_included
				if(IsValidDynamicObject(vSync_gVehicleInternal[vehicleid - 1][vSync_Neon][x]))
				{
				    DestroyDynamicObject(vSync_gVehicleInternal[vehicleid - 1][vSync_Neon][x]);
				}
			#else
				if(IsValidObject(vSync_gVehicleInternal[vehicleid - 1][vSync_Neon][x]))
				{
					DestroyObject(vSync_gVehicleInternal[vehicleid - 1][vSync_Neon][x]);
				}
			#endif
		}

		if((NEON_LIGHT_RED <= color <= NEON_LIGHT_WHITE) && enable)
		{
			#if defined _streamer_included
				vSync_gVehicleInternal[vehicleid - 1][vSync_Neon][0] = CreateDynamicObject(color, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
				vSync_gVehicleInternal[vehicleid - 1][vSync_Neon][1] = CreateDynamicObject(color, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

				AttachDynamicObjectToVehicle(vSync_gVehicleInternal[vehicleid - 1][vSync_Neon][0], vehicleid, vSync_VehicleInfo[idx][NeonOffsetX], vSync_VehicleInfo[idx][NeonOffsetY], vSync_VehicleInfo[idx][NeonOffsetZ], 0.0, 0.0, 0.0);
				AttachDynamicObjectToVehicle(vSync_gVehicleInternal[vehicleid - 1][vSync_Neon][1], vehicleid, -vSync_VehicleInfo[idx][NeonOffsetX], vSync_VehicleInfo[idx][NeonOffsetY], vSync_VehicleInfo[idx][NeonOffsetZ], 0.0, 0.0, 0.0);
			#else
				vSync_gVehicleInternal[vehicleid - 1][vSync_Neon][0] = CreateObject(color, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
				vSync_gVehicleInternal[vehicleid - 1][vSync_Neon][1] = CreateObject(color, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

				AttachObjectToVehicle(vSync_gVehicleInternal[vehicleid - 1][vSync_Neon][0], vehicleid, vSync_VehicleInfo[idx][NeonOffsetX], vSync_VehicleInfo[idx][NeonOffsetY], vSync_VehicleInfo[idx][NeonOffsetZ], 0.0, 0.0, 0.0);
				AttachObjectToVehicle(vSync_gVehicleInternal[vehicleid - 1][vSync_Neon][1], vehicleid, -vSync_VehicleInfo[idx][NeonOffsetX], vSync_VehicleInfo[idx][NeonOffsetY], vSync_VehicleInfo[idx][NeonOffsetZ], 0.0, 0.0, 0.0);
			#endif
		}
		return 1;
	}
	return 0;
}

stock GetVehicleNeonLightsState(vehicleid)
{
	if(1 <= vehicleid <= MAX_VEHICLES)
 	{
		#if defined _streamer_included
			if(IsValidDynamicObject(vSync_gVehicleInternal[vehicleid - 1][vSync_Neon][0]) && IsValidDynamicObject(vSync_gVehicleInternal[vehicleid - 1][vSync_Neon][1]))
			{
				return 1;
			}
		#else
			if(IsValidObject(vSync_gVehicleInternal[vehicleid - 1][vSync_Neon][0]) && IsValidObject(vSync_gVehicleInternal[vehicleid - 1][vSync_Neon][1]))
			{
				return 1;
			}
		#endif
	}
	return 0;
}


// ================================== [OTHER FUNCTIONS] ================================== //
stock IsPlayerInsidePNS(playerid)
{
	for(new i = 0; i < sizeof(vSync_gPaynSprayPos); i++)
    {
        if(IsPlayerInRangeOfPoint(playerid, 10.0, vSync_gPaynSprayPos[i][0], vSync_gPaynSprayPos[i][1], vSync_gPaynSprayPos[i][2]))
        {
            return 1;
        }
    }
    return 0;
}

stock GetTickDiff(newtick, oldtick)
{
	if(oldtick < 0 && newtick >= 0)
	{
		return newtick - oldtick;
	}
	else if(oldtick >= 0 && newtick < 0 || oldtick > newtick)
	{
		return (cellmax - oldtick + 1) - (cellmin - newtick);
	}
	return newtick - oldtick;
}

stock IsComponentFrontBullbar(componentid)
{
	for(new x = 0; x < 5; x++)
	{
		if(vSync_gVehFrontBullbars[x] == componentid)
		{
			return 1;
		}
	}
	return 0;
}
