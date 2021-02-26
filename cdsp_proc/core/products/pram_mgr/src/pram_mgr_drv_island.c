

#include "pram_mgr.h"
#include "pram_mgr_v.h"
#include "pram_mgr_qdi_v.h"


qurt_mutex_t pram_mgr_mutex;

#define PRAM_MGR_LOCK() \
   qurt_rmutex_lock(&pram_mgr_mutex); \

#define PRAM_MGR_UNLOCK() \
   qurt_rmutex_unlock(&pram_mgr_mutex); \


pram_t g_pram = {
   PRAM_PHYSPOOL_NAME,
   0,
   0,
   0,  //Need to get this from qurt_mem_pool_attach
   PRAM_SIZE,
   { 
     { "QMP",
       0,
       0,
       1*1024,
       -1
     },   
     { "SENSORS",
       0,
       0,
       11*1024,
       -1
     },
     { "BUSES",
       0,
       0,
       2*1024,
       -1
     },
     { "GPI",
       0,
       0,
       2*1024,
       -1
     },
     { "TOUCH",
       0,
       0,
       10*1024,
       -1
     }      
   }
};



int pram_mgr_lookup_phys_addr(int client_handle, qurt_addr_t vaddr)
{
   qurt_paddr_t phys_addr;
   qurt_paddr_t pa_base;
   qurt_addr_t va_base;
   qurt_addr_t va_limit;
   int i;

  // phys_addr =  qurt_lookup_physaddr(vaddr); NG: qurt_lookup_physaddr not available in Isand

   PRAM_MGR_LOCK();
   
   for(i=0; i<MAX_PARTITIONS; i++) {
      if(g_pram.partitions[i].owner == client_handle) {

         pa_base = g_pram.partitions[i].base_pa;
         va_base = g_pram.partitions[i].base_va; 
         va_limit = va_base +  g_pram.partitions[i].size;
         
         /* Ensure the physical address is within bounds */
         /*if( (pa_base <= phys_addr) && (phys_addr < pa_limit) ) {
            break;
         }*/
         if( (va_base <= vaddr) && (vaddr < va_limit) ) {
            break;
         }
		 
		 /* If not, continue checking the next partition as there can be 
		    multiple clients within the same PD and client_handle, which we
			cannot differentiate between */
	  } 
   }

   if(i == MAX_PARTITIONS) {
     /* This process does not own any PRAM partitions */
     /* Return Error or error fatal*/
     PRAM_MGR_UNLOCK();
     return 0;
   }

   phys_addr = pa_base + (vaddr-va_base);
   PRAM_MGR_UNLOCK();
   return phys_addr;
}

/*This API should only be called for partitions acquired from within the RootPD, and can be called only from RootPD*/
int pram_mgr_lookup_phys_addr_by_name(char *name, qurt_addr_t vaddr)
{
	qurt_paddr_t phys_addr;
	qurt_paddr_t pa_base;
	qurt_addr_t va_limit;
	qurt_addr_t va_base;

	int i;
	
	// phys_addr =  qurt_lookup_physaddr(vaddr); NG: qurt_lookup_physaddr not available in island
	
	PRAM_MGR_LOCK();
	
	for(i=0; i<MAX_PARTITIONS; i++) {
	   if(!strcmp(g_pram.partitions[i].name, name)) {
	
		  pa_base = g_pram.partitions[i].base_pa;
          va_base = g_pram.partitions[i].base_va;
		  va_limit = va_base + g_pram.partitions[i].size;
	
		  /* Ensure the physical address is within bounds */
		  if( (va_base <= vaddr) && (vaddr < va_limit) ) {
			 break;
		  }
		  
		  /* If not, continue checking the next partition as there can be 
			 multiple clients within the same PD and client_handle, which we
			 cannot differentiate between */
	   } 
	}
	
	if(i == MAX_PARTITIONS) {
	  /* This process does not own any PRAM partitions */
	  /* Return Error or error fatal*/
	  PRAM_MGR_UNLOCK();
	  return 0;
	}
	
	phys_addr = pa_base + (vaddr - va_base);
	PRAM_MGR_UNLOCK();
	return phys_addr;
}



