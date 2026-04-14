# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

SmartMet Meta plugin (`smartmet-plugin-meta`) — provides HTTP access to observation and forecast metadata via the `/meta` endpoint. Serves quality codes, observable properties, forecast parameter metadata, and flash counts. Responses are XML rendered through CTPP2 templates.

## Build commands

```bash
make                  # Build meta.so
make clean            # Clean build artifacts
make format           # Run clang-format
make test             # Run integration tests (requires DB backends unless CI)
make rpm              # Build RPM package
```

Templates in `cnf/templates/` must be compiled (`.template` -> `.c2t`) — `make` handles this via `all-templates`.

## Testing

Tests are integration tests using `smartmet-plugin-test`, not Boost unit tests. Each test is an HTTP request file in `test/input/` with expected response in `test/output/`.

```bash
make -C test test                    # Run all configured test backends
make -C test test-dummy              # Run with dummy observation engine (no DB needed)
make -C test test-oracle             # Run with Oracle backend
make -C test test-postgresql         # Run with PostgreSQL backend
```

Test input files are HTTP GET requests (e.g., `GET /meta?language=eng&observableProperty=forecast&param=Temperature`). Tests ignored per backend are listed in `test/input/.testignore_<backend>` — observation tests are skipped in dummy mode.

In CI, a local geonames DB is created and used; locally, tests use the system geonames DB.

## Architecture

**Three source files** in `meta/`:
- `Plugin.cpp/.h` — main plugin class: request routing, config parsing, engine initialization, response generation
- `DataQuality.cpp/.h` — registry for observation quality codes (0-9) with multilingual labels, alias mapping, and legacy flag-to-code conversion
- `ForecastMetaData.h/.cpp` — struct holding per-parameter forecast metadata (name, label, unit, statistical measures)

**Request routing** in `Plugin::query()` dispatches on the `observableProperty` parameter:
- `forecast` → `getForecastMetadata()` — returns metadata from config, no engine query
- `observation` → `getObsEngineMetadata()` — queries the observation engine
- `qualitycode` parameter → `getDataQualityMetadata()` — returns quality code definitions
- `flashcount` parameter → `getFlashCount()` — queries observation engine for lightning data

**Engine dependencies** (resolved at runtime by the server):
- `Querydata` — forecast data access
- `Geonames` — geographic lookups
- `Observation` — observation data (conditionally compiled via `WITHOUT_OBSERVATION` macro)

**Configuration** (`cnf/meta.conf`): defines forecast parameter metadata inline (labels, units, statistical measures per language), template paths, data quality config directory, and default language.

**Templates** (`cnf/templates/`): CTPP2 templates for XML responses — `observable_properties.template`, `observation_data_quality.template`, `forecast_properties.template`, `exception.template`.

## Conditional compilation

The `WITHOUT_OBSERVATION` preprocessor macro excludes all observation engine dependencies. When defined, only forecast metadata and quality code endpoints are available.
