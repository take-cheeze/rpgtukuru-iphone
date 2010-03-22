=========================================
  RPGツクール for iPhone
                            project.kuto
=========================================

== 更新履歴 ==

2010/ 3/22 : 雑多に編集。(take-cheeze)

== 構成 ==
- branches
- tags
- trunk
  - KutoEngine/    ... ゲームエンジン　OpenGLのラッパーとか色々
  - libpng_iphone/ ... libpngのiPhone用ライブラリ作成用プロジェクト
  - rpg2kdevSDK/   ... sue445さん作、takuto改良のRPGツクール2000解析SDK
  - rpgtukuru/     ... ゲーム本体　
  - font/          ... 東雲フォントから抽出したフォントデータと、それを扱うためのプログラム
  - encode/        ... iconvを用いた文字コード変換
  - Makefile       ... Windows/Linux向けのMakefile
  - psp.mak        ... PSP向けのMakefile
  - objs.mak       ... オブジェクトファイルの列挙、他のMakefileと組み合わせて使用


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
ただし、下のライブラリを必要とします。
・psptoolchain
・psplibraries
・pspgl
それと、psptoolchainは、以下のようにしているのが前提です。
※ newlibにもiconvが含まれているのですが、デフォルトだとdisableされています。
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


== ソースの説明 ==
各フォルダ内のreadme.txtを参照


== rpg2kdevSDKに関して ==
このプロジェクトで使用しているrpg2kdevSDKは、sue445さん原作のソースに、
kutoが色々付け足したり、iPhoneエンジンに合うように修正したりしたものです。
sue445さんの元ソースは下記からダウンロードできます。

http://rpg2kdev.sue445.net/


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


