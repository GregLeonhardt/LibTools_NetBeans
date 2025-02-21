/*******************************  COPYRIGHT  ********************************/
/*
 *  Copyright (c) 2017 Gregory N. Leonhardt All rights reserved.
 *
 ****************************************************************************/

/******************************** JAVADOC ***********************************/
/**
 *  This file contains queing tools that may be called from functions
 *  external to this library.
 *
 *  @note
 *
 *  Queue API.
 *
 *  Queues are the primary method for passing information between
 *  process threads.  Each queue has two addressing modes:
 *      1) Queue-Name
 *      2) Queue-ID
 *  Sending and receiving payloads always address the queue by it's Queue-ID.
 *  For cases where the queue name is known but the Queue-ID isn't known you
 *  must lookup the Queue-ID.
 *
 *  @note   payloads:
 *      The queue does not move the contents of a payload.  Only the
 *      payload pointer passed through the queue.
 *
 *  @note
 *
 ****************************************************************************/

/****************************************************************************
 *  Compiler directives
 ****************************************************************************/

#define ALLOC_QUEUE             ( 1 )

/****************************************************************************
 * System Function API
 ****************************************************************************/

                                //*******************************************
#include <stdint.h>             //  Alternative storage types
#include <stdbool.h>            //  TRUE, FALSE, etc.
#include <stdio.h>              //  Standard I/O definitions
                                //*******************************************
#include <string.h>             //  Functions for managing strings
#include <stdlib.h>             //  ANSI standard library.
#include <unistd.h>             //  Access to the POSIX operating system API
                                //*******************************************

/****************************************************************************
 * Application APIs
 ****************************************************************************/

                                //*******************************************
#include <libtools_api.h>       //  Everything public
                                //*******************************************
#include "queue_lib.h"          //  API for all QUEUE__*         PRIVATE
                                //*******************************************

/****************************************************************************
 * API Enumerations
 ****************************************************************************/

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/****************************************************************************
 * Private Definitions
 ****************************************************************************/

//----------------------------------------------------------------------------
#define ALLOC_QUEUE             ( 1 )
//----------------------------------------------------------------------------

/****************************************************************************
 * Private Structures
 ****************************************************************************/

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/****************************************************************************
 * Private Storage Allocation
 ****************************************************************************/

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************/
/**
 *  Initialize a queue stack.
 *
 *  @param  void            No information is passed to this function.
 *
 *  @return queue_rc        See queue_rc_e for a list of return codes.
 *
 *  @note
 *
 ****************************************************************************/

enum queue_rc_e
queue_init(
    )
{
    /**
     *  @param  queue_rc        Return code                                 */
    enum    queue_rc_e              queue_rc;

    /************************************************************************
     *  Function Initialization
     ************************************************************************/

    //  Set the default return code.
    queue_rc = QUEUE_RC_SUCCESS;

    /************************************************************************
     *  Initialize the Queue
     ************************************************************************/

    //  Should we continue ?
    if ( queue_rc == QUEUE_RC_SUCCESS )
    {
        //  Allocate the base Name/ID link list.
        queue_name_id_base_p = list_new( );

        //  Was the structure allocated successfully ?
        if ( queue_name_id_base_p == NULL )
        {
            //  NO:     This isn't good.
            log_write( MID_FATAL, "queue_init",
                          "Failed to allocate the Name/ID "
                          "base list structure.\n" );
        }
        else
        {
            log_write( MID_INFO, "queue_init",
                          "Allocate the Name/ID "
                          "base list structure 'queue_name_id_base_p' [%p].\n",
                          queue_name_id_base_p );
        }
    }

    /************************************************************************
     *  Function Exit
     ************************************************************************/

    //  All done.
    return( queue_rc );
}

/****************************************************************************/
/**
 *  Create a new queue for the queue stack defined by 'queue_name'.
 *
 *  @param  queue_name_p    Pointer to an ASCII string containing the name
 *                          that will be assigned to the new queue.
 *
 *  @return queue_id        The Queue-ID number when greater then
 *                          zero. When less then zero see queue_rc_e for a
 *                          list of failure codes.
 *
 *  @note
 *
 ****************************************************************************/

int
queue_new(
    char                        *   queue_name_p,
    int                             queue_depth
    )
{
    /**
     *  @param  queue_id        The Queue-ID number (handle)                */
    int                             queue_id;
    /**
     *  @param  queue_rc        Return code                                 */
    enum    queue_rc_e              queue_rc;

    /************************************************************************
     *  Function Initialization
     ************************************************************************/


    /************************************************************************
     *  Validate the Queue-Name
     ************************************************************************/

    //  Validate the Queue-Name
    queue_rc = QUEUE__verify_queue_name( queue_name_p );

    /************************************************************************
     *  Verify the queue name isn't already in use.
     ************************************************************************/

    //  Should we continue ?
    if ( queue_rc == QUEUE_RC_SUCCESS )
    {
        //  Create a new Queue
        queue_id = QUEUE__find_queue_name( queue_name_p );

        /********************************************************************
         *  Create a new Queue
         ********************************************************************/

        //  Does the Queue-Name already exist ?
        if ( queue_id == QUEUE_RC_QUEUE_NAME_NOT_PRESENT  )
        {
            //  NO:     Create a new Queue
            queue_id = QUEUE__new( queue_name_p, queue_depth );
        }
        else
        {
            //  YES:    Can't create what is already in existence.
            queue_id = (int)QUEUE_RC_QUEUE_NAME_EXISTS;
        }
    }

    /************************************************************************
     *  Log the results
     ************************************************************************/

    //  Was the queue successfully created ?
    if ( queue_id > 0 )
    {
        //  YES:    Log the name and port number
        log_write( MID_DEBUG_0, "queue_new",
                      "New messaging Queue-ID %04d for '%s'\n",
                      queue_id, queue_name_p );
    }

    /************************************************************************
     *  Function Exit
     ************************************************************************/

    //  All done.
    return( queue_id );
}

/****************************************************************************/
/**
 *  When the Queue-ID is not known and has a predetermined name, the
 *  Queue-ID can be looked up using this function.
 *
 *  @param  queue_name_p    Pointer to an ASCII string containing the name
 *                          of the queue of the requested Queue-ID.
 *
 *  @return queue_id        Upon success, the Queue-ID of the corresponding
 *                          queue name, else ??
 *
 *  @note
 *
 ****************************************************************************/

int
queue_get_id(
    char                        *   queue_name_p
    )
{
    /**
     *  @param  queue_id        The Queue-ID number (handle)        */
    int                             queue_id;

    /************************************************************************
     *  Function Initialization
     ************************************************************************/

    //  Set the default return code.
    queue_id = (int)QUEUE_RC_SUCCESS;

    /************************************************************************
     *  Validate the Queue-Name
     ************************************************************************/

    //  Should we continue ?
    if ( queue_id == QUEUE_RC_SUCCESS )
    {
        //  Validate the Queue-Name
        queue_id = QUEUE__verify_queue_name( queue_name_p );
    }

    /************************************************************************
     *  Lookup the Queue-ID
     ************************************************************************/

    //  Should we continue ?
    if ( queue_id == QUEUE_RC_SUCCESS )
    {
        //  Lookup the Queue-ID
        queue_id = QUEUE__find_queue_name( queue_name_p );
    }

    /************************************************************************
     *  Log the results
     ************************************************************************/

    //  Was the queue successfully created ?
    if ( queue_id <= 0 )
    {
        //  YES:    Log the name and port number
        log_write( MID_FATAL, "queue_get_msg_id",
                      "Couldn't locate a queue for %s.\n",
                      queue_name_p );
    }
    else
    {
        //  YES:    Log the name and port number
        log_write( MID_DEBUG_0, "queue_get_msg_q_id",
                      "%s:%04d was found.\n",
                      queue_name_p, queue_id );
    }

    /************************************************************************
     *  Function Exit
     ************************************************************************/

    //  All done.
    return( queue_id );
}

/****************************************************************************/
/**
 *  Return a pointer to the queue state structure.
 *
 *  @param  queue_id        A Queue-ID number (handle)
 *
 *  @return queue_msg_count The number of payloads in the queue.
 *
 *  @note
 *
 ****************************************************************************/

int
queue_get_count(
    int                             queue_id
    )
{
    /**
     *  @param  queue_rc        Return code                                 */
    enum    queue_rc_e              queue_rc;
    /**
     *  @param  queue_msg_count Number of payloads in the queue     */
    int                             queue_msg_count;

    /************************************************************************
     *  Function Initialization
     ************************************************************************/

    //  Set the default return code.
    queue_rc = QUEUE_RC_SUCCESS;

    /************************************************************************
     *  Get the queue state pointer.
     ************************************************************************/

    //  Should we continue ?
    if ( queue_rc == QUEUE_RC_SUCCESS )
    {
        //  Lookup the Queue-ID
        queue_msg_count = QUEUE__get_count( queue_id );
    }

    /************************************************************************
     *  Log the results
     ************************************************************************/

    //  Log the number of payloads on the queue
    log_write( MID_DEBUG_0, "queue_get_count",
                  "There are %d payloads on queue %04d.\n",
                  queue_msg_count, queue_id );

    /************************************************************************
     *  Function Exit
     ************************************************************************/

    //  All done.
    return( queue_msg_count );
}

/****************************************************************************/
/**
 *  Put a mew payload on the defined queue.
 *
 *  @param  queue_id        A Queue-ID number (handle)
 *  @param  payload_p       Pointer to a payload that will be added to the
 *                          queue.
 *
 *  @return queue_rc        See queue_rc_e for a list of return codes.
 *
 *  @note
 *
 ****************************************************************************/

enum queue_rc_e
queue_put_payload(
    int                             queue_id,
    void                        *   void_p
    )
{
    /**
     *  @param  queue_rc        Return code                                 */
    enum    queue_rc_e              queue_rc;

    /************************************************************************
     *  Function Initialization
     ************************************************************************/

    //  Set the default return code.
    queue_rc = QUEUE_RC_SUCCESS;

    /************************************************************************
     *  Add the new payload pointer to the queue.
     ************************************************************************/

    //  Should we continue ?
    if ( queue_rc == QUEUE_RC_SUCCESS )
    {
        //  Lookup the Queue-ID
        queue_rc = (enum queue_rc_e)QUEUE__put_payload( false, queue_id, void_p );
    }

    /************************************************************************
     *  Log the results
     ************************************************************************/

    //  Was the payload successfully put on the queue ?
    if ( queue_rc == true )
    {
        //  YES:    Log the name and port number
        log_write( MID_DEBUG_0, "queue_put_payload",
                      "Payload %p successfully put on messaging queue %04d.\n",
                      void_p, queue_id );
    }
    else
    {
        //  YES:    Log the name and port number
        log_write( MID_FATAL, "queue_put_payload",
                      "Putting payload %p on messaging queue %04d FAILED.\n",
                      void_p, queue_id );
    }

    /************************************************************************
     *  Function Exit
     ************************************************************************/

    //  All done.
    return( queue_rc );
}

/****************************************************************************/
/**
 *  Put a mew payload on the defined queue.
 *
 *  @param  queue_id        A Queue-ID number (handle)
 *  @param  payload_p       Pointer to a payload that will be added to the
 *                          queue.
 *
 *  @return queue_rc        See queue_rc_e for a list of return codes.
 *
 *  @note
 *
 ****************************************************************************/

enum queue_rc_e
queue_override_put_payload(
    int                             queue_id,
    void                        *   void_p
    )
{
    /**
     *  @param  queue_rc        Return code                                 */
    enum    queue_rc_e              queue_rc;

    /************************************************************************
     *  Function Initialization
     ************************************************************************/

    //  Set the default return code.
    queue_rc = QUEUE_RC_SUCCESS;

    /************************************************************************
     *  Add the new payload pointer to the queue.
     ************************************************************************/

    //  Should we continue ?
    if ( queue_rc == QUEUE_RC_SUCCESS )
    {
        //  Lookup the Queue-ID
        queue_rc = (enum queue_rc_e)QUEUE__put_payload( true, queue_id, void_p );
    }

    /************************************************************************
     *  Log the results
     ************************************************************************/

    //  Was the payload successfully put on the queue ?
    if ( queue_rc == true )
    {
        //  YES:    Log the name and port number
        log_write( MID_DEBUG_0, "queue_put_payload",
                      "Payload %p successfully put on messaging queue %04d.\n",
                      void_p, queue_id );
    }
    else
    {
        //  YES:    Log the name and port number
        log_write( MID_FATAL, "queue_put_payload",
                      "Putting payload %p on messaging queue %04d FAILED.\n",
                      void_p, queue_id );
    }

    /************************************************************************
     *  Function Exit
     ************************************************************************/

    //  All done.
    return( queue_rc );
}

/****************************************************************************/
/**
 *  Return the next payload pointer in the queue.
 *
 *  @param  queue_id        A Queue-ID number (handle)
 *
 *  @return payload_p       Pointer to the next payload on the queue.
 *
 *  @note
 *
 ****************************************************************************/

void *
queue_get_payload(
    int                             queue_id
    )
{
    /**
     *  @param  queue_rc        Return code                                 */
    enum    queue_rc_e              queue_rc;
    /**
     *  @param  void_p          Pointer to the return information           */
    void                        *   void_p;

    /************************************************************************
     *  Function Initialization
     ************************************************************************/

    //  Set the default return code.
    queue_rc = QUEUE_RC_SUCCESS;

    //  Log that we are waiting for a payload.
    log_write( MID_DEBUG_0, "queue_get_payload",
                  "Waiting on payload on QUEUE-ID %04d.\n",
                  queue_id );

    /************************************************************************
     *  Add the new payload pointer to the queue.
     ************************************************************************/

    //  Should we continue ?
    if ( queue_rc == QUEUE_RC_SUCCESS )
    {
        //  Lookup the Queue-ID
        void_p = QUEUE__get_payload( queue_id );
    }

    /************************************************************************
     *  Log the results
     ************************************************************************/

    //  Was the payload successfully pulled from the queue ?
    if ( void_p != NULL )
    {
        //  YES:    Log the name and port number
        log_write( MID_DEBUG_0, "queue_get_payload",
                      "Successfully pulled payload (%p) from Queue-ID %04d.\n",
                      void_p, queue_id );
    }
    else
    {
        //  YES:    Log the name and port number
        log_write( MID_FATAL, "queue_get_payload",
                      "Pulling from messaging queue %04d FAILED.\n",
                      queue_id );
    }

    /************************************************************************
     *  Function Exit
     ************************************************************************/

    //  All done.
    return( void_p );
}

/****************************************************************************/
