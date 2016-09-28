*** This tar file contains the following files:

* libjp1.a: Test suite for the first submission code. It contains the user calls
  'runjp' and 'runjp_rank' to execute the tests plus all the necessary code to
  execute them.
* README_E1.txt: shows the list of executed tests and the exact output that you should see from one of the tests in libjp1.a

*** Usage:

* In order to execute the tests you have to:
	1) Create a soft link named libjp.a pointing to the library with the
  tests that you want to run (libjp1.a).
		$ ln -s libjp1.a libjp.a

	2) Modify the Makefile so the user target is linked with the libjp.a file.
		...
		USROBJ = libc.o  libjp.a
		...

	3) Modify the main function of the user.c file to call the 'runjp' routine.
		...
		main(void)
		{
		runjp();
		while(1);
		}

	4) Enable Bochs to write on the console, in order to read all the
  test messages without problems (.bochsrc).
		...
		port_e9_hack: enabled=1

* You have to take into account that the tests in a test suite are cumulative.
  That is, if one test fails then the execution of the rest of the tests may be
  inconsistent.

* You can execute just a range of tests, replacing  the call to runjp() in the
  main function by a call to runjp_rank(int first, int last), where "first" is
  the identifier of the first test in the range that you want to execute, and
  "last" is the last one.




