#include "stdlib.h"
#include "qurt_elite.h"
#include "qurt.h"
#include "qurt_qdi_constants.h"
#include "qurt_qdi_driver.h"
#include "qurt_qdi.h"
#include "adsp_err_fatal.h"
#include "adspfatalerr_qdi.h"
typedef struct
{
    int handle;
    int initialized;
} QDI_Adspfatalerr_Client_Ctx_Type;

//static QDI_Adspfatalerr_Client_Ctx_Type Adspfatalerr_Client_Ctx;
QDI_Adspfatalerr_Client_Ctx_Type Adspfatalerr_Client_Ctx;


void Adspfatalerr_Init_Client(void)
{
   int size_Adspfatalerr_Client_Ctx = sizeof(Adspfatalerr_Client_Ctx);
   int address_handle = (int)&(Adspfatalerr_Client_Ctx.handle);
   int address_initialized = (int)&(Adspfatalerr_Client_Ctx.initialized );

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "size of structure Adspfatalerr_Client_Ctx: %d, handle address: %d, initilaized flag address: %d",size_Adspfatalerr_Client_Ctx,address_handle,address_initialized);

    Adspfatalerr_Client_Ctx.initialized = 0;
    Adspfatalerr_Client_Ctx.handle = qurt_qdi_open(ADSP_FATALERR_DRV_NAME);
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Adspfatalerr_Client_Ctx.handle value is : %d",Adspfatalerr_Client_Ctx.handle);
    // Init adsp fatal err user dispatcher thread
    if(Adspfatalerr_Client_Ctx.handle >= 0)
    {
        Adspfatalerr_Client_Ctx.initialized = 1;
        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Adspfatalerr_Client_Ctx.initialized value is : %d",Adspfatalerr_Client_Ctx.initialized);
    }
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPFATALERR client is up and running -- Init !");
}

/*
 * @func Adspfatalerr_Deinit_Client
 * @brief Deinitialize Adsp fatal err modules
 **/
void Adspfatalerr_Deinit_Client(void)
{
    qurt_qdi_close(Adspfatalerr_Client_Ctx.handle);
    Adspfatalerr_Client_Ctx.initialized = 0;
}

void AdspfatalerrApi(char* err_str,int strlength)
{
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPFATALERR client is up and running -- Api before invokation !");
#ifdef FEATURE_ENABLE_ADSP_FATAL_ERROR
    qurt_qdi_handle_invoke(Adspfatalerr_Client_Ctx.handle, ADSP_FATAL_ERROR,err_str,strlength);
#endif //FEATURE_ENABLE_ADSP_FATAL_ERROR
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPFATALERR client is up and running -- Api after invokation !");
}
