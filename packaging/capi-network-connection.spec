#sbs-git:api/connection

Name:       capi-network-connection
Summary:    Network Connection library in TIZEN C API
Version:    0.1.0
Release:    19
Group:      System/Network
License:    Apache License Version 2.0
Source0:    %{name}-%{version}.tar.gz
Source1001: packaging/capi-network-connection.manifest 
BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(capi-base-common)
Requires(post): /sbin/ldconfig  
Requires(postun): /sbin/ldconfig

%description
Network Connection library in Tizen C API

%package devel
Summary:  Network Connection library in Tizen C API (Development)
Group:    TO_BE/FILLED_IN
Requires: %{name} = %{version}-%{release}

%description devel
Network Connection library in Tizen C API (Development)


%prep
%setup -q


%build
cp %{SOURCE1001} .
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
cmake . -DCMAKE_INSTALL_PREFIX=/usr -DFULLVER=%{version} -DMAJORVER=${MAJORVER}


make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%manifest capi-network-connection.manifest
%{_libdir}/libcapi-network-connection.so.*

%files devel
%manifest capi-network-connection.manifest
%{_includedir}/network/*.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/libcapi-network-connection.so
