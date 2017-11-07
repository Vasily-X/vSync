# Vehicle Synchronization
This include contains some bug fixes and new vehicle related functions.

## Bug Fixes
- No more desynchronized vehicle colors when using `-1` for the color parameters.
- Players won't collide anymore if they enter at once inside a mod shop.
- OnVehicleRespray isn't called anymore when you exit a mod shop if the colors didn't change.
- OnVehicleRespray isn't called anymore when a component is previewed inside a mod shop.
- Now the Firetruck (ladder variant) has the `addsiren` parameter set to true by default.
- Vehicles won't change their color to white anymore when a paintjob is removed.
- Bullbar components won't use anymore the same slots as front/rear bumpers (`GetVehicleComponentInSlot`).
- Now `GetVehicleComponentInSlot` works correctly when checking the `CARMODTYPE_STEREO` slot.
- Players won't crash anymore if someone tries to add an invalid component to a vehicle.

## New Features
- Now Pay 'n' Spray shops apply random colors and call `OnVehicleRespray`.
- Now `ChangeVehicleColor` applies random colors when using `-1` for the color parameters.
- When using random colors, vehicles get their own colors, those used single player.
- Two new vehicle component slots: `CARMODTYPE_FRONT_BULLBAR`, `CARMODTYPE_REAR_BULLBAR`. Check "Modified Functions" in the documentation.
- Neon lights related functions (with offsets for all vehicles and Streamer support).
- New functions and callbacks (check the documentation).

## External Links
##### Official SA-MP Forums thread
http://forum.sa-mp.com/showthread.php?t=642022

##### SA-MP Wiki documentation
http://wiki.sa-mp.com/wiki/Vehicle_Synchronization
