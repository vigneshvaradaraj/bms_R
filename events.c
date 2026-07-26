#include "events.h"
#include <stdint.h>
#include <stdbool.h>

/*init to NULL other wise ISR might call fp before initAllEventHandlers executes */
void (*eventHandlerArray[MAX_EVENT_HANDLER_10_MS])(void) = {NULL};

/*battery_R.c events*/
void postbattery_Revent(BatteryEvent_t* event)
{
    xQueueSend(battery_RqueueHandle,event,pdMS_TO_TICKS(QUEUE_EVENT_WAIT_TIME));
}

void sendPduEvent(pduTaskEvent_t* event)
{
	xQueueSend(pduQueueHandle,event,pdMS_TO_TICKS(QUEUE_EVENT_WAIT_TIME));
}

bool initAllEventHandlers(void(*pfun_t)(void),eventTimer_t eventTimer)
{
    static int8_t handlerCounter_10ms = 0;
    static int8_t handlerCounter_100ms = 0;
    static int8_t handlerCounter_500ms = 0;
    static int8_t handlerCounter_1s = 0;

    switch (eventTimer)
    {

        case EVENT_10_MS:
        {
            if((NULL != pfun_t) && (MAX_EVENT_HANDLER_10_MS > handlerCounter_10ms))
            {
                eventHandlerArray[handlerCounter_10ms] = pfun_t;
                handlerCounter_10ms++;
            }
            break;
        }
        case EVENT_100_MS:
        {

            break;
        }
        case EVENT_500_MS:
        {

            break;
        }
        case EVENT_1_S:
        {

            break;
        }
        default:

        break;
    }
}

