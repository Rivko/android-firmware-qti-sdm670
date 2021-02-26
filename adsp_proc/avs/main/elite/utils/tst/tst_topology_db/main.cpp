/*==========================================================================
This file implements the unit tests for the topology database.

*/

/*---------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#include "tst_topology_db.h"
#include "EliteTopology_db_if.h"
#include <cstring>
#include <stdlib.h>

/*---------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/
static int get_num_leaks(void);
static void reset_stats(void);
static void simulate_malloc_failure(int num_mallocs_to_skip);
/*---------------------------------------------------------------------------
 * Definitions and Constants
 * -------------------------------------------------------------------------*/
typedef ADSPResult (*test_fn)(void);

#define CHECK(x) \
   if ((result = (x)) != ADSP_EOK) \
   { \
      printf("Got an error code %d at line %d.\n", result, __LINE__);\
      goto cleanup; \
   }

static const uint32_t MIN_TOPOLOGY_ID = 0x10000000;
static const uint32_t MAX_TOPOLOGY_ID = 0x1FFFFFFF;

/*---------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
 * Function Definitions
 * -------------------------------------------------------------------------*/

static ADSPResult create_destroy(void)
{
   ADSPResult result = ADSP_EFAILED;

   CHECK(topo_db_init());

cleanup:
   topo_db_deinit();

   return result;
}

static ADSPResult add_single_topo(void)
{
   ADSPResult result = ADSP_EFAILED;
   audproc_topology_definition_t def;
   const audproc_topology_definition_t *ret_topo = NULL;

   CHECK(topo_db_init());

   memset(&def, 0, sizeof(def));
   def.topology_id = MIN_TOPOLOGY_ID;
   CHECK(topo_db_add_topologies((elite_topo_db_client_type)0, 1, &def));
   CHECK(topo_db_get_topology((elite_topo_db_client_type)0, MIN_TOPOLOGY_ID, &ret_topo));
   CHECK(memcmp(&def, ret_topo, sizeof(def)));

cleanup:
   {
      topo_db_deinit();
   }

   return result;
}

static ADSPResult add_multiple_topo(void)
{
   ADSPResult result = ADSP_EFAILED;
   const uint32_t NUM_TOPOS_TO_ALLOC = 10;
   audproc_topology_definition_t def[NUM_TOPOS_TO_ALLOC];
   const audproc_topology_definition_t *ret_topo = NULL;

   CHECK(topo_db_init());

   {
      memset(&def, 0, sizeof(def));
      for (uint32_t i = 0; i < NUM_TOPOS_TO_ALLOC; i++)
      {
         def[i].topology_id = MIN_TOPOLOGY_ID + i;
         def[i].num_modules = (i + 1) % 16;
         for (uint32_t j = 0; j < def[i].num_modules; j++)
         {
            def[i].module_info[j].module_id = j;
         }
      }
   }

   CHECK(topo_db_add_topologies((elite_topo_db_client_type)0, NUM_TOPOS_TO_ALLOC, def));
   CHECK(topo_db_get_topology((elite_topo_db_client_type)0, MIN_TOPOLOGY_ID, &ret_topo));
   CHECK(memcmp(&def[0], ret_topo, sizeof(def[0])));
   CHECK(topo_db_get_topology((elite_topo_db_client_type)0, MIN_TOPOLOGY_ID + 5, &ret_topo));
   CHECK(memcmp(&def[5], ret_topo, sizeof(def[5])));
   result = topo_db_get_topology((elite_topo_db_client_type)0, MIN_TOPOLOGY_ID + NUM_TOPOS_TO_ALLOC, &ret_topo);
   CHECK((ADSP_ENOTEXIST == result) ? ADSP_EOK : ADSP_EFAILED);

   // Add with a different PP id
   {
      memset(&def, 0, sizeof(def));
      for (uint32_t i = 0; i < NUM_TOPOS_TO_ALLOC; i++)
      {
         def[i].topology_id = MAX_TOPOLOGY_ID - i;
         def[i].num_modules = (i + 1) % 16;
         for (uint32_t j = 0; j < def[i].num_modules; j++)
         {
            def[i].module_info[j].module_id = j;
         }
      }
   }

   CHECK(topo_db_add_topologies((elite_topo_db_client_type)0xFFFFFFFF, NUM_TOPOS_TO_ALLOC, def));
   CHECK(topo_db_get_topology((elite_topo_db_client_type)0xFFFFFFFF, MAX_TOPOLOGY_ID - 5, &ret_topo));
   CHECK(memcmp(&def[5], ret_topo, sizeof(def[5])));
   result = topo_db_get_topology((elite_topo_db_client_type)0, MAX_TOPOLOGY_ID - 5, &ret_topo);
   CHECK((ADSP_ENOTEXIST == result) ? ADSP_EOK : ADSP_EFAILED);
   result = topo_db_get_topology((elite_topo_db_client_type)0xFFFFFFFF, MIN_TOPOLOGY_ID + 5, &ret_topo);
   CHECK((ADSP_ENOTEXIST == result) ? ADSP_EOK : ADSP_EFAILED);

cleanup:
   {
      topo_db_deinit();
   }

   return result;
}

static ADSPResult add_topo_err(void)
{
   ADSPResult result = ADSP_EFAILED;
   const uint32_t NUM_TOPOS_TO_ALLOC = 129;
   audproc_topology_definition_t def[NUM_TOPOS_TO_ALLOC];
   const audproc_topology_definition_t *ret_topo = NULL;

   CHECK(topo_db_init());

   {
      memset(&def, 0, sizeof(def));
      for (uint32_t i = 0; i < NUM_TOPOS_TO_ALLOC; i++)
      {
         def[i].topology_id = MIN_TOPOLOGY_ID + i;
         def[i].num_modules = i % 16;
         for (uint32_t j = 0; j < def[i].num_modules; j++)
         {
            def[i].module_info[j].module_id = j;
         }
      }
   }

   // Store up to the max topologies, but no more
   CHECK(topo_db_add_topologies((elite_topo_db_client_type)0, NUM_TOPOS_TO_ALLOC - 1, def));
   CHECK(topo_db_get_topology((elite_topo_db_client_type)0, MIN_TOPOLOGY_ID + 127, &ret_topo));
   CHECK(memcmp(&def[127], ret_topo, sizeof(def[127])));
   topo_db_deinit();

   // Store more than max topologies
   CHECK(topo_db_init());
   result = topo_db_add_topologies((elite_topo_db_client_type)0, NUM_TOPOS_TO_ALLOC, def);
   CHECK((ADSP_ENORESOURCE == result) ? ADSP_EOK : ADSP_EFAILED);
   topo_db_deinit();

   // Out of memory when storing
   CHECK(topo_db_init());
   simulate_malloc_failure(0);
   result = topo_db_add_topologies((elite_topo_db_client_type)0, 1, def);
   CHECK((ADSP_ENOMEMORY == result) ? ADSP_EOK : ADSP_EFAILED);

cleanup:
   {
      topo_db_deinit();
   }

   return result;
}

static ADSPResult add_multiple_times(void)
{
   ADSPResult result = ADSP_EFAILED;
   audproc_topology_definition_t def;
   const audproc_topology_definition_t *ret_topo = NULL;

   CHECK(topo_db_init());

   memset(&def, 0, sizeof(def));
   def.topology_id = MIN_TOPOLOGY_ID;
   CHECK(topo_db_add_topologies((elite_topo_db_client_type)0, 1, &def));
   CHECK(topo_db_get_topology((elite_topo_db_client_type)0, MIN_TOPOLOGY_ID, &ret_topo));
   CHECK(memcmp(&def, ret_topo, sizeof(def)));

   def.num_modules = 5;
   for (uint32_t j = 0; j < def.num_modules; j++)
   {
      def.module_info[j].module_id = j;
   }
   CHECK(topo_db_add_topologies((elite_topo_db_client_type)0, 1, &def));
   CHECK(topo_db_get_topology((elite_topo_db_client_type)0, MIN_TOPOLOGY_ID, &ret_topo));
   CHECK(memcmp(&def, ret_topo, sizeof(def)));

   def.module_info[0].module_id = 7;
   def.module_info[1].module_id = 8;
   CHECK(topo_db_add_topologies((elite_topo_db_client_type)0, 1, &def));
   CHECK(topo_db_get_topology((elite_topo_db_client_type)0, MIN_TOPOLOGY_ID, &ret_topo));
   CHECK(memcmp(&def, ret_topo, sizeof(def)));

cleanup:
   {
      topo_db_deinit();
   }

   return result;
}

static ADSPResult add_invalid_topo(void)
{
   ADSPResult result = ADSP_EFAILED;
   audproc_topology_definition_t def;
   const audproc_topology_definition_t *ret_topo = NULL;

   {
      memset(&def, 0, sizeof(def));
      def.topology_id = MIN_TOPOLOGY_ID;
      for (uint32_t i = 0; i < sizeof(def.module_info)/sizeof(def.module_info[0]); i++)
      {
         def.module_info[i].module_id = i;
         def.module_info[i].use_lpm = i % 2;
      }
   }

   // Valid topology with zero modules
   CHECK(topo_db_init());
   CHECK(topo_db_add_topologies((elite_topo_db_client_type)0, 1, &def));
   CHECK(topo_db_get_topology((elite_topo_db_client_type)0, MIN_TOPOLOGY_ID, &ret_topo));
   CHECK(memcmp(&def, ret_topo, sizeof(def)));
   topo_db_deinit();

   // Valid topology with MAX modules
   def.num_modules = sizeof(def.module_info)/sizeof(def.module_info[0]);
   CHECK(topo_db_init());
   CHECK(topo_db_add_topologies((elite_topo_db_client_type)0, 1, &def));
   CHECK(topo_db_get_topology((elite_topo_db_client_type)0, MIN_TOPOLOGY_ID, &ret_topo));
   CHECK(memcmp(&def, ret_topo, sizeof(def)));
   topo_db_deinit();

   // Topology with MAX+1 modules
   def.num_modules = sizeof(def.module_info)/sizeof(def.module_info[0]) + 1;
   CHECK(topo_db_init());
   result = topo_db_add_topologies((elite_topo_db_client_type)0, 1, &def);
   CHECK((ADSP_EBADPARAM == result) ? ADSP_EOK : ADSP_EFAILED);
   result = topo_db_get_topology((elite_topo_db_client_type)0, MIN_TOPOLOGY_ID, &ret_topo);
   CHECK((ADSP_ENOTEXIST == result) ? ADSP_EOK : ADSP_EFAILED);
   topo_db_deinit();
   def.num_modules = sizeof(def.module_info)/sizeof(def.module_info[0]);

   // Topology with invalid LPM flag
   def.num_modules = 5;
   def.module_info[4].use_lpm = 2;
   CHECK(topo_db_init());
   result = topo_db_add_topologies((elite_topo_db_client_type)0, 1, &def);
   CHECK((ADSP_EBADPARAM == result) ? ADSP_EOK : ADSP_EFAILED);
   result = topo_db_get_topology((elite_topo_db_client_type)0, MIN_TOPOLOGY_ID, &ret_topo);
   CHECK((ADSP_ENOTEXIST == result) ? ADSP_EOK : ADSP_EFAILED);
   def.module_info[4].use_lpm = 1;
   def.module_info[5].use_lpm = 2;
   CHECK(topo_db_add_topologies((elite_topo_db_client_type)0, 1, &def));
   CHECK(topo_db_get_topology((elite_topo_db_client_type)0, MIN_TOPOLOGY_ID, &ret_topo));
   CHECK(memcmp(&def, ret_topo, sizeof(def)));
   topo_db_deinit();
   def.module_info[5].use_lpm = 1;

   // Topology with repeated modules.
   def.num_modules = 10;
   def.module_info[8].module_id = def.module_info[9].module_id = def.module_info[10].module_id;
   CHECK(topo_db_init());
   result = topo_db_add_topologies((elite_topo_db_client_type)0, 1, &def);
   CHECK((ADSP_EBADPARAM == result) ? ADSP_EOK : ADSP_EFAILED);
   result = topo_db_get_topology((elite_topo_db_client_type)0, MIN_TOPOLOGY_ID, &ret_topo);
   CHECK((ADSP_ENOTEXIST == result) ? ADSP_EOK : ADSP_EFAILED);
   def.module_info[8].module_id = 8;
   CHECK(topo_db_add_topologies((elite_topo_db_client_type)0, 1, &def));
   CHECK(topo_db_get_topology((elite_topo_db_client_type)0, MIN_TOPOLOGY_ID, &ret_topo));
   CHECK(memcmp(&def, ret_topo, sizeof(def)));
   topo_db_deinit();

   // Topology with invalid topology ID.
   CHECK(topo_db_init());
   def.topology_id = MIN_TOPOLOGY_ID - 1;
   result = topo_db_add_topologies((elite_topo_db_client_type)0, 1, &def);
   CHECK((ADSP_EBADPARAM == result) ? ADSP_EOK : ADSP_EFAILED);
   result = topo_db_get_topology((elite_topo_db_client_type)0, def.topology_id, &ret_topo);
   CHECK((ADSP_ENOTEXIST == result) ? ADSP_EOK : ADSP_EFAILED);
   def.topology_id = MIN_TOPOLOGY_ID;
   CHECK(topo_db_add_topologies((elite_topo_db_client_type)0, 1, &def));
   CHECK(topo_db_get_topology((elite_topo_db_client_type)0, def.topology_id, &ret_topo));
   CHECK(memcmp(&def, ret_topo, sizeof(def)));
   def.topology_id = MAX_TOPOLOGY_ID + 1;
   result = topo_db_add_topologies((elite_topo_db_client_type)0, 1, &def);
   CHECK((ADSP_EBADPARAM == result) ? ADSP_EOK : ADSP_EFAILED);
   result = topo_db_get_topology((elite_topo_db_client_type)0, def.topology_id, &ret_topo);
   CHECK((ADSP_ENOTEXIST == result) ? ADSP_EOK : ADSP_EFAILED);
   def.topology_id = MAX_TOPOLOGY_ID;
   CHECK(topo_db_add_topologies((elite_topo_db_client_type)0, 1, &def));
   CHECK(topo_db_get_topology((elite_topo_db_client_type)0, def.topology_id, &ret_topo));
   CHECK(memcmp(&def, ret_topo, sizeof(def)));
   topo_db_deinit();

cleanup:
   {
      topo_db_deinit();
   }

   return result;
}

static ADSPResult add_multiple_times_internal(void)
{
   ADSPResult result = ADSP_EFAILED;
   audproc_topology_definition_t def;
   const audproc_topology_definition_t *ret_topo = NULL;

   CHECK(topo_db_init());

   memset(&def, 0, sizeof(def));
   def.topology_id = MIN_TOPOLOGY_ID;
   CHECK(topo_db_add_internal_topologies((elite_topo_db_client_type)0, 1, &def));
   CHECK(topo_db_get_topology((elite_topo_db_client_type)0, MIN_TOPOLOGY_ID, &ret_topo));
   CHECK(memcmp(&def, ret_topo, sizeof(def)));

   def.num_modules = 5;
   for (uint32_t j = 0; j < def.num_modules; j++)
   {
      def.module_info[j].module_id = j;
   }
   CHECK(topo_db_add_internal_topologies((elite_topo_db_client_type)0, 1, &def));
   CHECK(topo_db_get_topology((elite_topo_db_client_type)0, MIN_TOPOLOGY_ID, &ret_topo));
   CHECK(memcmp(&def, ret_topo, sizeof(def)));

   def.module_info[0].module_id = 7;
   def.module_info[1].module_id = 8;
   CHECK(topo_db_add_internal_topologies((elite_topo_db_client_type)0, 1, &def));
   CHECK(topo_db_get_topology((elite_topo_db_client_type)0, MIN_TOPOLOGY_ID, &ret_topo));
   CHECK(memcmp(&def, ret_topo, sizeof(def)));
   def.topology_id = MIN_TOPOLOGY_ID - 1;
   CHECK(topo_db_add_internal_topologies((elite_topo_db_client_type)0, 1, &def));
   CHECK(topo_db_get_topology((elite_topo_db_client_type)0, MIN_TOPOLOGY_ID - 1, &ret_topo));
   CHECK(memcmp(&def, ret_topo, sizeof(def)));

   // Topology with MAX+1 modules
   topo_db_deinit();
   def.num_modules = sizeof(def.module_info)/sizeof(def.module_info[0]) + 1;
   result = topo_db_add_internal_topologies((elite_topo_db_client_type)0, 1, &def);
   CHECK((ADSP_EBADPARAM == result) ? ADSP_EOK : ADSP_EFAILED);
   result = topo_db_get_topology((elite_topo_db_client_type)0, MIN_TOPOLOGY_ID, &ret_topo);
   CHECK((ADSP_ENOTEXIST == result) ? ADSP_EOK : ADSP_EFAILED);
   topo_db_init();

cleanup:
   {
      topo_db_deinit();
   }

   return result;
}

const test_fn test_fn_list[] = {
      create_destroy,
      add_single_topo,
      add_multiple_topo,
      add_multiple_times_internal,
      add_topo_err,
      add_multiple_times,
      add_invalid_topo
};

int main(void)
{
   int num_failed = 0;
   int num_ran = 0;
   for(unsigned int i = 0; i < sizeof(test_fn_list)/sizeof(test_fn_list[0]); i++)
   {
      int failed = 0;
      ADSPResult result = test_fn_list[i]();
      num_ran++;
      if (ADSP_EOK != result)
      {
         printf("Test %u has failed\n", i);
         failed = 1;
      }

      if (0 != get_num_leaks())
      {
         printf("Test %u has %u memory leaks\n", i, get_num_leaks());
         failed = 1;
         reset_stats();
      }

      num_failed += failed;
   }

   printf("%d tests failed from %d.\n", num_failed, num_ran);

   return num_failed;
}

// QURT ELITE STUBS
static unsigned num_leaks = 0;
static int malloc_failure_counter = -1;

void *qurt_elite_memory_malloc(uint32_t unBytes, QURT_ELITE_HEAP_ID heapId)
{
   if (0 == malloc_failure_counter)
   {
      malloc_failure_counter = -1;
      return NULL;
   }

   if (malloc_failure_counter > 0) malloc_failure_counter--;

   num_leaks++;
   return malloc(unBytes);
}

void qurt_elite_memory_free(void *ptr)
{
   num_leaks--;
   free(ptr);
}

int get_num_leaks(void)
{
   return num_leaks;
}

void reset_stats(void)
{
   num_leaks = 0;
}

static void simulate_malloc_failure(int num_mallocs_to_skip)
{
   malloc_failure_counter = num_mallocs_to_skip;
}
