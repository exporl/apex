#!/bin/sh

# Test for right directory
[ -d src ] || exit 1

WIXDIR="c:/program files/wix"

echo "Making APEX installer"

"$WIXDIR"/candle.exe -out bin/apex.wixobj build/installer/wix/apex.wxs
"$WIXDIR"/light.exe -out bin/apex_`cat VERSION`.msi bin/apex.wixobj "$WIXDIR"/wixui.wixlib -loc "$WIXDIR"/WixUI_en-us.wxl


echo "Making SPIN plugin installer"

"$WIXDIR"/candle.exe -out bin/spin.wixobj build/installer/wix/spin.wxs
"$WIXDIR"/light.exe -out bin/apex_spin_plugin_`cat VERSION`.msi bin/spin.wixobj "$WIXDIR"/wixui.wixlib -loc "$WIXDIR"/WixUI_en-us.wxl
