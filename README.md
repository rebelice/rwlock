This is a simple implementation of rwlock.

Usage:
	help					Display this information.
	make					Compile the project.
	make run [options]		Run the test.
	Options:
		READER=<num>		Set up <num>(default=5) readers.
							Can not be greater than ASIZE(default=100).
		WRITER=<num>		Set up <num>(default=1) writers.
	make check				Check the log file.