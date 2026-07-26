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

static const float OCV_V_dchg[ECM_N] = {
    2.8442f, 3.1634f, 3.2057f, 3.2217f, 3.2475f, 3.2636f, 3.281f, 3.2897f, 3.2907f, 3.2913f, 3.292f, 3.2934f, 3.2978f, 3.3258f, 3.3302f, 3.3303f, 3.3307f, 3.3311f, 3.3316f, 3.3329f, 3.5014f
};
static const float OCV_V_chg[ECM_N] = {
    2.8442f, 3.16934f, 3.22208f, 3.2420199999999997f, 3.2720000000000002f, 3.29142f, 3.30092f, 3.30352f, 3.30416f, 3.3050800000000002f, 3.30646f, 3.30802f, 3.31464f, 3.33434f, 3.34042f, 3.3401f, 3.34004f, 3.33972f, 3.33908f, 3.339775f, 3.5014f
};

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
	if(getChargingOCV() > EOC_CUTT_OFF_THRESHOLD_H)
	{
		s_pduData.isEocReached = true;
		if(true == s_pduData.mainFetStatus)
		{
			xQueueSend(pduQueueHandle,&event,pdMS_TO_TICKS(QUEUE_EVENT_WAIT_TIME));
		}
	}
	else if (getChargingOCV() < EOC_CUTT_OFF_THRESHOLD_L)
	{
		s_pduData.isEocReached = false;
	}
}

bool isEodReached(void)
{
	pduTaskEvent_t event = PDU_DISENGAGE_FET;
	if(getDisChargingOCV() < EOD_CUTT_OFF_THRESHOLD_L)
	{
		s_pduData.isEodReached = true;
		if(true == s_pduData.mainFetStatus)
		{
			xQueueSend(pduQueueHandle,&event,pdMS_TO_TICKS(QUEUE_EVENT_WAIT_TIME));
		}
	}
	else if (getDisChargingOCV() > EOD_CUTT_OFF_THRESHOLD_H)
	{
		s_pduData.isEodReached = false;
	}
}

bool isChargingCuttOfCurrentReached(void)
{
	pduTaskEvent_t event = PDU_DISENGAGE_FET;
	if(gCVT_s.current.chargeCurrent <= CC_CUTT_OFF_THREASHOLD_L)
	{
		if(true == s_pduData.mainFetStatus)
		{
			/*update the flag first and disengage the fet (we might have timing mismatch on CAN)*/
			/*TODO: when do we need to clear this flag*/
			s_pduData.isChargingCuttOfCurrentReached = true;
			xQueueSend(pduQueueHandle,&event,pdMS_TO_TICKS(QUEUE_EVENT_WAIT_TIME));
		}
	}
}
float ecm_lookup(const float *table, float soc)
{
    if (soc <= 0.0f) return table[0];
    if (soc >= 1.0f) return table[ECM_N - 1];
    float pos = soc * (ECM_N - 1);     /* 0..20 */
    int   i   = (int)pos;
    float f   = pos - (float)i;
    return table[i] * (1.0f - f) + table[i + 1] * f;
}
float getDisChargingOCV(void)
{
	/*get min cell voltage*/
	return ecm_lookup(OCV_V_dchg,soc);
}

float getChargingOCV(void)
{
	/*get min cell voltage*/
	return ecm_lookup(OCV_V_chg,soc);
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

