//---------------------------------------------------------------------------
// ViconSample - Vicon Shogun Live DataStream 受信サンプル
//
// Shogun Live が配信している Transform データをコンソールに表示します。
//   - Subject (Skeleton) の Segment : グローバル位置 [mm] / オイラー角 [deg]
//   - Marker (Labeled / Unlabeled)  : グローバル位置 [mm]
//   - --ez を付けると ezTracker_Vicon 経由で読んだ値も並べて表示
//   - --bones を付けると Subject の全骨 (Segment) を ezTracker 経由で表示
//
// 使い方:
//   ViconSample.exe [host:port] [--seconds N] [--rate HZ]
//                   [--all-segments] [--markers] [--ez] [--bones]
//
// 例:
//   ViconSample.exe                      … 127.0.0.1:801 に 10 秒接続
//   ViconSample.exe --seconds 30         … 30 秒間表示
//   ViconSample.exe --markers            … マーカー位置も表示
//   ViconSample.exe --all-segments       … 全セグメントを表示
//   ViconSample.exe 192.168.0.10:801 --ez
//---------------------------------------------------------------------------
#define _USE_MATH_DEFINES
#include <cmath>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <string>
#include <thread>

#include "ezTrack_Vicon.h"
#include "Vicon/DataStreamClient.h"

using namespace ViconDataStreamSDK::CPP;

namespace {
const double kPi = 3.14159265358979323846;
volatile std::sig_atomic_t g_stop = 0;

void OnSignal(int) { g_stop = 1; }

struct Options {
    std::string hostport = "127.0.0.1:801";
    double seconds = 10.0;
    double rate = 30.0;
    bool allSegments = false;
    bool markers = false;
    bool ez = false;
    bool bones = false;
    bool serverPush = false;
    bool preFetch = false;
};

void PrintUsage(const char* exe) {
    printf(
        "Usage: %s [host:port] [--seconds N] [--rate HZ] [--all-segments] "
        "[--markers] [--ez] [--bones]\n"
        "  host:port      接続先 (既定: 127.0.0.1:801)\n"
        "  --seconds N    表示時間[秒] (既定: 10)\n"
        "  --rate HZ      表示更新レート[Hz] (既定: 30)\n"
        "  --all-segments ルート以外の全セグメントも表示\n"
        "  --markers      Labeled/Unlabeled マーカー位置も表示\n"
        "  --ez           ezTracker_Vicon 経由の値も表示\n"
        "  --bones        Subject の全骨 (Segment) を ezTracker 経由で表示 (--ez を兼ねる)\n"
        "  --push         StreamMode を ServerPush にする (既定: ClientPull)\n"
        "  --prefetch     StreamMode を ClientPullPreFetch にする\n",
        exe);
}

bool ParseArgs(int argc, char** argv, Options* opt) {
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        if (a == "-h" || a == "--help") {
            PrintUsage(argv[0]);
            return false;
        } else if (a == "--seconds" && i + 1 < argc) {
            opt->seconds = atof(argv[++i]);
        } else if (a == "--rate" && i + 1 < argc) {
            opt->rate = atof(argv[++i]);
        } else if (a == "--all-segments") {
            opt->allSegments = true;
        } else if (a == "--markers") {
            opt->markers = true;
        } else if (a == "--ez") {
            opt->ez = true;
        } else if (a == "--bones") {
            opt->bones = true;
        } else if (a == "--push") {
            opt->serverPush = true;
        } else if (a == "--prefetch") {
            opt->preFetch = true;
        } else if (!a.empty() && a[0] != '-') {
            opt->hostport = a;
        } else {
            printf("Unknown option: %s\n", a.c_str());
            PrintUsage(argv[0]);
            return false;
        }
    }
    if (opt->seconds <= 0.0) opt->seconds = 10.0;
    if (opt->rate <= 0.0) opt->rate = 30.0;
    return true;
}

// Vicon SDK の Result を文字列にする (デバッグ用)
const char* ResultName(Result::Enum r) {
    switch (r) {
        case Result::Success:                return "Success";
        case Result::InvalidHostName:        return "InvalidHostName";
        case Result::ClientConnectionFailed: return "ClientConnectionFailed";
        case Result::ClientAlreadyConnected: return "ClientAlreadyConnected";
        case Result::NotConnected:           return "NotConnected";
        case Result::NoFrame:                return "NoFrame";
        default:                             return "Other";
    }
}

// 接続して必要なストリームを有効化する
bool ConnectClient(Client* client, const std::string& hostport, bool serverPush,
                   bool preFetch) {
    // Vicon 公式テストと同じ手順:
    //   IsConnected() が真になるまで Connect を繰り返す
    //   (SetConnectionTimeout は DSDK 1.12 以降の API のため使わない → 1.11 でもビルド可)
    printf("Connecting to %s ...", hostport.c_str());
    const int kMaxAttempts = 10;
    int attempt = 0;
    while (!client->IsConnected().Connected && attempt < kMaxAttempts && !g_stop) {
        ++attempt;
        const Output_Connect out = client->Connect(hostport);
        if (out.Result == Result::Success ||
            out.Result == Result::ClientAlreadyConnected) {
            printf(".");
        } else {
            printf("\nConnect attempt %d failed (%s)", attempt,
                   ResultName(out.Result));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    printf("\n");
    if (!client->IsConnected().Connected) {
        printf("ERROR: could not connect to %s\n", hostport.c_str());
        return false;
    }

    const Output_EnableSegmentData enSeg = client->EnableSegmentData();
    printf("EnableSegmentData        : %s\n", ResultName(enSeg.Result));
    const Output_EnableMarkerData enMarker = client->EnableMarkerData();
    printf("EnableMarkerData         : %s\n", ResultName(enMarker.Result));
    const Output_EnableUnlabeledMarkerData enUnlabeled =
        client->EnableUnlabeledMarkerData();
    printf("EnableUnlabeledMarkerData: %s\n", ResultName(enUnlabeled.Result));
    if (serverPush) {
        const Output_SetStreamMode sm = client->SetStreamMode(StreamMode::ServerPush);
        printf("SetStreamMode(ServerPush): %s\n", ResultName(sm.Result));
    } else if (preFetch) {
        const Output_SetStreamMode sm =
            client->SetStreamMode(StreamMode::ClientPullPreFetch);
        printf("SetStreamMode(PreFetch)  : %s\n", ResultName(sm.Result));
    } else {
        const Output_SetStreamMode sm = client->SetStreamMode(StreamMode::ClientPull);
        printf("SetStreamMode(ClientPull): %s\n", ResultName(sm.Result));
    }
    // ezTracker_Vicon と同じ Z-up に合わせる
    const Output_SetAxisMapping am =
        client->SetAxisMapping(Direction::Forward, Direction::Left, Direction::Up);
    printf("SetAxisMapping           : %s\n", ResultName(am.Result));

    const Output_GetVersion ver = client->GetVersion();
    printf("Connected to %s  (DataStream Client %u.%u.%u.%u)\n",
           hostport.c_str(), ver.Major, ver.Minor, ver.Point, ver.Revision);

    const Output_GetFrameNumber fn = client->GetFrameNumber();
    printf("Connected flag=%d  GetFrameNumber: %s (%u)\n\n",
           client->IsConnected().Connected ? 1 : 0,
           ResultName(fn.Result), fn.FrameNumber);
    return true;
}

void PrintSegment(Client* client, const std::string& subject,
                  const std::string& segment, const char* tag) {
    const Output_GetSegmentGlobalTranslation t =
        client->GetSegmentGlobalTranslation(subject, segment);
    const Output_GetSegmentGlobalRotationEulerXYZ r =
        client->GetSegmentGlobalRotationEulerXYZ(subject, segment);
    if (t.Result != Result::Success || r.Result != Result::Success) return;

    printf("%s %-24s T=(%9.2f,%9.2f,%9.2f)mm  R=(%8.2f,%8.2f,%8.2f)deg  occ=%d\n",
           tag, segment.c_str(),
           t.Translation[0], t.Translation[1], t.Translation[2],
           r.Rotation[0] * 180.0 / kPi, r.Rotation[1] * 180.0 / kPi,
           r.Rotation[2] * 180.0 / kPi, t.Occluded ? 1 : 0);
}

void PrintFrame(Client* client, const Options& opt) {
    const unsigned int subjectCount =
        client->GetSubjectCount().SubjectCount;
    const unsigned int unlabeledCount =
        client->GetUnlabeledMarkerCount().MarkerCount;
    const unsigned int frameNumber = client->GetFrameNumber().FrameNumber;
    const double frameRate = client->GetFrameRate().FrameRateHz;

    printf("---- frame %u : %.1f Hz  subjects=%u unlabeledMarkers=%u ----\n",
           frameNumber, frameRate, subjectCount, unlabeledCount);

    for (unsigned int si = 0; si < subjectCount; ++si) {
        const std::string subject =
            client->GetSubjectName(si).SubjectName;
        const std::string root =
            client->GetSubjectRootSegmentName(subject).SegmentName;
        const unsigned int segmentCount =
            client->GetSegmentCount(subject).SegmentCount;

        printf("  Subject[%u] %s  (segments=%u, root=%s)\n",
               si, subject.c_str(), segmentCount, root.c_str());
        PrintSegment(client, subject, root, "    root");

        if (opt.allSegments) {
            for (unsigned int gi = 0; gi < segmentCount; ++gi) {
                const std::string seg =
                    client->GetSegmentName(subject, gi).SegmentName;
                if (seg != root) PrintSegment(client, subject, seg, "    seg ");
            }
        }

        if (opt.markers) {
            const unsigned int markerCount =
                client->GetMarkerCount(subject).MarkerCount;
            for (unsigned int mi = 0; mi < markerCount; ++mi) {
                const std::string marker =
                    client->GetMarkerName(subject, mi).MarkerName;
                const Output_GetMarkerGlobalTranslation t =
                    client->GetMarkerGlobalTranslation(subject, marker);
                if (t.Result != Result::Success) continue;
                printf("    marker %-20s (%9.2f,%9.2f,%9.2f)mm  occ=%d\n",
                       marker.c_str(), t.Translation[0], t.Translation[1],
                       t.Translation[2], t.Occluded ? 1 : 0);
            }
        }
    }

    if (opt.markers) {
        for (unsigned int mi = 0; mi < unlabeledCount; ++mi) {
            const Output_GetUnlabeledMarkerGlobalTranslation t =
                client->GetUnlabeledMarkerGlobalTranslation(mi);
            if (t.Result != Result::Success) continue;
            printf("  UMarker[%3u] (%9.2f,%9.2f,%9.2f)mm\n", mi,
                   t.Translation[0], t.Translation[1], t.Translation[2]);
        }
    }
}

// ezTracker_Vicon が保持している Subject 単位のトラックを表示 (従来と同じビュー)
void PrintEzTracks(ezTracker_Vicon* tracker) {
    for (int i = 0; i < _n_tracks; ++i) {
        const ezTrackDataT* d = tracker->getTrackData(i);
        if (d->id == -1) continue;
        printf("    [ez] id=%2d name=%-20s pos=(%8.4f,%8.4f,%8.4f)m  "
               "rot=(%8.3f,%8.3f,%8.3f)deg\n",
               d->id, d->name, d->x, d->y, d->z, d->roll, d->pitch, d->yaw);
    }
}

// ezTracker_Vicon の Subject 単位 ezTracker (骨格) を表示
void PrintEzBones(ezTracker_Vicon* vicon) {
    const int subjectCount = vicon->getSubjectCount();
    for (int si = 0; si < subjectCount; ++si) {
        const char* subjectName = vicon->getSubjectName(si);
        ezTracker* trk = vicon->getSubject(si);
        if (trk == NULL) continue;
        int boneCount = 0;
        for (int i = 0; i < _n_tracks; ++i) {
            if (trk->getTrackData(i)->id != -1) ++boneCount;
        }
        printf("    [bones] %s : %d bones\n", subjectName, boneCount);
        for (int i = 0; i < _n_tracks; ++i) {
            const ezTrackDataT* d = trk->getTrackData(i);
            if (d->id == -1) continue;
            printf("      %2d %-20s parent=%3d pos=(%8.4f,%8.4f,%8.4f)m "
                   "rot=(%8.2f,%8.2f,%8.2f)deg\n",
                   d->id, d->name, d->parent, d->x, d->y, d->z,
                   d->roll, d->pitch, d->yaw);
        }
    }
}
}  // namespace

int main(int argc, char** argv) {
    Options opt;
    if (!ParseArgs(argc, argv, &opt)) return 1;

    std::signal(SIGINT, OnSignal);

    Client client;
    if (!ConnectClient(&client, opt.hostport, opt.serverPush, opt.preFetch)) return 1;

    // --ez / --bones : ezTracker_Vicon でも同じストリームを受信する
    ezTracker_Vicon ezTracker(true);
    if (opt.ez || opt.bones) {
        ezTracker.init();
        std::string host = opt.hostport;
        if (!ezTracker.open(&host[0], false)) {
            printf("ERROR: ezTracker_Vicon::open failed\n");
            return 1;
        }
        printf("ezTracker_Vicon connected.\n\n");
    }

    const int totalFrames = static_cast<int>(opt.seconds * opt.rate);
    const int sleepMs = static_cast<int>(1000.0 / opt.rate + 0.5);
    unsigned int failCount = 0;

    for (int f = 0; f < totalFrames && !g_stop; ++f) {
        const Output_GetFrame frame = client.GetFrame();
        if (frame.Result != Result::Success) {
            if (failCount == 0 || failCount % 30 == 0) {
                printf("GetFrame failed: %s (result=%d) connected=%d. retrying...\n",
                       ResultName(frame.Result), (int)frame.Result,
                       client.IsConnected().Connected ? 1 : 0);
                fflush(stdout);
            }
            ++failCount;
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
            continue;
        }
        if (failCount > 0) {
            printf("GetFrame recovered after %u failure(s).\n", failCount);
            failCount = 0;
        }

        PrintFrame(&client, opt);
        if (opt.ez || opt.bones) {
            ezTracker.read();
            if (opt.ez) PrintEzTracks(&ezTracker);
            if (opt.bones) PrintEzBones(&ezTracker);
        }
        fflush(stdout);

        std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
    }

    if (opt.ez || opt.bones) ezTracker.close();
    client.Disconnect();
    printf("\nDone.\n");
    return 0;
}
