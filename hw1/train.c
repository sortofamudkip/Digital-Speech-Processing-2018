#include "hmm.h"
#include <math.h>
#include <string.h>
#define DATA_SIZE 10000
char input_data[DATA_SIZE][51];
int input_len;
double prob_O_given_lambda;

double Alpha[MAX_STATE][MAX_SEQ]; // Alpha[i][t]
double Beta[MAX_STATE][MAX_SEQ];  // Beta[i][t]
double Gamma[MAX_SEQ]; // Gamma[i] = sum(t=0 to t=T-1) Gamma[i][t]
double Gamma_one[MAX_SEQ]; // Gamma_one[i] = Gamma[i][0]
double Gamma_minus_one[MAX_SEQ]; // Gamma_minus_one[i] = sum(t=0 to t=T-2) Gamma[i][t]
double indiv_Gamma[DATA_SIZE][MAX_STATE][MAX_SEQ]; // Gamma[n][i][t], n = nth input
double Epsilon[MAX_STATE][MAX_STATE]; // Epsilon[i][j] = sum(t=0 to t=T-2) Epsilon[t][i][j]

void compute_alpha(HMM *hmm, int data_no) {
	int n = hmm->state_num, T = input_len;
	// compute the first row
	for (int i = 0; i < n; ++i) {
		Alpha[i][0] = hmm->initial[i] * hmm->observation[input_data[data_no][0]][i];
	}
	// compute the remaining rows
	for (int t = 0; t < T-1; ++t) {
		for (int j = 0; j < n; ++j) {
			double alpha_by_A = 0;
			for (int i = 0; i < n; i++) alpha_by_A += Alpha[i][t] * hmm->transition[i][j];
			Alpha[j][t+1] = alpha_by_A * hmm->observation[input_data[data_no][t+1]][j];
		}
	}
	prob_O_given_lambda = 0.f;
	for (int i = 0; i < n; ++i) {
		prob_O_given_lambda += Alpha[i][T-1];
	}
}

void compute_beta(HMM *hmm, int data_no) {
	int n = hmm->state_num, T = input_len;
	// compute the last row
	for (int i = 0; i < n; ++i) {
		Beta[i][T-1] = 1.f;
	}
	// compute the remaining rows
	for (int t = T-2; t >= 0; t--) {
		for (int i = 0; i < n; ++i) {
			Beta[i][t] = 0;
			for (int j = 0; j < n; ++j) {
				double val = hmm->transition[i][j] * hmm->observation[input_data[data_no][t+1]][j] * Beta[j][t+1];
				Beta[i][t] += val;
			}
		}
	}
}

void compute_gamma(HMM *hmm, int data_no) {
	int n = hmm->state_num, T = input_len;
	for (int i = 0; i < n; ++i) {
		for (int t = 0; t < T; ++t) {
			double val = Alpha[i][t] * Beta[i][t] / prob_O_given_lambda;
			Gamma[i] += val;
			if (t != T-1) Gamma_minus_one[i] += val;
			if (t == 0) Gamma_one[i] += val;
			indiv_Gamma[data_no][i][t] = val;
		}
	}
}

void compute_epsilon(HMM *hmm, int data_no) {
	int n = hmm->state_num, T = input_len;
	for (int t = 0; t < T-1; ++t) {
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < n; ++j) {
				Epsilon[i][j] += Alpha[i][t] * hmm->transition[i][j] * hmm->observation[input_data[data_no][t+1]][j] 
					* Beta[j][t+1] / prob_O_given_lambda;
			}
		}
	}
}

void update_pi(HMM *hmm, int data_count) {
	int N = hmm->state_num;
	for (int i = 0; i < N; ++i) { //for each state
		hmm->initial[i] = Gamma_one[i] / data_count;
	}
}

void clear_arrays(int n, int T) {
	for (int i = 0; i < n; ++i) {
		Gamma[i] = Gamma_one[i] = Gamma_minus_one[i] = 0;
		for (int j = 0; j < n; ++j) Epsilon[i][j] = 0;
	}
}

void print_debug(HMM *hmm) {
	// debugging
	double sum = 0; int N = hmm->state_num;
	puts("\nPi:");
	for (int i = 0; i < N; ++i) {
		printf("%f ", hmm->initial[i]);
		sum += hmm->initial[i];
	}
	printf("sum = %f\n\n", sum);


	puts("A:"); 
	for (int i = 0; i < N; ++i) {
		double col_sum = 0;
		for (int j = 0; j < N; ++j) {
			printf("%f ", hmm->transition[i][j]);
			col_sum += hmm->transition[i][j];
		} printf("(sum: %f)\n", col_sum);
	} 
	puts("\n");


	puts("B:"); 
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			printf("%f ", hmm->observation[i][j]);
		} puts("");
	} puts("----sums----");
	for (int j = 0; j < N; ++j) {
		double row_sum = 0;
		for (int i = 0; i < N; ++i) {
			row_sum += hmm->observation[i][j];
		}
		printf("%f ", row_sum);
	} puts("\n");

}

void update_hmm(HMM *hmm, int data_count) { // data_count = 10000
	int N = hmm->state_num, T = input_len;
	update_pi(hmm, data_count);

	// update A
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			hmm->transition[i][j] = Epsilon[i][j] / Gamma_minus_one[i];
		}
	}

	// update B
	double dummy[MAX_OBSERV][MAX_STATE] = {};

	for (int j = 0; j < N; ++j) {  // for each state
		for (int n = 0; n < data_count; ++n) { // for each input
			for (int t = 0; t < input_len; ++t) { // for each symbol in input
				dummy[input_data[n][t]][j] += indiv_Gamma[n][j][t]; // b[k][j] += indiv_Gamma[n][j][t]
			}
		}
	}
	for (int k = 0; k < hmm->observ_num; ++k) { // for each state type
		for (int j = 0; j < N; ++j) {
			dummy[k][j] /= (Gamma[j]);
			hmm->observation[k][j] = dummy[k][j];
		}
	}


}

int main(int argc, char const *argv[]) {
	// parse args
	if (argc != 5) puts("usage: ./train #iterations model_init_file model_training_file model_output_file"), exit(0);
	HMM hmm;
	int iterations = atoi(argv[1]);             // number of times to train
	FILE* training_file = fopen(argv[3], "r");  // where to read the input data from
	FILE* output_file = fopen(argv[4], "w");    // where to write stuff out to

	loadHMM(&hmm, argv[2]);

	// read input
	int input_count = 0;
	while (fscanf(training_file, "%s", input_data[input_count]) != EOF) {
		int len = strlen(input_data[input_count]);
		input_len = len;
		for (int i = 0; i < len; i++) input_data[input_count][i] -= 'A';
		input_count++;
	}

	// train model
	while (iterations--) {
		for (int q = 0; q < input_count; ++q) {
			compute_alpha(&hmm, q);
			compute_beta(&hmm, q);
			compute_gamma(&hmm, q);
			compute_epsilon(&hmm, q);
		}
		update_hmm(&hmm, input_count);
		clear_arrays(hmm.state_num, hmm.observ_num);
	}
	// print_debug(&hmm);
	dumpHMM(output_file, &hmm);
	return 0;
}



