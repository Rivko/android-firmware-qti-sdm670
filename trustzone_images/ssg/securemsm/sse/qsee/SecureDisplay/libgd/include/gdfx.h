#ifndef GDFX_H
#define GDFX_H

/*===========================================================================
   Copyright (c) 2013 Qualcomm Technologies, Inc.       
   All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/securemsm/sse/qsee/SecureDisplay/libgd/include/gdfx.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
07/02/13   ts       Initial version.

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd.h"

#ifndef SECURE_UI_SUPPORTED

	/* im MUST be square, but can have any size. Returns a new image
		of width and height radius * 2, in which the X axis of
		the original has been remapped to theta (angle) and the Y axis
		of the original has been remapped to rho (distance from center).
		This is known as a "polar coordinate transform." */

	BGD_DECLARE(gdImagePtr) gdImageSquareToCircle(gdImagePtr im, int radius);

	/* Draws the text 'top' and 'bottom' on 'im', curved along the
		edge of a circle of radius 'radius', with its
		center at 'cx' and 'cy'. 'top' is written clockwise
		along the top; 'bottom' is written counterclockwise
		along the bottom. 'textRadius' determines the 'height'
		of each character; if 'textRadius' is 1/2 of 'radius',
		characters extend halfway from the edge to the center.
		'fillPortion' varies from 0 to 1.0, with useful values
		from about 0.4 to 0.9, and determines how much of the
		180 degrees of arc assigned to each section of text
		is actually occupied by text; 0.9 looks better than
		1.0 which is rather crowded. 'font' is a freetype
		font; see gdImageStringFT. 'points' is passed to the
		freetype engine and has an effect on hinting; although
		the size of the text is determined by radius, textRadius,
		and fillPortion, you should pass a point size that
		'hints' appropriately -- if you know the text will be
		large, pass a large point size such as 24.0 to get the
		best results. 'fgcolor' can be any color, and may have
		an alpha component, do blending, etc.

		Returns 0 on success, or an error string. */

	BGD_DECLARE(char *) gdImageStringFTCircle(
	    gdImagePtr im,
	    int cx,
	    int cy,
	    double radius,
	    double textRadius,
	    double fillPortion,
	    char *font,
	    double points,
	    char *top,
	    char *bottom,
	    int fgcolor);

	/* 2.0.16:
	 * Sharpen function added on 2003-11-19
	 * by Paul Troughton (paul<dot>troughton<at>ieee<dot>org)
	 * Simple 3x3 convolution kernel
	 * Makes use of seperability
	 * Faster, but less flexible, than full-blown unsharp masking
	 * pct is sharpening percentage, and can be greater than 100
	 * Silently does nothing to non-truecolor images
	 * Silently does nothing for pct<0, as not a useful blurring function
	 * Leaves transparency/alpha-channel untouched
	 */

	BGD_DECLARE(void) gdImageSharpen (gdImagePtr im, int pct);

#endif

#ifdef __cplusplus
}
#endif

#endif /* GDFX_H */
