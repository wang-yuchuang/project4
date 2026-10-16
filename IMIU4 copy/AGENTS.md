# IMIU4 - Main BMS Application (Dev Branch)

**Variant:** Dev branch of imiu4_load — ~20 files diverged
**Canonical:** imiu4_load/ is the authoritative main variant

## OVERVIEW
Development branch of the main BMS application. Structure mirrors imiu4_load/ exactly but ~20 files have diverged (KA84917 async SPI, UDS interface, SystemDebug, SystemDiag, Measure, LoadApp, etc.).

## KEY DIFFERENCES FROM imiu4_load
Diverged files include:
- `BSPDevice/KA84917/BSPKA84917_async.*` — async SPI changes
- `BSPDevice/KA84917/spi_async.c` — SPI async protocol
- `BSPDevice/rn8209/drv_rn8209_interface.h` — energy meter interface
- `Addition/UDSCanfd/can_uds_interface.*` — UDS interface changes
- `Addition/UDSCanfd/services_layer/device_role/sid_internal_header.h`
- `Addition/cm_backtrace/cmb_cfg.h` — backtrace config
- `BSPDriver/BSPTIM.c` — timer changes
- `BSPUser/BSPUserHook.c` — hook changes
- `Funtion/LoadApp/LoadApp.c` — boot loading changes
- `Funtion/Measure/Measure.*` — measurement changes
- `Funtion/SystemDebug/SystemDebug.*` — debug output changes
- `Funtion/SystemDiag/SystemDiag.*` — diagnostics changes
- `Funtion/SystemPara/SystemPara.c` — parameter changes
- `Funtion/Test/TEST.c` — test changes

## CONVENTIONS & BUILD
Identical to imiu4_load/ — see `imiu4_load/AGENTS.md` for full details.

```bash
# GCC
make
# IAR: open EWARM/Project.eww
```
