# ViconSample

Vicon Shogun Live の DataStream から Transform データを受信するサンプルです。
2 つのプロジェクトが入っています。

- **ViconSample** : コンソールで Subject / Segment / マーカーを表示する確認用ツール
- **BoneView** : 受信中の Subject の骨格を GLUT で可視化する学生向けサンプル

クライアント環境に合わせて **32bit (Win32) / DataStream SDK 1.11** でビルドします。
（リポジトリ同梱の `include/Vicon`・`lib`・`Debug`/`Release` の一式を使用）

- Subject (Skeleton) の Segment : グローバル位置 [mm] / オイラー角 [deg]
- Marker (Labeled / Unlabeled)  : グローバル位置 [mm]
- `--ez` を付けると `ezTracker_Vicon` 経由で読んだ値も表示
- `--bones` を付けると Subject の全骨 (Segment) を `ezTracker_Vicon::getSubject()` 経由で表示

## ビルド

`ViconSample.sln` を Visual Studio で開いて **Win32** でビルドします。
コマンドラインの場合:

```
msbuild ViconSample.sln /p:Configuration=Release /p:Platform=Win32
```

ビルド後、以下の DLL が実行ファイルの隣に自動コピーされます。

- `bin\<Configuration>\` (ViconSample)
- `bin\BoneView\<Configuration>\` (BoneView)

コピーされる DLL: `ViconDataStreamSDK_CPP.dll`、`boost_*-vc140-mt-x32-1_68.dll`、
`freeglut.dll`、`glut32.dll`

## 実行

```
bin\Release\ViconSample.exe [host:port] [--seconds N] [--rate HZ]
                            [--all-segments] [--markers] [--ez] [--bones]
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
| `--bones` | Subject の全骨 (Segment) を `getSubject()` 経由で表示 (`--ez` を兼ねる) |
| `--push` | StreamMode を ServerPush にする (既定: ClientPull) |
| `--prefetch` | StreamMode を ClientPullPreFetch にする |

### 例

```
bin\Release\ViconSample.exe
bin\Release\ViconSample.exe --markers --all-segments --seconds 30
bin\Release\ViconSample.exe --bones --seconds 3
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

- Vicon SDK: `SetAxisMapping(Forward, Left, Up)` で Vicon データを Z-up で受け取る
- `ezTracker_Vicon`: `x = -Left`, `y = Up`, `z = -Forward` に変換して `ezTrackDataT` に格納
  → VR_Project と同じ **Y-up** (x=右, y=上, z=後ろ)・単位 m
  - 例: 人が Vicon の X- 方向に歩くと `ezTrackDataT` では +Z 方向になる
- BoneView も Y-up で描画 (床グリッドは XZ 平面、カメラの up は +Y)

## 骨データの利用

```cpp
ezTracker_Vicon vicon(true);
vicon.open("127.0.0.1:801", false);
...
vicon.read();
ezTracker* human = vicon.getSubject("Subject1");   // 1 Subject = 1 ezTracker
for (int i = 0; i < 73; ++i) {
    ezTrackDataT* bone = human->getTrackData(i);   // bone->parent (-1 = root)
}
```

- 従来の `vicon.getTrackData("CAP")` などの Subject 単位の使い方は変更なし
- 遮蔽フレームでは前回値を保持（NaN 回避）

## BoneView (骨格の可視化)

受信中の Subject の骨格を描画します。描画は VR_Project と同じ
GLUT + `Shapes.cpp` の図形 (`ezSolidSphere` / `ezSolidCylinder`) を使い、
`src/sim.cpp` の `copyTrackToObj()` と `src/calc.cpp` の `applyObjTransform()`
と同じ流れで `ezTrackDataT` を描画しています。

```
bin\BoneView\Release\BoneView.exe [host:port] [--subject NAME]
```

| 引数 | 説明 |
| --- | --- |
| `host:port` | 接続先 (既定: `127.0.0.1:801`) |
| `--subject NAME` | 表示する Subject 名 (既定: 骨格を自動選択) |

- `q` / `ESC` : 終了
- 矢印キー : 視点回転
- `+` / `-` : ズーム

骨格データの流れ:

```cpp
vicon.read();                                  // 1周期に1回 (全Subjectが同一フレーム)
ezTracker* human = vicon.getSubject("Subject1"); // 1 Subject = 1 ezTracker
for (int i = 0; i < _n_tracks; ++i) {
    ezTrackDataT* bone = human->getTrackData(i); // 1 骨 = 1 ezTrackDataT
    if (bone->id == -1) continue;
    // bone->parent (-1 = root) で親子をたどれる
    // bone->x, y, z [m] / roll, pitch, yaw [deg]
}
```

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
