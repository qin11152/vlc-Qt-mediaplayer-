#include "MyMediaplayer.h"
#include <algorithm>

int MyMediaplayer::curTime = { 0 };
MyMediaplayer* MyMediaplayer::m_ptrThis = { nullptr };
MyMediaplayer::MyMediaplayer(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    initData();
    initConnect();
    m_ptrThis = this;
}

void MyMediaplayer::onPlayCurrentVedio()
{
    if (m_ptrMediaPlayer == nullptr)
    {
        return;
    }
    if (bool(libvlc_media_list_player_is_playing(m_ptrListMediaPlayer)))
    {
        return;
    }
    libvlc_media_list_player_play(m_ptrListMediaPlayer);
    int volumes = libvlc_audio_get_volume(m_ptrMediaPlayer);
    ui.VolumeSlider->setValue(volumes);
}

void MyMediaplayer::onPauseCurrentVideo()
{
    if (m_ptrListMediaPlayer == nullptr)
    {
        return;
    }
    if (bool(libvlc_media_list_player_is_playing(m_ptrListMediaPlayer)))
    {
        m_bIsPlay = false;
        libvlc_media_list_player_pause(m_ptrListMediaPlayer);
    }
}

void MyMediaplayer::onStopCurrentVideo()
{
    if (m_ptrListMediaPlayer == nullptr)
    {
        return;
    }
    m_bIsPlay = false;
    libvlc_media_list_player_stop(m_ptrListMediaPlayer);
    freeVlc();
}

void MyMediaplayer::onOpenPushButtonClicked()
{
    m_vecPlayList.clear();
    if (m_ptrVlcInstance)
    {
        freeVlc();
    }
    QStringList tmp = QFileDialog::getOpenFileNames(this, u8"打开视频", ".", u8"视频文件(*.mp4 *.mp3 *.flv *.3gp *.rmvb)");
    if (tmp.empty())
    {
        return;
    }
    for (auto& item : tmp)
    {
        if (std::find(m_vecPlayList.begin(), m_vecPlayList.end(), item) == m_vecPlayList.end())
        {
            m_vecPlayList.push_back(QDir::toNativeSeparators(item));
        }
    }
    m_strCurPath = QDir::toNativeSeparators(tmp[0]);
    setplayList();
}

void MyMediaplayer::onPausePushButtonClicked()
{
    onPauseCurrentVideo();
}

void MyMediaplayer::onPlayPushButtonClicked()
{
    onPlayCurrentVedio();
}

void MyMediaplayer::onStopPushButtonClicked()
{
    onStopCurrentVideo();
}

void MyMediaplayer::onSliderRelease(int val)
{
    int time = ui.horizontalSlider->value();
    libvlc_media_player_set_time(m_ptrMediaPlayer, val);
}

void MyMediaplayer::onSingleButtonClicked()
{
    if (m_bIsSingleCycle)
    {
        ui.pushButton->setText(u8"列表循环");
        m_bIsSingleCycle = false;
        if (nullptr != m_ptrListMediaPlayer)
        {
            libvlc_media_list_player_set_playback_mode(m_ptrListMediaPlayer, libvlc_playback_mode_loop);
        }
    }
    else
    {
        ui.pushButton->setText(u8"单曲循环");
        m_bIsSingleCycle = true;
        if (nullptr != m_ptrListMediaPlayer)
        {
            libvlc_media_list_player_set_playback_mode(m_ptrListMediaPlayer, libvlc_playback_mode_repeat);
        }
    }
}

void MyMediaplayer::onSignalFreshButtonClicked()
{
    /*if (!m_ptrVlcInstance)
    {
        return;
    }
    QString item = u8"D:/技改技措业务_产品技术.mp4";
    item = QDir::toNativeSeparators(item);
    m_ptrMedia = libvlc_media_new_path(m_ptrVlcInstance, item.toUtf8().data());
    if (!m_ptrMedia)
    {
        return;
    }
    libvlc_media_list_add_media(m_ptrMediaList, m_ptrMedia);
    libvlc_media_parse(m_ptrMedia);
    libvlc_media_release(m_ptrMedia);*/
    return;
}

void MyMediaplayer::initData()
{
    ui.VolumeSlider->setMinimum(0);
    ui.VolumeSlider->setMaximum(100);
    m_ptrFileDialog = new QFileDialog(this);
    m_ptrFileDialog->hide();
}

bool MyMediaplayer::initVlcData()
{
    m_ptrVlcInstance = libvlc_new(0, NULL);
    if (!m_ptrVlcInstance)
    {
        qDebug() << "qqqq create vlc failed";
        qDebug() << libvlc_errmsg;
        freeVlc();
        return false;
    }
    m_ptrListMediaPlayer = libvlc_media_list_player_new(m_ptrVlcInstance);
    if (!m_ptrListMediaPlayer)
    {
        qDebug() << "qqqq create m_ptrListMediaPlayer failed";
        freeVlc();
        return false;
    }
    m_ptrMediaList = libvlc_media_list_new(m_ptrVlcInstance);
    if (!m_ptrMediaList)
    {
        qDebug() << "qqqq create m_ptrMediaList failed";
        freeVlc();
        return false;
    }
    m_ptrMediaPlayer = libvlc_media_player_new(m_ptrVlcInstance);
    libvlc_video_set_mouse_input(m_ptrMediaPlayer, false);
    libvlc_video_set_key_input(m_ptrMediaPlayer, false);
    if (!m_ptrMediaPlayer)
    {
        freeVlc();
        qDebug() << "qqqq my media player failed";
        return false;
    }
    libvlc_event_manager_t* em = libvlc_media_player_event_manager(m_ptrMediaPlayer);
    libvlc_event_attach(em, libvlc_MediaPlayerTimeChanged, vlcEvents, this);
    libvlc_event_attach(em, libvlc_MediaPlayerEndReached, vlcEvents, this);
    libvlc_event_attach(em, libvlc_MediaPlayerStopped, vlcEvents, this);
    libvlc_event_attach(em, libvlc_MediaPlayerPlaying, vlcEvents, this);
    libvlc_event_attach(em, libvlc_MediaPlayerPaused, vlcEvents, this);
    libvlc_event_attach(em, libvlc_MediaPlayerPositionChanged, vlcEvents, this);
    libvlc_event_attach(em, libvlc_MediaPlayerLengthChanged, vlcEvents, this);
    libvlc_event_attach(em, libvlc_MediaPlayerTitleChanged, vlcEvents, this);
    libvlc_event_attach(em, libvlc_MediaPlayerVout, vlcEvents, this);
    //libvlc_media_player_set_hwnd(m_ptrMediaPlayer, (void *)ui.playWidget->winId());
    return true;
}

void MyMediaplayer::initConnect()
{
    connect(ui.openPushButton, &QPushButton::clicked, this, &MyMediaplayer::onOpenPushButtonClicked);
    connect(ui.playPushButton, &QPushButton::clicked, this, &MyMediaplayer::onPlayPushButtonClicked);
    connect(ui.pausePushButton, &QPushButton::clicked, this, &MyMediaplayer::onPausePushButtonClicked);
    connect(ui.stopPushButton, &QPushButton::clicked, this, &MyMediaplayer::onStopPushButtonClicked);
    connect(ui.horizontalSlider, &QSlider::sliderMoved, this, &MyMediaplayer::onSliderRelease);
    connect(ui.pushButton, &QPushButton::clicked, this, &MyMediaplayer::onSingleButtonClicked);
    connect(ui.VolumeSlider, &QSlider::valueChanged, this, [&](int val)
        {
            if (nullptr != m_ptrMediaPlayer)
            {
                qDebug() << "qqqq val:" << val;
                libvlc_audio_set_volume(m_ptrMediaPlayer, val);
            }
        });
}

void MyMediaplayer::freeVlc()
{
    if (m_ptrMedia)
    {
        libvlc_media_release(m_ptrMedia);
        m_ptrMedia = nullptr;
    }
    if (m_ptrMediaPlayer)
    {
        libvlc_media_player_stop(m_ptrMediaPlayer);
        libvlc_media_player_release(m_ptrMediaPlayer);
        m_ptrMediaPlayer = nullptr;
    }
    if (m_ptrVlcInstance)
    {
        libvlc_release(m_ptrVlcInstance);
        m_ptrVlcInstance = nullptr;
    }
    if (m_ptrListMediaPlayer)
    {
        libvlc_media_list_player_stop(m_ptrListMediaPlayer);
        libvlc_media_list_player_release(m_ptrListMediaPlayer);
        m_ptrListMediaPlayer = nullptr;
    }
    if (m_ptrMediaList)
    {
        libvlc_media_list_release(m_ptrMediaList);
        m_ptrMediaList = nullptr;
    }
}

void MyMediaplayer::transferTime(QString& time, int msTime)
{
    msTime = msTime / 1000;
    int hour = msTime / 3600;
    msTime = msTime % 3600;
    int minute = msTime / 60;
    msTime = msTime % 60;
    if (hour > 0)
    {
        time += QString::number(hour);
        time += ":";
    }
    if (minute < 10)
    {
        time += "0";
        time += QString::number(minute);
        time += ":";
    }
    else
    {
        time += QString::number(minute);
        time += ":";
    }
    if (msTime < 10)
    {
        time += "0";
        time += QString::number(msTime);
    }
    else
    {
        time += QString::number(msTime);
    }
}

void MyMediaplayer::setplayList()
{
    if (m_ptrVlcInstance)
    {
        freeVlc();
    }
    if (!initVlcData())
    {
        return;
    }
    bool first = true;
    for (auto& item : m_vecPlayList)
    {
        qDebug() << "qqqq path is" << item.toUtf8().data();
        m_ptrMedia = libvlc_media_new_path(m_ptrVlcInstance, item.toUtf8().data());
        if (m_ptrMedia)
        {
            qDebug() << "qqqq create media succ";
        }
        else
        {
            qDebug() << "qqqq create media failed";
        }
        libvlc_media_list_add_media(m_ptrMediaList, m_ptrMedia);
        libvlc_media_parse(m_ptrMedia);
        if (first)
        {
            first = false;
            int time = libvlc_media_get_duration(m_ptrMedia);
            QString strTime = "";
            ui.horizontalSlider->setMaximum(time);
            ui.horizontalSlider->setMinimum(0);
            transferTime(strTime, time);
            ui.totalTimelabel->setText(strTime);
        }
        libvlc_media_release(m_ptrMedia);
    }
    m_ptrMedia = nullptr;
    libvlc_media_list_player_set_media_list(m_ptrListMediaPlayer, m_ptrMediaList);
    libvlc_media_list_player_set_media_player(m_ptrListMediaPlayer, m_ptrMediaPlayer);
    if (m_bIsSingleCycle)
    {
        libvlc_media_list_player_set_playback_mode(m_ptrListMediaPlayer, libvlc_playback_mode_repeat);
    }
    else
    {
        libvlc_media_list_player_set_playback_mode(m_ptrListMediaPlayer, libvlc_playback_mode_loop);
    }
    libvlc_media_player_set_hwnd(m_ptrMediaPlayer, (void*)ui.playWidget->winId());
}

void MyMediaplayer::vlcEvents(const libvlc_event_t* ev, void* param)
{
    Q_UNUSED(param);
    switch (ev->type)
    {
    case libvlc_MediaPlayerTimeChanged:
    {
        //curTime = ev->u.media_player_time_changed.new_time;
        auto tmpTime = ev->u.media_player_time_changed.new_time;
        QString time = "";
        m_ptrThis->transferTime(time, tmpTime);
        m_ptrThis->ui.horizontalSlider->setValue(tmpTime);
        m_ptrThis->ui.curTimeLabel->setText(time);
    }
    break;
    case libvlc_MediaPlayerEndReached:
        break;
    case libvlc_MediaPlayerStopped:
        //m_ptrThis->onPlayFinish();
        break;
    case libvlc_MediaPlayerPlaying:
        break;
    case libvlc_MediaPlayerPaused:
        break;
    case libvlc_MediaPlayerPositionChanged:
    {
    }
    break;
    case libvlc_MediaPlayerVout:
    {
        qDebug() << ev->u.media_player_vout.new_count;
    }
    break;
    case libvlc_MediaPlayerLengthChanged:
    {
        int time = (ev->u.media_player_length_changed.new_length);
        qDebug() << "qqqq" << time;
        QString strtime = "";
        m_ptrThis->transferTime(strtime, time);
        m_ptrThis->ui.horizontalSlider->setMaximum(time);
        m_ptrThis->ui.totalTimelabel->setText(strtime);
    }
    break;
    default:
        break;
    }
}

MyMediaplayer::~MyMediaplayer()
{
    freeVlc();
    delete m_ptrFileDialog;
    m_ptrFileDialog = nullptr;
}
