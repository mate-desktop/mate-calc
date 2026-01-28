#!/usr/bin/bash

# Use grouped output messages
infobegin() {
	echo "::group::${1}"
}
infoend() {
	echo "::endgroup::"
}

# Required packages on Fedora
requires=(
	ccache # Use ccache to speed up build
	meson  # Used for meson build
)

# https://src.fedoraproject.org/cgit/rpms/mate-calc.git
requires+=(
	autoconf-archive
	bison
	desktop-file-utils
	flex
	gcc
	git
	gmp-devel
	gtk3-devel
	libmpc-devel
	libxml2-devel
	make
	mate-common
	mate-desktop-devel
	mpfr-devel
	redhat-rpm-config
)

infobegin "Update system"
dnf update -y
infoend

infobegin "Install dependency packages"
dnf install -y ${requires[@]}
infoend
