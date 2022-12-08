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

#include "../libtarim.h"

const void ascii_art()
{
	const char ascii_art[] = "\n \
     #######################################################################\n \
   ##                                                                       ##\n \
 ##     \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\        \\\\\\\\                 \\\\\\\\      \\\\\\\\        ##\n \
 ##     \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\       \\\\\\\\\\\\               \\\\\\\\\\\\    \\\\\\\\\\\\       ##\n \
 ##             |||             \\\\\\    \\\\\\           |||    ||  ||    |||     ##\n \
 ##             |||           |||        |||         |||    ||  ||    |||     ##\n \
 ##             |||           |||        |||         |||    ||  ||    |||     ##\n \
 ##              |             ||________||          |||    ||  ||    |||     ##\n \
 ##              |             ||________||          |||     ____     |||     ##\n \
 ##             |||           |||        |||         |||              |||     ##\n \
 ##             |||           |||        |||         |||              |||     ##\n \
 ##             |||           |||        |||         |||              |||     ##\n \
 ##            |||||    <^>  |||||      |||||  <^>  |||||            |||||    ##\n \
 ##                                                                           ##\n \
 ##                                                  TARIM ARCHIVE MANAGER    ##\n \
 ##                                                  ---------------------    ##\n \
 ##                                                                           ##\n \
 ##                                                                           ##\n \
 ##      ============================================                         ##\n \
 ##     | @Author:      Krishnendu Marathe           |                        ##\n \
 ##     | @Repository:  github.com/KrishnenduMarathe |                        ##\n \
 ##     | @Association: youtube.com/@kodespace       |                        ##\n \
 ##      ============================================                         ##\n \
 ##                                                                           ##\n \
 ##                                                                           ##\n \
 ##                                          +---------------------------+    ##\n \
 ##                                          |          FEATURE          |    ##\n \
 ##                                          +===========================+    ##\n \
 ##                                          |           Just Like 'TAR' |    ##\n \
 ##                                          |  New file format (.tarim) |    ##\n \
 ##                                          | Selective File Extraction |    ##\n \
 ##                                          |     Custom File Structure |    ##\n \
 ##                                          | Uses Tarim shared Library |    ##\n \
 ##                                          |       Supports Encryption |    ##\n \
 ##                                          |          Requires OpenSSL |    ##\n \
 ##                                          +---------------------------+    ##\n \
   ##                                                                       ##\n \
     #######################################################################\n \
\n\n";

	printf("%s\n", ascii_art);
}


int main(int argc, char** argv)
{
	if (argc < 3)
	{
		printf("\nInvalid Number of Arguments\n\t(Use 'tam.x86/tam.x64 -h' for more information.)\n");
		return 1;
	}
	ascii_art();
	return 0;
}
