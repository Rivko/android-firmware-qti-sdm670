#ifndef FASTRPC_MIGRATE_CPZ_H
#define FASTRPC_MIGRATE_CPZ_H

/**
 * Migrate process into CPZ Context.
 *
 * Migrate the current process into a Content Protected Zone.  When the
 * user calls this api, FastRPC will restart all the suspended threads by
 * issuing an AEE_INTERRUPTED error code.  The user should return this code
 * back to the HLOS and have the HLOS application retry.  When migration
 * is completed it will return AEE_SUCCESS.
 *
 * @param heapid, the ION heap id to use for allocating protected memory
 * @retval,
 *    AEE_EINTERRUPTED, the user should return to the HLOS and retry the call
 *    AEE_SUCCESS, the process is migrated
 *    error code, something went wrong
 */
int fastrpc_migrate_cpz(int heapid);

#endif// FASTRPC_MIGRATE_CPZ_H

