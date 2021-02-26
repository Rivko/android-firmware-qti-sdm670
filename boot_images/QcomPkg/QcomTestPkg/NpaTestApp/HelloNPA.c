/*============================================================================
HelloNPA.c

A very basic example of NPA use. 

Copyright (c) 2013-2014, Qualcomm Technologies Inc. All rights reserved. 
All Rights Reserved.
QUALCOMM Proprietary

============================================================================*/
#include "npa.h"
#include "npa_init.h"
#include "npa_resource.h"
#ifdef TARGET_UEFI
    #include <Uefi.h>
    #include <Library/UefiLib.h>
    #include <Library/UefiApplicationEntryPoint.h>
    #include <Library/UefiBootServicesTableLib.h>
    #include <Library/TestInterface.h>

    #include <Library/DebugLib.h>
    #define exit(a) ASSERT(a)

#endif



/*----------------------------------------------------------------------------
Hello World Driver Function
Print's out Hello World
-----------------------------------------------------------------------------*/
static npa_resource_state helloworld_driver_fcn( npa_resource *resource, 
    npa_client *client, 
    npa_resource_state state )
{
  unsigned int i;
  if(client->type == NPA_CLIENT_INITIALIZE)
  {
    /* The resource driver will be called with client type set to NPA_CLIENT_INITIALIZE, when the 
     * resource/node is first created. Put any custom initialization you may want to do here. */
     AsciiPrint("-------------------------------------------------------------------------\n");
     AsciiPrint("The driver function in the /hello/world_node node called with\n");
     AsciiPrint("a NPA_CLIENT_INITIALIZE client type\n");
     AsciiPrint("from a [%a] client request on it's [%a] resource\n", client->name, resource->definition->name);
     AsciiPrint ("with a state of [%d].\n", state );
  }
  else
  {
    AsciiPrint("-------------------------------------------------------------------------\n");
    AsciiPrint("The driver function in the /hello/world_node node is being accessed\n");
    AsciiPrint("from a [%a] client request on it's [%a] resource\n", client->name, resource->definition->name);
    AsciiPrint ("with a state of [%d].\n", state );
     
    for (i=0; i < state; i++){
      AsciiPrint("Hello World\n");
    }
  }

  // Return the actual state of the resource
  return state;
}


/*
 * This resource will get entered into the node. Multiple resources may reside within a node
 */
static npa_resource_definition hello_resource[] =
{
  {"/hello/world",            // Resource Name
    "Hellos",                 // Resource Units - Informational only
    100,                      // Resource Max - arbitrary value here
    &npa_identity_plugin,     // Use pass-thru (ie no max, min or other algorithms)
    NPA_RESOURCE_DEFAULT,     /* Attributes - seting default resource behavior -- other available types are:
                               * NPA_RESOURCE_DRIVER_UNCONDITIONAL: always calls driver
                               * NPA_RESOURCE_SINGLE_CLIENT: only allows one client to be created at a time
                               * NPA_RESOURCE_VECTOR_STATE: allows a vector (eg a buffer) tobe sent to the resource
                               * NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED: allowed to be accessed from another process
                               * NPA_RESOURCE_REMOTE: A remote resource on the RPM -- kept in sync across processors
                               * NPA_RESOURCE_REMOTE_PROXY: A state query to remote resources with this attribute
                               *                      returns remote state, if supported by protocol
                               * NPA_RESOURCE_REMOTE_NO_INIT: a resource that must not be initialized by NPA
                               */
    NULL,                      // User Data - In case you need resource specific data
    NULL,         /*query_fcn*/
    NULL,         /*link_query_fcn*/
    NULL          /*handle*/
  }
};


// Define the resource graph node.  One node can contain multiple resources.
// All resources within a Node share the same lock and driver function.
static npa_node_definition hello_node = 
{ 
  "/hello/world_node",         // Node name - used when making dependency requests       
  helloworld_driver_fcn,       // Node Driver Function - Used for all resources defined in the node. 
  NPA_NODE_DEFAULT,            // Attributes - NPA defined default attributes.  If other attributes are desired assign
                               //              attributes from enum npa_node_attribute, ORed together
  NULL,                        // User Data - In case you need Node specific data
  NPA_EMPTY_ARRAY,             // Node Dependencies - None
  NPA_ARRAY(hello_resource)    // Node Resources 
};


/**
 * <!--  create_myHelloReqClient -->
 *
 * @brief Callback to create our example client of the "/hello/world" resource.
 *
 * This function is passed to npa's npa_resource_available_cb as a callback to 
 * be called when the "/hello/world" resource is available.  
 *
 * @param arg1 : The context we requested passed by npa_resource_available_cb.
 * In this case it's the pointer to main's myHelloReqClient.  
 * @param arg2 : event identifier (0 for resource created) 
 * @param arg3 : name of the resource now available
 * @param arg4 : length of the resource name
 *
 * @return The client pointer passed in via context/arg1 will be filled in. 
 */
void create_myHelloReqClient( void *arg1,
                              unsigned int arg2,
                              void *arg3,
                              unsigned int arg4 )
{
  npa_client_handle *clientPtr;

  //caller passes the pointer to the client handle it would like filled as argument 1
  if( !arg1 )
  {
    AsciiPrint("Fatal error: arg1 is NULL\n");
    exit(0);
  }
  else
  {
 /* In main() we told npa's npa_resource_available_cb the pointer to
   * myHelloReqClient would be the context to pass into this function.
   */
      clientPtr = (npa_client_handle*) arg1;

      // Initialize the client by registering it with the /hello/world
      *clientPtr =  npa_create_sync_client(
                   "/hello/world",          // Connect to the "/hello/world" resource
               "Our Example Client", // Descriptive client name
                   NPA_CLIENT_REQUIRED); /* Your client type --  types can be:
                                          * NPA_CLIENT_REQUIRED: Request must honor request 
                                          * NPA_CLIENT_IMPULSE: Poke the resource - node defines the impulse response
                                          * NPA_CLIENT_LIMIT_MAX: Set maximum value for the resource
                                          * NPA_CLIENT_VECTOR: Vector request
                                          * NPA_CLIENT_SUPPRESSIBLE: Suppressible Scalar request -- requests 
                                          *                          honored only in active set
                                          * NPA_CLIENT_SUPPRESSIBLE_VECTOR: Suppressible Vector request 
                                          */
  if (*clientPtr == NULL){
        AsciiPrint("Fatal Error creating /hello/world client!\n");
        exit(0);
      }
  }
}



/**
 * <!-- main -->
 *
 * @brief Example usage of NPA.  
 * 
 * Real device code won't have to do the DAL and NPA initialization shown
 * below because this will have already been done for you elsewhere.  Also
 * most of the time resource creation and client usage are seperated.  
 */

int HelloNPA_main( void )
{
  // handle for the client we're going to create.
  npa_client_handle myHelloReqClient = NULL;
   myHelloReqClient =  npa_create_sync_client(
                   "/hello/world",          // Connect to the "/hello/world" resource
                   "Hello World Printer",   // Descriptive name
                   NPA_CLIENT_REQUIRED); /* Your client type --  types can be:
                                          * NPA_CLIENT_REQUIRED: Request must honor request 
                                          * NPA_CLIENT_IMPULSE: Poke the resource - node defines the impulse response
                                          * NPA_CLIENT_LIMIT_MAX: Set maximum value for the resource
                                          * NPA_CLIENT_VECTOR: Vector request
                                          * NPA_CLIENT_SUPPRESSIBLE: Suppressible Scalar request -- requests 
                                          *                          honored only in active set
                                          * NPA_CLIENT_SUPPRESSIBLE_VECTOR: Suppressible Vector request 
                                          */

   if ( NULL == myHelloReqClient)
   {
      // NPA passes an initial client to the driver function for each resource
      // in the node (see driver function above).  This array corresponds to the 
      // example_resource array above, and provides the initial state value for
      // each resource in the Node.
      npa_resource_state initialState[] = {0};

  // Tell the NPA framework to publish the "/hello/world" node into the NPA graph.
  // Not passing any callback or context for this simple resource/node creation.
  npa_define_node_cb(&hello_node, initialState, NULL, NULL);


  // Create the client when the resource becomes available.  The callback function, 
  // create_myHelloReqClient will be called when "/hello/world" becomes available.
  // The arg1 context to create_myHelloReqClient will be a pointer to myHelloReqClient.
      npa_resource_available_cb( "/hello/world", 
                                 create_myHelloReqClient, 
                                 (void *)&myHelloReqClient );


      // Sit and poll until reqClient is created. This is needed for this example program.
      // Normally the resource will be created in another thread and we won't act until it has been created.
      // Since we know that the client has already been created, there won't be an actual wait here. 
      while (myHelloReqClient == NULL);
   }
   
  // Once the client is valid, we can issue a request.  Since the plugin is
  // the identity plugin, our driver will always be called. 
  npa_issue_required_request(myHelloReqClient, 1);   //Sends the driver a 1

  // Send an example complete_request to the resource.  By complete, we mean
  // we no longer want to use this resource. 
  npa_complete_request(myHelloReqClient);           //Sends the driver a 0

  return 0;
}


