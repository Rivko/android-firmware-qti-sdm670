/*==============================================================================
@file npa_remote_qdi_drv.c

NPA QDI Remote driver functions

Copyright (c) 2012-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.qdsp6/2.1.c4/power/npa/src/remote/npa_remote_qdi_drv.c#1 $
==============================================================================*/
#include <string.h>
#include <stddef.h>
#include <qurt_qdi_driver.h>
#include "npa_internal.h"
#include "npa_remote_publish_resource.h"
#include "npa_remote_qdi_protocol.h"

#ifdef PD_IMAGE_NAME
/* TODO: Change this to a QuRT macro, if available */
#define NPA_QDI_PD_NAME_MAX 64
static char npa_qdi_pd_image_name[NPA_QDI_PD_IMAGE_NAME_MAX];
#endif

#define USERMALLOC_VECTOR_BUFSZ \
  (NPA_QDI_USERMALLOC_VECTOR_LEN * sizeof(npa_resource_state))

typedef struct
{
  qurt_qdi_obj_t base;
  npa_client_handle client;
  
  char client_name[NPA_MAX_STR_LEN+1];
  char resource_name[NPA_MAX_STR_LEN+1];

  /* If .client is a client to a vector resource, vector points to a
     usermalloc-d buffer, used to pass vector request data across PDs. */
  unsigned int *vector;
  /* vector_alloc_handle is the client_handle used to usermalloc vector */
  int vector_alloc_handle;
  
  int qdih;
} npa_qdi_client_object;

typedef struct
{
  qurt_qdi_obj_t base;
  npa_query_handle query;
} npa_qdi_query_object;


int npa_qdi_query_invoke( int client_handle, qurt_qdi_obj_t *ptr, int method,
                           qurt_qdi_arg_t a1, qurt_qdi_arg_t a2,
                           qurt_qdi_arg_t a3, qurt_qdi_arg_t a4,
                           qurt_qdi_arg_t a5, qurt_qdi_arg_t a6,
                           qurt_qdi_arg_t a7, qurt_qdi_arg_t a8,
                           qurt_qdi_arg_t a9 )
{
  npa_qdi_query_object *obj = (npa_qdi_query_object *)ptr;
  CORE_VERIFY_PTR( obj );
  
  switch ( method )
  {  
  case NPA_QDI_QUERY:
  {
    int err;
    npa_query_type result = { 0 };
    
    npa_query_status ret = npa_query( obj->query, (uint32)a1.num, &result );

    err = qurt_qdi_copy_to_user( client_handle, a2.ptr, 
                                 &result, sizeof(npa_query_type) );
    if ( err < 0 ) { return -1; }
    
    return (int)ret;
  }
  
  default:
    return qurt_qdi_method_default( client_handle, ptr, method,
                                    a1, a2, a3, a4, a5, a6, a7, a8, a9 );
  } /* switch */
}

void npa_qdi_query_release( npa_qdi_query_object *obj )
{
  npa_destroy_query_handle( obj->query );
  Core_Free( obj );
}


int npa_qdi_client_invoke( int client_handle, qurt_qdi_obj_t *ptr, int method,
                           qurt_qdi_arg_t a1, qurt_qdi_arg_t a2,
                           qurt_qdi_arg_t a3, qurt_qdi_arg_t a4,
                           qurt_qdi_arg_t a5, qurt_qdi_arg_t a6,
                           qurt_qdi_arg_t a7, qurt_qdi_arg_t a8,
                           qurt_qdi_arg_t a9 )
{
  npa_qdi_client_object *obj = (npa_qdi_client_object *)ptr;
  CORE_VERIFY_PTR( obj );
  
  /* Save the client handle to return in response to 
     npa_get_qdi_client_handle */
  obj->qdih = client_handle;  
  
  switch ( method )
  {  
  case NPA_QDI_ISSUE_SCALAR_REQUEST:
  { /* state, request_attr */
    obj->client->request_attr = 
      (unsigned int)a2.num | NPA_REQUEST_REMOTE_CLIENT;
    npa_issue_scalar_request( obj->client, (npa_resource_state)a1.num );
    return 0;
  }
  
  case NPA_QDI_ISSUE_VECTOR_REQUEST:
  { /* state (len), vector, request_attr */
    int err = 0;
    unsigned int len = (unsigned int)a1.num;

    obj->client->request_attr = 
      (unsigned int)a3.num | NPA_REQUEST_REMOTE_CLIENT;
    
    if ( len )
    {
      if ( a2.ptr == obj->vector )
      { /* The request is in the shared/usermalloc-d buffer */
        CORE_VERIFY( len <= NPA_QDI_USERMALLOC_VECTOR_LEN );
        
        npa_issue_vector_request( obj->client, 
                                  len, (npa_resource_state *)obj->vector );
      }
      else
      { /* The request is not in the shared/usermalloc-d buffer */
        void *vector = Core_Malloc( len * sizeof(npa_resource_state) );
        CORE_VERIFY_PTR( vector );

        err = qurt_qdi_copy_from_user( client_handle, vector, a2.ptr, 
                                       len * sizeof(npa_resource_state) );
        if ( err == 0 )
        {
          npa_issue_vector_request( obj->client, 
                                    len, (npa_resource_state *)vector );
        }
      
        Core_Free( vector );
      }
    }
    else
    {
      npa_complete_request( obj->client );
    }
    
    return err;
  }
  
  case NPA_QDI_GET_USERMALLOC_VECTOR:
  {
    return qurt_qdi_copy_to_user( client_handle, a1.ptr, 
                                  &obj->vector, sizeof(void*) );
  }
   
  case NPA_QDI_ISSUE_LIMIT_MAX_REQUEST:
  { /* state, request_attr */
    obj->client->request_attr = 
      (unsigned int)a2.num | NPA_REQUEST_REMOTE_CLIENT;
    npa_issue_limit_max_request( obj->client, (npa_resource_state)a1.num );
    return 0;
  }
  
  default:
    return qurt_qdi_method_default( client_handle, ptr, method,
                                    a1, a2, a3, a4, a5, a6, a7, a8, a9 );
  } /* switch */
}

void npa_qdi_client_release( npa_qdi_client_object *obj )
{
  npa_destroy_client( obj->client );
  
  if ( obj->vector )
  {
    qurt_qdi_user_free( obj->vector_alloc_handle, obj->vector );
  }
  Core_Free( obj );
}

int npa_qdi_base_invoke( int client_handle, qurt_qdi_obj_t *ptr, int method,
                         qurt_qdi_arg_t a1, qurt_qdi_arg_t a2,
                         qurt_qdi_arg_t a3, qurt_qdi_arg_t a4,
                         qurt_qdi_arg_t a5, qurt_qdi_arg_t a6,
                         qurt_qdi_arg_t a7, qurt_qdi_arg_t a8,
                         qurt_qdi_arg_t a9 )
{
  int err;
  
  switch ( method )
  {
  case QDI_OPEN:
  {
    return qurt_qdi_new_handle_from_obj_t( client_handle, ptr );
  }

  case NPA_QDI_GET_PUBLISHED_RESOURCE_COUNT:
  {
    npa_remote_domain_id domain_id = (npa_remote_domain_id)a1.num;
    unsigned int count = 0;
    npa_remote_published_resource *pubr = npa_remote_published_resource_list;

    while ( pubr )
    {
      if ( domain_id & pubr->remote_domains ) { ++count; }
      pubr = pubr->next;
    }
    
    err = qurt_qdi_copy_to_user( client_handle, a2.ptr, &count, sizeof(count) );
    if ( err < 0 ) { return -1; }

    return 0;
  }

  case NPA_QDI_GET_PUBLISHED_RESOURCE_DEFINITION:
  {
    npa_remote_domain_id domain_id = (npa_remote_domain_id)a1.num;
    unsigned int i = 0, namelen, count = (unsigned int)a2.num;
    npa_remote_resource_definition *definition = 
                        (npa_remote_resource_definition *)a3.ptr;
    npa_remote_published_resource *pubr = npa_remote_published_resource_list;
    
    if ( count != 0 )
    {
      while ( pubr )
      {
        if ( domain_id & pubr->remote_domains )
        {
          npa_resource *resource;
          
          if ( i >= count)
          {
            /* More resources published to this domain than expected */
            ULOG_RT_PRINTF_2( npa.log, 
                              "WARN: There are more than %d resources "
                              "published to domain %u", count, domain_id);
            return 0;
          }
          
          namelen = strlen( pubr->local_link->name );
          CORE_VERIFY( namelen <= NPA_MAX_STR_LEN );
          err = qurt_qdi_copy_to_user( client_handle, 
                                       definition[i].remote_resource_name,
                                       pubr->local_link->name,
                                       namelen+1 );
          if ( err < 0 ) { return -1; }
                                       
          resource = pubr->local_link->resource;
          
          namelen = strlen( resource->definition->units );
          CORE_VERIFY( namelen <= NPA_QDI_REMOTE_MAX_RESOURCE_UNITS_LEN );
          err = qurt_qdi_copy_to_user( client_handle,
                                       (void *)definition[i].units,
                                       resource->definition->units,
                                       namelen+1 );
          if ( err < 0 ) { return -1; }

          err = qurt_qdi_copy_to_user( client_handle,
                                       &definition[i].max,
                                       &resource->definition->max,
                                       sizeof(npa_resource_state) );
          if ( err < 0 ) { return -1; }
                                       
          err = qurt_qdi_copy_to_user( client_handle,
                                       &definition[i].attributes,
                                       &resource->definition->attributes,
                                       sizeof(unsigned int) );
          if ( err < 0 ) { return -1; }
          
          i++;
        }
        pubr = pubr->next;
      }
      if ( i != count )
      {
        ULOG_RT_PRINTF_1( npa.log, 
                          "ERROR: There are less resources published to "
                          "domain %u than expected", domain_id );
        return -1;
      }
    }
    
    return 0;
  }
   
  case NPA_QDI_CREATE_SYNC_CLIENT_EX:
  {
    npa_client *client;
    
    npa_qdi_client_object *obj = Core_Malloc( sizeof(npa_qdi_client_object) );
    CORE_VERIFY_PTR( obj );
    memset( obj, 0, sizeof(npa_qdi_client_object) );
      
    /* Validate the resource_name parameter; a2.num is the strlen */
    err = qurt_qdi_copy_from_user( client_handle, 
                                   obj->resource_name, a1.ptr, 
                                   MIN(a2.num, NPA_MAX_STR_LEN) );
    if ( err < 0 ) return -1;
    
    /* Validate the client_name parameter; a4.num is the strlen */
    err = qurt_qdi_copy_from_user( client_handle, 
                                   obj->client_name, a3.ptr, 
                                   MIN(a4.num, NPA_MAX_STR_LEN) );
    if ( err < 0 ) return -1;

    client = npa_create_sync_client_ex( obj->resource_name, obj->client_name,
                                        (npa_client_type)a5.num,
                                        (unsigned int)a6.num, a7.ptr );

    if ( client )
    { /* Associate the QDI object with the local client and return a handle to
       * this object to caller. QURT will call the object's invoke routine when
       * the guest PD calls methods on the handle and the release routine when
       * the guest PD shuts down or restarts.
       */
      int qh;

      /* Set up */
      obj->base.invoke = (qurt_qdi_pfn_invoke_t) npa_qdi_client_invoke;
      obj->base.refcnt = QDI_REFCNT_INIT;
      obj->base.release = (qurt_qdi_pfn_release_t) npa_qdi_client_release;
      obj->client = client;

      /* Create a QDI handle from the object */
      qh = qurt_qdi_handle_create_from_obj_t( client_handle, &obj->base );
      CORE_VERIFY( qh >= 0 );
      
      /* If this is a client to a vector resource, create a usermalloc-d 
         buffer to pass request data across PDs */
      if ( npa_resource_has_attribute( client->resource, 
                                       NPA_RESOURCE_VECTOR_STATE ) )
      {
        obj->vector = \
          qurt_qdi_user_malloc( client_handle, USERMALLOC_VECTOR_BUFSZ );
        CORE_VERIFY_PTR( obj->vector );
        memset( obj->vector, 0, USERMALLOC_VECTOR_BUFSZ );
        obj->vector_alloc_handle = client_handle;
      }
      
      /* Return handle to caller */
      err = qurt_qdi_copy_to_user( client_handle, a8.ptr, &qh, sizeof(qh) );
      if ( err >= 0 ) return 0;
      
      /* If unable to return handle to caller.. */
      npa_destroy_client( client );
    }
    
    Core_Free( obj );
    return -1;
  }
  
  case NPA_QDI_CREATE_QUERY_HANDLE:
  {
    npa_query_handle query;
    
    npa_qdi_query_object *obj = Core_Malloc( sizeof(npa_qdi_query_object) );
    CORE_VERIFY_PTR( obj );
    memset( obj, 0, sizeof(npa_qdi_query_object) );
  
    query = npa_create_query_handle( a1.ptr );
    
    if ( query )
    { /* Associate the QDI object with the local query handle and return a
         handle to this object to caller. QURT will call the object's
       * invoke routine when the guest PD calls methods on the handle and the
       * release routine when the guest PD shuts down or restarts.
       */
      int qh;

      /* Set up */
      obj->base.invoke = (qurt_qdi_pfn_invoke_t) npa_qdi_query_invoke;
      obj->base.refcnt = QDI_REFCNT_INIT;
      obj->base.release = (qurt_qdi_pfn_release_t) npa_qdi_query_release;
      obj->query = query;
      
      /* Create a QDI handle from the object */
      qh = qurt_qdi_handle_create_from_obj_t( client_handle, &obj->base );
      CORE_VERIFY( qh >= 0 );
      
      /* Return handle to caller */
      err = qurt_qdi_copy_to_user( client_handle, a2.ptr, &qh, sizeof(qh) );
      if ( err >= 0 ) return 0;
      
      /* If unable to return handle to caller.. */
      npa_destroy_query_handle( query );
    }
    
    Core_Free( obj );    
    return -1;
  }

  default:
    return qurt_qdi_method_default( client_handle, ptr, method,
                                    a1, a2, a3, a4, a5, a6, a7, a8, a9 );
  } /* switch */
}

unsigned int npa_remote_get_qdi_client_handle( npa_client *client )
{
  npa_qdi_client_object *obj;
  
  /* Verify that this call is issued from within the request pipeline
     and on a proxy (remote) client */
  CORE_VERIFY( client->request_attr & NPA_REQUEST_REMOTE_CLIENT );
  
  obj = (npa_qdi_client_object *)
    (client->name - offsetof(npa_qdi_client_object, client_name));
  
  /* Verify that the obj we retrieved actually corresponds to this client */
  CORE_VERIFY( obj->client == client );
  
  return obj->qdih;
}

static const struct
{
  qurt_qdi_obj_t qdiobj;
} npa_qdi =
{
  { npa_qdi_base_invoke, QDI_REFCNT_PERM, 0 }
};

void npa_qdi_init( void )
{
#ifdef PD_IMAGE_NAME
  strlcpy( npa_qdi_pd_image_name, PD_IMAGE_NAME, NPA_QDI_PD_IMAGE_NAME_MAX );
#endif  

  CORE_VERIFY( 0 == qurt_qdi_register_devname( "/dev/npa", (void *)&npa_qdi ) );
}
