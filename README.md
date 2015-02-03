# LANMusicSync
Stream synchronized audio between your devices. Any device you own becomes a speaker to playback your music.

Currently the supported platforms are Windows 7/8, and the intent is to add support for Unix-based systems, as well as Android. Server functionality will likely be restricted to the desktop version.

Progress:
- Music can be streamed across the network from a single server to single client
- Supports almost all audio formats, using Ffmpeg
- Threaded audio and decompression, networking will get its own thread soon
- Synchronization code not yet started
- Need to support multiple clients
- Windows 7 seems broken
