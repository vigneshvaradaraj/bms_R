typedef struct
{
    bool PrechargeFetStatus;
    bool mainFetStatus;
    bool isEocReached;
    bool isEodReached;
    bool isCCisCCcuttOfCurrentReached;
}pduData_t;


#define QUEUE_EVENT_WAIT_TIME   (500) /*500 MS*/
#define ECM_N                   (21U)
typedef enum
{
	PDU_ENGAGE_FET,
	PDU_DISENGAGE_FET,
	
	PDU_EVENT_MAX
}pduTaskEvent_t;


/*Each module should have qHandler*/
extern QueueHandle_t pduQueueHandle;

