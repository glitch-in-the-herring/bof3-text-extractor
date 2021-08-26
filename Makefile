all: extractor jpextractor
extractor: src_c/extractor.c src_c/utils.c
	gcc -o extractor src_c/extractor.c src_c/utils.c
jpextractor: src_c/jpextractor.c src_c/jputils.c
	gcc -o jpextractor src_c/jpextractor.c src_c/jputils.c