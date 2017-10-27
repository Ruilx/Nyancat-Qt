#ifndef NYANDIALOG_H
#define NYANDIALOG_H

#include <QtWidgets>
#include <QMediaPlayer>

class NyanDialog : public QWidget
{
	Q_OBJECT

	const QSize titleSize = QSize(6, 8);
	const QPoint titlePos[21] = {
		QPoint(1, 17), QPoint(7, 17), QPoint(13, 17), QPoint(19, 17), QPoint(25, 17),
		QPoint(31, 17), QPoint(37, 17), QPoint(43, 17), QPoint(49, 17), QPoint(55, 17),
		QPoint(61, 17), QPoint(67, 17), QPoint(73, 17),
		QPoint(16, 25), QPoint(22, 25), QPoint(28, 25), QPoint(34, 25), QPoint(40, 25),
		QPoint(46, 25), QPoint(52, 25), QPoint(58, 25),
	};

	const QSize nyanCatImageSize = QSize(80, 50);
	const QPoint nyanCatImagePos[12] = {
		QPoint(0, 0), QPoint(0, 50), QPoint(0, 100), QPoint(0, 150), QPoint(0, 200),
		QPoint(0, 250), QPoint(0, 300), QPoint(0, 350), QPoint(0, 400), QPoint(0, 450),
		QPoint(0, 500), QPoint(0, 550),
	};

//	QTimeLine *headTimeline;
//	QTimeLine *bodyTimeline;

	QTimer *headTimer;
	QTimer *bodyTimer;
	int currentHeadFrame = 0;
	int currentBodyFrame = 0;

	QMediaPlayer *player;
	bool enterLoop = false;
	bool showHint = false;

	QPixmap nyanImage;
	QPixmap titleImage;
	QPixmap currentImage;
	QPixmap hintImage;

	void paintEvent(QPaintEvent *){
		if(this->currentImage.isNull()){
			return;
		}
		//qDebug() << "Enter PaintEvent";
		QPainter p(this);
		p.setRenderHint(QPainter::Antialiasing, false);
		if(this->currentImage.size() == this->size()){
			p.drawPixmap(0, 0, this->currentImage);
		}else{
			p.drawPixmap(0, 0, this->currentImage.scaled(this->size()));
		}
		if(this->showHint){
			p.drawPixmap(0, this->size().height() - this->hintImage.height(), this->hintImage);
		}
	}

	void closeEvent(QCloseEvent *e){
//		if(this->headTimeline->state() != QTimeLine::NotRunning){
//			this->headTimeline->stop();
//		}
//		if(this->bodyTimeline->state() != QTimeLine::NotRunning){
//			this->bodyTimeline->stop();
//		}
		if(this->headTimer->isActive()){
			this->headTimer->stop();
		}
		if(this->bodyTimer->isActive()){
			this->bodyTimer->stop();
		}
		if(this->player->state() != QMediaPlayer::StoppedState){
			while(this->player->volume() > 0){
				this->player->setVolume((this->player->volume() - 5) < 0? 0: (this->player->volume() - 5));
				QThread::currentThread()->msleep(10);
			}
			this->player->stop();
		}
		e->accept();
	}

	void keyPressEvent(QKeyEvent *e){
		if(e->key() == Qt::Key_Escape){
			this->close();
			e->accept();
		}
	}

//	void wheelEvent(QWheelEvent *e){
//		int interval = this->bodyTimer->interval();
//		if(e->delta() > 0){
//			if(interval > 10){
//				this->bodyTimer->setInterval(interval - 10);
//			}
//		}else{
//			if(interval < 1000){
//				this->bodyTimer->setInterval(interval + 10);
//			}
//		}
//		qDebug() << "current timer Interval:" << this->bodyTimer->interval();
//	}

public:
	NyanDialog(QWidget *parent = 0): QWidget(parent){
		this->setMinimumSize(160, 100);
		this->resize(this->minimumSize());

		this->nyanImage.load(":/data/nyan.png");
		this->titleImage.load(":/data/title.png");
		this->hintImage.load(":/data/hint.png");

		this->player = new QMediaPlayer(this, QMediaPlayer::StreamPlayback);
		this->player->setVolume(100);
		this->player->setMedia(QUrl("qrc:/data/head.mp3"));
		this->connect(this->player, &QMediaPlayer::stateChanged, this, [this](QMediaPlayer::State state){
			if(this->enterLoop == false){
				if(state == QMediaPlayer::StoppedState){
					this->enterLoop = true;
					if(QApplication::arguments().contains("bass", Qt::CaseInsensitive)){
						this->player->setMedia(QUrl("qrc:/data/bassbody.mp3"));
					}else{
						this->player->setMedia(QUrl("qrc:/data/body.mp3"));
					}
//					if(this->headTimeline->state() != QTimeLine::NotRunning){
//						this->headTimeline->stop();
//					}
//					if(this->bodyTimeline->state() == QTimeLine::NotRunning){
//						this->bodyTimeline->start();
//					}
					this->player->play();
					if(this->headTimer->isActive()){
						this->headTimer->stop();
					}
					if(!this->bodyTimer->isActive()){
						this->bodyTimer->start();
					}
				}
			}else{
				if(state == QMediaPlayer::StoppedState){
					this->player->play();
					if(!QApplication::arguments().contains("hintless", Qt::CaseInsensitive)){
						this->showHint = true;
					}
				}
			}
		});

		this->currentImage = QPixmap(this->nyanCatImageSize);
		this->currentImage.fill(Qt::black);

		this->headTimer = new QTimer(this);
		this->headTimer->setInterval(168);
		this->connect(this->headTimer, &QTimer::timeout, this, [this](){
			if(this->currentHeadFrame > 20){
				if(this->headTimer->isActive()){
					this->headTimer->stop();
				}
			}else{
				if(this->currentHeadFrame < 0 || this->currentHeadFrame > 20){
					qDebug() << "Frame is out of range(this->headTimer).";
					return;
				}
				QPainter p(&this->currentImage);
				p.drawPixmap(this->titlePos[this->currentHeadFrame],
							this->titleImage.copy(QRect(QPoint(this->titleSize.width() * this->currentHeadFrame, 0),
														this->titleSize)));
				this->update();
				this->currentHeadFrame++;
			}
		});


//		this->headTimeline = new QTimeLine(3680, this);
//		this->headTimeline->setFrameRange(0, 20);
//		this->headTimeline->setLoopCount(1);
//		this->connect(this->headTimeline, &QTimeLine::frameChanged, this, [this](int frame){
//			if(frame < 0 || frame >= 21){
//				qDebug() << "Frame is out of range";
//				return;
//			}
//			//qDebug() << "headTimeLine frame:" << frame;
//			QPainter p(&this->currentImage);
//			p.drawPixmap(this->titlePos[frame],
//						this->titleImage.copy(QRect(QPoint(this->titleSize.width() * frame, 0),
//													this->titleSize)));
//			this->update();
//		});
//		this->connect(this->headTimeline, &QTimeLine::finished, this, [this](){
//			if(this->headTimeline->state() != QTimeLine::NotRunning){
//				this->headTimeline->stop();
//			}
//		});

		this->bodyTimer = new QTimer(this);
		this->bodyTimer->setInterval(77);
		this->connect(this->bodyTimer, &QTimer::timeout, this, [this](){
			if(this->currentBodyFrame > 11 || this->currentBodyFrame < 0){
				this->currentBodyFrame = 0;
			}
			QPainter p(&this->currentImage);
			p.drawPixmap(QPoint(0, 0),
						this->nyanImage.copy(QRect(this->nyanCatImagePos[this->currentBodyFrame],
												   this->nyanCatImageSize)));
			this->update();
			this->currentBodyFrame++;
		});

//		this->bodyTimeline = new QTimeLine(2166, this);
//		this->bodyTimeline->setFrameRange(0, 11);
//		this->bodyTimeline->setLoopCount(0);
//		this->connect(this->bodyTimeline, &QTimeLine::frameChanged, this, [this](int frame){
//			if(frame < 0 || frame >= 12){
//				qDebug() << "Frame is out of range(bodyTimeLine)";
//				return;
//			}
//			//qDebug() << "bodyTimeLine frame:" << frame;
//			QPainter p(&this->currentImage);
//			p.drawPixmap(QPoint(0, 0),
//						this->nyanImage.copy(QRect(this->nyanCatImagePos[frame],
//												   this->nyanCatImageSize)));
//			this->update();
//		});


		QTimer::singleShot(1000, this, &NyanDialog::ready);

	}

	~NyanDialog();

private slots:
	void ready(){
//		if(this->bodyTimeline->state() != QTimeLine::NotRunning){
//			this->bodyTimeline->stop();
//		}
//		if(this->headTimeline->state() == QTimeLine::NotRunning){
//			this->headTimeline->start();
//		}
		if(this->bodyTimer->isActive()){
			this->bodyTimer->stop();
		}
		if(!this->headTimer->isActive()){
			this->headTimer->start();
		}
		if(this->player->state() == QMediaPlayer::StoppedState){
			this->player->play();
		}

//		QPainter p(&this->currentImage);
//		p.drawPixmap(this->titlePos[0],
//					this->titleImage.copy(QRect(QPoint(0, 0),
//												this->titleSize)));
	}
};

#endif // NYANDIALOG_H
