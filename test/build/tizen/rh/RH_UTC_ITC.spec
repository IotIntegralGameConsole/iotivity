Name: org.tizen.RH_UTC_ITC
Version: 0.0.1
Release: 1
License: To be filled
Summary: Resource Hosting UTC ITC build for Tizen
Packager: Md. Arafat Hossain <arafat.hsn@samsung.com>
Group: Test Application
Source: %{name}-%{version}.tar.gz

BuildRequires: boost-devel
BuildRequires: boost-thread
BuildRequires: boost-system
BuildRequires: boost-filesystem

BuildRequires: pkgconfig(glib-2.0)
Buildrequires: gettext-tools
Buildrequires: expat-devel
BuildRequires: pkgconfig(appcore-efl)
BuildRequires: pkgconfig(ecore-x)
BuildRequires: pkgconfig(elementary)
#BuildRequires: pkgconfig(efl-assist)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(uuid)
BuildRequires: pkgconfig(capi-network-wifi)
BuildRequires: pkgconfig(capi-network-bluetooth)
BuildRequires: scons
BuildRequires: iotivity
BuildRequires: iotivity-service

%description
Platform Project

%prep
%setup -q

%build
scons

%install
mkdir -p $RPM_BUILD_ROOT/usr/bin/rh
mkdir -p $RPM_BUILD_ROOT/usr/lib
cp rh_temp_hum_sensor $RPM_BUILD_ROOT/usr/bin/rh
cp rh_temp_hum_sensor2 $RPM_BUILD_ROOT/usr/bin/rh
cp rh_non_hostable_provider $RPM_BUILD_ROOT/usr/bin/rh
cp iotivity_rh_test $RPM_BUILD_ROOT/usr/bin/rh
cp iotivity/out/tizen/armv7l/release/*.so $RPM_BUILD_ROOT/usr/lib/

%clean
scons -c

%files
%defattr(644, root, root)
%attr(755, root, root)
/usr/bin/rh
/usr/lib

%changelog
* Sat Mar 24 2012 Md. Arafat Hossain <arafat.hsn@samsung.com>
 - initial release
