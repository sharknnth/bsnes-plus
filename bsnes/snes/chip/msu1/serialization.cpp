#ifdef MSU1_CPP

void MSU1::serialize(serializer &s) {
  Processor::serialize(s);

  s.integer(mmio.data_offset);
  s.integer(mmio.data_seek_offset);
  s.integer(mmio.audio_offset);
  s.integer(mmio.audio_loop_offset);

  s.integer(mmio.audio_track);
  s.integer(mmio.audio_volume);
  s.integer(mmio.audio_resume_track);
  s.integer(mmio.audio_resume_offset);

  s.integer(mmio.data_busy);
  s.integer(mmio.audio_busy);
  s.integer(mmio.audio_repeat);
  s.integer(mmio.audio_play);
  s.integer(mmio.audio_error);

  if(datafile.open()) datafile.close();
  if(datafile.open(string(cartridge.basename(), ".msu"), file::mode::read)) {
    datafile.seek(mmio.data_offset);
  }

  if(audiofile.open()) audiofile.close();
  if(audiofile.open(string(cartridge.basename(), "-", mmio.audio_track, ".pcm"), file::mode::read)) {
    audiofile.seek(mmio.audio_offset);
  }
}

#endif
