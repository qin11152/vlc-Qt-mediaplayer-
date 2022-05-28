#pragma once
#ifdef _WIN32
#include <basetsd.h>
typedef SSIZE_T ssize_t;
#endif
#include "vlc/vlc.h"
#include "ui_MyMediaplayer.h"
#include <QtWidgets/QWidget>
#include <QTimer>
#include <QFileDialog>
#include <QDebug>
#include <vector>

enum CycleStatus
{
    SingleCycle,
    ListCycle,
    None
};

class MyMediaplayer : public QWidget
{
    Q_OBJECT

public:
    MyMediaplayer(QWidget *parent = Q_NULLPTR);
    ~MyMediaplayer();

public slots:
    //播放当前视频
    void onPlayCurrentVedio();
    //暂停视频播放
    void onPauseCurrentVideo();
    //停止播放视频
    void onStopCurrentVideo();
    //点击打开按钮后
    void onOpenPushButtonClicked();
    //点击暂停按钮
    void onPausePushButtonClicked();
    //点击开始播放按钮
    void onPlayPushButtonClicked();
    //点击停止播放按钮
    void onStopPushButtonClicked();
    //滑块拖动到某一个位置
    void onSliderRelease(int val);
    //单曲循环按钮点击
    void onSingleButtonClicked();
    //刷新按钮，为了测试动态添加一个播放连接用
    void onSignalFreshButtonClicked();
    //当播放列表item被点击
    void onSignalListWidItemClicked(QListWidgetItem* item);

private:
    //初始化数据
    void initData();
    //初始化vlc数据
    bool initVlcData();
    //初始化链接
    void initConnect();
    //释放vlc
    void freeVlc();
    //清空播放列表
    void clearPlayList();
    //把毫秒转化为分：秒的形式 
    void transferTime(QString& time, int msTime);
    //设置播放列表
    void addToPlayList(QString path);
    //根据index播放列表中视频
    void playVideoAcordIndex(int idx);
    //设置listwidget中当前项
    void setListWidCurrentRow();

protected:
    //vlc回调事件的处理
    static void vlcEvents(const libvlc_event_t* ev, void* param);

private:
    Ui::MyMediaplayerClass ui;
    libvlc_instance_t* m_ptrVlcInstance{ nullptr };
    libvlc_media_t* m_ptrMedia{ nullptr };
    libvlc_media_player_t* m_ptrMediaPlayer{ nullptr };
    libvlc_media_list_player_t* m_ptrListMediaPlayer{ nullptr };
    libvlc_media_list_t* m_ptrMediaList{ nullptr };
    QFileDialog* m_ptrFileDialog{ nullptr };

    bool m_bIsSingleCycle{ false };         //是否单曲循环
    std::vector<QString> m_vecPlayList;     //播放列表
    std::vector<QString> m_vecPlayedVideoPathList;   //所有已经播放过的视频路径列表
    std::vector<QString> m_vecPlayedVideoNameList;  //播放过的视频的名称，顺序和路径列表一致
    int m_iCurVideoPos{ -1 };       //当前视频在列表中的位置
    bool m_bIsPlay{ false };       //是否正在播放
    QString m_strCurPath{ "" };     //当前要播放的视频文件的路径
    int m_iCurVedioTotalTime{ 0 };     //当前视频总时间

    static MyMediaplayer* m_ptrThis;    //指向自己的指针
    static int curTime;
};
