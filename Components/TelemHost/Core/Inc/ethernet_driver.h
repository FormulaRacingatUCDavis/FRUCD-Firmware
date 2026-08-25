/*
 * ethernet_driver.h
 *
 *  Created on: Feb 4, 2026
 *      Author: Vicle
 */

#ifndef INC_ETHERNET_DRIVER_H_
#define INC_ETHERNET_DRIVER_H_

// includes the built-in PHY interface functions
#include "../../LWIP/Target/eth_custom_phy_interface.h"


// used for getting specific bits in interrupt registers (e.g. MISR)
#define USER_PHY_INT_8       ((uint16_t)0x0100U)
#define USER_PHY_INT_7       ((uint16_t)0x0080U)
#define USER_PHY_INT_6       ((uint16_t)0x0040U)
#define USER_PHY_INT_5       ((uint16_t)0x0020U)
#define USER_PHY_INT_4       ((uint16_t)0x0010U)
#define USER_PHY_INT_3       ((uint16_t)0x0008U)
#define USER_PHY_INT_2       ((uint16_t)0x0004U)
#define USER_PHY_INT_1       ((uint16_t)0x0002U)
#define USER_PHY_INT_0       ((uint16_t)0x0000U)

// MISR1
#define USER_PHY_LINK_QUALITY_IT		USER_PHY_INT_7
#define USER_PHY_ENERGY_DETECT_IT		USER_PHY_INT_6
#define USER_PHY_LINKED_STATUS_CHANGED	USER_PHY_INT_5
#define USER_PHY_SPEED_CHANGED_IT		USER_PHY_INT_4
#define USER_PHY_DUPLEX_MODE_CHANGED_IT	USER_PHY_INT_3
#define USER_PHY_AUTONEGO_COMPLETE_IT	USER_PHY_INT_2 // auto negotiation complete interrupt
#define USER_PHY_FALSE_CARRIER_HF		USER_PHY_INT_1
#define USER_PHY_RECEIVE_ERROR_HF		USER_PHY_INT_0

// MISR2
#define USER_PHY_EEE_ERROR_IT				USER_PHY_INT_7
#define USER_PHY_AUTONEGO_ERROR_IT			USER_PHY_INT_6 // auto negotiation error interrupt
#define USER_PHY_PAGE_RECEIVED_IT			USER_PHY_INT_5
#define USER_PHY_LOOPBACK_FIFO_OFUF			USER_PHY_INT_4 // loopback fifo overflow / underflow
#define USER_PHY_MDI_CROSSOVER				USER_PHY_INT_3 // medium dependent interface crossover
#define USER_PHY_SLEEP_MODE_EVENT			USER_PHY_INT_2
#define USER_PHY_POLARITY_CHANGED_WOL		USER_PHY_INT_1
#define USER_PHY_JABBER_DETECT				USER_PHY_INT_0


// public function declarations for functions NOT already in eth_custom_phy_interface.h
int32_t USER_PHY_DisablePowerDownMode(user_phy_Object_t *pObj);
int32_t USER_PHY_EnablePowerDownMode(user_phy_Object_t *pObj);
int32_t USER_PHY_StartAutoNego(user_phy_Object_t *pObj);
int32_t USER_PHY_EnableIT_MISR1(user_phy_Object_t *pObj, uint32_t Interrupt);
int32_t USER_PHY_EnableIT_MISR2(user_phy_Object_t *pObj, uint32_t Interrupt);
int32_t USER_PHY_DisableIT_MISR1(user_phy_Object_t *pObj, uint32_t Interrupt);
int32_t USER_PHY_DisableIT_MISR2(user_phy_Object_t *pObj, uint32_t Interrupt);
int32_t USER_PHY_ClearIT(user_phy_Object_t *pObj);
int32_t USER_PHY_GetITStatus_MISR1(user_phy_Object_t *pObj, uint32_t Interrupt);
int32_t USER_PHY_GetITStatus_MISR2(user_phy_Object_t *pObj, uint32_t Interrupt);
int32_t USER_PHY_GenericRegisterEnable(user_phy_Object_t *pObj, uint32_t reg, uint32_t bitmask);
int32_t USER_PHY_GenericRegisterDisable(user_phy_Object_t *pObj, uint32_t reg, uint32_t bitmask);
int32_t USER_PHY_ExtendedRegisterWrite(user_phy_Object_t *pObj, uint32_t reg, uint32_t value);
int32_t USER_PHY_ExtendedRegisterEnable(user_phy_Object_t *pObj, uint32_t reg, uint32_t bitmask);
int32_t USER_PHY_ExtendedRegisterDisable(user_phy_Object_t *pObj, uint32_t reg, uint32_t bitmask);


#endif /* INC_ETHERNET_DRIVER_H_ */
