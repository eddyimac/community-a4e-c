local count = 0
local function counter()
	count = count + 1
	return count
end
-------DEVICE ID-------
devices                     = {}
devices["AVIONICS"]         = counter()
devices["ELECTRIC_SYSTEM"]  = counter()
devices["INTERCOM"]         = counter()
devices["UHF_RADIO"]        = counter()
devices["WEAPON_SYSTEM"]    = counter()
devices["HYDRAULIC_SYSTEM"] = counter()
devices["CLOCK"]            = counter()
devices["ADI"]              = counter()
devices["RADAR"]            = counter()
devices["EXTANIM"]          = counter()
devices["SLATS"]            = counter()
devices["AIRBRAKES"]        = counter()
devices["FLAPS"]            = counter()
devices["GEAR"]             = counter()
devices["SPOILERS"]         = counter()
devices["CANOPY"]           = counter()
devices["HUFFER"]           = counter()
devices["RADARWARN"]        = counter()
devices["ENGINE"]           = counter()
devices["BYPASS_FAN"]       = counter()
devices["GUNSIGHT"]         = counter()
devices["NAV"]              = counter()
devices["ILS"]              = counter()
devices["NAV_TERRAIN"]      = counter()
devices["EXT_LIGHTS"]       = counter()
devices["TRIM"]             = counter()
devices["AFCS"]             = counter()
devices["RADIO"]            = counter()
devices["OXYGEN"]           = counter()
devices["HUD"]              = counter()
--devices["CARRIER"]          = counter()
devices["RWR"]              = counter()
devices["COUNTERMEASURES"]  = counter()
--devices["SFMEXTENDER"]      = counter()
devices["SHRIKE"]           = counter()
devices["SOUNDSYSTEM"]      = counter()
devices["NVG"]              = counter()
devices["EFM_DATA_BUS"]	  = counter()

-- temporary dummy devices
devices["TEMP1"]            = counter()
devices["TEMP2"]            = counter()
devices["TEMP3"]            = counter()
devices["TEMP4"]            = counter()
devices["TEMP5"]            = counter()
