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

#include "tam.h"

void ascii_art()
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
\n";

	printf("%s\n", ascii_art);
}

void help_text()
{
	const char help_text[] = "\n \
Usage: tam [options] [Arguments] ...\n \
Create/Extract/View '.tarim' archives from command line.\n\n \
 [options]      [Arguments]              [Description]\n \
 ---------      -----------              -------------\n\n \
 -h, --help      <none>                   Display help text\n\n \
 -c, --create    File/Directory Path(s)   Create Archive\n \
                                          (File Paths are maintained in Archive and Extraction)\n\n \
 -x, --extract   Archive Path             Extract the archive given\n\n \
 -E, --encrypt   Algorithm                (optional) Enable Encryption.\n \
                                          (Algorithms: aes256, aria256 & camellia256)\n\n \
 -o, --out       Output File/Directory    (optional) Defaults to Current Directory for extraction\n \
                                          and creates new archive tam.tarim in current director.\n \
                                          Path & Filename of the Archive created\n \
                                          Path to Directory to Extract into\n\n \
 -v, --view      Path to the Archive      Display the content of the archive\n \
\n";

	printf(help_text);
}

void display_tree(const TARIM_METADATA meta, const TARIM_FILESAVE* fsave)
{
	long long int max = meta.numFile + meta.numFolder;
	for (long long itr = 0; itr < max; itr++)
	{
		if (fsave[itr].type != FS_FOLDER)
		{
			printf("[%llu]  %s (%lu)\n", itr+1, fsave[itr].fpath, fsave[itr].fsize);
		}
	}
}

