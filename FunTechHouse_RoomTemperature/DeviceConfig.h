/**
 * @file DeviceConfig.h
 * @author Johan Simonsson  
 * @brief Device uniq config isolated
 */

#ifndef  __DEVICECONFIG_H
#define  __DEVICECONFIG_H

// Update these with values suitable for your network.
uint8_t mac[]    = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0x01 };
uint8_t ip[]     = { 192, 168, 0, 31 };
uint8_t server[] = { 192, 168, 0, 64 };

char project_name[] = "FunTechHouse_RoomTemperature__Device01";

#endif  // __DEVICECONFIG_H 
