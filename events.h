
#include "battery_R.h"
#include "pdu.h"

#define MAX_EVENT_HANDLER_10_MS         (20)
#define MAX_EVENT_HANDLER_100_MS        (20)
#define MAX_EVENT_HANDLER_1_S           (20)
#define MAX_EVENT_HANDLER_500_MS        (20)

typedef enum
{
    EVENT_10_MS,
    EVENT_100_MS,
    EVENT_500_MS,
    EVENT_1_S
}eventTimer_t;

extern void (*eventHandlerArray[MAX_EVENT_HANDLER_10_MS])(void);
/*all modules post event functions protoType*/
void sendPduEvent(pduTaskEvent_t* event);