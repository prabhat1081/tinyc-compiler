Instructions for compiling and running : 

1. Run the command :  make
2. Now for any file use the following command
	./tinyc filename -d(y/n) -s(2 digit number)
	
	It generates a.out which can be run to execute the program
	
	-d and -s are optional but order is important
	Use prefix 0 incase of single digit numbers for option s 
	
	Details of options : 
	
	d : This option is used to enable/disable debug information
		If the option is not provided, it is disabled
		
	s : This option is used to generate files as specified in the assignment
	
3. To run on test files use : make run<number>
 	where number is the test number between 1 to 11
4. Use : make runall to compile all test files at once
	Note: The a.out will have to be independently generated
5. Cleaning : 
	a) To clean use : make clean
	b) To clean tests use : make cleantests
	

Shortcomings and/or known bugs:

In general, all instructions given in the assignment has been tried to be followed.
The following bugs are known:
1. Initialization of pointers in the global scope is not done
2. The conversion to bool may in some situations produce undesired results
3. Registers allocation is only done for passing parameters and not in general.
4. No optimization of the code generated is done


General Comments: 

1. The code is mostly written with well named varaibles and functions making it self explanatory and hence comments are not required in such cases
2. For passing parameters, 6 are passed through registers and rest through stack as the convention of x86_64 specifies.
3. The support for strings is limited to use in prints and non modifying uses.
4. For including the library declarations, temporary file is generated with the decalarations included.
5. A lot of code is also modified/added in ass6_13CS10060_translator.cxx as was required and moving it to ass6_13CS10060_target_translator.cxx would not have been advisable.

====================================================END=====================================================
