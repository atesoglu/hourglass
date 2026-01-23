#pragma once

#include <QObject>
#include <QSoundEffect>
#include <QString>

class SoundPlayer : public QObject {
    Q_OBJECT

public:
    explicit SoundPlayer(QObject *parent = nullptr);
    ~SoundPlayer() override;

    enum class SoundPreset {
        Chime,
        Beep,
        Bell,
        None
    };

    void setPreset(SoundPreset preset);
    SoundPreset currentPreset() const { return m_currentPreset; }
    void loadPresetFromSettings();

public slots:
    void play();

private:
    QString getResourcePath(SoundPreset preset) const;

    QSoundEffect *m_soundEffect;
    SoundPreset m_currentPreset;
};
