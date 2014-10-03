grabify
=======

Prototype caveats:
- expects proxy on localhost:8118 (privox to filter ads)
- expects avconv, pulseaudio
- use pavucontrol etc. to route sound output back to grabify ("force record" helps you there)
- no cookies :(  (means you have to log into spotify every time you start grabify)
- no hardware acceleration for rendering, due to a but in QtWebkit that would make most of the interface disappear (=> high CPU load)
