cxitool TJAPlayer_on_3DS.3dsx TJAPlayer_on_3DS.cxi --banner resources/banner.bin
makerom -f cia -o TJAPlayer_on_3DS.cia    -target t -i TJAPlayer_on_3DS.cxi:0:0
rm TJAPlayer_on_3DS.cxi