all:

clean:
	rm -f PluginTest *~
	rm -f cnf/geonames.conf
	rm -fr tmp-geonames-db
	rm -f tmp-geonames-db.log

TEST_PREPARE_TARGETS := cnf/geonames.conf cnf/meta.conf
TEST_FINISH_TARGETS := .dummy
ifdef CI
GEONAMES_HOST_EDIT := sed -e 's/"smartmet-test"/"localhost"/g'
META_CONF_EDIT := sed -e 's|../cnf/templates|/etc/smartmet/plugins/meta/templates|g'
TEST_PREPARE_TARGETS += geonames-database
TEST_FINISH_TARGETS += stop-test-db
TEST_TARGET := dummy
else
GEONAMES_HOST_EDIT := cat
META_CONF_EDIT := cat
TEST_TARGET := oracle
endif

TEST_DB_DIR := tmp-geonames-db
TESTER_PARAM := --handler=/meta --reactor-config=cnf/reactor_$(TEST_TARGET).conf --ignore input/.testignore_$(TEST_TARGET)

test:	$(TEST_PREPARE_TARGETS)
	@rm -f failures/*
	@echo Running tests:
	ok=true; smartmet-plugin-test $(TESTER_PARAM) || ok=false; $(MAKE) $(TEST_FINISH_TARGETS); $$ok

cnf/geonames.conf: cnf/geonames.conf.in .dummy
	$(GEONAMES_HOST_EDIT) $< >$@

cnf/meta.conf: cnf/meta.conf.in .dummy
	$(META_CONF_EDIT) $< >$@

geonames-database:
	rm -rf tmp-geonames-db
	if ! /usr/share/smartmet/test/db/init-and-start.sh $(TEST_DB_DIR) >tmp-geonames-db.log 2>&1 ; then \
	    cat tmp-geonames-db.log; \
	    false; \
	fi
	/usr/share/smartmet/test/db/install-test-db.sh $(TEST_DB_DIR)

stop-test-db:
	/usr/share/smartmet/test/db/test-db-ctl.sh $(TEST_DB_DIR) stop

.dummy:
	true
