
enum SoundType {
  SQUISH = "/src/sounds/splat.wav",
  CLICK = "/src/sounds/clinck.wav",
}

class SoundHandler {
  private audio: HTMLAudioElement;

  constructor() {
    this.audio = new Audio();
  }

  playSound(soundType: SoundType) {
    this.audio.src = soundType;
    this.audio.play();
  }

  stopSound() {
    this.audio.pause();
  }
  
}