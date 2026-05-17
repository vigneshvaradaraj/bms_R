#define UNDER_VOLTAGE_THRESHOLD_H
#define UNDER_VOLTAGE_THRESHOLD_L
#define OVER_VOLTAGE_THRESHOLD_H
#define OVER_VOLTAGE_THRESHOLD_L

#define UNDER_TEMP_THRESHOLD_H
#define UNDER_TEMP_THRESHOLD_L
#define OVER_TEMP_THRESHOLD_H
#define OVER_TEMP_THRESHOLD_L

#define SHORT_CIRCUIT_THRESHOLD_H
#define SHORT_CIRCUIT_THRESHOLD_L

#define CELL_IMBALANCE_H
#define CELL_IMBALANCE_L

#define CHARGE_RECO_CURRENT
#define DISCHARGE_RECO_CURRENT

#define CHARGE_PEAK_RECO_CURRENT
#define DISCHARGE_PEAK_RECO_CURRENT

#define CHARGE_TANK_VALUE
#define DISCHARGE_TANK_VALUE

#define TANK_DELTA_TIME    

#define CELLS_IN_SERIES_COUNT

#define CELL_TEMP_SENSOR_COUNT

#define THERMAL_RUNAWAY_TEMP_THRESHOLD
#define THERMAL_RUNAWAY_VOLTAGE_THRESHOLD
#define THERMAL_RUNAWAY_TEMP_DELTA_THRESHOLD
#define THERMAL_RUNAWAY_TEMP_SAMPLE_COUNT

#define CELL_BALANCING_DELTA_THRESHOLD_START
#define CELL_BALANCING_DELTA_THRESHOLD_STOP
#define CELL_BALANCING_THRESHOLD_FORCE_START
#define CELL_BALANCING_THRESHOLD_FORCE_STOP

#define PRE_CHARGE_RESISTANCE
#define PRECHARGE_CAPACITANCE


#define EOC_CUTT_OFF_THRESHOLD_DELTA /*for handling hysteresis*/
#define EOD_CUTT_OFF_THRESHOLD_DELTA 
#define CC_CUTT_OFF_THREASHOLD_DELTA

#define EOC_CUTT_OFF_THRESHOLD_H
#define EOC_CUTT_OFF_THRESHOLD_L (EOC_CUTT_OFF_THRESHOLD_H - EOC_CUTT_OFF_THRESHOLD_DELTA)
#define EOD_CUTT_OFF_THRESHOLD_H (EOD_CUTT_OFF_THRESHOLD_L + EOD_CUTT_OFF_THRESHOLD_DELTA)
#define EOD_CUTT_OFF_THRESHOLD_L

#define CC_CUTT_OFF_THREASHOLD_H (CC_CUTT_OFF_THREASHOLD_DELTA_L + CC_CUTT_OFF_THREASHOLD_DELTA)
#define CC_CUTT_OFF_THREASHOLD_L (1)/*least current in cc mode while charging*/

#define PRECHARGE_DELAY (5 * PRE_CHARGE_RESISTANCE * PRECHARGE_CAPACITANCE)

#define MIN(a,b) a<b?a:b
#define MAX(a,b) a<b?b:a


typedef enum
 {
     OVER_VOLTAGE,
     UNDER_VOLTAGE,
     OVER_TEMP,
     UNDER_TEMP,
     OVER_LOAD,/*diff over current*/
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
    THERMAL_RUNAWAY_EVENT,     
    SOO_EVENT,					
    BLANCING_EVENT,           
    EVENT_MAX
 }BatteryEvent_t;

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

 typedef struct
 {
	 uint8_t cellVoltageMax;
	 uint8_t cellVoltageMin;
 }cellVoltageData_t

  typedef struct
 {
     bool isErrorPresent;
 }gErrInfo_s;

 extern gErrInfo_s bmsError[ERROR_MAX];
