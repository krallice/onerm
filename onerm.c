#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

double calc_brzycki(const double *w, const double *r) {
	return *w * (36 / (37 - *r));
}

int main(int argc, char **argv) {

	const double RM_TABLE[] = {1, 0.97, 0.94, 0.92, 0.89,
				   0.86, 0.83, 0.81, 0.78, 0.75};
	const int RM_TABLE_SIZE = sizeof(RM_TABLE)/sizeof(double);

	double reps;
	double weight;
	double bodyweight;
	int bodyweightflag = 0;

	int c;

	while (1) {

		static struct option long_options[] = {
			{"reps", required_argument, 0, 'r'},
			{"weight", required_argument, 0, 'w'},
			{"bodyweight", required_argument, 0, 'b'}
		};

		int option_index = 0;

		c = getopt_long(argc, argv, "r:w:b:", long_options, &option_index);

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
				bodyweightflag = 1;
				break;
			default:
				abort();
		}
	}

	if (bodyweightflag == 0) {
		bodyweight = 0;
	}

	printf("Reps\tWeight\n");
	printf("--------------\n");
	for (int i = 0; i < RM_TABLE_SIZE; i++) {
		double w = weight + bodyweight;
		if (bodyweightflag == 0) {
			printf("%dRM\t%0.2f\n", i+1, RM_TABLE[i] * calc_brzycki(&w, &reps));
		} else {
			printf("%dRM\t%0.2f (%0.2f + %0.2f)\n", i+1, RM_TABLE[i] * calc_brzycki(&w, &reps), bodyweight, (RM_TABLE[i] * calc_brzycki(&w, &reps)) - bodyweight);
		}
	}
	
	return 0;
}
