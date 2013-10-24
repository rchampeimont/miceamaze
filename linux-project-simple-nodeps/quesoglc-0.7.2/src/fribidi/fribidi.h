/* FriBidi - Library of BiDi algorithm
 * Copyright (C) 1999,2000 Dov Grobgeld, and
 * Copyright (C) 2001,2002 Behdad Esfahbod. 
 * 
 * This library is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU Lesser General Public 
 * License as published by the Free Software Foundation; either 
 * version 2.1 of the License, or (at your option) any later version. 
 * 
 * This library is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
 * Lesser General Public License for more details. 
 * 
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library, in a file named COPYING; if not, write to the 
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
 * Boston, MA 02111-1307, USA  
 * 
 * For licensing issues, contact <dov@imagic.weizmann.ac.il> and 
 * <fwpg@sharif.edu>. 
 */

#ifndef FRIBIDI_H
#define FRIBIDI_H

#ifndef NULL
#define NULL 0
#endif

#ifdef HAVE_CONFIG_H
#include "qglc_config.h"
#endif
#include "fribidi_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define FRIBIDI_TRUE	1
#define FRIBIDI_FALSE	0

fribidi_boolean fribidi_log2vis (	/* input */
						FriBidiChar *str,
						FriBidiStrIndex len,
						FriBidiCharType *pbase_dirs,
						/* output */
						FriBidiChar *visual_str,
						FriBidiStrIndex
						*position_L_to_V_list,
						FriBidiStrIndex
						*position_V_to_L_list,
						FriBidiLevel
						*embedding_level_list);

/*======================================================================
 *  fribidi_get_type() returns bidi type of a character.
 *----------------------------------------------------------------------*/
FriBidiCharType fribidi_get_type (FriBidiChar uch);

/*======================================================================
 *  fribidi_get_mirror_char() returns the mirrored character, if input
 *  character has a mirror, or the input itself.
 *  if mirrored_ch is NULL, just returns if character has a mirror or not.
 *----------------------------------------------------------------------*/
fribidi_boolean fribidi_get_mirror_char (	/* Input */
							FriBidiChar ch,
							/* Output */
							FriBidiChar
							*mirrored_ch);
#ifdef	__cplusplus
}
#endif

#endif				/* FRIBIDI_H */
