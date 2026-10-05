# ViconSample

Vicon Shogun Live の DataStream から Transform データを受信してコンソールに表示する確認用サンプルです。

クライアント環境に合わせて **32bit (Win32) / DataStream SDK 1.11** でビルドします。
（リポジトリ同梱の `include/Vicon`・`lib`・`Debug`/`Release` の一式を使用）

- Subject (Skeleton) の Segment : グローバル位置 [mm] / オイラー角 [deg]
- Marker (Labeled / Unlabeled)  : グローバル位置 [mm]
- `--ez` を付けると `ezTracker_Vicon` 経由で読んだ値も表示

## ビルド

`ViconSample.sln` を Visual Studio で開いて **Win32** でビルドします。
コマンドラインの場合:

```
msbuild ViconSample.sln /p:Configuration=Release /p:Platform=Win32
```

ビルド後、`bin\<Configuration>\` に `ViconDataStreamSDK_CPP.dll` と
`boost_*-vc140-mt-x32-1_68.dll` が自動コピーされます。

## 実行

```
bin\Release\ViconSample.exe [host:port] [--seconds N] [--rate HZ]
                            [--all-segments] [--markers] [--ez]
                            [--push | --prefetch]
```

| 引数 | 説明 |
| --- | --- |
| `host:port` | 接続先 (既定: `127.0.0.1:801`) |
| `--seconds N` | 表示時間 [秒] (既定: 10) |
| `--rate HZ` | 表示更新レート [Hz] (既定: 30) |
| `--all-segments` | ルート以外の全セグメントも表示 |
| `--markers` | Labeled / Unlabeled マーカー位置も表示 |
| `--ez` | `ezTracker_Vicon` 経由の値も表示 |
| `--push` | StreamMode を ServerPush にする (既定: ClientPull) |
| `--prefetch` | StreamMode を ClientPullPreFetch にする |

### 例

```
bin\Release\ViconSample.exe
bin\Release\ViconSample.exe --markers --all-segments --seconds 30
bin\Release\ViconSample.exe 192.168.0.10:801 --ez
```

## 動作確認の見方

正常にフレームが届いていれば、次のように表示されます。

```
---- frame 1234 : 100.0 Hz  subjects=1 unlabeledMarkers=53 ----
  Subject[0] Hoge  (segments=24, root=Hips)
    root Hips T=(  1234.56, -234.56,  987.65)mm  R=(  12.34,  -5.67,  89.01)deg  occ=0
  UMarker[  0] (  1234.56, -234.56,  987.65)mm
```

`GetFrame failed: NoFrame ...` と出続ける場合は、クライアント側ではなく
Shogun Live 側がフレームを配信していません。次を確認してください。

1. Shogun Live が Live 状態 (再生中) になっているか
2. Shogun Live の設定で DataStream の配信が有効になっているか
3. ポート番号が合っているか (既定 801)

## 座標系

`ezTracker_Vicon` に合わせて Z-up (Forward, Left, Up) に設定しています。
Vicon のグローバル位置は mm、`ezTracker_Vicon` は m に変換して保持します。

## 注意 (実装メモ)

- `ezTracker_Vicon::read()` は Subject ごとに最後の Segment の値で
  トラックデータを上書きする実装です。ルートセグメントの値が必要な場合は
  `ezTrack_Vicon.cpp` の修正を検討してください。
- `ezTracker_Vicon` の `name` は `char[16]` のため、16 文字以上の Subject 名は
  切り詰め／オーバーフローします。
- セグメントが遮蔽されているフレーム (`occ=1`) では Vicon が回転行列を
  すべて 0 で返すため、`getRot()` の roll が `-nan(ind)` になります。
- DSDK 1.12 向けの変更（`include/Vicon` の 1.12 ヘッダ化と x64 ビルド）は
  stash `dsdk1.12対応: include/Vicon 1.12ヘッダ更新(未採用)` に退避してあります。
