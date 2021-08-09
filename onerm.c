#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

double calc_brzycki(const double *w, const double *r) {
	return *w * (36 / (37 - *r));
}

void print_rep_table(void) {
	printf("RepMax\tPercentage\n");
	printf("--------------\n");
	printf("1RM\t100%%\n");
	printf("2RM\t97%%\n");
	printf("3RM\t94%% (~95%%)\n");
	printf("4RM\t92%%\n");
	printf("5RM\t89%% (~90%%)\n");
	printf("6RM\t86%% (~85%%)\n");
	printf("7RM\t83%%\n");
	printf("8RM\t81%% (~80%%)\n");
	printf("9RM\t78%%\n");
	printf("10RM\t75%%\n");
	exit(0);
}

int main(int argc, char **argv) {

	const double RM_TABLE[] = {1, 0.97, 0.94, 0.92, 0.89,
				   0.86, 0.83, 0.81, 0.78, 0.75};
	const int RM_TABLE_SIZE = sizeof(RM_TABLE)/sizeof(double);

	double reps = 0;
	double weight = 0;
	double bodyweight = 0;

	char indicator[16] = "";

	int c;

	while (1) {

		static struct option long_options[] = {
			{"reps", required_argument, 0, 'r'},
			{"weight", required_argument, 0, 'w'},
			{"bodyweight", required_argument, 0, 'b'},
			{"reptable", no_argument, 0, 't'}
		};

		int option_index = 0;

		c = getopt_long(argc, argv, "r:w:b:t", long_options, &option_index);

		if (c == -1) {
			break;
		}

		switch(c) {
			case 'r':
				sscanf(optarg, "%lf", &reps);
				break;
			case 'w':
				sscanf(optarg, "%lf", &weight);
				break;
			case 'b':
				sscanf(optarg, "%lf", &bodyweight);
				break;
			case 't':
				print_rep_table();
				break;
			default:
				abort();
		}
	}

	printf("Reps\tWeight\n");
	printf("--------------\n");

	double total_weight = weight + bodyweight;
	double onerm_result = calc_brzycki(&total_weight, &reps);

	for (int i = 0; i < RM_TABLE_SIZE; i++) {

		double xrm_result = RM_TABLE[i] * onerm_result;

		if (i + 1 == reps) {
			memcpy(indicator, " <--", 5);
		} else {
			memcpy(indicator, "", 1);
		}
		if (bodyweight == 0) {
			printf("%dRM\t%0.2f%s\n", i+1, xrm_result, indicator);
		} else {
			printf("%dRM\t%0.2f (%0.2f + %0.2f)%s\n", i+1, xrm_result, bodyweight, xrm_result - bodyweight, indicator);
		}
	}
	
	return 0;
}
