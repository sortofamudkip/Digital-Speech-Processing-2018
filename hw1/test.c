#include "hmm.h"
#include <math.h>
#include <assert.h>
#include <string.h>

typedef struct {
	int id; 
	double prob;
} result;

double viterbiiiii(HMM* hmm, char o[], int T) {
	int n = hmm->state_num;
	double Delta[MAX_STATE][MAX_SEQ] = {};
	// initialization
	for (int i = 0; i < n; ++i) Delta[i][0] = hmm->initial[i] * hmm->observation[o[0]][i];
	// recursion thingy
	for (int t = 0; t < T-1; ++t) {
		for (int j = 0; j < n; ++j) {
			// get max
			double max = -1.f;
			for (int i = 0; i < n; ++i) {
				double val = Delta[i][t] * hmm->transition[i][j];
				if (val > max) max = val;
			}
			Delta[j][t+1] = max * hmm->observation[o[t+1]][j];
		}
	}
	double max = -1.f;
	for (int i = 0; i < n; ++i) {
		if (Delta[i][T-1] > max) max = Delta[i][T-1];
	}

	// printf("=========delta========= (T = %d)\n", T);
	// for (int i = 0; i < n; ++i) {
	// 	for (int t = 0; t < T; ++t) {
	// 		printf("%f ", Delta[i][t]);
	// 	} puts("");
	// } puts("=======================");
	// printf("max = %.50f\n", max);
	// assert(max > 0.f);
	return max;
}


result get_best_HMM(HMM hmm[], int HMM_count, char observation[], int len) {
	int best_index; double best_prob = -1.f; //highest prob
	for (int i = 0; i < HMM_count; ++i) {
		double prob = viterbiiiii(&hmm[i], observation, len);
		// assert(prob > 0);
		if (prob > best_prob) best_prob = prob, best_index = i;
	}
	result R = {best_index, best_prob}; return R;
}

// FILE* debug;

void test(HMM hmm[], int HMM_count, FILE* infile, FILE* outfile) {
	char observation[510];
	while (fscanf(infile, "%s", observation) != EOF) {
		// for each file, give it to the HMMs and return which HMM gives the best result
		int len = strlen(observation);
		for (int i = 0; i < len; i++) observation[i] -= 'A'; 
		result R = get_best_HMM(hmm, HMM_count, observation, len);
		// printf("model_0%d.txt %e\n", R.id+1, R.prob);
		fprintf(outfile,  "model_0%d.txt %e\n", R.id+1, R.prob);
	}

}

int main(int argc, char const *argv[]) {
	if (argc != 4) puts("usage: ./test modellist.txt testing_data*.txt result*.txt"), exit(0);
	// parse args
	FILE* testing_data_file = fopen(argv[2], "r");
	FILE* result_file = fopen(argv[3], "w");
	assert(testing_data_file && result_file);
	HMM hmm[7]; //why not
	int HMM_count = load_models(argv[1], hmm, 7);

	// dump_models(hmm, HMM_count);
	// debug = fopen("testing_answer.txt", "r");

	test(hmm, HMM_count, testing_data_file, result_file);

	return 0;
}