all:
	@ meson compile -C build && ./build/myst

meson:
	@ CC=/usr/bin/clang CXX=/usr/bin/clang++ meson setup build
