#include <stdio.h>
#include <stdint.h>

#define SOC_STEPS_MAX (11u)
#define TEMP_STEPS_MAX (5u)

typedef enum
{
	CHARGING,
	DISCHARGING,
	
	MAX
}chargeDischarge_t;

uint8_t linearInterpolation(uint8_t x0,uint8_t x1,uint8_t y0,uint8_t y1,uint8_t x)
{
    return y0 + ((x - x0) * (y1 - y0)) / (x1 - x0);
}

uint8_t biLinearInterpolationDischarging(uint8_t SOC_L,uint8_t SOC_H,uint8_t TEMP_L,uint8_t TEMP_H,\
                              uint8_t SOC_L_INDEX,uint8_t SOC_H_INDEX,uint8_t TEMP_L_INDEX,\
                              uint8_t TEMP_H_INDEX,uint8_t refSOC,uint8_t refTemp)
{
    uint8_t socdisChargingSopFactorTable_1 = 0,socdisChargingSopFactorTable_2 = 0;
    socdisChargingSopFactorTable_1 = linearInterpolation(SOC_L,SOC_H,disChargingSopFactorTable[SOC_L_INDEX][TEMP_H_INDEX],disChargingSopFactorTable[SOC_H_INDEX][TEMP_H_INDEX],refSOC);
    socdisChargingSopFactorTable_2 = linearInterpolation(SOC_L,SOC_H,disChargingSopFactorTable[SOC_L_INDEX][TEMP_L_INDEX],disChargingSopFactorTable[SOC_H_INDEX][TEMP_L_INDEX],refSOC);
    return linearInterpolation(TEMP_L,TEMP_H,socdisChargingSopFactorTable_2,socdisChargingSopFactorTable_1,refTemp);
}

uint8_t biLinearInterpolationCharging(uint8_t SOC_L,uint8_t SOC_H,uint8_t TEMP_L,uint8_t TEMP_H,\
                              uint8_t SOC_L_INDEX,uint8_t SOC_H_INDEX,uint8_t TEMP_L_INDEX,\
                              uint8_t TEMP_H_INDEX,uint8_t refSOC,uint8_t refTemp)
{
    uint8_t socChargingSopFactorTable_1 = 0,socChargingSopFactorTable_2 = 0;
    socChargingSopFactorTable_1 = linearInterpolation(SOC_L,SOC_H,chargingSopFactorTable[SOC_L_INDEX][TEMP_H_INDEX],chargingSopFactorTable[SOC_H_INDEX][TEMP_H_INDEX],refSOC);
    socChargingSopFactorTable_2 = linearInterpolation(SOC_L,SOC_H,chargingSopFactorTable[SOC_L_INDEX][TEMP_L_INDEX],chargingSopFactorTable[SOC_H_INDEX][TEMP_L_INDEX],refSOC);
    return linearInterpolation(TEMP_L,TEMP_H,socChargingSopFactorTable_2,socChargingSopFactorTable_1,refTemp);
}

uint8_t calculateSOPfactor(chargeDischarge_t chargeDischargeInfo,uint8_t soc, uint8_t temp)
{
    uint8_t socIndex_h = 0,socIndex_l = 0,tempIndex_h = 0,tempIndex_l = 0;
    
    int i = 0;
    //calculate SOC index
    for (i = 0;i < SOC_STEPS_MAX;i++)
    {
        if(soc <= sopRefSteps[i]) break;
    }
    /*SOC_H = sopRefSteps[i]*/
    /*SOC_L = sopRefSteps[i-1]*/
    socIndex_h = i;
    socIndex_l = i-1;
    
    //calculate TEMP index
    for (i = 0;i < TEMP_STEPS_MAX;i++)
    {
        if(temp <= tempRefSteps[i]) break;
    }
    /*TEMP_H = tempRefSteps[i]*/
    /*TEMP_L = tempRefSteps[i-1]*/
    tempIndex_h = i;
    tempIndex_l = i-1;
	if(DISCHARGING == chargeDischargeInfo)
	{
		return biLinearInterpolationDisharging(sopRefSteps[socIndex_l],sopRefSteps[socIndex_h],tempRefSteps[tempIndex_l],\
                                tempRefSteps[tempIndex_h],socIndex_l,socIndex_h,tempIndex_l,tempIndex_h,soc,temp);
	}
	else if(CHARGING == chargeDischargeInfo)
	{
		return biLinearInterpolationCharging(sopRefSteps[socIndex_l],sopRefSteps[socIndex_h],tempRefSteps[tempIndex_l],\
                                tempRefSteps[tempIndex_h],socIndex_l,socIndex_h,tempIndex_l,tempIndex_h,soc,temp);
	}

}