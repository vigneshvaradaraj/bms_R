#include "battery_R.h"
#include "pdu.h"

/*
	Note:
	Fet should be disEngaged in below condition
	1)If any error 
	2)If EOC/EOD reached reached
	3)If minimum charging current is reached in CV area while charging
*/

pduData_t s_pduData;/* we amy need to intialize all the global struct to zero*/

QueueHandle_t pduQueueHandle;

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

bool isAnyErrorPresent(void)/*1Sec event*/
{
	int errorCount = 0;
	bool isErrorPresent = false;
	for (;errorCount < ERROR_MAX;errorCount++)
	{
		if(true == bmsError[i].isErrorPresent)
		{
			isErrorPresent =  true;
		}
	}
	return isErrorPresent;
}

bool isEocReached(void)
{
	pduTaskEvent_t event = PDU_DISENGAGE_FET;
	if(gCVT_s.cellvolt.cellVoltageMax - getDynamicVoltage() > EOC_CUTT_OFF_THRESHOLD_H)
	{
		s_pduData.isEocReached = true;
		if(true == s_pduData.mainFetStatus)
		{
			xQueueSend(pduQueueHandle,&event,pdMS_TO_TICKS(QUEUE_EVENT_WAIT_TIME));
		}
	}
	else if (gCVT_s.cellvolt.cellVoltageMax - getDynamicVoltage() < EOC_CUTT_OFF_THRESHOLD_L)
	{
		s_pduData.isEocReached = false;
	}
}

bool isEodReached(void)
{
	pduTaskEvent_t event = PDU_DISENGAGE_FET;
	if(gCVT_s.cellvolt.cellVoltageMin + getDynamicVoltage() < EOD_CUTT_OFF_THRESHOLD_L)
	{
		s_pduData.isEodReached = true;
		if(true == s_pduData.mainFetStatus)
		{
			xQueueSend(pduQueueHandle,&event,pdMS_TO_TICKS(QUEUE_EVENT_WAIT_TIME));
		}
	}
	else if (gCVT_s.cellvolt.cellVoltageMin + getDynamicVoltage() > EOD_CUTT_OFF_THRESHOLD_H)
	{
		s_pduData.isEodReached = false;
	}
}

bool isCCcuttOfCurrentReached(void)
{
	pduTaskEvent_t event = PDU_DISENGAGE_FET;
	if(gCVT_s.current.chargeCurrent <= CC_CUTT_OFF_THREASHOLD_L)
	{
		if(true == s_pduData.mainFetStatus)
		{
			/*update the flag first and disengage the fet (we might have timing mismatch on CAN)*/
			/*TODO: when do we need to clear this flag*/
			s_pduData.isCCisCCcuttOfCurrentReached = true;
			xQueueSend(pduQueueHandle,&event,pdMS_TO_TICKS(QUEUE_EVENT_WAIT_TIME));
		}
	}
}

/* ********** Fet driver gpio ********* */
void setPrechargeFet(void)
{
	/*Set the corresponding gpio pin*/
	s_pduData.PrechargeFetStatus = true;
}

void setMainFet(void)
{
	/*Set the corresponding gpio pin*/
	s_pduData.mainFetStatus = true;
}

void clearPrechargeFet(void)
{
	/*clear the corresponding gpio pin*/
	s_pduData.PrechargeFetStatus = false;
}

void clearMainFet(void)
{
	/*clear the corresponding gpio pin*/
	s_pduData.mainFetStatus = false;
}

