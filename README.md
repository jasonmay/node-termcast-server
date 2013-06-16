node-termcast-server
====================

Installation
------------

```
make
npm install
```

Running the server
------------------

There is no server yet. There is a script that tests the VT emulator that will be used to paint the terminal on the web screen. That can be run with:

```
node server.js
```

TODO
----

Decide on how to communicate between the server and the static site. Either a templating engine or just straight-up AJAX to a minimal endpoint server would suffice.
