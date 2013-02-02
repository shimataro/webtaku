###Setup Instructions

# install tools to create thumbnail
sudo apt-get install -y xvfb libqtwebkit-dev libicu-dev

# install MS fonts and Japanese fonts
sudo apt-get install -y ttf-mscorefonts-installer ttf-umefont

# make
git clone https://github.com/shimataro/wiget.git
cd wiget
qmake
make

# create image
# xvfb is only necessary for running wiget on a headless server (i.e. a server without a GUI or even a graphics card)
xvfb-run --server-args="-screen 0, 1024x768x24" ./wiget --format=PNG --min-width=1024 http://example.com/ >output.png

Note:
You can create thumbnail with ImageMagick as below.
./wiget http://example.com/ | convert -resize 320 -crop 320x240+0+0 - thumb.png
