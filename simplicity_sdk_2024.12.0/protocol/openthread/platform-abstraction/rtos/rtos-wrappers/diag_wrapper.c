/*******************************************************************************
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Diag APIs
 *   allowing access to the thread stack in a multi-threaded environment.
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_ot_rtos_adaptation.h"
#include <openthread/diag.h>

#if defined(__GNUC__)
#define REAL_NAME(function) __real_##function
#define WRAPPER_NAME(function) __wrap_##function
#define OT_API_REAL_NAME(function) REAL_NAME(function)
#define OT_API_WRAPPER_NAME(function) WRAPPER_NAME(function)
// #elif defined(__IAR_SYSTEMS_ICC__)
//     #define REAL_NAME(function)             $Super$$##function
//     #define WRAPPER_NAME(function)          $Sub$$##function
//     #define OT_API_REAL_NAME(function)      $Super$$__iar_dl##function
//     #define OT_API_WRAPPER_NAME(function)   $Sub$$__iar_dl##function
#else
#error Unsupported compiler
#endif

extern bool    OT_API_REAL_NAME(otDiagIsEnabled)(otInstance *aInstance);
extern otError OT_API_REAL_NAME(otDiagProcessCmd)(otInstance *aInstance, uint8_t aArgsLength, char *aArgs[]);
extern otError OT_API_REAL_NAME(otDiagProcessCmdLine)(otInstance *aInstance, const char *aString);
extern void    OT_API_REAL_NAME(otDiagSetOutputCallback)(otInstance          *aInstance,
                                                      otDiagOutputCallback aCallback,
                                                      void                *aContext);

bool OT_API_WRAPPER_NAME(otDiagIsEnabled)(otInstance *aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otDiagIsEnabled)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otDiagProcessCmd)(otInstance *aInstance, uint8_t aArgsLength, char *aArgs[])
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otDiagProcessCmd)(aInstance, aArgsLength, aArgs);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otDiagProcessCmdLine)(otInstance *aInstance, const char *aString)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otDiagProcessCmdLine)(aInstance, aString);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otDiagSetOutputCallback)(otInstance *aInstance, otDiagOutputCallback aCallback, void *aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otDiagSetOutputCallback)(aInstance, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
}
