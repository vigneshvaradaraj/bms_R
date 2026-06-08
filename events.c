#include "events.h"


/*battery_R.c events*/
void postbattery_Revent(BatteryEvent_t* event)
{
    xQueueSend(battery_RqueueHandle,event,pdMS_TO_TICKS(QUEUE_EVENT_WAIT_TIME));

}

void sendPduEvent(pduTaskEvent_t* event)
{
	xQueueSend(pduQueueHandle,event,pdMS_TO_TICKS(QUEUE_EVENT_WAIT_TIME));
}

