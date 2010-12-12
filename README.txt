=========================================
  RPGツクール for iPhone

brought you by
- Project.Kuto http://projectkuto.web.fc2.com/
- take_cheeze  http://twitter.com/take_cheeze
- Nanashia     http://twitter.com/Nanashia
- ◆RGSS..WS4M  http://twitter.com/rgssws4m
=========================================

== 目次 ==

- 操作 -
-- キーボード
-- マウス
-- PSP
- 構成
- ビルド方法
-- libpng_iphone
-- デバッグビルドの特典
- rpgtukuru本体のビルド
- ソースの説明
-- kuto_memory.cpp
- rpg2kdevSDKに関して
- 参考文献
- 動作確認
- 更新履歴


== 操作 ==

=== キーボード ===
↑ :
→ :
↓ :
← :
Esc : スタートボタン
A : 決定/エンター/調べる
B : 取り消し/キャンセル/メニュー
X : デバッグ系
Y : メモリ情報(使用状況)の出力

=== マウス ===
----------------------------
|Screen                    |
|                          |
|                          |
|                          |
|                          |
----------------------------
|                          |
|    ↑              Ｘ    |
|  ←  →          Ｙ  Ａ  |
|    ↓              Ｂ    |
|          START           |
----------------------------
(レイアウトが崩れてるのは気にしないでください)
(ひょっとして、GameBoy?)

=== PSP ===
↑ :
→ :
↓ :
← :
START : スタートボタン
○ : 決定/エンター/調べる
× : 取り消し/キャンセル/メニュー
△ : デバッグ系
□ : メモリ情報(使用状況)の出力


== 構成 ==
- branches
  - rpg2kLib_trunk/ ... rpg2kemuのソースと統合したもの
- tags
- trunk
  - KutoEngine/    ... ゲームエンジン　OpenGLのラッパーとか色々
  - libpng_iphone/ ... libpngのiPhone用ライブラリ作成用プロジェクト
  - rpg2kdevSDK/   ... sue445さん作、takuto改良のRPGツクール2000解析SDK
  - rpg2kLib/      ... RPGツクール2kのデータ構造を扱うためのライブラリ
  - rpgtukuru/     ... ゲーム本体　
  - malloc.c       ... dlmalloc ( http://g.oswego.edu/dl/html/malloc.html )
  - Makefile       ... Linux向けのMakefile
  - psp.mak        ... PSP向けのMakefile
  - windows.mak    ... Windows/Linux向けのMakefile(最近は、動作確認をしていないので色々不明)
  - objs.mak       ... オブジェクトファイルの列挙、他のMakefileと組み合わせて使用
  - README.txt     ... このファイル


== ビルド方法 ==

=== libpng_iphone ===
ビルドにはlibpng、zlibが必要です。
下記サイトよりソースコードをダウンロードして下さい。

libpng
http://www.libpng.org/pub/png/libpng.html

zlib
http://www.zlib.net/

kutoはlibpng-1.2.37、zlib-1.2.3でビルド確認しています。
これをlibpng_iphoneのフォルダに入れ、下記のようなフォルダ構成にしてください。

- libpng_iphone
  - libpng
  - libpng_iphone.xcodeproj
  - zlib

アクティブSDK、アクティブな構成、アクティブターゲットがありますが、
すべての組み合わせをビルドする必要があります。
（アクティブターゲットをBuild Allにするのが良いかと）

=== rpgtukuru本体のビルド ===
libpng_iphoneのビルド後にビルドしてください。
コード署名は自分の環境に合わせて適当に変えてください。

非iPhoneでは、trunk下のMakefileを使ってください。
かなりいい加減なので、動かなかった場合は、適切に編集してください。
また、行った変更をこちらにフィードバックしていただけると、助かります。

それから、trunk下で"make -f psp.mak"すると、PSP向けにクロスコンパイル出来ます。
take-cheezeは、実機を持っていないので動作確認はしていません。
ただし、下のライブラリを必要とします。
・psptoolchain
・psplibraries
それと、psptoolchainは、以下のようにしているのが前提です。
※ newlibにもiconvが含まれているのですが、enableされてません。
take-cheezeは、以下のパッチをあてました。

Index: scripts/004-newlib-1.16.0.sh
===================================================================
--- scripts/004-newlib-1.16.0.sh	(リビジョン 2493)
+++ scripts/004-newlib-1.16.0.sh	(作業コピー)
@@ -20,7 +20,7 @@
  cd build-psp
 
  ## Configure the build.
- ../configure --prefix="$PSPDEV" --target="psp"
+ ../configure --prefix="$PSPDEV" --target="psp" --enable-newlib-iconv
 
  ## Compile and install.
  make clean

=== デバッグビルドの特典 ===
データ構造をすべて展開するので遅いです。
データ構造を破棄するときに、解析もするので終了も遅いです。
色んな値チェックをしているので遅いです。
バッテリに優しくないと想います。
解析結果は、"analyze.txt"に出力されます。※ そこそこ大きファイルになるので注意してください。


== ソースの説明 ==
タブ幅は、4です。
その他、詳細は各フォルダ内のreadmeなどを参照してください。


=== kuto_memory.cpp ===
PSP/Windows以外では、new/deleteで以下のmallocを使っています。
http://gee.cs.oswego.edu/dl/html/malloc.html
Unix系のOS(Mac OS X/iPhone/Linux)なら、問題なく使えるはずです。


== rpg2kdevSDKに関して ==
このプロジェクトで使用しているrpg2kdevSDKは、sue445さん原作のソースに、
kutoが色々付け足したり、iPhoneエンジンに合うように修正したりしたものです。
sue445さんの元ソースは下記からダウンロードできます。

http://rpg2kdev.sue445.net/


== 動作確認 ==
今のところ以下のページにある『夜明けの口笛吹き』を想定してます。
http://tukusure.hp.infoseek.co.jp/
上のページにある"yoake.zip"をtrunkに展開して、
"夜明けの口笛吹き"のディレクトリ名を"yoake"に変更してください。

*動作確認に使わせてくださる作品を募集中です。*


== 参考文献 ==
解析には下記サイトを参考にさせていただきました。

RPGツクール2000/2003/XP 解析データまとめページ
http://rpg2kdev.sue445.net/

PSPRPGツクールエミュまとめ 
http://www7.atwiki.jp/rpg2kpsp/

これらサイトを運営しているsue445さん、take-cheezeさんに感謝！
※ 『PSPRPGツクールエミュまとめ』の管理・運営に、take-cheezeは関係ありません。
take-cheezeは、編集をしているだけです。
※ 上記の事情により　PSPRPGツクールエミュまとめ』には突然消える可能性があります。
take-cheezeは、『PSPRPGツクールエミュまとめ』からsue445さんのwikiに移行することを推奨します。


また、ほぼFrameworkの作成に下記サイトを参考にしました。

http://d.hatena.ne.jp/paella/20090219/1235037185

paellaさんに感謝！

更新情報などが
http://twitter.com/#!/take_cheeze/rpg2k-creator
で、流れているコトがあるかもしれません。

== 更新履歴 ==
2010/12/13 : リンクとかの追加。(take-cheeze)
2010/ 8/ 9 : 『操作』を追加。その他ところどころ加筆。(take-cheeze)
2010/ 7/29 : 『動作確認』/"kuto_memory.cpp"を追加。(take-cheeze)
2010/ 6/ 8 : ソースコードについて。(take-cheeze)
2010/ 3/22 : 雑多に編集。(take-cheeze)
