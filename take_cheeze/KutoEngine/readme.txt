=========================================
  KutoEngine
                            project.kuto
=========================================

== 概要 ==
iPhone用のゲームエンジン。
グラフィックスや実行ルーチン、ロードシステムなどを扱う。


== 基本 ==
* ファイル名はすべてkuto_から始まり、小文字で構成される。
* 一部マクロを除き、namespace kutoに囲まれている。
* クラス名は大文字から始める。
* 関数名は小文字から始める。
* メンバ変数は最後に'_'をつける。


== 主なソースの説明 ==
=== グラフィックス関係 ===
* kuto_eagl_view
  OpenGLのUIViewクラス。
* kuto_graphics_device
  OpenGLの管理。基本的にはgl〜関数を使用せず、すべてこのクラスを通してステートなどを設定する。
  ステートのキャッシングなどをしており、無駄なステート変更をしないのでパフォーマンスが上がる。
* kuto_graphics2d
  2D描画用のクラス。基本的な2D描画はこのクラスの関数を使用すればOK。
* kuro_irender
  render用インターフェース。画面に描画したい場合はこのクラスを継承し、RenderManagerに登録する。
  プライオリティでソートされた順にrender()が呼ばれる。
* kuto_render_manager
  描画管理クラス。レイヤーごとに登録されたIRenderをソート、描画を実行する。
* kuto_texture
  テクスチャクラス。すべての画像はこのクラスで管理する。
* kuto_font
  フォント描画クラス。自前でフォントテクスチャをもっていないので、
  新しい文字が出る度にOSのフォントで必要な文字をビットマップに変換し、
  GLテクスチャを生成している。
  そのため、初出の文字を描画するときちょっと重い。

=== デバッグメニュー ===
* kuto_debug_menu_view
  最初のデバッグメニューUIView
* kuto_section_handle
  デバッグメニュー用セクション。
  これをSectionManagerに登録しておくと、DebugMenuから呼び出せる。
* kuto_section_manager
  セクション管理。ここに登録しておくことでDebugMenuからの呼び出しや、Section変更などができる。

=== ロード ===
* kuto_load_manager
  ロード管理。キャッシュシステムを搭載しており、同じファイルを二重に読み込まないようにできる。
* kuto_load_handle
  ロードハンドル。
  読み込み、解放をすべてこのハンドルを通すことで、キャッシュを有効活用することができる。
  クラスのメンバとして持てば、デストラクタで必ずファイルも解放されるので安心。
  LoadBinaryやLoadTextureが継承している。
* kuto_png_loader
  PNG専用ローダー。libpng使用。
  抜き色あり、hsvで色変化などの機能を備える。
* kuto_image_loader
  png以外のイメージ（JPEGとか）の読み込み用。OS標準の機能を使用。
* kuto_file
  単純なファイル/ディレクトリアクセス。ファイルの存在チェックなどに。

=== タッチ処理 ===
* kuto_touch_pad
  タッチ処理管理。どの部分がタッチされたか、スクロールされたかなど、毎フレームチェックする。
* kuto_virtual_pad
  バーチャルパッド。画面下に出てるスーファミライクなあれ。

=== タスク ===
* kuto_task
  タスクシステム。
  継承先でinitialize、update、drawをオーバーライドすると、これら関数が毎フレーム呼ばれる。
  親子関係を持っており、親がreleaseされると子もrelease、親がpauseすると子もpauseする。


== FAQ ==
Q. なんでObjective-CでなくC++なの？
A. OpenGLにはC++の方が合ってる感じ。あとパフォーマンス的にも。

Q. GraphicsDevice通してGLにアクセスする方がほんとにいいの？
A. 試せばわかるが、恐ろしくパフォーマンスが違う。

Q. png専用のロード処理があるのはなぜ？
A. OS標準だと抜き色とか指定できなかったから。

Q. Taskはなんでupdateとdrawが分かれてるの？
A. updateだけpauseして描画はしたいときがあったから。

Q. TaskのdrawでRenderManagerに登録とか遠回りせずに、Taskのdraw=renderでよくね？
A. Taskの実行順とrenderの順序を変えたかったから。

Q. boost使わないの？
A. iPhoneアプリに使うにはちょい大げさかなと思い、必要なものだけ自作。

Q. てかコメント少なくね？
A. すいません。


