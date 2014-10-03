grabify
=======

Prototype caveats:
- expects proxy on localhost:8118 (privoxy to filter ads)
- expects avconv, pulseaudio
- use pavucontrol etc. to route sound output back to grabify ("force record" helps you there)
- make sure to set all volume sliders, which (could) affect the input to grabify to 100%
- no cookies :(  (means you have to log into spotify every time you start grabify)
- no hardware acceleration for rendering, due to a bug in QtWebkit that would make most of the interface disappear (=> high CPU load)
- does not automatically create folders yet
