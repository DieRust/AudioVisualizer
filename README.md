# Audio Visualizer
This is project to make simple audio vizualizer in c, now working only on windows./
To use it you must **turn on mix audio in Windows**, then you can turn on the program and select mix audio divice by its number.
I use here two external library:
* PortAudio
* fftw
# logic of code
Its working on simple logic, first we declare our device to portaudio and use PA_sleep() to works constantly then, fftw library calculate our frequencics which we represents us colorfull bars.\
I use here multi threading throught Windows API to register when user press 'q' to exit.\
have fun
