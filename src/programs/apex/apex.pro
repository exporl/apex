CLEBS *= apexmain apextools common asciicast
!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT *= widgets

# androiddeployqt does not take library dependencies into account, so link all of them directly
android:QT *= network printsupport script scripttools svg xml xmlpatterns webview testlib androidextras websockets webchannel qml quick webview quickwidgets

win32:RC_FILE = ../../../data/icons/icon.rc
