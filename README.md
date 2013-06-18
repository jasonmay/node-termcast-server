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

The server doesn't do much except list streams on an unstyled static page using AJAX requests.

Make sure you have a termcast server running from here: https://github.com/jasonmay/app-termcast-server

Take note of where the **connections.sock** unix socket is hosted and place the full page in your config:

```
cp config.json.example config.json
<edit config.json>
```

Then you should be good to go.

```
node server.js
```

TODO
----

Decide on how to communicate between the server and the static site. Either a templating engine or just straight-up AJAX to a minimal endpoint server would suffice.
