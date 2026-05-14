
uint8_t disChargingSopFactorTable[SOC_STEPS_MAX][TEMP_STEPS_MAX] = \
{
   /*10'C 20'C 30'C 40'C 50'C*/
    {0,   1,   2,   4,   5}, //0% SOC
    {20,  30,  45,  50,  53}, //10% SOC
    {0,   40,  55,   0,   0}, //20% SOC
    {0,   0,   0,   0,   0}, //30% SOC
    {0,   0,   0,   0,   0}, //40% SOC
    {0,   0,   0,   0,   0}, //50% SOC
    {0,   0,   0,   0,   0}, //60% SOC
    {0,   0,   0,   0,   0}, //70% SOC
    {0,   0,   0,   0,   0}, //80% SOC
    {0,   0,   0,   0,   0}, //90% SOC
    {0,   0,   0,   0,   0}, //100% SOC
};

uint8_t sopRefSteps[SOC_STEPS_MAX] = {0,10,20,30,40,50,60,70,80,90,100};
uint8_t tempRefSteps[TEMP_STEPS_MAX] = {10,20,30,40,50};