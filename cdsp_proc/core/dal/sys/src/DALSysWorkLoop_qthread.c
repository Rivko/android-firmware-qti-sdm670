/*==============================================================================
@file  _DALSYSWorkLoopObj.c

DAL SYS Work Loop implementation for windows

        Copyright © 2008 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary
==============================================================================*/
#include "DALSys.h"
#include "DALStdDef.h"
#include "DALSysCmn.h"
#include "qurt.h"

extern void _WorkLoopExecute(void *pArg);

DALResult 
DALSYS_WorkLoopThreadCreate(_DALSYSWorkLoopObj *pWorkLoopObj, 
                             char * pszname, 
                             unsigned int dwStackSize, 
                             unsigned int dwPriority )
{
   qurt_thread_attr_t threadAttr;
   unsigned short qurtPriority;
   int ret;

   // priority conversion is needed from DAL WL priority to QuRT priority
   // DAL WL priority is highest at 255 and lowest at 0
   // QuRT thread priority is highest at 0
   qurtPriority = 255-dwPriority;

   qurt_thread_attr_init( &threadAttr );
   qurt_thread_attr_set_name( &threadAttr, pszname );
   qurt_thread_attr_set_stack_size( &threadAttr, dwStackSize );
   qurt_thread_attr_set_stack_addr( &threadAttr, pWorkLoopObj->pStackBuffer );
   qurt_thread_attr_set_priority( &threadAttr, qurtPriority );

   ret = qurt_thread_create( (qurt_thread_t *)&pWorkLoopObj->hThread, 
         &threadAttr, _WorkLoopExecute, pWorkLoopObj );
   if (ret != -1)
      return DAL_SUCCESS;

   return DAL_ERROR;
}

void DALSYS_WorkLoopThreadExit(_DALSYSWorkLoopObj * pWorkLoopObj)
{
   pWorkLoopObj->hThread = (void *)~0; // indicate this thread is terminated
   qurt_thread_exit(0);
}

void DALSYS_WorkLoopThreadJoin(_DALSYSWorkLoopObj * pWorkLoopObj)
{
   if ((void *)~0 != pWorkLoopObj->hThread)
   {
      int status;
      qurt_thread_join((unsigned int)pWorkLoopObj->hThread, &status);
   }
}
