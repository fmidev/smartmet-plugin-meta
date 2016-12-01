%define DIRNAME meta
%define SPECNAME smartmet-plugin-%{DIRNAME}
Summary: SmartMet meta plugin
Name: %{SPECNAME}
Version: 16.11.30
Release: 1%{?dist}.fmi
License: FMI
Group: SmartMet/Plugins
URL: http://www.weatherproof.fi
Source0: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires: libconfig-devel
BuildRequires: oracle-instantclient11.2-devel
BuildRequires: smartmet-library-spine-devel >= 16.11.29
BuildRequires: libsmartmet-macgyver-devel >= 16.9.30
BuildRequires: smartmet-engine-querydata-devel >= 16.11.30
BuildRequires: smartmet-engine-observation-devel >= 16.11.30
BuildRequires: ctpp2
BuildRequires: protobuf
BuildRequires: imake
Requires: ctpp2
Requires: libconfig
Requires: libsmartmet-macgyver >= 16.9.30
Requires: smartmet-server >= 16.11.30
Requires: smartmet-engine-observation >= 16.11.30
Requires: smartmet-engine-querydata >= 16.11.30
Requires: smartmet-library-spine >= 16.11.29
Provides: %{SPECNAME}
Obsoletes: smartmet-brainstorm-metaplugin < 16.11.1
Obsoletes: smartmet-brainstorm-metaplugin-debuginfo < 16.11.1

%description
FMI SmartMet meta plugin

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q -n plugins/%{DIRNAME}
 
%build -q -n plugins/%{DIRNAME}
make %{_smp_mflags}

%install
%makeinstall
mkdir -p $RPM_BUILD_ROOT%{_sysconfdir}/smartmet/plugins
mkdir -p $RPM_BUILD_ROOT%{_sysconfdir}/smartmet/plugins/meta/templates
for file in cnf/templates/*.c2t; do
    install -m 664 $file  $RPM_BUILD_ROOT%{_sysconfdir}/smartmet/plugins/meta/templates/
done

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(0775,root,root,0775)
%{_datadir}/smartmet/plugins/%{DIRNAME}.so
%defattr(0664,root,root,0775)
%{_sysconfdir}/smartmet/plugins/meta/templates/*.c2t

%changelog
* Wed Nov 30 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.11.30-1.fmi
- No installation for configuration

* Tue Nov 29 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.11.29-1.fmi
- Recompiled due to spine API changes

* Tue Nov  1 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.11.1-1.fmi
- Namespace and directory name changed

* Tue Sep  6 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.9.6-1.fmi
- New exception handler

* Tue Aug 30 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.8.30-1.fmi
- Base class API change
- Use response code 400 instead of 503

* Mon Aug 15 2016 Markku Koskela <markku.koskela@fmi.fi> - 16.8.15-1.fmi
- The init(),shutdown() and requestHandler() methods are now protected methods
- The requestHandler() method is called from the callRequestHandler() method

* Wed Jun 29 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.6.29-1.fmi
- QEngine API changed

* Tue Jun 14 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.6.14-1.fmi
- Full recompile

* Thu Jun  2 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.6.2-1.fmi
- Full recompile

* Wed Jun  1 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.6.1-1.fmi
- Added graceful shutdown

* Mon May 16 2016 Tuomo Lauri <tuomo.lauri@fmi.fi> - 16.5.16-1.fmi
- Added WavePeriod - parameter

* Tue May  3 2016 Tuomo Lauri <tuomo.lauri@fmi.fi> - 16.5.3-1.fmi
- Added timezone support to flash query

* Wed Apr 27 2016 Santeri Oksman <santeri.oksman@fmi.fi> - 16.4.27-1.fmi
- Use LocationOptions to parse flash options

* Wed Apr 20 2016 Tuomo Lauri <tuomo.lauri@fmi.fi> - 16.4.20-1.fmi
- Report flashcount with observableProperty=flashcount

* Fri Feb 26 2016 Tuomo Lauri <tuomo.lauri@fmi.fi> - 16.2.26-1.fmi
- Fixed the metadata of sea water salinity.

* Mon Jan 18 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.1.18-1.fmi
- newbase API changed, full recompile

* Tue Jan  5 2016 Santeri Oksman <santeri.oksman@fmi.fi> - 16.1.5-1.fmi
- MeanIceThickness values of HELMI model are rescaled

* Wed Nov 18 2015 Tuomo Lauri <tuomo.lauri@fmi.fi> - 15.11.18-1.fmi
- SmartMetPlugin now receives a const HTTP Request
- Added HourlyMaximumGust - forecast parameter

* Mon Nov  9 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.11.9-1.fmi
- Using fast case conversion without locale locks when possible

* Mon Oct 26 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.10.26-1.fmi
- Added proper debuginfo packaging

* Tue Sep 29 2015 Tuomo Lauri <tuomo.lauri@fmi.fi> - 15.9.29-1.fmi
- Hilatar additions

* Wed Sep  2 2015 Tuomo Lauri <tuomo.lauri@fmi.fi> - 15.9.2-1.fmi
- Added new MyOcean-related forecast parameters

* Mon Aug 24 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.8.24-1.fmi
- Recompiled due to Convenience.h API changes

* Tue Aug 18 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.8.18-1.fmi
- Use time formatters from macgyver to avoid global locks from sstreams

* Mon Aug 17 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.8.17-1.fmi
- Use -fno-omit-frame-pointer to improve perf use

* Fri Aug 14 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.8.14-1.fmi
- Recompiled due to string formatting changes

* Mon May  4 2015 Tuomo Lauri <tuomo.lauri@fmi.fi> - 15.5.4-1.fmi
- Fixed validation errors of a result document.

* Thu Apr  9 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.4.9-1.fmi
- newbase API changed

* Wed Apr  8 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.4.8-1.fmi
- Using dynamic linking for smartmet libraries

* Tue Feb 17 2015 Jukka A. Pakarinen <jukka.pakarinen@fmi.fi> - 15.2.17-1.fmi
- PAK-437 Measurand conversion bug fix.

* Mon Jan 12 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.1.12-1.fmi
- Fixed typo in configuration file

* Thu Dec 18 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.12.18-1.fmi
- Recompiled due to spine API changes

* Tue Jul 1 2014 Mikko Visa <mikko.visa@fmi.fi> - 14.7.1-1.fmi
- Rebuild, new release after API changes of library dependencies

* Wed May 14 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.5.14-1.fmi
- Use shared macgyver and locus libraries

* Tue May  6 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.5.6-1.fmi
- qengine API changed

* Mon Apr 28 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.4.28-1.fmi
- Full recompile due to large changes in spine etc APIs

* Thu Mar 20 2014 Mikko Visa <mikko.visa@fmi.fi> - 14.3.20-1.fmi
- Open data 2014-04-01 release
- Added a missing netcdf unit for PseudoAdiabaticPotentialTemperature.
- Unallowed character auto change in textual fields of xml result files.
- Hotfix: Removed pointless error messages.

* Mon Feb 3 2014 Mikko Visa <mikko.visa@fmi.fi> - 14.2.3-2.fmi
- Open data 2014-02-03 release
- Activatation of radiation accumulation parameter.
- Included VelocityPotential parameter configuration.
~ Improved parameter handling with correct pointer handling of parameter vectors.
- Implemented metadata element block for quality codes of observations.
- Implemented support for quality code parameters (eg. qc_t2m).
- Added netcdf units for the parameters of HELMI model.
- Added and fixed grib and netcdf units of radiation parameters in the parameter configuration.
- Fixed the unit of geopotential height to meters.
- Defined positive velocity direction info for VerticalVelocityMMS parameter.

* Thu Dec 12 2013 Tuomo Lauri <tuomo.lauri@fmi.fi> - 13.12.12-1.fmi
- Added 4 HELMI Ice Model parameter metadata configurations.
- Fixed PrecipitationAmount and GeopHeight units.

* Mon Nov 25 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.11.25-1.fmi
- Configured grib units for forecast parameters.
- Restrictions and exeception message added for units parameter.

* Thu Nov 14 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.11.14-1.fmi
- Added 'units' request KVP and created multi unit configuration for forecast parameters.

* Mon Nov 11 2013 Tuomo Lauri <tuomo.lauri@fmi.fi> - 13.11.11-1.fmi
- Added APP-forecast related parameters to metaplugin conf

* Tue Nov  5 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.11.5-1.fmi
- Added two new parameters VerticalVelocityMMS and PseudoAdiabaticPotentialTemperature
- Fixed libsmartmet-macgyver build dependency.
- Code conversion only for integers and minor description changes into QC-configurations.

* Wed Oct  9 2013 Tuomo Lauri <tuomo.lauri@fmi.fi> - 13.10.9-1.fmi
- Added data quality code support for new codes and old codes as aliases.
- Now conforming with the new Reactor initialization API

* Fri Sep 6  2013 Tuomo Lauri    <tuomo.lauri@fmi.fi>    - 13.9.6-1.fmi
- Recompiled due Spine changes

* Thu Sep 5 2013  Tuomo Lauri    <tuomo.lauri@fmi.fi>    - 13.9.5-1.fmi
- Added UVMaximum forecast parameter

* Tue Jul 23 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.7.23-1.fmi
- Recompiled due to thread safety fixes in newbase & macgyver

* Wed Jul  3 2013 mheiskan <mika.heiskanen@fmi.fi> - 13.7.3-1.fmi
- Update to boost 1.54

* Mon Jun  3 2013 lauri  <tuomo.lauri@fmi.fi>    - 13.6.3-1.fmi
- Built against the new Spine

* Thu May 30 2013 lauri  <tuomo.lauri@fmi.fi>    - 13.5.30-1.fmi
- Added Access-Control-Allow-Origin header

* Wed May 29 2013 lauri  <tuomo.lauri@fmi.fi>    - 13.5.29-2.fmi
- gmlid is now correct when querying a single parameter

* Wed May 29 2013 lauri  <tuomo.lauri@fmi.fi>    - 13.5.29-1.fmi
- Added functionality to query all ObservableProperties

* Wed May 22 2013 oksman <santeri.oksman@fmi.fi> - 13.5.22-1.fmi
- Support the case that multiple gml ids use same observable property.

* Thu May 16 2013 oksman <santeri.oksman@fmi.fi> - 13.5.16-1.fmi
- Added language support to observation meta queries.

* Tue May 14 2013 lauri <tuomo.lauri@fmi.fi> - 13.5.14-1.fmi
- Added templates to the specfile

* Mon May 13 2013 tervo <roope.tervo@fmi.fi> - 13.5.13-2.fmi
- New parameters

* Mon May 13 2013 lauri <tuomo.lauri@fmi.fi> - 13.5.13-1.fmi
- Added observableProperty - queries for data sources other than observations

* Sat May 11 2013 tervo <roope.tervo@fmi.fi> - 13.5.11-1.fmi
- Fixes

* Wed May 08 2013 oksman <santeri.oksman@fmi.fi> - 13.5.8-1.fmi
- Added conf file to the rpm.

* Tue Apr 30 2013 oksman <santeri.oksman@fmi.fi> - 13.4.30-2.fmi
- Register also GeoEngine because ObsEngine needs it.

* Tue Apr 30 2013 oksman <santeri.oksman@fmi.fi> - 13.4.30-1.fmi
- Support custom gml ids in observation queries.

* Mon Apr 22 2013 mheiskan <mika.heiskanen@fi.fi> - 13.4.22-1.fmi
- Brainstorm API changed

* Fri Apr 12 2013 lauri <tuomo.lauri@fmi.fi>    - 13.4.12-1.fmi
- Rebuild due to changes in Spine

* Thu Mar 14 2013 oksman <santeri.oksman@fmi.fi>- 13.3.14-1.fmi
- First release
