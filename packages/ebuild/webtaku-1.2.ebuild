# Copyright 1999-2014 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v3
# $Header: $

EAPI=5
inherit qt4-r2 git-r3 bash-completion-r1

DESCRIPTION="WEBTAKU is webpage snapshot image generator sounds like GYOTAKU(魚拓; literally, fish-rubbing), forked from raphaelcruzeiro/webimage."
HOMEPAGE="https://github.com/shimataro/webtaku"
SRC_URI=""

EGIT_REPO_URI="https://github.com/shimataro/${PN}.git"
EGIT_COMMIT="v${PV}"

LICENSE="GPL-3"
SLOT="1"
KEYWORDS="amd64 x86 ~ppc ~ppc64"
IUSE="nogui"

RDEPEND="nogui? ( x11-base/xorg-server[xvfb] )"
DEPEND=">=dev-qt/qtwebkit-4"

src_configure() {
	eqmake4 CONFIG+=release PREFIX=/usr BASH_COMPLETION_DIR=$(get_bashcompdir)
}
