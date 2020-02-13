Name:		cpuid
Version:	1.1
Release:	1%{?dist}
Summary:	CLI interface to read CPUID information

Group:		Application/System
License:	GPLv2
URL:		https://github.com/drepper/cpuid
Source0:	%{name}-%{version}.tar.xz

%description

%prep
%setup -q


%build
make %{?_smp_mflags}


%install
rm -rf ${RPM_BUILD_ROOT}
%make_install


%files
%doc COPYING README.md
%{_bindir}/cpuid


%changelog
* Thu Feb 13 2020 Ulrich Drepper <drepper@gmail.com> - 1.1-1
- Add support for recent extensions
* Sat Feb 9 2019 Ulrich Drepper <drepper@gmail.com> - 1.0-1
- First version with spec file
