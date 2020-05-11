.PHONY: all clean

CFLAGS+=
LDFLAGS+=-lm     # link to math library

TRAIN=train
TEST=test
iter=2000

all: $(TRAIN) $(TEST) 
# type make/make all to compile test_hmm

run: 
	./train $(iter) model_init.txt seq_model_01.txt model_01.txt
	./train $(iter) model_init.txt seq_model_02.txt model_02.txt
	./train $(iter) model_init.txt seq_model_03.txt model_03.txt
	./train $(iter) model_init.txt seq_model_04.txt model_04.txt
	./train $(iter) model_init.txt seq_model_05.txt model_05.txt
	./test modellist.txt testing_data1.txt result1.txt
	./test modellist.txt testing_data2.txt result2.txt
