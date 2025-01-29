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


#include <silabs_utils.h>
#include "CodeUtils.h"
#include <task.h>
#include "common/debug.hpp"
#include "LightingManager.h"


#ifndef APP_TASK_STACK_SIZE
#define APP_TASK_STACK_SIZE (4096)
#endif
#define APP_TASK_PRIORITY 8

TaskHandle_t sCoapTaskHandle;

StackType_t CoapStack[APP_TASK_STACK_SIZE /2 / sizeof(StackType_t)];
StaticTask_t CoapTaskStruct;
osThreadId_t sCoapThread;

TaskHandle_t sThreadTaskHandle;

StackType_t ThreadStack[APP_TASK_STACK_SIZE / sizeof(StackType_t)];
StaticTask_t ThreadTaskStruct;
osThreadId_t sThread;

bool server_started = false;
otInstance * coapInstance = NULL;
extern "C" otInstance * otGetMyInstance(void);

uint8_t gpioState[15] = "NONE";

#define URI     "gpio"
char UriPath[]=URI;

void CoapTaskMain(void * pvParameter);
void ProprietaryThreadTaskMain(void * pvParameter);

otMessage *rspMessage;
otMessage *cmdMessage;
otMessageInfo messageInfo;

// Define the multicast address (example: all nodes in the network)
const char *multicastAddress = "ff03::1";

void send_coap_command(void)
{
  if (coapInstance == NULL)
    return;

  cmdMessage = otCoapNewMessage(coapInstance, NULL);
  OT_ASSERT(cmdMessage != NULL);

  // Prepare the CoAP message
  otCoapMessageInit(cmdMessage, OT_COAP_TYPE_NON_CONFIRMABLE, OT_COAP_CODE_PUT);
  otCoapMessageGenerateToken(cmdMessage, OT_COAP_DEFAULT_TOKEN_LENGTH);
  otCoapMessageAppendUriPathOptions(cmdMessage, UriPath);
  otCoapMessageSetPayloadMarker(cmdMessage);

   // Add payload to the message
   const char *payload = "TOGGLE";
   otMessageAppend(cmdMessage, payload, strlen(payload));

   // Send the CoAP message
   otError error = otCoapSendRequest(coapInstance, cmdMessage, &messageInfo, NULL, NULL);

   /* After calling otCoapSendRequest, you should not free the message yourself.
    * However, if otCoapSendRequest returns an error, indicating that the
    * message was not successfully sent, you remain responsible for freeing the
    *  message to prevent memory leaks */

   if (error != OT_ERROR_NONE) {
      otMessageFree(cmdMessage);
      otLogWarnPlat("\nmessage send error %d\r", error);
      }
    else
      otLogWarnPlat("\nmessage sent\r");

}

// Function to handle CoAP requests
void coap_request_handler(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
  otError error = OT_ERROR_NONE;

  otCoapCode responseCode = OT_COAP_CODE_CHANGED;
  otCoapCode messageCode = otCoapMessageGetCode(aMessage);
  otCoapType messageType = otCoapMessageGetType(aMessage);

  if (messageCode == OT_COAP_CODE_GET || OT_COAP_TYPE_CONFIRMABLE == messageType)
  {
    rspMessage = otCoapNewMessage(coapInstance, NULL);
    OT_ASSERT(rspMessage != NULL);

    otCoapMessageInitResponse(rspMessage, aMessage, OT_COAP_TYPE_ACKNOWLEDGMENT, responseCode);
    otCoapMessageSetToken(rspMessage, otCoapMessageGetToken(aMessage),
                       otCoapMessageGetTokenLength(aMessage));
    otCoapMessageSetPayloadMarker(rspMessage);
  }

  strcpy((char*)gpioState, LightMgr().IsLightOn() ? "ON": "OFF");

  switch (messageCode)
  {
    case OT_COAP_CODE_GET:

      error = otMessageAppend(rspMessage, gpioState,
                              strlen((const char*)gpioState));
      otEXPECT(OT_ERROR_NONE == error);

      error = otCoapSendResponse((otInstance*)aContext, rspMessage,
                                 aMessageInfo);
      otEXPECT(OT_ERROR_NONE == error);
      break;

    case OT_COAP_CODE_POST:
    case OT_COAP_CODE_PUT:
    {
      char data[32];
      uint16_t offset = otMessageGetOffset(aMessage);
      uint16_t read = otMessageRead(aMessage, offset, data, sizeof(data) - 1);
      data[read] = '\0';

      LightingManager::Action_t action = LightingManager::ON_ACTION;

      bool ret = false;

      /* process message */
      if(strcmp("ON", data) == 0)
      {
        action = LightingManager::ON_ACTION;
        otLogWarnPlat("GPIO ON\n");
      }
      else if(strcmp("OFF", data) == 0)
      {
        action = LightingManager::OFF_ACTION;
        otLogWarnPlat("GPIO OFF\n");
      }
      else if (strcmp("TOGGLE", data) == 0)
      {
        strcpy((char*)data, LightMgr().IsLightOn() ? "OFF": "ON");
        action = LightMgr().IsLightOn() ? LightingManager::OFF_ACTION : LightingManager::ON_ACTION;
        otLogWarnPlat("GPIO TOGGLE\n");
      }
      else
      {
        /* no valid body, fail without response */
        otEXPECT_ACTION(false, error = OT_ERROR_NO_BUFS);
      }
      /* We use kEventType_Button to update cluster after action */
      ret = LightMgr().InitiateAction(AppEvent::kEventType_Coap, action);

      if (OT_COAP_TYPE_CONFIRMABLE == messageType)
      {
        if (ret)
        /* Action has been initiated, we return expected new value */
          error = otMessageAppend(rspMessage, data, strlen((const char*)data));
        else
          /* Action has not been initiated, we return pin state */
          error = otMessageAppend(rspMessage, gpioState, strlen((const char*)gpioState));

        otEXPECT(OT_ERROR_NONE == error);

        error = otCoapSendResponse((otInstance*)aContext,
                                   rspMessage, aMessageInfo);
        otEXPECT(OT_ERROR_NONE == error);
      }
    }
      break;
    default:
      break;
  }

  exit:
  if (error != OT_ERROR_NONE && rspMessage != NULL)
     {
       otMessageFree(rspMessage);
     }

  return;
}

otCoapResource coapResourcegpio;

// Function to initialize CoAP server
otError coap_server_init(otInstance *coapInstance)
{
    otError err = otCoapStart(coapInstance, OT_DEFAULT_COAP_PORT);

    if (err != OT_ERROR_NONE)
      return err;

    coapResourcegpio.mContext=coapInstance;
    coapResourcegpio.mUriPath=UriPath;
    coapResourcegpio.mHandler=coap_request_handler;
    coapResourcegpio.mNext=NULL;

    strncpy(UriPath, URI, sizeof(URI));

    // Register the CoAP resource
    otCoapAddResource(coapInstance, &coapResourcegpio);

     // Prepare message info
     memset(&messageInfo, 0, sizeof(messageInfo));
     otIp6AddressFromString(multicastAddress, &messageInfo.mPeerAddr);

     messageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;

    otLogWarnPlat("CoAP server initialized\n");

    return OT_ERROR_NONE;
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

  otLogWarnPlat("Coap Task started");

  while (!otSysPseudoResetWasRequested())
  {
      switch (state)
      {
        case IDLE:
          if (otDatasetIsCommissioned(coapInstance) == false)
          {
             /* Create Dataset */
             OT_ASSERT(otDatasetCreateNewNetwork(coapInstance, &dataset) == OT_ERROR_NONE);
             dataset.mChannel = 16;
             strcpy(dataset.mNetworkName.m8, "OTNAME");
             otDatasetConvertToTlvs(&dataset, &sDatasetTlvs);
             /* Commit it */
             OT_ASSERT(otDatasetSetActiveTlvs(coapInstance, &sDatasetTlvs) == OT_ERROR_NONE);
             otLogWarnPlat("OT proprietary: Dataset created and committed");
           }
          else
           state = ATTACHING;
          break;
        case ATTACHING:
          if (otThreadGetDeviceRole(coapInstance) == OT_DEVICE_ROLE_DISABLED && otDatasetIsCommissioned(coapInstance))
          {
             // Enable the Thread IPv6 interface.
             OT_ASSERT(otIp6SetEnabled(coapInstance, true) == OT_ERROR_NONE);
             OT_ASSERT(otThreadSetEnabled(coapInstance, true) == OT_ERROR_NONE);
             otLogWarnPlat("OT proprietary: ifconfig up and thread start");
             state = STARTING_COAP;
          }
          else if (otThreadGetDeviceRole(coapInstance) > OT_DEVICE_ROLE_DETACHED)
          {
            state = STARTING_COAP;
          }
          break;
        case STARTING_COAP:
         if (otThreadGetDeviceRole(coapInstance) > OT_DEVICE_ROLE_DETACHED)
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
}


CHIP_ERROR StartCoapTask()
{
  // Start Thread task.
  sThreadTaskHandle = xTaskCreateStatic(ProprietaryThreadTaskMain, "Thread Task", ArraySize(ThreadStack), NULL, SL_OPENTHREAD_RTOS_STACK_TASK_PRIORITY, ThreadStack, &ThreadTaskStruct);
  sThread = (osThreadId_t)sThreadTaskHandle;

  if (sThreadTaskHandle == nullptr)
  {
      SILABS_LOG("Failed to create app Thread task");
      appError(APP_ERROR_CREATE_TASK_FAILED);
  }

  // Start App task.
  sCoapTaskHandle = xTaskCreateStatic(CoapTaskMain, "Coap Task", ArraySize(CoapStack), NULL, APP_TASK_PRIORITY, CoapStack, &CoapTaskStruct);
  sCoapThread = (osThreadId_t)sCoapTaskHandle;

  if (sCoapTaskHandle == nullptr)
  {
      SILABS_LOG("Failed to create app Coap task");
      appError(APP_ERROR_CREATE_TASK_FAILED);
  }

  return CHIP_NO_ERROR;
}


/* ---------------------------------------------------------------------------*/

otInstance * myOtInstance = NULL;

extern "C" void otAppCliInit(otInstance *aInstance);

otInstance *otInstanceInitSingle(void)
{
   return otInstanceInitMultiple(0);
}

extern "C" otInstance * otGetInstance(void);
extern "C" otInstance * otGetMyInstance(void)
{
  return myOtInstance;
}

#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE

#define MATTER_INST_ID 0
#define PROPRIETARY_INST_ID 1

otInstance *sInstances[OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_NUM];

extern "C" otInstance *otPlatMultipanIidToInstance(uint8_t aIid)
{
   return ((aIid - 1) == MATTER_INST_ID ? otGetInstance() : myOtInstance);
}

extern "C" uint8_t otPlatMultipanInstanceToIid(otInstance *aInstance)
{
  uint8_t idx = otInstanceGetIdx(aInstance);
  return idx + 1;
}

#endif


void ProprietaryThreadTaskMain(void * pvParameter)
{
    OT_UNUSED_VARIABLE(pvParameter);

    const TickType_t xDelay = 50;

#if defined(SL_CATALOG_OT_CRASH_HANDLER_PRESENT)
    efr32PrintResetInfo();
#endif

    myOtInstance = otInstanceInitMultiple(1);
    otInstance * matterOtInstance = otGetInstance();

    OT_ASSERT(matterOtInstance != NULL);
    OT_ASSERT(myOtInstance!= NULL);


    sInstances[MATTER_INST_ID] = otGetInstance();
    sInstances[PROPRIETARY_INST_ID] = myOtInstance;

    /* Move CLI to Proprietary Thread instance */
    otAppCliInit(myOtInstance);

    while (!otSysPseudoResetWasRequested())
    {
        // Acquire mutex for stack access
        sl_ot_rtos_acquire_stack_mutex();

        // Process callbacks and tasklets
        otSysProcessDrivers(myOtInstance);
        otTaskletsProcess(myOtInstance);

        // Release the stack mutex
        sl_ot_rtos_release_stack_mutex();

        vTaskDelay( xDelay );
    }
}

