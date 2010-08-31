** rpg2kLibの説明というか釈明というか弁解。 **

by take_cheeze( takechi101010 ＠ gmail.com )


- 目次
1. はじめに
2. 用語
3. 構造
4. できてないところ
5. 使っているライブラリ

1. はじめに
rpg2kLibは、RPGツクール2000/2003のデータ構造を扱うために書いたライブラリです。
わたしが未熟なために色々破綻とか支離滅裂していますのでご注意ください。

2. 用語
- BER圧縮整数 =
	小さい数字が多い場合にとても高い圧縮ができる形式。
	本来は符号無しの整数しか扱えないはずですが、int32_t型にキャストすることで負の数も扱えます。
- LDB = LcfDataBaseの略。単にデータベースとも。
- LMT = LcfMapTreeの略。単にマップツリーとも。
- LMU = LcfMapUnitの略。単にマップユニットとも。
- LSD = LcfSaveDataの略。単にセーブデータとも。

3. 構造

名前空間
- rpg2k     - ルート。未分類なものはすべてここにあります。
-- debug     - デバッグ用のクラスなど。
-- define    - LDB/LMT/LMU/LSDなどのデータ構造の記述。
-- font      - フォント関連。ほとんど用無し。
-- gamemode  - ゲームモードを扱うところ。使う予定は無いけど、ところどころ参考にするつもり。
-- input     - SDLを使って入力を抽象化する場所。用無し。
-- model     - LDB/LMT/LMU/LSDの抽象化とProjectクラス。
-- structure - データ構造の抽象化。イベント/配列/ストリーム etc.

文字列
- rpg2k下に二つ用意してあります。
- 両方ともstd::stringを継承してメンバ関数を足しただけのものです。
-- RPG2kString  - Shift_JISの文字列。toSystem()でSystemStringに変換できます。
-- SystemString - システムの文字コードの文字列。toRPG2k()でRPG2kStringに変換できます。
- 変換には、iconvをrpg2k::Encodeクラス経由で使っています。

データ型
(基本型以外は、rpg2k::structure下あります)
(Elementクラスは以下のいずれかの型を保持します)
(Descriptorクラスは型情報とデフォルト値を保持します)
- Array1D    - 一次元配列。Elementの連想配列。
- Array2D    - 二次元配列。Array1Dの連想配列。
- Music      - BGMの再生情報（単なるArray1Dのwrap）
- Sound      - SEの再生情報（単なるArray1Dのwrap）
- EventState - イベントの位置情報など（単なるArray1Dのwrap）
- Event      - イベントデータ。イベントコマンド（= Instruction)の配列。
- BerEnum    - LMTにだけあるナゾのBER圧縮整数の列挙。
- Binary     - 実体はstd::vector<uint8_t>を継承したクラス。どんなデータでも載せられます。
- int        - 整数型。
- bool       - boolean型。
- string     - 文字列。RPG2kStringをtypedefしたもの。
- double     - 浮動小数点数。今のところピクチャでしか使わない。

ストリーム
- 書き込みストリームStreamWriterと読み込みストリームStreamReaderの二つです。
- 双方ともpimplで実装しています。
- Binaryクラスとstdioを用いた方法を選べます。
- BER圧縮整数が扱えること、LDB/LMT/LMU/LSDを扱うのに都合が良い点を除いて色々劣っています。

解析
- デバッグビルドの場合、rpg2k::structure::Elementクラスのデコンストラクタで行われます。
- データの読み込みを速くするために、Array1Dで大きいデータをスキップする機能が働いている場合、正常に行われません。
- 結果は、"analyze.txt"というファイルに出力されます。
- Array1D/Array2Dの判定は行いません。（面倒なのとバイト列を確認したほうがはやそうだから）
- 解析は、主にrpg2k::debugを用いています。

Project
- ゲームデータ(LDB/LMT/LMU/LSD)を一通り管理するクラス。
- セーブデータ(1 - 15番まで)はコンストラクタで全て読み込まれます。（ファイルが存在する場合のみ）

4. できてないところ
- "// TODO"を検索してみてください。
- 壊れたLMTの判定。
- Boost.Serializationによる設定ファイル。

5. 使っているライブラリ

OpenGL - 描画
iconv  - 文字コード変換
boost  - スマートポインタ

下のライブラリは、もう使わないつもり
SDL       - スリープ、タイマ、入力、描画
SDL_mixer - BGM/SEの再生
SOIL      - (= Simple OpenGL Image Library) 画像の読み込み、テクスチャの作成
