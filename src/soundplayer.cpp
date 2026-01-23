#include "soundplayer.h"
#include <QDebug>
#include <QSettings>
#include <QUrl>
#include <QFile>
#include <QApplication>

SoundPlayer::SoundPlayer(QObject *parent)
    : QObject(parent)
    , m_soundEffect(new QSoundEffect(this))
    , m_currentPreset(SoundPreset::Chime)
{
    loadPresetFromSettings();
}

void SoundPlayer::loadPresetFromSettings()
{
    QSettings settings;
    QString presetStr = settings.value("soundPreset", "Chime").toString();
    
    if (presetStr == "Chime") {
        m_currentPreset = SoundPreset::Chime;
    } else if (presetStr == "Beep") {
        m_currentPreset = SoundPreset::Beep;
    } else if (presetStr == "Bell") {
        m_currentPreset = SoundPreset::Bell;
    } else {
        m_currentPreset = SoundPreset::None;
    }
    
    setPreset(m_currentPreset);
}

SoundPlayer::~SoundPlayer() = default;

void SoundPlayer::setPreset(SoundPreset preset)
{
    m_currentPreset = preset;
    
    if (preset == SoundPreset::None) {
        return;
    }
    
    QString resourcePath = getResourcePath(preset);
    // Use QUrl with resource path directly (qrc:/sounds/...)
    m_soundEffect->setSource(QUrl(resourcePath));
}

void SoundPlayer::play()
{
    // Reload preset from settings in case it was changed via system tray
    loadPresetFromSettings();
    
    if (m_currentPreset == SoundPreset::None) {
        return;
    }
    
    if (m_soundEffect->status() == QSoundEffect::Error) {
        // Fallback to system beep if sound file is missing
        QApplication::beep();
        return;
    }
    
    m_soundEffect->play();
}

QString SoundPlayer::getResourcePath(SoundPreset preset) const
{
    switch (preset) {
        case SoundPreset::Chime:
            return ":/sounds/chime.wav";
        case SoundPreset::Beep:
            return ":/sounds/beep.wav";
        case SoundPreset::Bell:
            return ":/sounds/bell.wav";
        case SoundPreset::None:
            return QString();
    }
    return QString();
}
