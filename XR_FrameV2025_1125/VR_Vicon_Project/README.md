# VR_Vicon_Project

Vicon Shogun Live の DataStream からボディトラッキング（骨格）を受信し、
VR_Frame のキャラクタ（腰・胸・頭・手・足）へ反映するクライアント向けサンプルです。

`XR_FrameV2025_1125/VR_Project` をコピーして作られています。
Vicon の組み込み方は **`sim.cpp` を読めば分かる**ように書いてあります。

## このサンプルで示していること

- `ezTracker_Vicon` を使った Vicon PC（Shogun Live）への接続
- Subject（骨格）の一覧取得と、骨（Segment）の名前による取り出し
- 骨の位置・姿勢を `ObjDataT`（腰・胸・頭・手・足）へ反映
- 受信した全骨を線で結んだ骨格の可視化（トラッキング確認用）
- Vicon に接続できないときのフォールバック（マウス操作）

## VR_Project との違い

| ファイル | 内容 |
| --- | --- |
| `sim.cpp` | サンプルの本体。Vicon の接続・骨の対応付け・可視化をすべてここに記述 |
| `draw.cpp` | `VR_Project` のコピー。`DrawScene()` / `PostDraw()` に `DrawTrackedSkeleton()` と `DrawTrackingInfo()` の呼び出しを追加 |
| `config.h` | `use_tracker = true` に変更（それ以外は `VR_Project` と同じ） |
| その他のファイル | `VR_Project` と同じコピー |

共有ソース（`..\src`、`..\include`、`..\lib_ObjLoader`）は `VR_Project` と共通です。
`sim.cpp` と `draw.cpp` だけをこのフォルダ内にコピーしてビルドするため、
サンプルを変更しても `VR_Project` には影響しません。

## ビルド

`XR_Frame.sln` を Visual Studio で開き、`VR_Vicon_Project` をビルドします。
構成は `Debug` / `Release` × `Win32` / `x64` の4つです。

コマンドラインの場合:

```
msbuild XR_Frame.sln /t:VR_Vicon_Project /p:Configuration=Release /p:Platform=Win32
msbuild XR_Frame.sln /t:VR_Vicon_Project /p:Configuration=Release /p:Platform=x64
```

出力先（`VR_Project` と同じ場所に出ます）:

| 構成 | 実行ファイル |
| --- | --- |
| `Release` / `Win32` | `XR_FrameV2025_1125\Release\VR_Vicon_Project.exe` |
| `Debug` / `Win32` | `XR_FrameV2025_1125\Debug\VR_Vicon_Project.exe` |
| `Release` / `x64` | `XR_FrameV2025_1125\x64\Release\VR_Vicon_Project.exe` |
| `Debug` / `x64` | `XR_FrameV2025_1125\x64\Debug\VR_Vicon_Project_64.exe` |

実行ファイルと同じフォルダに Vicon の DLL が必要です。
リポジトリ同梱の DLL（`ViconDataStreamSDK_CPP.dll` と `boost_*.dll`）は
上記の出力先にすでに置かれています。

## 実行前の設定

### 1. `config.h`

```cpp
const bool use_tracker = true; //◆トラッカーフラグ
const bool use_vicon   = true; //◆true:VICON, false:ARマーカー
```

### 2. `sim.cpp` の先頭

```cpp
// Vicon PC(Shogun Live)の IPアドレス:ポート番号
static char VICON_HOST[] = "172.23.85.186:801";

// 追跡する Subject 名（Shogun Live の Objects ペインに表示される名前）
static const char* VICON_SUBJECT_NAME = "Hoge";
```

- `VICON_HOST` はクライアント環境の Vicon PC に合わせて変更します。
- `VICON_SUBJECT_NAME` は Shogun Live の Objects ペインに表示される
  Subject 名に合わせます。空文字 `""` にすると最初の Subject を使います。

### 3. Shogun Live 側

1. Shogun Live を **Live 状態**（再生中）にする
2. DataStream の配信を有効にする
3. ポート番号（既定 801）を `VICON_HOST` と合わせる

## 起動と確認

起動するとコンソールに Subject と骨の一覧が表示されます。
画面には受信した骨格（緑の線と黄色い関節）と、左上に接続状態が表示されます。
カメラは頭に追従しますが、`C` キーで固定カメラに切り替えると骨格全体を
確認できます（もう一度押すと頭カメラに戻ります）。

```
---- Vicon Subjects (1) ----
  Subject[0] "Hoge" : 24 bones
    [ 0] Hips                     parent=-1
    [ 1] Spine                    parent= 0
    [ 2] Spine1                   parent= 1
    ...
-----------------------------
[Vicon] 骨の対応付け: hips=Hips head=Head body=Spine2 handR=RightHand handL=LeftHand footR=RightFoot footL=LeftFoot
```

骨の値は2秒ごとにコンソールへ表示されます。
位置は [m]、姿勢は [deg] です。

```
---- Vicon bones : subject=Hoge (24 bones) ----
  [ 0] Hips                     parent=-1 pos=(  0.000,  0.950,  0.000)m rot=(   0.0,   0.0,   0.0)deg
  [ 1] Spine                    parent= 0 pos=(  0.000,  1.050,  0.000)m rot=(   0.0,   0.0,   0.0)deg
```

## コードの読み方（組み込み手順）

`sim.cpp` は次の順に読むと分かりやすいように書いてあります。

### 1. 接続（`InitScene`）

```cpp
vicon = new ezTracker_Vicon(true);
if (vicon->open(VICON_HOST, false)) { ... }
```

`ezTracker_Vicon::open()` の引数は
「Vicon PC の IPアドレス:ポート番号」です。
接続後、`read()` を1回呼んで最初のフレームを取り込みます。

### 2. 毎フレームの受信（`UpdateScene` → `updateVicon`）

```cpp
vicon->read(); // 1フレームに1回だけ呼ぶ
```

`read()` は DataStream から最新フレームを取り込み、
Subject ごとの骨格データを内部に展開します。

### 3. Subject（骨格）の取得

```cpp
ezTracker* body = vicon->getSubject("Hoge"); // Subject 名で取得
```

`ezTracker_Vicon` は **1 Subject = 1 ezTracker** で骨格を保持しています。
Subject は `getSubjectCount()` / `getSubjectName(i)` で一覧できます。

### 4. 骨の取り出しと反映（`applyViconPose`）

```cpp
ezTrackDataT* head = body->getTrackData("Head"); // 骨の名前で取得
copyTrackToObj(head, &simdata.head);             // ObjDataT へコピー
```

骨の名前は Shogun Live の Skeleton 設定によって異なります。
`kHipsNames` などの候補配列に、実際に配信されている名前を
優先順に並べてください。候補が見つからない場合は `(未検出)` と表示されます。

| 変数 | 対応する骨（例） |
| --- | --- |
| `kHipsNames` | `Hips` / `Hip` / `Pelvis` / `Root` |
| `kHeadNames` | `Head` / `HeadTop_End` / `Neck` |
| `kChestNames` | `Spine2` / `Chest` / `Spine3` / `Spine1` / `Spine` |
| `kHandRNames` | `RightHand` / `RightWrist` / `RightHandIndex1` |
| `kHandLNames` | `LeftHand` / `LeftWrist` / `LeftHandIndex1` |
| `kFootRNames` | `RightFoot` / `RightToeBase` / `RightToe` |
| `kFootLNames` | `LeftFoot` / `LeftToeBase` / `LeftToe` |

### 5. 座標系と親子関係の注意

`ezTracker_Vicon` は Vicon の Z-up 座標を VR_Frame の Y-up 座標
（x:右, y:上, z:奥）へ変換し、単位を [mm] から [m] にして格納します。

格納される値は **ワールド座標** です。Vicon 接続中に手・頭をプレイヤの
子座標系にすると親の位置・姿勢が二重に適用されるため、サンプルでは
Vicon のデータが届いた時点で `applyViconPose()` 内で `setObjWorld()` を
呼んで子座標系を解除します（マウス操作デモのときだけ `setObjLocal()`
を使います）。

### 6. 全骨の可視化（`cacheSkeleton` / `DrawTrackedSkeleton`）

全骨を `skeleton[]` にコピーし、`parent` を使って親子を線で結びます。
`DrawTrackedSkeleton()` は `draw.cpp` の `DrawScene()` から呼ばれます。
親子関係は `ezTrackDataT::parent`（同じ Subject 内の index）で分かります。

## うまく表示されないとき

| 症状 | 確認すること |
| --- | --- |
| `[Vicon] 接続に失敗しました` | Vicon PC の IP・ポート、Shogun Live が起動しているか |
| 起動時に数十秒止まる | Vicon PC が応答しないときの DataStream SDK のタイムアウト待ちです。Vicon PC を起動するか `use_tracker = false` にしてください |
| `[Vicon] まだフレームが届いていません` | Shogun Live が Live 状態か、DataStream が有効か |
| 画面に骨が出ない | コンソールの Subject 一覧に骨格が出ているか。Subject 名は合っているか |
| 手・足だけ動かない | `(未検出)` と表示された骨の名前を実際の名前に変更する |
| モデルが表示されない | 実行時の作業フォルダが `VR_Vicon_Project` になっているか（`../models` を参照します） |

## 補足

- Vicon が使えないときは `use_tracker = false`（または接続失敗時）で
  これまでどおりマウス操作のデモとして動作します。
- 骨の座標は `ezTrackDataT`（`x, y, z, roll, pitch, yaw, name, parent`）に
  入っています。`name` と `parent` を使うと独自の骨格表示も作れます。
- ソースの文字コードは Shift-JIS (CP932) でクライアントへ渡す運用です。
  開発中に UTF-8 へ変換した場合は `tools/encoding_convert` を参照してください。
