@echo off

SETLOCAL EnableDelayedExpansion
:: Stuff you might need or want to edit.
::-----
set PKG_TITLE="PS4 SDL Template"
set PKG_VERSION="1.00"
set PKG_TITLE_ID="SDLT00001"
set PKG_CONTENT_ID="IV0000-%PKG_TITLE_ID%_00-PS4SDLTEMPLATE00"
set SOURCE_DIR=source
set INCLUDE_DIR=include
set BUILD_DIR=build
set PKG_DIR=build_pkg
set LIBS=-lc -lc++ -lkernel -lSceUserService -lSceVideoOut -lSceAudioOut -lScePad -lSceSysmodule -lSceFreeType -lSDL2
set MODULES=libc libSceFios2
::-----
set TARGET_NAME=%PKG_TITLE:"=%
set OUTPUT_ELF=%TARGET_NAME%.elf
set OUTPUT_OELF=%TARGET_NAME%.oelf
set SCE_SYS_DIR=%PKG_DIR%/sce_sys
set SCE_MODULE_DIR=%PKG_DIR%/sce_module
set ASSET_DIR=%PKG_DIR%/assets
::-----
:: Create %BUILD_DIR% and PKG_DIR(s)
mkdir "%BUILD_DIR%"
mkdir "%PKG_DIR%"
mkdir "%PKG_DIR%/"
mkdir "%SCE_SYS_DIR%"
mkdir "%SCE_SYS_DIR%/about"
mkdir "%SCE_MODULE_DIR%"
mkdir "%ASSET_DIR%"
mkdir "%ASSET_DIR%/audio"
mkdir "%ASSET_DIR%/fonts"
mkdir "%ASSET_DIR%/images"
mkdir "%ASSET_DIR%/misc"
mkdir "%ASSET_DIR%/videos"
:: Copy icon to sce_sys
copy icon0.png "%PKG_DIR%/sce_sys/icon0.png"
:: Copy right.sprx to about
copy "%OO_PS4_TOOLCHAIN%\\src\\modules\\right.sprx" "%PKG_DIR%/sce_sys/about/right.sprx"
:: Compile C source files.
for %%f in (%SOURCE_DIR%/*.c) do (
    clang --target=x86_64-pc-freebsd12-elf -fPIC -funwind-tables -I"%OO_PS4_TOOLCHAIN%/include" -I"%OO_PS4_TOOLCHAIN%/include/c++/v1" -I%INCLUDE_DIR% -c -o %BUILD_DIR%\%%~nf.o %SOURCE_DIR%\%%~nf.c
)
:: Compile C++ source files.
for %%f in (%SOURCE_DIR%/*.cpp) do (
    clang++ --target=x86_64-pc-freebsd12-elf -fPIC -funwind-tables -I"%OO_PS4_TOOLCHAIN%/include" -I"%OO_PS4_TOOLCHAIN%/include/c++/v1" -I%INCLUDE_DIR% -c -o %BUILD_DIR%\%%~nf.o %SOURCE_DIR%\%%~nf.cpp
)
:: Create list of object files.
set OBJECT_FILES = (
    for %%f in (%BUILD_DIR%/*.o) do set OBJECT_FILES=!OBJECT_FILES! %BUILD_DIR%/%%f
)
:: Modules. Copy from src to PKG_DIR and add to MODULE_FILES
set MODULE_FILES = (
    for %%m in (%MODULES%) do (
        set MODULE_FILES=!MODULE_FILES! sce_module/%%m.prx
        copy "%OO_PS4_TOOLCHAIN%/\\src\\modules\\%%m.prx" "%PKG_DIR%/sce_module/%%m.prx"
    )
)
:: Audio
set AUDIO_FILES = (
    for %%a in (%ASSET_DIR%/audio/*.*) do set AUDIO_FILES=!AUDIO_FILES! assets/audio/%%a
)
:: Fonts
set FONT_FILES = (
    for %%f in (%ASSET_DIR%/fonts/*.*) do set FONT_FILES=!FONT_FILES! assets/fonts/%%f
)
:: Images
set IMAGE_FILES = (
    for %%i in (%ASSET_DIR%/images/*.*) do set IMAGE_FILES=!IMAGE_FILES! assets/images/%%i
)
:: Misc
set MISC_FILES = (
    for %%m in (%ASSET_DIR%/misc/*.*) do set MISC_FILES=!MISC_FILES! assets/misc/%%m
)
:: Video
set VIDEO_FILES = (
    for %%v in (%ASSET_DIR%/videos/*.*) do set VIDEO_FILES=!VIDEO_FILES! assets/videos/%%v
)
:: Linking...
ld.lld -m elf_x86_64 -pie --script "%OO_PS4_TOOLCHAIN%\link.x" --eh-frame-hdr -o "%OUTPUT_ELF%" "-L%OO_PS4_TOOLCHAIN%/lib" %LIBS% --verbose "%OO_PS4_TOOLCHAIN%\lib\crt1.o" %OBJECT_FILES%
:: Create eboot
%OO_PS4_TOOLCHAIN%\bin\windows\create-fself.exe -in "%OUTPUT_ELF%" --out "%OUTPUT_OELF%" --eboot "%PKG_DIR%/eboot.bin" --paid 0x3800000000000011
:: Create param.sfo
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_new %SCE_SYS_DIR%/param.sfo
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry %SCE_SYS_DIR%/param.sfo APP_TYPE --type Integer --maxsize 4 --value 1
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry %SCE_SYS_DIR%/param.sfo APP_VER --type Utf8 --maxsize 8 --value %PKG_VERSION%
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry %SCE_SYS_DIR%/param.sfo ATTRIBUTE --type Integer --maxsize 4 --value 0
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry %SCE_SYS_DIR%/param.sfo CATEGORY --type Utf8 --maxsize 4 --value "gd"
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry %SCE_SYS_DIR%/param.sfo CONTENT_ID --type Utf8 --maxsize 48 --value %PKG_CONTENT_ID%
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry %SCE_SYS_DIR%/param.sfo DOWNLOAD_DATA_SIZE --type Integer --maxsize 4 --value 0
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry %SCE_SYS_DIR%/param.sfo SYSTEM_VER --type Integer --maxsize 4 --value 0
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry %SCE_SYS_DIR%/param.sfo TITLE --type Utf8 --maxsize 128 --value %PKG_TITLE%
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry %SCE_SYS_DIR%/param.sfo TITLE_ID --type Utf8 --maxsize 12 --value %PKG_TITLE_ID%
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry %SCE_SYS_DIR%/param.sfo VERSION --type Utf8 --maxsize 8 --value %PKG_VERSION%
:: GP4
%OO_PS4_TOOLCHAIN%\bin\windows\create-gp4.exe -out %PKG_DIR%/pkg.gp4 --content-id=%PKG_CONTENT_ID% --files "eboot.bin sce_sys/about/right.sprx sce_sys/param.sfo sce_sys/icon0.png %MODULE_FILES% %AUDIO_FILES% %FONT_FILES% %IMAGE_FILES% %MISC_FILES% %VIDEO_FILES%"
:: PKG
cd %PKG_DIR%
%OO_PS4_TOOLCHAIN%/bin/windows/PkgTool.Core.exe pkg_build pkg.gp4 ..
