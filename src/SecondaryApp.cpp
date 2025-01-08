#include <stdio.h>
#include <string.h>
#include <openthread/coap.h>
#include <openthread/logging.h>
#include "utils/code_utils.h"
#include <assert.h>
#include <stdio.h>

#include <openthread-core-config.h>
#include <openthread-system.h>
#include <openthread/platform/time.h>
#include <openthread-core-config.h>
#include <openthread/config.h>
#include <BaseApplication.h>


#include <mbedtls/platform.h>

#include "cmsis_os2.h"
#include "platform-efr32.h"
#include "sl_cmsis_os2_common.h"
#include "sl_component_catalog.h"
#include "sl_ot_init.h"
#include "sl_ot_rtos_adaptation.h"

#include <platform/OpenThread/OpenThreadUtils.h>
#include <platform/silabs/platformAbstraction/SilabsPlatform.h>

#include <assert.h>
#include <silabs_utils.h>
#include "CodeUtils.h"
#include <task.h>
#include "common/debug.hpp"
#include "LightingManager.h"



#ifndef APP_TASK_STACK_SIZE
#define APP_TASK_STACK_SIZE (8192)
#endif
#define APP_TASK_PRIORITY 8

TaskHandle_t sCoapTaskHandle;

StackType_t CoapStack[APP_TASK_STACK_SIZE / sizeof(StackType_t)];
StaticTask_t CoapTaskStruct;
osThreadId_t sMainThread;

bool server_started = false;
otInstance * coapInstance = NULL;
extern "C" otInstance * otGetMyInstance(void);

uint8_t gpioState[15] = "NONE";

#define URI     "gpio"
char UriPath[]=URI;

void CoapTaskMain(void * pvParameter);
otMessage *responseMessage;

// Function to handle CoAP requests
void coap_request_handler(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
  otError error = OT_ERROR_NONE;

  otCoapCode responseCode = OT_COAP_CODE_CHANGED;
  otCoapCode messageCode = otCoapMessageGetCode(aMessage);
  otCoapType messageType = otCoapMessageGetType(aMessage);

  otCoapMessageInitResponse(responseMessage, aMessage, OT_COAP_TYPE_ACKNOWLEDGMENT, responseCode);
  otCoapMessageSetToken(responseMessage, otCoapMessageGetToken(aMessage),
                       otCoapMessageGetTokenLength(aMessage));
  otCoapMessageSetPayloadMarker(responseMessage);

  strcpy((char*)gpioState, LightMgr().IsLightOn() ? "ON": "OFF");

  if(OT_COAP_CODE_GET == messageCode)
  {
      error = otMessageAppend(responseMessage, gpioState,
                              strlen((const char*)gpioState));
      otEXPECT(OT_ERROR_NONE == error);

      error = otCoapSendResponse((otInstance*)aContext, responseMessage,
                                 aMessageInfo);
      otEXPECT(OT_ERROR_NONE == error);
  }
  else if ((OT_COAP_CODE_POST == messageCode) || (OT_COAP_CODE_PUT == messageCode))

  {
      char data[32];
      uint16_t offset = otMessageGetOffset(aMessage);
      uint16_t read = otMessageRead(aMessage, offset, data, sizeof(data) - 1);
      data[read] = '\0';

      bool ret = false;

      /* process message */
      if(strcmp("ON", data) == 0)
      {
        ret = LightMgr().InitiateAction(AppEvent::kEventType_Light, LightingManager::ON_ACTION);

        otLogCritPlat("GPIO ON\n");
      }
      else if(strcmp("OFF", data) == 0)
      {
        ret = LightMgr().InitiateAction(AppEvent::kEventType_Light, LightingManager::OFF_ACTION);

        otLogCritPlat("GPIO OFF\n");
      }
      else if (strcmp("TOGGLE", data) == 0)
      {
        if (LightMgr().IsLightOn())
          {
            strcpy((char*)data, "OFF");
            ret = LightMgr().InitiateAction(AppEvent::kEventType_Light, LightingManager::OFF_ACTION);
          }
        else
          {
            strcpy((char*)data, "ON");
            ret = LightMgr().InitiateAction(AppEvent::kEventType_Light, LightingManager::ON_ACTION);
          }

        otLogCritPlat("GPIO TOGGLE\n");
      }
      else
      {
        /* no valid body, fail without response */
        otEXPECT_ACTION(false, error = OT_ERROR_NO_BUFS);
      }

      if (OT_COAP_TYPE_CONFIRMABLE == messageType)
      {
        if (ret)
        /* Action has been initiated, we return expected new value */
          error = otMessageAppend(responseMessage, data, strlen((const char*)data));
        else
          /* Action has not been initiated, we return pin state */
          error = otMessageAppend(responseMessage, gpioState, strlen((const char*)gpioState));

        otEXPECT(OT_ERROR_NONE == error);

        error = otCoapSendResponse((otInstance*)aContext,
                                   responseMessage, aMessageInfo);
        otEXPECT(OT_ERROR_NONE == error);
      }
  }

  exit:
  return;
}

otCoapResource coapResourcegpio;

// Function to initialize CoAP server
otError coap_server_init(otInstance *aInstance)
{
    otError err = otCoapStart(aInstance, OT_DEFAULT_COAP_PORT);

    if (err != OT_ERROR_NONE)
      return err;

    coapResourcegpio.mContext=aInstance;
    coapResourcegpio.mUriPath=UriPath;
    coapResourcegpio.mHandler=coap_request_handler;
    coapResourcegpio.mNext=NULL;

    strncpy(UriPath, URI, sizeof(URI));

    // Register the CoAP resource
    otCoapAddResource(aInstance, &coapResourcegpio);


    responseMessage = otCoapNewMessage(aInstance, NULL);
    if (responseMessage == NULL)
         return OT_ERROR_NO_BUFS;

    otLogCritPlat("CoAP server initialized\n");

    return OT_ERROR_NONE;
}

CHIP_ERROR StartCoapTask()
{
  // Start App task.
  sCoapTaskHandle = xTaskCreateStatic(CoapTaskMain, "Coap Task", ArraySize(CoapStack), NULL, APP_TASK_PRIORITY, CoapStack, &CoapTaskStruct);
  sMainThread = (osThreadId_t)sCoapTaskHandle;

  if (sCoapTaskHandle == nullptr)
  {
      SILABS_LOG("Failed to create app Coap task");
      appError(APP_ERROR_CREATE_TASK_FAILED);
  }
  return CHIP_NO_ERROR;
}


#include <openthread/dataset.h>
#include <openthread/dataset_ftd.h>
#include <openthread/dataset_updater.h>

otOperationalDataset dataset;
otOperationalDatasetTlvs sDatasetTlvs;

enum fsm_state
{
   IDLE,
   ATTACHING,
   STARTING_COAP,
   INITIATED,
};

volatile enum fsm_state state = IDLE;

void CoapTaskMain(void * pvParameter)
{
  OT_UNUSED_VARIABLE(pvParameter);

  coapInstance = otGetMyInstance();
  const TickType_t xDelay = 50;

  otLogCritPlat("Coap Task started");

  while (!otSysPseudoResetWasRequested())
  {
      switch (state)
      {
        case IDLE:
          if (otDatasetIsCommissioned(coapInstance) == false)
          {
             /* Create Dataset */
             OT_ASSERT(otDatasetCreateNewNetwork(coapInstance, &dataset) == OT_ERROR_NONE);
             otDatasetConvertToTlvs(&dataset, &sDatasetTlvs);
             /* Commit it */
             OT_ASSERT(otDatasetSetActiveTlvs(coapInstance, &sDatasetTlvs) == OT_ERROR_NONE);
             otLogCritPlat("OT proprietary: Dataset created and committed");
           }
           state = ATTACHING;
          break;
        case ATTACHING:
          if (otThreadGetDeviceRole(coapInstance) == OT_DEVICE_ROLE_DISABLED && otDatasetIsCommissioned(coapInstance))
          {
             // Enable the Thread IPv6 interface.
             OT_ASSERT(otIp6SetEnabled(coapInstance, true) == OT_ERROR_NONE);
             OT_ASSERT(otThreadSetEnabled(coapInstance, true) == OT_ERROR_NONE);
             otLogCritPlat("OT proprietary: ifconfig up and thread start");
             state = STARTING_COAP;
          }
          else if (otThreadGetDeviceRole(sInstance) > OT_DEVICE_ROLE_DETACHED)
          {
            state = STARTING_COAP;
          }
          break;
        case STARTING_COAP:
         if (otThreadGetDeviceRole(sInstance) > OT_DEVICE_ROLE_DETACHED)
         {
             // Thread network is active
             OT_ASSERT(coap_server_init(coapInstance) == OT_ERROR_NONE);
             state = INITIATED;
         }
         break;
        case INITIATED:
         break;
     }
     vTaskDelay( xDelay );

  }
  /* We should never get there */
  otInstanceFinalize(sInstance);
}



