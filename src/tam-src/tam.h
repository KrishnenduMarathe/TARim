// AUTHOR: Krishnendu Marathe
// PROGRAM: TARIM Archve Manager (TAM)

/*
** 					 TARIM Archive Manager (TAM)
** 					 ---------------------------
** 	  Archive Manager for custom tape archive format with added features
**    ".tarim" (for  tar improved).
**
** 					 	  	 License
** 	  						 -------
**
** 	  This program is free software: you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation, either version 3 of the License, or
**    (at your option) any later version.
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef TAM_H_
#define TAM_H_

#include "../libtarim.h"

// Function Prototypes
void ascii_art();
void help_text();
void display_tree(const TARIM_METADATA meta, const TARIM_FILESAVE* fsave);

#endif // TAM_H_
