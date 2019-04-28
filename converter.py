import sys
from pydub import AudioSegment
import soundfile

file_name = sys.argv[1]

ext_index = file_name.rfind('.')
ext = file_name[ext_index + 1:]

file_name_mono = file_name[:ext_index] + '_mono.' + ext
file_name_mono_8bit = file_name[:ext_index] + '_8bit_mono.' + ext

if ext != 'wav':
    print('Please input a wave file')
    sys.exit()

sound = AudioSegment.from_wav(file_name)
sound = sound.set_channels(1)
sound.export(file_name_mono, format="wav")

data, samplerate = soundfile.read(file_name_mono)
soundfile.write(file_name_mono_8bit, data, samplerate, subtype='PCM_U8')
