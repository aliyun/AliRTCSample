#include "userwidget.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

UserWidget::UserWidget(QString userId, QWidget *parent)
    : userId_(userId)
    , QWidget{parent} {

    // qDebug("UserWidget ctor, userId: %s", qPrintable(userId));

    videoView_ = new QWidget();

    QLabel *userIdLabel = new QLabel(QString("User %1").arg(userId), this);

    QPushButton *micButton = new QPushButton();
    QPushButton *cameraButton= new QPushButton();

    videoView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    videoView_->setStyleSheet("background-color: black;");
    // videoView_->setAttribute(Qt::WA_OpaquePaintEvent);
    // videoView_->setAttribute(Qt::WA_NativeWindow, true);

    // userIdLabel->setAttribute(Qt::WA_TranslucentBackground);
    // userIdLabel->setStyleSheet("QLabel { color: white; }");
    // userIdLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    // micButton->setStyleSheet("background-color: white;");
    // cameraButton->setStyleSheet("background-color: white;");
    micButton->setIcon(QIcon(":/images/mic_on.png"));
    cameraButton->setIcon(QIcon(":/images/camera_on.png"));

    // micButton->setAttribute(Qt::WA_TranslucentBackground);
    // cameraButton->setAttribute(Qt::WA_TranslucentBackground);

    QVBoxLayout *topLayout = new QVBoxLayout();
    topLayout->addWidget(userIdLabel);
    // topLayout->addWidget(statsLabel);
    topLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // Add bottom layout controls (microphone and camera toggle buttons)
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    // bottomLayout->addStretch();
    bottomLayout->addWidget(micButton);
    bottomLayout->addWidget(cameraButton);
    bottomLayout->setSpacing(10);
    bottomLayout->setAlignment(Qt::AlignBottom | Qt::AlignCenter);

    // Set the layout for videoDisplay to stack the bottom controls on top of it
    QVBoxLayout *videoLayout = new QVBoxLayout(this);
    videoLayout->addLayout(topLayout);
    videoLayout->addWidget(videoView_);
    // videoLayout->addStretch();
    videoLayout->addLayout(bottomLayout);

    userIdLabel->move(mapToGlobal(QPoint()));

    // QVBoxLayout *mainLayout = new QVBoxLayout(this);
    // mainLayout->addWidget(videoView_);


    // layout->addWidget(label, 1, Qt::AlignTop | Qt::AlignLeft);
    // label->move(10, 10); // Position the label within the video widget
    // layout->addWidget(label);

    // Layout for the top overlay widgets
    // QVBoxLayout *topLayout = new QVBoxLayout();
    // topLayout->addWidget(userIdLabel);
    // // topLayout->addWidget(statsLabel);
    // topLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // // Layout for bottom overlay buttons
    // QWidget *switches = new QWidget();
    // QHBoxLayout *bottomLayout = new QHBoxLayout();
    // bottomLayout->addWidget(mic);
    // bottomLayout->addWidget(camera);
    // bottomLayout->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    // switches->setLayout(bottomLayout);

    // topLayout->addWidget(switches);

    // video->setLayout(topLayout);



    // // Set layout for the entire widget
    // QVBoxLayout *mainLayout = new QVBoxLayout(this);
    // // // mainLayout->setMargin(0); // Remove margin if desired
    // mainLayout->addWidget(video);

    // setLayout(mainLayout);
    // mainLayout->addLayout(bottomLayout);


    // layout->addWidget(mic);
    // layout->addWidget(camera);

    connect(micButton, SIGNAL(clicked()), this, SLOT(onMuteMic()));
    connect(cameraButton, SIGNAL(clicked()), this, SLOT(onMuteCamera()));

}

void UserWidget::onMuteMic() {
    qDebug("UserWidget::onMuteMic");
}

void UserWidget::onMuteCamera() {
    qDebug("UserWidget::onMuteCamera");
}
