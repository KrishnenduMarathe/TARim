# *TAR Improved (TARim)*
    
    Archive Manager for custom tape archive format with added features ".tarim" (for  tar improved).
    
    Includes Shared Library (libtarim) to create or extract .tarim archive and default archive     
    manager called TAM / Tarim Archive Manager.
    
	![alt_tag](https://github.com/KrishnenduMarathe/TARim/blob/main/media/asciiArt.png)
    
## *Features*
	
    1. Built from scratch and completely open source
	2. Tape Archive - Bunch multiple files and directories into a single bulk file
	3. Selective Extraction supported
	4. AES-256, ARIA-256 and Camellia-256 encryption supported (OpenSSL EVP Interface)

## *Requirements*
	
    1. C Compiler
	2. OpenSSL

## *License*
	
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.


## *Build Instructions*
1. **Clone the Repository**

		git clone https://github.com/KrishnenduMarathe/TARim.git

2. **Change Directory to the cloned repository**

		cd TARim/

3. **Build the Executable and Library**

		make

4. **Execute the Program**

		make run

5. **(optional) [Run Executable from anywhere] Add Executable to the PATH environment variable and  
    Library to LD_LIBRARY_PATH environment variable**

		[OPEN YOUR SHELLS CONFIGURATION FILE IN YOUR HOME DIRECTORY LIKE .bashrc / .zshrc and add following lines]
		
		export PATH=PATH=$PATH:<Path to the cloned directory>
		export LD_LIBRARY_PATH=<Path to the cloned directory>:$LD_LIBRARY_PATH

####          OR

6. **(optional) Install to the system for everyone to use**
        
        make install

7. **(If Installed) Uninstall using**
        
        make uninstall

## *Limitations*
	
    1. File Path including File name should not exceed a total of 4096 characters
    2. Maximum File Size supported is 4 GB (32 bit) and 1759.218604 x 10^10 GB / 16777216 TB / 
       16384 PB / 16 EB (64 bit)
    3. No Extraction concurrent jobs added in TAM (Tarim Archive Manager) [You can implement 
       yourself as of now. The library has no issue with it]
    4. Maximum number of files is 9,223,372,036,854,775,807 (32 bit/64 bit)
    5. Files with a size of encryption block size [2464 B] or a multiple of it, the decryption fails with bad decrypt error.


