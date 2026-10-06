# ソースの文字コード変換

`XR_FrameV2025_1125` のソースは Shift-JIS (CP932) で管理している。開発中は
UTF-8 (BOM 付き) に変換して Visual Studio での文字化けを避け、クライアントへ
渡す前に元の文字コードへ戻すためのスクリプト。

## 使い方

リポジトリ直下で実行する。

```
python tools/encoding_convert/convert_encoding.py to-utf8
python tools/encoding_convert/convert_encoding.py to-sjis
```

## 動き

`to-utf8` は CP932 と UTF-16 のファイルを UTF-8 (BOM 付き) に変換する。
Visual Studio は BOM で UTF-8 と判定するので、プロジェクト設定を変える必要は
ない。変換前の文字コードは `.encoding-manifest.json` に記録する。

`to-sjis` はその記録を見て、ファイルごとに元の文字コードへ戻す。元から UTF-8
だったファイルは UTF-8 のまま、CP932 だったファイルは CP932 に戻る。記録の無い
新しいファイルは CP932 にする。CP932 で表せない文字を含むファイルは UTF-8 の
まま残し、警告を出す。

ASCII だけのファイルはどちらのコマンドでも書き換えない。

## オプション

- `--dry-run` 書き込まずに変更内容だけを表示する
- `--verbose` 変更しないファイルも表示する
- `--force` `to-sjis` でマニフェストを無視し、すべて CP932 に変換する
- `--no-bom` `to-utf8` で BOM を付けない。Visual Studio 側に
  `/source-charset:utf-8` の指定が必要になる
- `--root DIR` 対象ディレクトリを指定する

## クライアントへ渡すとき

`.encoding-manifest.json` は開発機に残すための記録なので、渡す物に含めない。
このツール一式 (`tools/encoding_convert`) も `XR_FrameV2025_1125` の外に置いて
あるため、通常の納品物には入らない。
