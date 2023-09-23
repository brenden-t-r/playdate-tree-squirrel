for i in *.mp3; do ffmpeg -i "$i" -acodec adpcm_ima_wav "${i%.*}.wav"; done
