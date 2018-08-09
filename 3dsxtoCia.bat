cxitool Tjaplayer_on_3DS.3dsx Tjaplayer_on_3DS.cxi --banner resources/banner.bin
makerom -f cia -o Tjaplayer_on_3DS.cia    -target t -i Tjaplayer_on_3DS.cxi:0:0
rm Tjaplayer_on_3DS.cxi