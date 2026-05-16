typedef void (*eventHandler_t)(void);

 typedef enum
 {
     OVER_VOLTAGE,
     UNDER_VOLTAGE,
     OVER_TEMP,
     UNDER_TEMP,
     OVER_LOAD,
     OVER_CURRENT,
     SHORT_CIRCUIT,
     CELL_IMBALANCE,
	 THERMAL_RUNAWAY,
    
     ERROR_MAX
 }error_t;
 typedef enum
 {
    CHARGING_STATE,
    DISCHARGING_STATE,
    STATE_MAX
 }BatteryState_t;

 typedef enum
 {
    VOLTAGE_TEMP_EVENT, //AFE ISR triggers when all the voltage and temp updated via DMA
    DERATED_CURRENT_CAL_EVENT, 
    PRECHARGE_EVENT,		   //TODO
    CHARGE_DISCHARGE_EVENT,    //TODO
    THERMAL_RUNAWAY_EVENT,     
    SOO_EVENT,					
    BLANCING_EVENT,            //TODO
    EVENT_MAX
 }BatteryEvent_t;

 struct
 {
     bool isErrorPresent;
 }gErrInfo_s;

 struct
 {
    TimerHandle_t voltageTempUpdateTimerHandle;
    TimerHandle_t deratedCurrentCalTimerHandle;
    TimerHandle_t dischargeEventTimerHandle;//one short timer triggered by Precharge handler
    
 }gEventTriggerTimers_s;

 struct
 {
     int16_t chargeRecoCurrent;
     int16_t dischargeRecoCurrent;
     int16_t chargePeakRecoCurrent;
     int16_t dischargePeakRecoCurrent;
     int32_t chargeTankValue;
     int32_t dischargeTankValue;
     uint32_t deratedDischargeTankValue;
     uint32_t deratedChargeTankValue;
     uint32_t remainingDischargeTankValue;
     uint32_t remainingChargeTankValue;
     uint8_t chargeSopDeratedFactor;//factor From SOP table WRT SOC & TEMP
     uint8_t dischargeSopDeratedFactor;//factor From SOP table WRT SOC & TEMP, get it using bilinear interpolation
	 uint8_t chargeDeratedCurrent;
	 uint8_t dischargeDeratedCurrent;
 }gSoo_s;

 typedef struct
 {
     int16_t chargeCurrent;
     int16_t dischargeCurrent;
 }current_t;

 typedef struct
 {
     uint8_t tempMax;
     uint8_t tempMin;
     uint8_t tempMean;
 }tempData_t;

 tyepdef struct
 {
	 uint8_t cellVoltageMax;
	 uint8_t cellVoltageMin;
 }cellVoltageData_t
 struct
 {
     int16_t allCellTemp[CELL_TEMP_SENSOR_COUNT];
     int16_t allCellVoltage[CELLS_IN_SERIES];
     current_t current;
     uint8_t soc;
     tempData_t cellTemp;
     cellVoltageData_t cellvolt;
 }gCVT_s;

 eventHandler_t evetArray[STATE_MAX][EVENT_MAX] = \
 {
    [CHARGING_STATE] = {
							VOLTAGE_TEMP_UPDATE_EVENT_h,\
							chargeDeratedCurrent_h,\
							PRECHARGE_EVENT_h,\
							CHARGE_EVENT_h,\
							thermalRunaway_h,\
							chargeSOO_EVENT_h,\
							cellBlancing_h\
						},
	[DISCHARGING_STATE] = {
							VOLTAGE_TEMP_UPDATE_EVENT_h,\
							disChargeDeratedCurrent_h,\
							PRECHARGE_EVENT_h,\
							DISCHARGE_EVENT_h,\
							thermalRunaway_h,\
							dischargeSOO_EVENT_h,\
							cellBlancing_h\
						  }

 };
 gErrInfo_s arrayError[ERROR_MAX];

 void errorInit(void)
 {
     for(int i = 0;i < ERROR_MAX;i++)
     {
         arrayError[i].isErrorPresent = false;
     }
 }
 void BatteryInit(void)
 {
     errorInit();
    
     //Timer init
     //Task init
 }
 void batteryProcess_R(void)
 {
    BatteryEvent_t event;
    BatteryState_t state = MAIN_STATE;
    while(1)
    {
        xQueueRecieve(queueHandle,&event,portMAX_DELAY);
        if(NULL != evetArray[state][event]())
        {
            evetArray[state][event]();
        }
    }        
 }
 
void VOLTAGE_TEMP_UPDATE_EVENT_h(void)//This event should be posted from AFE ISR
{
    gCVT_s.cellTemp.tempMin = gCVT_s.allCellTemp[0];
    
    for(int i = 1;i < CELL_TEMP_SENSOR_COUNT;i++)
    {
        gCVT_s.cellTemp.tempMin = MIN(gCVT_s.allCellTemp[i],gCVT_s.cellTemp.tempMin);
    }
    
    gCVT_s.cellTemp.tempMax = gCVT_s.allCellTemp[0];
    
    for(int i = 1;i < CELL_TEMP_SENSOR_COUNT;i++)
    {
        gCVT_s.cellTemp.tempMax = MAX(testArry[i],gCVT_s.cellTemp.tempMax);
    }

	gCVT_s.cellvolt.cellVoltageMin = gCVT_s.allCellVoltage[0];
    
    for(int i = 1;i < CELLS_IN_SERIES_COUNT;i++)
    {
        gCVT_s.cellvolt.cellVoltageMin = MIN(gCVT_s.allCellVoltage[i],gCVT_s.cellvolt.cellVoltageMin);
    }
    
    gCVT_s.cellvolt.cellVoltageMax = gCVT_s.allCellVoltage[0];
    
    for(int i = 1;i < CELLS_IN_SERIES_COUNT;i++)
    {
        gCVT_s.cellvolt.cellVoltageMax = MIN(gCVT_s.allCellVoltage[i],gCVT_s.cellvolt.cellVoltageMax);
    }
}

void chargeDeratedCurrent_h(void)
{
    gSoo_s.chargeDeratedCurrent = getChargeSOPderatedFactor() * CHARGE_RECO_CURRENT;
}

void disChargeDeratedCurrent_h(void)
{
	gSoo_s.dischargeDeratedCurrent = getdishargeSOPderatedFactor() * DISCHARGE_RECO_CURRENT;
}

void PRECHARGE_EVENT_h(void)
{
    
}
void DISCHARGE_EVENT_h(void)
{
    
}
void CHARGE_EVENT_h(void)
{
    
}

void thermalRunaway_h(void)
{
	if(gCVT_s.cellvolt.cellVoltageMax > THERMAL_RUNAWAY_VOLTAGE_THRESHOLD || gCVT_s.cellTemp.tempMax > THERMAL_RUNAWAY_TEMP_THRESHOLD)
	{
		setError(THERMAL_RUNAWAY);
	}
	
}

void chargeSOO_EVENT_h(void)//event needs to be triggered every 1 sec once
{
    updateChargeTankValue();
    gSoo_s.chargeTankValue += (gCVT_s.current.chargeCurrent * gCVT_s.current.chargeCurrent) - (CHARGE_RECO_CURRENT * CHARGE_RECO_CURRENT);
    
    gSoo_s.remainingChargeTankValue = gSoo_s.deratedChargeTankValue - gSoo_s.ChargeTankValue;
    
    gSoo_s.chargePeakRecoCurrent = sqrt((gSoo_s.remainingChargeTankValue/TANK_DELTA_TIME) + (gSoo_s.chargeRecoCurrent * gSoo_s.chargeRecoCurrent));
    
    if(gSoo_s.chargeTankValue > gSoo_s.deratedChargeTankValue)
    {
        setError(OVER_LOAD);
    }
}

void dischargeSOO_EVENT_h(void)//event needs to be triggered every 1 sec once
{
    updateDischargeTankValue();
    
    gSoo_s.dischargeTankValue += (gCVT_s.current.dischargeCurrent * gCVT_s.current.dischargeCurrent) - (DISCHARGE_RECO_CURRENT * DISCHARGE_RECO_CURRENT);
    
    gSoo_s.remainingDischargeTankValue = gSoo_s.deratedDischargeTankValue - gSoo_s.dischargeTankValue;
    
    gSoo_s.dischargePeakRecoCurrent = sqrt((gSoo_s.remainingDischargeTankValue/TANK_DELTA_TIME) + (gSoo_s.dischargeRecoCurrent * gSoo_s.dischargeRecoCurrent));
    
    if((gSoo_s.dischargeTankValue > gSoo_s.deratedDischargeTankValue))
    {
        setError(OVER_LOAD);
    }
}

void cellBlancing_h(void)
{
	uint16_t cellMask = 0;//uint16_t max 16 cells in series
	/*check list before balancing the cells
		1)There should be no error
		2)Check cell Temp threshold
	*/
	for(int i = 0; i < CELLS_IN_SERIES_COUNT;i++)
	{
		if(gCVT_s.allCellVoltage[i] > CELL_BALANCING_THRESHOLD_FORCE_START)
		{
			cellMask |= (1 << i);
			continue;
		}
		if(gCVT_s.allCellVoltage[i] < CELL_BALANCING_THRESHOLD_FORCE_STOP)
		{
			cellMask &= ~(1 << i);
			continue;
		}
        //TODO: delta implimentation pending
		
	}
	
	setBalancingCellMask(cellMask);
}

 void setError(error_t errorType)
 {
     arrayError[errorType].isErrorPresent = true;
 }

 void clearError(error_t errorType)
 {
     arrayError[errorType].isErrorPresent = false;
 }
 
void getChargeSOPderatedFactor(uint8_t* chargeSopDeratedFactor)
{
    *chargeSopDeratedFactor = calculateSOPfactor(CHARGING,gCVT_s.soc,gCVT_s.temp.tempMax);
}

void getDischargeSOPderatedFactor(uint8_t* dischargeSopDeratedFactor)
{
    *dischargeSopDeratedFactor = calculateSOPfactor(DISCHARGING,gCVT_s.soc,gCVT_s.temp.tempMax);
}
void updateDischargeTankValue(void)
{
    gSoo_s.deratedDischargeTankValue = DISCHARGE_TANK_VALUE * gSoo_s.dischargeSopDeratedFactor;
}

void updateChargeTankValue(void)
{
    gSoo_s.deratedChargeTankValue = CHARGE_TANK_VALUE * gSoo_s.chargeSopDeratedFactor;
}

