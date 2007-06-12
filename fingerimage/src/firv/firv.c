/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility  whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */
/******************************************************************************/
/* This program uses the ANSI/INCITS Finger Image-Based Record library to     */
/* validate the contents of a file containing finger image records.           */
/*                                                                            */
/* Return values:                                                             */
/*    0 - File contents are valid                                             */
/*    1 - File contents are invalid                                           */
/*   -1 - Other error occurred                                                */
/******************************************************************************/
#include <sys/queue.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <fir.h>
#include <m1io.h>

void
usage(char *name) {
	printf("usage: %s <datafile>\n", name);
	exit (EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	FILE *fp;
	struct stat sb;
	struct finger_image_record *fir;
	int ret;
	unsigned long long total_length;

	if (argc != 2)
		usage(argv[0]);

	fp = fopen(argv[1], "rb");
	if (fp == NULL)
		OPEN_ERR_EXIT(argv[optind]);

	if (fstat(fileno(fp), &sb) < 0) {
		fprintf(stderr, "Could not get stats on input file.\n");
		exit (EXIT_FAILURE);
	}

	if (new_fir(&fir) < 0)
		ALLOC_ERR_EXIT("Could not allocate FIR\n");

	total_length = 0;
	while (total_length < sb.st_size) {
		ret = read_fir(fp, fir);
		if (ret != READ_OK)
			break;
		total_length += fir->record_length;

		// Validate the FIR
		if (validate_fir(fir) != VALIDATE_OK)
			exit (EXIT_FAILURE);

		free_fir(fir);

		if (new_fir(&fir) < 0)
			ALLOC_ERR_EXIT("Could not allocate FIR\n");
	}
	if (ret != READ_OK)
		exit (EXIT_FAILURE);

	exit (EXIT_SUCCESS);
}