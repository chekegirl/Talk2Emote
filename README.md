# Talk2Emote

## 実行形式ファイルに変換する方法

mecab_funcs.c | gcc -DDLL_IMPORT -I. mecab_funcs.c -shared -o mecab_funcs.dll C:\Talk2Emote\libmecab.dll ※Cドライブ直下で行う必要があることに注意
Talk2Emote_for_Aviutl_PSDToolKit.cpp | gcc -I lua5_1_4_Win32_dll8_lib/include -L lua5_1_4_Win32_dll8_lib -shared -o Talk2Emote_for_Aviutl_PSDToolKit.dll Talk2Emote_for_Aviutl_PSDToolKit.cpp -llua51 -lstdc++
それ以外 | g++ -shared -o dictionary_memory.dll dictionary_memory.cpp -pedantic 等

なお、以下のバージョンのGCCを用いた。

gcc.exe (MinGW.org GCC-6.3.0-1) 6.3.0

Copyright (C) 2016 Free Software Foundation, Inc.

This is free software; see the source for copying conditions.  There is NO

warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
