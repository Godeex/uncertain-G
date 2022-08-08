CC=g++
CFLAGS=-O2 -DO2

BIN=bin
SRC=src

METHOD_SRC_DIR=${SRC}/method
METHODS=method pruned_landmark_labeling shortest_path_dijkstra naive_monte_carlo truth recursive_sampling rss lazy_propagation_sampling modified_pll spi rss1

BIN_METHOD=${BIN}/method
METHOD_SRC=$(patsubst %,$(METHOD_SRC_DIR)/%.cc,$(METHODS))
METHOD_HEADERS=$(patsubst %,header/%.h,$(METHODS))
METHOD_OBJ=$(patsubst %,$(BIN_METHOD)/%.o,$(METHODS))

OBJ=struct.o graph.o query.o logger.o breadth_first_search.o ${METHOD_OBJ}
HEADER=query.h struct.h graph.h logger.h fib.h ${METHOD_HEADERS}

obj: $(OBJ)

%.o: %.cc %.h
	$(CC) -c $< -o $@ ${CFLAGS}

$(BIN_METHOD)/%.o: ${METHOD_SRC_DIR}/%.cc header/%.h header/method.h struct.h
	$(CC) -c $< -o $@ -I header -I . ${CFLAGS}

graph.o: struct.h logger.h
query.o: struct.h query.h graph.h logger.h
recursive_method.o: struct.h query.h graph.h logger.h
breadth_first_search.o: struct.h fib.h

run: main.cc $(HEADER) $(OBJ)
	$(CC) -o $@ main.cc $(OBJ) -I header -I . ${CFLAGS}

ai: main.cc $(HEADER) $(OBJ) analyze_index.cc
	$(CC) -o $@ analyze_index.cc $(OBJ) -I header -I . ${CFLAGS}

test_data:
	g++ erdos_renyi.cc && ./a.out > datasets/test.txt

clean:
	rm $(OBJ)

# datasets
# DATASET_DIR=datasets
# DBLP_FILE=$(DATASET_DIR)/dblp.txt
# DBLP_XML=dblp-2019-04-01.xml
# $(DBLP_FILE): $(DBLP_XML)
# 	run datasets/DblpExampleParser.java to load xml file and run dblp_collect_edge.cc to count edge
# $(DBLP_XML): $(DBLP_XML).gz
# 	gzip -d $(DBLP_XML).gz -k
# $(DBLP_XML).gz:
# 	cd datasets && wget http://dblp.org/src/mmdb-2019-04-29.jar \
# 	http://dblp.org/xml/release/dblp-2019-04-01.xml.gz \
# 	http://dblp.org/xml/release/dblp-2017-08-29.dtd

CONDMAT_FILE=$(DATASET_DIR)/ca-CondMat.txt
$(CONDMAT_FILE): $(CONDMAT_FILE).gz
	gzip -d $(CONDMAT_FILE).gz -k
$(CONDMAT_FILE).gz:
	cd datasets && curl https://snap.stanford.edu/data/ca-CondMat.txt.gz -O

HEPT_FILE=$(DATASET_DIR)/ca-HepTh.txt
$(HEPT_FILE): $(HEPT_FILE).gz
	gzip -d $(HEPT_FILE).gz -k
$(HEPT_FILE).gz:
	cd datasets && curl https://snap.stanford.edu/data/ca-HepTh.txt.gz -O

datasets: $(DBLP_FILE) $(CONDMAT_FILE) $(HEPT_FILE)

