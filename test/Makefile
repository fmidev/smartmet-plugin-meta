all:

clean:
	rm -f PluginTest *~
	rm -f cnf/geonames.conf
	rm -fr tmp-geonames-db
	rm -f tmp-geonames-db.log
	rm -rf failures-dummy failures-oracle failures-postgresql

TEST_PREPARE_TARGETS := templates cnf/geonames.conf cnf/meta.conf
TEST_FINISH_TARGETS := .dummy
TEST_DB_DIR := $(shell pwd)/tmp-geonames-db
ifdef CI
GEONAMES_HOST_EDIT := sed -e 's|"smartmet-test"|"$(TEST_DB_DIR)"|g'
META_CONF_EDIT := cat
TEST_PREPARE_TARGETS += start-geonames-db
TEST_FINISH_TARGETS += stop-geonames-db
TEST_TARGET := dummy
else
GEONAMES_HOST_EDIT := cat
META_CONF_EDIT := cat
TEST_TARGETS := oracle postgresql
endif

TESTER_PARAM := --handler=/meta --reactor-config=cnf/reactor_$(TEST_TARGET).conf --ignore input/.testignore_$(TEST_TARGET)

test:	$(TEST_PREPARE_TARGETS)
	@rm -f failures/*
	ok=true; \
	for target in $(TEST_TARGETS); do \
		echo Running tests for $$target; \
		$(MAKE) do-test-$$target || ok=false; \
	done; \
	$(MAKE) $(TEST_FINISH_TARGETS); \
	$$ok

test-dummy test-oracle test-postgresql: $(TEST_PREPARE_TARGETS)
	ok=true; $(MAKE) do-$@ || ok = false; $(MAKE) $(TEST_FINISH_TARGETS); $$ok

do-test-dummy do-test-oracle do-test-postgresql:
	smartmet-plugin-test --handler=/meta \
		--reactor-config=cnf/reactor_$(patsubst do-test-%,%,$@).conf \
		--failures-dir failures-$(patsubst do-test-%,%,$@) \
		--ignore input/.testignore_$(patsubst do-test-%,%,$@)

cnf/geonames.conf: cnf/geonames.conf.in .dummy
	$(GEONAMES_HOST_EDIT) $< >$@

cnf/meta.conf: cnf/meta.conf.in .dummy
	$(META_CONF_EDIT) $< >$@

geonames-database:
	@-$(MAKE) stop-geonames-db
	rm -rf tmp-geonames-db
	if ! /usr/share/smartmet/test/db/create-local-db.sh $(TEST_DB_DIR) >tmp-geonames-db.log 2>&1 ; then \
	    cat tmp-geonames-db.log; \
	    false; \
	fi

start-geonames-db: geonames-database
	/usr/share/smartmet/test/db/test-db-ctl.sh $(TEST_DB_DIR) start -w

stop-geonames-db:
	-/usr/share/smartmet/test/db/test-db-ctl.sh $(TEST_DB_DIR) stop -w

templates:
	$(MAKE) -C .. all-templates

.dummy:
	true

.PHONY: meta.conf geonames.conf
