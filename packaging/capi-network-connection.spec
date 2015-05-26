Name:       capi-network-connection
Summary:    Network Connection library in TIZEN C API
Version:    0.1.3_22
Release:    1
Group:      System/Network
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1001: 	capi-network-connection.manifest
BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(network)

%description
Network Connection library in Tizen C API

%package devel
Summary:  Network Connection library in Tizen C API (Development)
Group:    System/Network
Requires: %{name} = %{version}-%{release}

%description devel
Network Connection library in Tizen C API (Development)

%prep
%setup -q
cp %{SOURCE1001} .


%build
unset LD_AS_NEEDED
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
%cmake . -DFULLVER=%{version} -DMAJORVER=${MAJORVER}

make %{?_smp_mflags}


%install
%make_install

#License
mkdir -p %{buildroot}%{_datadir}/license
cp LICENSE.APLv2 %{buildroot}%{_datadir}/license/capi-network-connection

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%manifest %{name}.manifest
%attr(644,-,-) %{_libdir}/libcapi-network-connection.so.*
%{_datadir}/license/capi-network-connection

%files devel
%manifest %{name}.manifest
%{_includedir}/network/*.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/libcapi-network-connection.so
