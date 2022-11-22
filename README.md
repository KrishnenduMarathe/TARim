# *TAR Improved (TARim)*
	Archive Manager for custom tape archive format with added features ".tarim" (for  tar improved).

## *Features*
	1. Built from scratch and completely open source
	2. Tape Archive - Bunch multiple files and directories into a single bulk file
	3. Selective Extraction supported
	4. AES-256, ARIA-256 and Camellia-256 encryption supported (OpenSSL EVP Interface)

## *Limitations*
	1. 

## *Requirements*
	1. C Compiler
	2. OpenSSL

## *Build Instructions*
1. **Clone the Repository**

		git clone https://github.com/KrishnenduMarathe/TARim.git

2. **Change Directory to the cloned repository**

		cd TARim/

3. **Build the Executable and Library**

		make

4. **Execute the Program**

		make run

5. **(optional) [Run Executable from anywhere] Add Executable to the PATH environment variable and Library to LD_LIBRARY_PATH environment variable**

		[OPEN YOUR SHELLS CONFIGURATION FILE IN YOUR HOME DIRECTORY LIKE .bashrc / .zshrc and add following lines]
		
		export PATH=PATH=$PATH:<Path to the cloned directory>
		export LD_LIBRARY_PATH=<Path to the cloned directory>;$LD_LIBRARY_PATH
