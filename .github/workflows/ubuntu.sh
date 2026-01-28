#!/usr/bin/bash

# Use grouped output messages
infobegin() {
	echo "::group::${1}"
}
infoend() {
	echo "::endgroup::"
}

# Required packages on Ubuntu
requires=(
	ccache # Use ccache to speed up build
	meson  # Used for meson build
)

# https://git.launchpad.net/ubuntu/+source/mate-calc/tree/debian/control
requires+=(
	autopoint
	bison
	flex
	git
	libatk1.0-dev
	libglib2.0-dev
	libgmp-dev
	libgtk-3-dev
	libmpc-dev
	libmpfr-dev
	libxml2-dev
	make
	mate-common
	yelp-tools
)

infobegin "Update system"
apt-get update -y
infoend

infobegin "Install dependency packages"
env DEBIAN_FRONTEND=noninteractive \
	apt-get install --assume-yes \
	${requires[@]}
infoend
