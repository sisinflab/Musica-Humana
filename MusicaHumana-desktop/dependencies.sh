install_name_tool -change lib/libopencv_core.2.4.dylib @executable_path/../Frameworks/libopencv_core.2.4.dylib MusicaHumana.app/Contents/MacOS/MusicaHumana; install_name_tool -change lib/libopencv_highgui.2.4.dylib @executable_path/../Frameworks/libopencv_highgui.2.4.dylib MusicaHumana.app/Contents/MacOS/MusicaHumana; install_name_tool -change lib/libopencv_imgproc.2.4.dylib @executable_path/../Frameworks/libopencv_imgproc.2.4.dylib MusicaHumana.app/Contents/MacOS/MusicaHumana; install_name_tool -change lib/libopencv_objdetect.2.4.dylib @executable_path/../Frameworks/libopencv_objdetect.2.4.dylib MusicaHumana.app/Contents/MacOS/MusicaHumana; install_name_tool -change lib/libopencv_features2d.2.4.dylib @executable_path/../Frameworks/libopencv_features2d.2.4.dylib MusicaHumana.app/Contents/MacOS/MusicaHumana; install_name_tool -change /Users/giuseppe/flandmark-master/bin/libflandmark/libflandmark_shared.dylib @executable_path/../Frameworks/libflandmark_shared.dylib MusicaHumana.app/Contents/MacOS/MusicaHumana
