ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

install:
	install -d $(PREFIX)/include/b63/counters
	install -d $(PREFIX)/include/b63/utils
	install -m 644 include/b63/*.h $(PREFIX)/include/b63/
	install -m 644 include/b63/counters/*.h $(PREFIX)/include/b63/counters/
	install -m 644 include/b63/utils/*.h $(PREFIX)/include/b63/utils/
