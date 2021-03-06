# Setup Instructions

## preparation

### [Ubuntu](http://www.ubuntu.com/)

    # essential packages
    sudo apt-get -y install libqtwebkit-dev libicu-dev

    # fonts
    sudo apt-get -y install ttf-mscorefonts-installer ttf-umefont

    # if following message is displayed (especially Ubuntu 11.10 or later):
    # (webtaku:XXXXX): Gtk-WARNING **: Unable to locate theme engine in module_path: "pixmap"
    sudo apt-get -y install gtk2-engines-pixbuf

    # virtual X frame buffer (if your system has no GUI)
    sudo apt-get -y install xvfb

### [Gentoo Linux](http://www.gentoo.org/)

    # essential packages
    emerge dev-qt/qtwebkit

    # fonts
    emerge media-fonts/umeplus-fonts media-fonts/ubuntu-font-family

    # X.Org Server (if your system has no GUI, specify "xvfb" flag in USE)
    emerge x11-base/xorg-server

Or you can create overlay package easily by using ebuild files in "packages/ebuild" directory.

(if your system has no GUI, specify "nogui" flag in USE)

## build & install

    git clone https://github.com/shimataro/webtaku.git
    cd webtaku
    qmake CONFIG+=release
    make
    sudo make install

to change prefix(default is "/usr/local"), run `qmake` as following

    qmake PREFIX=/usr

to change bash completion directory(default is "/etc/bash\_completion.d"), run `qmake` as following

    # e.g., in Gentoo Linux, specify as following
    qmake BASH_COMPLETION_DIR=/usr/share/bash-completion

if you are using Intel C++ Compiler, run `qmake` as following
("icc" USE flag does same thing)

    qmake QMAKE_CC=icc QMAKE_CXX=icpc QMAKE_LINK=icpc

---

# Usage

## basic usage

### dump image data into stdout (default file format is PPM)

    webtaku "http://example.com/" >output.ppm

### if scheme is omitted, "http://" will be complemented

    webtaku "example.com" >output.ppm

### "--format" to specify file format (BMP|JPG|PNG|PPM|XBM|XPM)

    webtaku --format=JPG "http://example.com/" >output.jpg

### "--output" to specify output file

    # file format will be determined by extension ("--format" option will be ignored)
    webtaku --output=output.png "http://example.com/"

### when your system has no GUI, use virtual frame buffer

    # terminal 1: run Xvfb ("-noreset" option is necessary to avoid memory leaks)
    Xvfb :0 -noreset
    # terminal 2: generate image
    DISPLAY=:0 webtaku "http://example.com/" >output.ppm

    # in Ubuntu, xvfb-run can be used to generate image in one-liner
    # note: use "--output" option because xvfb-run redirects stderr to stdout
    xvfb-run --auto-servernum --server-args="-noreset" webtaku --output=output.png "http://example.com/"

### "--accept-language" to set acceptable languages list

    webtaku --accept-language="en-US,jp;q=0.8,en;q=0.6" "http://example.com/" >output.ppm

    # you can use multiple "--accept-language" options to set multiple acceptable languages in this order
    webtaku --accept-language="en-US" --accept-language=="jp;q=0.8" --accept-language="en;q=0.6" "http://example.com/" >output.ppm

### "--cookie" to set cookie

    webtaku --cookie="name1=value1;name2=value2" "http://example.com/" >output.ppm

    # you can use multiple "--cookie" options to set multiple cookies
    webtaku --cookie="name1=value1" --cookie="name2=value2" "http://example.com/" >output.ppm

### "--min-size" to specify minimum viewport size (default is 1024x768)

    webtaku --min-size=1200x900 "http://example.com/" >output.ppm
    webtaku --min-size=1200x "http://example.com/" >output.ppm
    webtaku --min-size=x900 "http://example.com/" >output.ppm

### "--scaled-size" to resize image as below

    # this will create 300x225 image
    webtaku --scaled-size=300x300 "http://example.com/" >output.ppm

    # use upper case "X" to resize by long-side (this will create 400x300 image)
    webtaku --scaled-size=300X300 "http://example.com/" >output.ppm

    # use colon ":" NOT to keep aspect ratio (this will create 300x300 image)
    webtaku --scaled-size=300:300 "http://example.com/" >output.ppm

    # omit height to resize by width (300x225)
    webtaku --scaled-size=300x "http://example.com/" >output.ppm

    # omit width to resize by height (400x300)
    webtaku --scaled-size=x300 "http://example.com/" >output.ppm

### "--crop" to crop image by minimum viewport size

    webtaku --crop "http://example.com/" >output.ppm

### "--silent" to output no message

    webtaku --silent "http://example.com/" >output.ppm

### "--user-agent" to specify user agent

    # iPhone5 (iOS6)
    webtaku --user-agent="Mozilla/5.0 (iPhone; CPU iPhone OS 6_0 like Mac OS X) AppleWebKit/536.26 (KHTML, like Gecko) Version/6.0 Mobile/10A403 Safari/8536.25" --min-size=1136x640 "http://example.com/" >output.ppm

    # iPad (iOS6)
    webtaku --user-agent="Mozilla/5.0 (iPad; CPU OS 6_0 like Mac OS X) AppleWebKit/536.26 (KHTML, like Gecko) Version/6.0 Mobile/10A403 Safari/8536.25" --min-size=1024x768 "http://example.com/" >output.ppm

## advanced usage

### to create 320x240 thumbnail from 1200x900 viewport

    webtaku --min-size=1200x900 --crop --scaled-size=320x240 --output=output.png "http://example.com/"

### for more complex conversion, use ImageMagick as below

    webtaku --min-size=1200x900 "http://example.com/" | convert -crop 500x500+100+100 -rotate +90 -resize 250x250 - output.ppm

# Project Page

http://shimataro.github.io/webtaku/
