del /q kclient.vc.db
del /q bin/x64client/Debug/*.ilk
del /q bin/x64client/Debug/*.exp
del /q bin/x64client/Debug/*.log
del /q bin/x64client/Debug/*.txt
del /q bin/x64client/Debug/*_static.*
rmdir /s /q Debug
rmdir /s /q .vs
rmdir /s /q _intermediate
rmdir /s /q build_android/native-activity/app/.cxx/Debug
rmdir /s /q build_android/native-activity/app/.cxx/RelWithDebInfo
rmdir /s /q build_android/native-activity/app/build
rmdir /s /q build_android/native-activity/app/src/main/assets/asset
