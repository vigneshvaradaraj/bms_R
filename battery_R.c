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
    VOLTAGE_TEMP_UPDATE_EVENT,
    DERATED_CURRENT_CAL_EVENT,
    PRECHARGE_EVENT,
    DISCHARGE_EVENT,
    CHARGE_EVENT,
    THERMAL_RUNAWAY_EVENT,
    SOO_EVENT,
    BLANCING_EVENT,
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

 struct
 {
     int16_t allCellTemp[CELL_TEMP_SENSOR_COUNT];
     int16_t allCellVoltage[CELLS_IN_SERIES];
     current_t current;
     uint8_t soc;
     tempData_t cellTemp;
    
 }gCVT_s;

 eventHandler_t evetArray[STATE_MAX][EVENT_MAX] = \
 {
    [CHARGING_STATE] = {VOLTAGE_TEMP_UPDATE_EVENT_h,chargeDeratedCurrent_h,PRECHARGE_EVENT_h,DISCHARGE_EVENT_h,CHARGE_EVENT_h,SOO_EVENT_h},
	[DISCHARGING_STATE] = {VOLTAGE_TEMP_UPDATE_EVENT_h,disChargeDeratedCurrent_h,PRECHARGE_EVENT_h,DISCHARGE_EVENT_h,CHARGE_EVENT_h,SOO_EVENT_h}

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
        gCVT_s.cellTemp.tempMin = MIN(testArry[i],gCVT_s.cellTemp.tempMin);
    }
    
    gCVT_s.cellTemp.tempMax = gCVT_s.allCellTemp[0];
    
    for(int i = 1;i < CELL_TEMP_SENSOR_COUNT;i++)
    {
        gCVT_s.cellTemp.tempMax = MAX(testArry[i],gCVT_s.cellTemp.tempMax);
    }
    
    //TODO: voltage needs to be implemented
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

void SOO_EVENT_h(void)//event needs to be triggered every 1 sec once
{
    updateDischargeTankValue();
    updateChargeTankValue();
    
    gSoo_s.dischargeTankValue += (gCVT_s.current.dischargeCurrent * gCVT_s.current.dischargeCurrent) - (DISCHARGE_RECO_CURRENT * DISCHARGE_RECO_CURRENT);
    gSoo_s.chargeTankValue += (gCVT_s.current.chargeCurrent * gCVT_s.current.chargeCurrent) - (CHARGE_RECO_CURRENT * CHARGE_RECO_CURRENT);
    
    gSoo_s.remainingDischargeTankValue = gSoo_s.deratedDischargeTankValue - gSoo_s.dischargeTankValue;
    gSoo_s.remainingChargeTankValue = gSoo_s.deratedChargeTankValue - gSoo_s.ChargeTankValue;
    
    gSoo_s.dischargePeakRecoCurrent = sqrt((gSoo_s.remainingDischargeTankValue/TANK_DELTA_TIME) + (gSoo_s.dischargeRecoCurrent * gSoo_s.dischargeRecoCurrent));
    gSoo_s.chargePeakRecoCurrent = sqrt((gSoo_s.remainingChargeTankValue/TANK_DELTA_TIME) + (gSoo_s.chargeRecoCurrent * gSoo_s.chargeRecoCurrent));
    
    if((gSoo_s.dischargeTankValue > gSoo_s.deratedDischargeTankValue) || (gSoo_s.chargeTankValue > gSoo_s.deratedChargeTankValue))
    {
        setError(OVER_LOAD);
    }
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
    *chargeSopDeratedFactor = calculateSOPfactor(gCVT_s.soc,gCVT_s.temp.tempMax);
	//TODO: add charge parameter to calculateSOPfactor()
}

void getDischargeSOPderatedFactor(uint8_t* dischargeSopDeratedFactor)
{
    *dischargeSopDeratedFactor = calculateSOPfactor(gCVT_s.soc,gCVT_s.temp.tempMax);
	//TODO: add discharge parameter to calculateSOPfactor()
}
void updateDischargeTankValue(void)
{
    gSoo_s.deratedDischargeTankValue = DISCHARGE_TANK_VALUE * gSoo_s.dischargeSopDeratedFactor;
}

void updateChargeTankValue(void)
{
    gSoo_s.deratedChargeTankValue = CHARGE_TANK_VALUE * gSoo_s.chargeSopDeratedFactor;
}

