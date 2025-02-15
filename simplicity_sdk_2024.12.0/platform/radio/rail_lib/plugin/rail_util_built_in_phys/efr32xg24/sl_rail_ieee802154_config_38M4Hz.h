/***************************************************************************//**
 * @brief RAIL Configuration
 * @details
 *   WARNING: Auto-Generated Radio Config Header  -  DO NOT EDIT
 *   Radio Configurator Version: 2404.4.4
 *   RAIL Adapter Version: 2.4.33
 *   RAIL Compatibility: 2.x
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef __SL_RAIL_IEEE802154_CONFIG_38M4HZ_H__
#define __SL_RAIL_IEEE802154_CONFIG_38M4HZ_H__

#include <stdint.h>
#include "rail_types.h"

extern const uint32_t sl_rail_ieee802154_phy_2G4Hz_38M4Hz_modemConfigBase[];
extern const uint32_t sl_rail_ieee802154_phy_2G4Hz_fast_switch_38M4Hz_modemConfigBase[];

extern const uint32_t sl_rail_ieee802154_phy_2G4Hz_38M4Hz_modemConfig[];
extern const uint32_t sl_rail_ieee802154_phy_2G4Hz_antdiv_38M4Hz_modemConfig[];
extern const uint32_t sl_rail_ieee802154_phy_2G4Hz_antdiv_fem_38M4Hz_modemConfig[];
extern const uint32_t sl_rail_ieee802154_phy_2G4Hz_fem_38M4Hz_modemConfig[];

#define RAIL0_SL_RAIL_IEEE802154_PHY_2G4HZ_38M4HZ_PHY_IEEE802154_2P4GHZ_PROD
#define RAIL0_SL_RAIL_IEEE802154_PHY_2G4HZ_38M4HZ_PROFILE_IEEE802154OQPSK
extern const RAIL_ChannelConfig_t sl_rail_ieee802154_phy_2G4Hz_38M4Hz_channelConfig;
extern const RAIL_ChannelConfigEntry_t sl_rail_ieee802154_phy_2G4Hz_38M4Hz_channels[];

#define RAIL0_SL_RAIL_IEEE802154_PHY_2G4HZ_FAST_SWITCH_38M4HZ_PHY_IEEE802154_2P4GHZ_ANTDIV_FASTSWITCH
#define RAIL0_SL_RAIL_IEEE802154_PHY_2G4HZ_FAST_SWITCH_38M4HZ_PROFILE_BASE
extern const RAIL_ChannelConfig_t sl_rail_ieee802154_phy_2G4Hz_fast_switch_38M4Hz_channelConfig;
extern const RAIL_ChannelConfigEntry_t sl_rail_ieee802154_phy_2G4Hz_fast_switch_38M4Hz_channels[];

#define RAIL0_SL_RAIL_IEEE802154_PHY_2G4HZ_ANTDIV_38M4HZ_PHY_IEEE802154_2P4GHZ_DIVERSITY_PROD
#define RAIL0_SL_RAIL_IEEE802154_PHY_2G4HZ_ANTDIV_38M4HZ_PROFILE_IEEE802154OQPSK
extern const RAIL_ChannelConfig_t sl_rail_ieee802154_phy_2G4Hz_antdiv_38M4Hz_channelConfig;
extern const RAIL_ChannelConfigEntry_t sl_rail_ieee802154_phy_2G4Hz_antdiv_38M4Hz_channels[];

#define RAIL0_SL_RAIL_IEEE802154_PHY_2G4HZ_ANTDIV_FEM_38M4HZ_PHY_IEEE802154_2P4GHZ_DIVERSITY_FEM_PROD
#define RAIL0_SL_RAIL_IEEE802154_PHY_2G4HZ_ANTDIV_FEM_38M4HZ_PROFILE_IEEE802154OQPSK
extern const RAIL_ChannelConfig_t sl_rail_ieee802154_phy_2G4Hz_antdiv_fem_38M4Hz_channelConfig;
extern const RAIL_ChannelConfigEntry_t sl_rail_ieee802154_phy_2G4Hz_antdiv_fem_38M4Hz_channels[];

#define RAIL0_SL_RAIL_IEEE802154_PHY_2G4HZ_FEM_38M4HZ_PHY_IEEE802154_2P4GHZ_FEM_PROD
#define RAIL0_SL_RAIL_IEEE802154_PHY_2G4HZ_FEM_38M4HZ_PROFILE_IEEE802154OQPSK
extern const RAIL_ChannelConfig_t sl_rail_ieee802154_phy_2G4Hz_fem_38M4Hz_channelConfig;
extern const RAIL_ChannelConfigEntry_t sl_rail_ieee802154_phy_2G4Hz_fem_38M4Hz_channels[];

#endif // __SL_RAIL_IEEE802154_CONFIG_38M4HZ_H__
