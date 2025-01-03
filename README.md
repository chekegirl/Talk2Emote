# Talk2Emote

## 利用規約

MITライセンスです。

## 利用方法

リリースのページからダウンロードした実行形式ファイルを用いてください。

もし自分でプログラムを改造して利用する場合は、頑張ってください。各dllについて、基本的にそのdllだけを変更して差し替えて実行できるように作ってあります。

## ソースコードからの使用方法

### 必要なものの導入、フォルダ構成

まず前提として、MeCabをインストールしてある必要があります。

以下の構成にします。

```
./Talk2Emote/
├── lua5_1_4_Win32_dll8_lib/
│   ├── include/
│   │   ├── lauxlib.h
│   │   ├── lua.h
│   │   ├── lua.hpp
│   │   ├── luaconf.h
│   │   └── lualib.h
│   ├── lua5.1.dll
│   ├── lua5.1.lib
│   ├── lua51.dll
│   └── lua51.lib
├── script/
│   ├── @Talk2Emote効果.anm
│   └── Talk2Emote.anm
└── Talk2Emote_for_Aviutl_PSDToolKit.cpp

C:/
└── Talk2Emote/
    ├── dictionary/
    │   └── FaceDictionary.t2edict
    ├── face_predict/
    │   ├── gimon.cpp
    │   ├── ikari.cpp
    │   ├── negaposi.cpp
    │   ├── odoroki.cpp
    │   └── priority.txt
    ├── ban_list.cpp
    ├── dictionary_memory.cpp
    ├── diff_type.h
    ├── face_predict.cpp
    ├── libmecab.dll
    ├── libmecab.lib
    ├── mecab.h
    └── mecab_funcs.c
```

これについて、

```
│   ├── libmecab.dll
│   ├── libmecab.lib
│   ├── mecab.h
```

ここはMeCabからコピーしてくる必要があります。また、

```
├── lua5_1_4_Win32_dll8_lib/
│   ├── include/
│   │   ├── lauxlib.h
│   │   ├── lua.h
│   │   ├── lua.hpp
│   │   ├── luaconf.h
│   │   └── lualib.h
│   ├── lua5.1.dll
│   ├── lua5.1.lib
│   ├── lua51.dll
│   └── lua51.lib
```

ここはLuaのSDKをダウンロードしてくる必要があります。

## 実行形式への変換

| ファイル | ビルド方法 |
| ---- | ---- |
| mecab_funcs.c | gcc -DDLL_IMPORT -I. mecab_funcs.c -shared -o mecab_funcs.dll C:\Talk2Emote\libmecab.dll ※Cドライブ直下で行う必要があることに注意 |
| Talk2Emote_for_Aviutl_PSDToolKit.cpp | gcc -I lua5_1_4_Win32_dll8_lib/include -L lua5_1_4_Win32_dll8_lib -shared -o Talk2Emote_for_Aviutl_PSDToolKit.dll Talk2Emote_for_Aviutl_PSDToolKit.cpp -llua51 -lstdc++ |
| それ以外 | g++ -shared -o dictionary_memory.dll dictionary_memory.cpp -pedantic 等 |

なお、以下のバージョンのGCCを用いています。ちょっと古いですね。

```
gcc.exe (MinGW.org GCC-6.3.0-1) 6.3.0
Copyright (C) 2016 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

## 辞書ファイルについて

PSDファイルに依存する辞書ファイルはPSDファイルと同じフォルダに、
 - [PSDファイル名].str2ptkl.t2edict
 - [PSDファイル名].face2ptkl.t2edict
のファイル名で存在します。

（ということは、PSDファイルの置き場所は、読み書き両方の権限が付与されている必要があります。）

PSDファイルに依存しない辞書ファイルは
 - C:\Talk2Emote\dictionary\FaceDictionary.t2edict
 - C:\Talk2Emote\dictionary\Str2FaceDictionary.t2edict
の２つです。

なお、立ち絵の設定はこれらの辞書ファイルに依存するので、辞書ファイルが更新されたら同じ立ち絵を使う全てのプロジェクトに影響が及びます。

よって、編集し終えたプロジェクトファイルは辞書ファイルと一緒に保存しておいた方がいいです（この辺もなんかいい方法考えたいね。）

## 今後について
 - より自由度の高いレイヤ構成に対してTalk2Emoteを適用できるようにしたい
 - 表情推定（特にネガポジ判定）機能を強化したい
 - 導入を簡単にしたい
