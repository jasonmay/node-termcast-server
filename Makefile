all: ext/libtermchanges/libtermchanges.la

ext/libtermchanges/libtermchanges.la:
	cd ext/libtermchanges && make
