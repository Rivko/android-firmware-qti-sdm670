SUBDIRS= ./graphite-client

all: clean
	for dir in $(SUBDIRS) ; do (cd $$dir && $(MAKE) all) || exit 1; done

clean:
	for dir in $(SUBDIRS) ; do (cd $$dir && $(MAKE) clean) || exit 1; done
