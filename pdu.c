#include "battery_R.h"

typedef enum
{
	PDU_ENGAGE_FET,
	PDU_DISENGAGE_FET,
	
	PDU_EVENT_MAX
}pduTaskEvent_t;

void pduTask(void) /* This Task should be high priority */
{
	pduTaskEvent_t event;
	while(1)
	{
		xQueueRecieve(pduQueueHandle,&event,portMAX_DELAY);
		
		if(PDU_ENGAGE_FET == event)/* Precharge may not needed for charging*/
		{
			setPrechargeFet();
			vTaskDelay(pdMS_TO_TICKS(PRECHARGE_DELAY));
			setMainFet();
			vTaskDelay(pdMS_TO_TICKS(PRECHARGE_DELAY / 10)); /*Extra 10% delay for precharge*/
			clearPrechargeFet();
		}
		else if(PDU_DISENGAGE_FET == event)
		{
			clearMainFet();
		}
		
	}

}	