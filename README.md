node-termcast-server
====================

Broadcast your work!

![Preview](https://github.com/jasonmay/node-termcast-server/raw/master/doc/preview.png "node-termcast-server")

Installation
------------

```
npm install -g node-gyp
node-gyp rebuild
npm install
```

Running the server
------------------

Make sure you have a termcast server running from here: https://github.com/jasonmay/app-termcast-server

Take note of where the **connections.sock** unix socket is hosted and place the full path in your config:

```
cp config.json.example config.json
<edit config.json>
```

Then you should be good to go.

```
./bin/server
```

TODO
----

Need to track the cursor. The canvas doesn't have anything for this yet.

Need to track background colors.

Need to track other cell attributes, like boldness.

BUGS
----

Oh there are tons of them. Don't get me started. Check the Github issues for anything:

https://github.com/jasonmay/node-termcast-server/issues
