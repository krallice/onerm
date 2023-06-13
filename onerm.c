#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

const char *VERSION = "1.4.0";

static inline double calc_brzycki(const double *w, const double *r) {
	return *w * (36 / (37 - *r));
	// Epley included for good measure:
	// return *w * (1 + (*r / 30));
}

void print_help(void) {

	printf("One Rep Max Calculator\n");

	printf("\nUsage:\n");
	printf("  onerm (--weight | -w) <argument> (--reps | -r) <argument>\n");
	printf("Include Bodyweight:\n");
	printf("  onerm (--bodyweight | -b) <argument> (--weight | -w) <argument> (--reps | -r) <argument>\n");

	printf("\nAdditional Functions:\n\n");

	printf("Print Brzycki RM to RepMax%% Table:\n");
	printf("  onerm (--reptable | -t)\n");

	printf("Print three cycles of 5/3/1 programming:\n");
	printf("  onerm (--weight | -w) <argument> (--reps | -r) <argument> (--531 | -5)\n");
	printf("  onerm (--bodyweight | -b) <argument> (--weight | -w) <argument> (--reps | -r) <argument> (--531 | -5)\n");

	printf("Print hexagon template:\n");
	printf("  onerm (--weight | -w) <argument> (--reps | -r) <argument> (--hexagon | -6)\n");
	printf("  onerm (--bodyweight | -b) <argument> (--weight | -w) <argument> (--reps | -r) <argument> (--hexagon | -6)\n");

	printf("Print Help:\n");
	printf("  onerm (--help | -h)\n\n");
	exit(0);
}

static inline void print_rep_table(void) {

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
	printf("13RM\t70%%\n");
	printf("16RM\t65%%\n");
	printf("20RM\t60%%\n");
	exit(0);
}

void print_brzycki(const double *reps, const double *weight, const double *bodyweight) {

	// Linear table down to 10RM, then 5% jumps from 70 to 60 inclusive
	const double RM_TABLE[] = {1, 0.97, 0.94, 0.92, 0.89,
				   0.86, 0.83, 0.81, 0.78, 0.75,
				   // 5% Jumps:
				   0.70, 0.65, 0.60};
	const unsigned int RM_TABLE_SIZE = sizeof(RM_TABLE)/sizeof(double);

	// To cater for the 5% Jumps:
	const unsigned int RM_JUMPS[] = {13, 16, 20};
	const unsigned int RM_JUMPS_SIZE = sizeof(RM_JUMPS)/sizeof(unsigned int);

	char indicator[16] = "";

	// Calculate 1RM:
	double total_weight = *weight + *bodyweight;
	double onerm_result = calc_brzycki(&total_weight, reps);

	printf("Reps\tPercent\tWeight\n");
	printf("------------------------\n");

	for (int unsigned i = 0; i < RM_TABLE_SIZE; i++) {

		double xrm_result = RM_TABLE[i] * onerm_result;

		if (i + 1 == *reps) {
			memcpy(indicator, " <--", 5);
		} else {
			memcpy(indicator, "", 1);
		}
		if (*bodyweight == 0) {
			printf("%dRM\t%0.0f%%\t%0.2f%s\n", 
			// Start by incrementing the Repcount by one, when we to the end of the RM_TABLE, use the RM_JUMPS array to
			// indicate the actual rep acount (as we skip a few):
			i < RM_TABLE_SIZE - RM_JUMPS_SIZE ? (i+1) : RM_JUMPS[i - (RM_TABLE_SIZE - RM_JUMPS_SIZE)], 
			RM_TABLE[i] * 100, xrm_result, indicator);
		} else {
			printf("%dRM\t%0.0f%%\t%0.2f (%0.2f + %0.2f)%s\n", 
			// Same logic as above:
			i < RM_TABLE_SIZE - RM_JUMPS_SIZE ? (i+1) : RM_JUMPS[i - (RM_TABLE_SIZE - RM_JUMPS_SIZE)], 
			RM_TABLE[i] * 100, xrm_result, *bodyweight, xrm_result - *bodyweight, indicator);
		}
	}
	exit(0);
}

void calc_531(const double *reps, const double *weight, const double *bodyweight) {

	const int CYCLECOUNT = 3;
	const double CYCLEINCREMENT = 2.5;
	const double SETPERCENTAGES[] = { 0.65, 0.75, 0.85, 
					  0.70, 0.80, 0.90, 
					  0.75, 0.85, 0.95, 
					  0.40, 0.50, 0.60 };
	const char *SETREPS[] = { "5", "5", "5+",
				  "3", "3", "3+",
				  "5", "3", "1+",
				  "5", "5", "5" };

	double total_weight = *weight + *bodyweight;

	// Calculate true 1RM:
	double onerm_result = calc_brzycki(&total_weight, reps);

	double training_max = onerm_result * 0.9;
	printf("1RM: %0.0f\nTraining Max: %0.0f\n", onerm_result, training_max);
	for (int i = 0; i < CYCLECOUNT; i++) {

		printf("Cycle %d (Training Max: %0.0f):\n", i+1, training_max);
		printf("\tWeek 1\t\tWeek 2\t\tWeek 3\t\tWeek 4\n");
		for (int j = 0; j < 3; j++) {
			printf("Set %d:\t%0.0fx%s\t\t%0.0fx%s\t\t%0.0fx%s\t\t%0.0fx%s\n", j+1, 
					training_max * SETPERCENTAGES[j], SETREPS[j], 
					training_max * SETPERCENTAGES[j+3], SETREPS[j+3],
				       	training_max * SETPERCENTAGES[j+6], SETREPS[j+6],
					training_max * SETPERCENTAGES[j+9], SETREPS[j+9]);

			if (*bodyweight != 0) {
				printf("        (%0.0f+%0.0f)\t\t(%0.0f+%0.0f)\t\t(%0.0f+%0.0f)\t\t(%0.0f+%0.0f)\n",
						*bodyweight, (training_max * SETPERCENTAGES[j]) - *bodyweight,
						*bodyweight, (training_max * SETPERCENTAGES[j+3]) - *bodyweight,
						*bodyweight, (training_max * SETPERCENTAGES[j+6]) - *bodyweight,
						*bodyweight, (training_max * SETPERCENTAGES[j+9]) - *bodyweight);
			}

			// Print percentages:
			printf("        (%0.0f%%)\t\t(%0.0f%%)\t\t(%0.0f%%)\t\t(%0.0f%%)\n", SETPERCENTAGES[j] * 100, SETPERCENTAGES[j+3] * 100, SETPERCENTAGES[j+6] * 100, SETPERCENTAGES[j+9] * 100);
		}
		training_max += CYCLEINCREMENT;
		printf("\n");
	}
	exit(0);
}

void calc_hexagon(const double *reps, const double *weight, const double *bodyweight) {

	// Calculate true 1RM:
	double total_weight = *weight + *bodyweight;
	double onerm_result = calc_brzycki(&total_weight, reps);

	if (*bodyweight == 0) {
		printf("1RM: %0.2f\n", onerm_result);
		printf("Top Set:\t1x3:\t%0.2f (85%%)\n", onerm_result * 0.85);
		printf("Volume Sets:\t3x8:\t%0.2f (75%%)\n", onerm_result * 0.75);
		printf("Backoff Set:\t12+:\t%0.2f (65%%)\n", onerm_result * 0.65);
	} else {
		printf("1RM: %0.2f (%0.2f + %0.2f)\n", onerm_result, onerm_result - *bodyweight, *bodyweight);
		printf("Top Set:\t1x3:\t%0.2f (%0.2f + %0.2f) (85%%)\n", onerm_result * 0.85, onerm_result * 0.85 - *bodyweight, *bodyweight);
		printf("Volume Sets:\t3x8:\t%0.2f (%0.2f + %0.2f) (75%%)\n", onerm_result * 0.75, onerm_result * 0.75 - *bodyweight, *bodyweight);
		printf("Backoff Set:\t12+:\t%0.2f (%0.2f + %0.2f) (65%%)\n", onerm_result * 0.65, onerm_result * 0.65 - *bodyweight, *bodyweight);
	}

	exit(0);
}

int main(int argc, char **argv) {

	double reps = 0;
	double weight = 0;
	double bodyweight = 0;

	int flag_531 = 0;
	int flag_hexagon = 0;

	int c;

	// Read our arguments:
	while (1) {

		static struct option long_options[] = {
			{"reps", required_argument, 0, 'r'},
			{"weight", required_argument, 0, 'w'},
			{"bodyweight", required_argument, 0, 'b'},
			{"reptable", no_argument, 0, 't'},
			{"531", no_argument, 0, '5'},
			{"hexagon", no_argument, 0, '6'},
			{"help", no_argument, 0, 'h'}
		};

		int option_index = 0;

		c = getopt_long(argc, argv, "r:w:b:th56", long_options, &option_index);

		if (c == -1) {
			break;
		}

		switch(c) {
			case 'h':
				print_help();
				break;
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
			case '5':
				flag_531 = 1;
				break;
			case '6':
				flag_hexagon = 1;
				break;
			default:
				print_help();
		}
	}

	if (reps == 0 || weight == 0) {
		print_help();
	}

	if (flag_531) {
		calc_531(&reps, &weight, &bodyweight);
	} else if (flag_hexagon) {
		calc_hexagon(&reps, &weight, &bodyweight);
	} else {
		print_brzycki(&reps, &weight, &bodyweight);
	}
	return 0;
}
