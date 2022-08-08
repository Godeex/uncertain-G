javac -cp mmdb-2019-04-29.jar DblpExampleParser.java
java -cp mmdb-2019-04-29.jar:. DblpExampleParser dblp-2019-04-01.xml dblp-2017-08-29.dtd

g++ dblp_collect_edge.dblp_collect_edge
./a.out