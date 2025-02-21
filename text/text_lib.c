/*******************************  COPYRIGHT  ********************************/
/*
 *  Copyright (c) 2017 Gregory N. Leonhardt All rights reserved.
 *
 ****************************************************************************/

/******************************** JAVADOC ***********************************/
/**
 *  This file contains private functions that makeup the internal
 *  library components of the 'text' library.
 *
 *  @note
 *
 ****************************************************************************/

/****************************************************************************
 *  Compiler directives
 ****************************************************************************/


/****************************************************************************
 * System Function API
 ****************************************************************************/

                                //*******************************************
#include <stdint.h>             //  Alternative storage types
#include <stdbool.h>            //  TRUE, FALSE, etc.
                                //*******************************************
#include <ctype.h>              //  Determine the type contained
#include <string.h>             //  Functions for managing strings
                                //*******************************************

/****************************************************************************
 * Application APIs
 ****************************************************************************/

                                //*******************************************
#include <libtools_api.h>       //  Everything public
                                //*******************************************
#include "text_lib.h"           //  API for all TEXT__*             PRIVATE
                                //*******************************************

/****************************************************************************
 * Enumerations local to this file
 ****************************************************************************/

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/****************************************************************************
 * Definitions local to this file
 ****************************************************************************/

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/****************************************************************************
 * Structures local to this file
 ****************************************************************************/

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/****************************************************************************
 * Storage Allocation local to this file
 ****************************************************************************/

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/****************************************************************************
 * LIB Functions
 ****************************************************************************/

/****************************************************************************/
/**
 *  Skip over leading whitespace
 *
 *  @param  line_of_text_p      Pointer to the data string.
 *
 *  @return first_char_p
 *
 *  @note
 *
 ****************************************************************************/

char    *
TEXT__skip_past_whitespace(
    char                        *   line_of_text_p
    )
{
    char                        *   first_char_p;

    /************************************************************************
     *  Function Initialization
     ************************************************************************/


    /************************************************************************
     *  Locate the first non-whitespace character
     ************************************************************************/

    //  Scan the whole text buffer.
    for ( first_char_p = line_of_text_p;
          first_char_p[ 0 ] != '\0';
          first_char_p ++ )
    {
        //  Is this an alpha or numeric character ?
        if ( isgraph( first_char_p[ 0 ] ) !=  0  )
        {
            //  YES:    We are done here so exit the loop.
            break;
        }
    }

    /************************************************************************
     *  Function Exit
     ************************************************************************/

    //  DONE!
    return( first_char_p );
}

/****************************************************************************/
/**
 *  Strip off any terminating CR and / or LF characters.
 *
 *  @param  line_of_text_p      Pointer to the data string.
 *
 *  @return void
 *
 *  @note
 *
 ****************************************************************************/

void
TEXT__remove_crlf(
    char                        *   line_of_text_p
    )
{
    char                        *   s_char_p;

    /************************************************************************
     *  Function Initialization
     ************************************************************************/


    /************************************************************************
     *  Delete any CR or LF characters at the trailing end of the line
     ************************************************************************/

    s_char_p = strchr( line_of_text_p, '\r' );
    if ( s_char_p != NULL )
    {
        s_char_p[ 0 ] = ' ';
    }
    s_char_p = strchr( line_of_text_p, '\n' );
    if ( s_char_p != NULL )
    {
        s_char_p[ 0 ] = ' ';
    }

    /************************************************************************
     *  Function Exit
     ************************************************************************/

}

/****************************************************************************/
/**
 *  Strip off any trailing white space from a string.
 *
 *  @param  line_of_text_p      Pointer to the data string.
 *
 *  @return void
 *
 *  @note
 *
 ****************************************************************************/

void
TEXT__strip_whitespace(
    char                        *   line_of_text_p
    )
{
    int                             ndx;

    /************************************************************************
     *  Function Initialization
     ************************************************************************/


    /************************************************************************
     *  Strip off any trailing white space from a string.
     ************************************************************************/

    for ( ndx = strlen( line_of_text_p );
          ndx >= 0;
          ndx -= 1 )
    {
        //  Is this a white space character
        if (    ( line_of_text_p[ ndx ] == '\0' )
             || ( line_of_text_p[ ndx ] ==  ' ' )
             || ( line_of_text_p[ ndx ] == '\t' ) )
        {
            //  YES:    Get rid of it.
            line_of_text_p[ ndx ] = '\0';
        }
        else
        {
            //  NO:     Get out of here.  Nothing else to do.
            break;
        }
    }

    /************************************************************************
     *  Function Exit
     ************************************************************************/

}

/****************************************************************************/
/**
 *  Test the line of text for a zero length or only whitespace characters.
 *
 *  @param  line_of_text_p      Pointer to the data string.
 *
 *  @return text_rc             TRUE  = Zero length or only whitespace
 *                              FALSE = Non whitespace characters found
 *
 *  @note
 *
 ****************************************************************************/

int
TEXT__is_blank_line(
    char                        *   line_of_text_p
    )
{
    int                             text_rc;
    int                             char_ndx;

    /************************************************************************
     *  Function Initialization
     ************************************************************************/

    //  Assume this is a blank line.
    text_rc = true;

    /************************************************************************
     *  Test the line of text for a zero length or only whitespace characters.
     ************************************************************************/

    //  Is the text buffer empty ?
    if ( strlen( line_of_text_p ) > 0 )
    {
        //  NO:     Scan the text buffer for anything other then whitespace.
        for ( char_ndx = 0;
              char_ndx < strlen( line_of_text_p );
              char_ndx ++ )
        {
            //  Is this character something other then whitespace ?
            if ( isblank( line_of_text_p[ char_ndx ] ) == 0 )
            {
                //  YES:    Change the return code.
                text_rc = false;

                //  We are done here.
                break;
            }
        }
    }

    /************************************************************************
     *  Function Exit
     ************************************************************************/

    //  DONE!
    return( text_rc );
}

/****************************************************************************/
/**
 *  Test for binary data.
 *
 *  @param  line_of_text_p      Pointer to the data string.
 *
 *  @return                     TRUE  = if binary data
 *                              FALSE = if text data
 *
 *  @note
 *      When there is a two characters sequence where both characters are
 *      greater than or equal to 0x80 or is equal to 0x00 the data string
 *      is defined to be binary.
 *
 ****************************************************************************/

int
TEXT__is_binary(
    char                    *   data_p,
    int                         data_l
    )
{
    /**
     *  @param  text_rc         Return Code                                 */
    int                         text_rc;
    /**
     *  @param  char_ndx        Index into data buffer                      */
    int                         char_ndx;

    /************************************************************************
     *  Function Initialization
     ************************************************************************/

    //   The assumption is that this is NOT binary
    text_rc = false;

    /************************************************************************
     *  How many printable characters in the data
     ************************************************************************/

    //  Is the text buffer empty ?
    if ( data_l > 0 )
    {
        //  NO:     Scan the text buffer for binary data.
        for ( char_ndx = 0;
              char_ndx < data_l - 1;
              char_ndx ++ )
        {
#if 1
            //  Are the next two characters >= 0x80 ?
            if (    (    ( data_p[ char_ndx     ] == 0x00 )
                      || ( data_p[ char_ndx     ] >= 0x80 ) )
                 && (    ( data_p[ char_ndx + 1 ] == 0x00 )
                      || ( data_p[ char_ndx + 1 ] >= 0x80 ) ) )
            {
                //  YES:    Then this is a binary data string
                text_rc = true;

                //  We can stop looking
                break;
            }
#else
            //  Is this character NOT a printable character ?
            if (    ( isprint( data_p[ char_ndx ] ) == 0 )
                 && ( isblank( data_p[ char_ndx ] ) == 0 ) )
            {
                //  NO:     Is it x'00 ?
                if( data_p[ char_ndx ] == 0x00 )
                {
                    //  YES:    Then it's binary
                    text_rc = true;

                    //  We can stop looking
                    break;
                }

                //  Not binary and not printable, make it a space.
                data_p[ char_ndx ] = ' ';
            }
#endif
        }
    }

    /************************************************************************
     *  Function Exit
     ************************************************************************/

    //  DONE!
    return( text_rc );
}
/****************************************************************************/
