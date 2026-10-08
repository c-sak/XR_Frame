# VR_Vicon_Project

TODO: x64ビルドで使用する際に"include64"をinclude pathに指定する必要がある旨を記載

ViconDataStreamから、人（Character）のトラッキング結果を受信し、
VR_Frame のキャラクタ（腰・胸・頭・手・足）へ反映するサンプルです。

`XR_FrameV2025_1125/VR_Project` をコピーして作られています。
Vicon の組み込み方は`sim.cpp`に極力寄せる形で作成しています。

## 用語の整理

ドキュメント内で使用する、Vicon関連の用語を整理します。

- Character
  - Viconシステムで動きをキャプチャ可能な物体の総称です。
- Prop
  - Characterのうち、剛体であり、1つの骨から構成されているものの総称です。Propのトラッキングは、以前の`ezTracker_Vicon`でもサポートされていました。
- Subject
  - Characterのうち、複数の骨とそれを接続する関節を持つものの総称です。今回の`ezTracker_Vicon`のアップデートによって、Propに加えてSubjectのデータを扱うことができるようになりました。
- Segment
  - Subjectを構成するの骨の呼称です。1本の骨 = 1 segmentとなります。

## サンプルで示していること

- バージョンアップされた`ezTracker_Vicon`の使用方法
- `ezTracker_Vicon`を使った、ViconDataStreamからのデータ取得
- Characterの一覧取得
- SubjectのSegmentの、名前による取得
- Subjectデータの利用方法（骨格の可視化）
- 骨の位置・姿勢の`ObjDataT`（腰・胸・頭・足）への反映
- 複数 Subject の同時取得・使用
- 従来のPropトラッキング機能との共存

## VR_Project との違い

| ファイル | 内容 |
| --- | --- |
| `sim.cpp` | サンプル本体。Vicon の接続・骨の対応付け・可視化。 |
| `draw.cpp` | `VR_Project` のコピー。`DrawScene()` / `PostDraw()` に `DrawTrackedSubject()` と `DrawTrackingInfo()` の呼び出しを追加。 |
| `config.h` | `use_tracker = true` に変更（それ以外は `VR_Project` と同じ）。 |
| その他のファイル | `VR_Project` と同じコピー。 |

## 起動と確認

画面の左上に、トラッキング中の Character 名とボーン数が一覧表示されます。骨格は Character ごとに色分けされた線で表示され、関節はオレンジのキューブです。

カメラは、Subjectが見やすいよう(0, 0, 0)を見渡せる位置に固定されていますが、、`C` キーVR_Projectと同じで頭（head）カメラに切り替えられます。`C`キーを再度押すことで、固定カメラに戻ることができます。

```text
Character2 : 73 bones
Character3 : 73 bones
TREE_A   : 1 bones
Character1 : 73 bones
```

状態の確認用に、特定のSubjectのの骨の一覧と移動・回転情報をログに出力することができます。`sim.cpp` の `printViconCharacter("SubjectName");` を使用してください。

## アップデート済みezTracking_Viconの利用方法

### 1. 共通の手順

#### 1-1. 接続

```cpp
// sim.cpp@89
static char VICON_HOST[] = "127.0.0.1:801";
```

```cpp
// sim.cpp@152 等
vicon = new ezTracker_Vicon(true);
if (vicon->open(VICON_HOST, false)) { ... }
```

`ezTracker_Vicon::open()` の引数は
「Vicon PC の IPアドレス:ポート番号」です。
接続後、`read()` でViconDataStreamから受信したデータを取り込むことができます。

#### 1-2. 毎フレームの受信（`UpdateScene` → `updateVicon`）

```cpp
vicon->read(); // 1フレームに1回だけ呼ぶ
```

`read()` は ViconDataStream から最新フレームを取り込みます。

### 2. Propのトラッキングを行う場合

```cpp
trackHead = tracker->getTrackData("CAP"); // 名前で取得
copyTrackToObj(trackHead, &simdata.head); // ObjDataT へコピー
```

Propのトラッキングは従来通りです。

`ezTracker.getTrackData("名前")`で特定のPropを表す`ezTrackDataT`への参照を取得し、`simdata`内のオブジェクトに反映させることができます。既存の`CAP` / `TREE_A` / `TREE_B` / `Chest` / `Candy` /`RightFoot` / `LeftFoot`のPropトラッキングは、本サンプル内で共存しています。

### 3. Subjectのトラッキングを行う場合

#### 3-1. Subjectの取得

```cpp
// ezTrack_Vicon.cpp@28

class ezTracker_Vicon :
    public ezTracker{
      public:
// 中略

int getSubjectCount() const;                  // トラッキングされているSubjectのトータル数
const char* getSubjectName(int index) const;  // index（Subject配列内での順番）番目のSubjectの名前
ezTracker* getSubject(const char* name);      // Subjectの名前で、Subjectを表すezTrackerへの参照を取得
ezTracker* getSubject(int index);             // Subjectのindexで、Subjectを表すezTrackerへの参照を取得

// 中略
};
```

Subjectの一覧取得・参照取得のために、ezTracking_Viconの上記のメソッドが使用できます。

```cpp
// sim.cpp@338

// UpdateScene()：Character への参照をキャッシュする
Character_count = 0;
if (vicon != NULL) {
    int n = vicon->getCharacterCount();
    if (n > VICON_MAX_CharacterS) n = VICON_MAX_CharacterS;
    for (int i = 0; i < n; i++) {
        Characters[i] = vicon->getCharacter(i);
    }
    Character_count = n;
}
```

`ezTracker_Vicon` は **1 Subject = 1 ezTracker** で骨格を保持しています。
サンプル`sim.cpp`では、上記のコードでSubjectを表す`ezTracker`への参照を全Subject分取得しています。

サンプルの`sim.cpp`では、既存のPropのトラッキングと同じように、Subjectを表す`ezTracker`を取得し、そこから骨の動きを取得してで描画しています。

ただし、Propの場合は **1 Prop = 1 ezTrackDataT**、Subjectの場合は **1 Subject = 1 ezTracker**であることに注意してください。これは、Propは関節を持たないため1点の移動・回転を追えば正しく姿勢を再現できる一方、Subjectは複数の関節を持つため、複数の移動・回転を束ねて1つのCharacterとする必要があるためです。

#### 3-2. 骨の取り出し

それぞれの骨の動きの情報は、その骨が属する`ezTracker`に`ezTrackDataT`として格納されています。つまり、[Propのトラッキングと同じアプローチ](#2-propのトラッキングを行う場合)で動きが取得できます。

```cpp
ezTracker* sub1 = vicon->getSubject("Subject1");
ezTrackDataT* hips = sub1->getTrackData("Hips");
```

### 4. 全骨の可視化（`DrawTrackedSubject`）

`Characters[]` の各 `ezTracker` が持つ `getTrackArray()` を走査し、`parent` が有効な骨同士を線で結んで可視化します。骨同士の親子関係は、`ezTrackDataT::parent`から取得できます。

ezTracing_Viconが受信しているすべてのCharacterが画面上で確認できるよう、`DrawTrackedSubject()`が状態を示したテキストを描画します。これは `draw.cpp` の `DrawScene()` から呼ばれます。
