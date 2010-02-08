=========================================
  RPGツクール for iPhone
                            project.kuto
=========================================

== ソースツリー ==
- branches
- tags
- trunk
  - KutoEngine     ... ゲームエンジン　OpenGLのラッパーとか色々
  - libpng_iphone  ... libpngのiPhone用ライブラリ作成用プロジェクト
  - rpg2kdevSDK    ... sue445さん作、takuto改良のRPGツクール2000解析SDK
  - rpgtukuru      ... ゲーム本体　


== libpng_iphone ==
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


== rpgtukuru本体のビルド ==
libpng_iphoneのビルド後にビルドしてください。
コード署名は自分の環境に合わせて適当に変えてください。


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


また、ほぼFrameworkの作成に下記サイトを参考にしました。

http://d.hatena.ne.jp/paella/20090219/1235037185

paellaさんに感謝！


