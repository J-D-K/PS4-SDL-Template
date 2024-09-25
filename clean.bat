:: This isn't really a good cleaning script.
:: I don't wipe the build_pkg folder because it contains assets.
@echo off
rm -r build_pkg
rm -r build
rm *.elf
rm *.oelf
rm *.pkg
